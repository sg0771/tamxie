//-------------------------------------------

#include <jni.h>

#include <stdio.h>
#include <stdlib.h>  
#include <string.h>
#include <unistd.h>
#include <android/log.h>  
#include <sys/socket.h>  
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <linux/tcp.h>
#include <mutex>
#include <thread>
#include <queue>
#include <map>
#include "WXBase.h"

#define   BUFFER_SIZE  (1920*1080)
#define   TCP_SIZE  (64*1024)
#define   TS_LEN    188

#define   LOG_TAG    "WX"
#define   LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)  

// jni上下文
typedef struct jni_context {
     JavaVM *  jVM;//虚拟机
     jclass    jClz;//类
     jobject   jObj;//对象
     jmethodID cbData;//数据回调
     jmethodID cbEvent;//连接事件
} JniContext;

static  JniContext s_ctx;

//获取env
static JNIEnv* Android_JNI_GetEnv(void) {
    JNIEnv *env = NULL;
    int status = s_ctx.jVM->AttachCurrentThread(&env, NULL);
    if (status < 0) {
        LOGE("failed to attach current thread");
        return 0;
    }
    LOGE("Android_JNI_GetEnv OK---");
    return env;
}

class MpegtsHandler: public WXThread {
	std::ofstream m_fout;
	int m_fd = 0; //socket uid
	WXFifo  m_fifo;//ring buffer
	jbyteArray m_jarrayBuf = nullptr; //local Data
	void* m_pTsDemuxer = nullptr;
	bool m_bGetExtra = false;

	//协议层使用的回调函数，此处C++调用Java层方法，实现回调过程
	 void OnData(int bVideo, uint8_t *buf, int bufSize) {
	    JNIEnv *env = Android_JNI_GetEnv();
		env->SetByteArrayRegion(m_jarrayBuf, 0, bufSize, (const jbyte *) buf);//memcpy to java buufer
		env->CallVoidMethod(s_ctx.jObj, s_ctx.cbData, m_fd, bVideo, m_jarrayBuf, bufSize);//callback Java Function
	}
public:
	//输入数据
	void Write(uint8_t* buf, int buf_size){
		m_fifo.Write(buf, buf_size);
	}

	void RecordStart(const char* szName){
		WXTask task = [this,szName]{
			m_fout.open(szName, std::iostream::binary);
		};
		RunTask(task);
	}
	void RecordStop(){
		WXTask task = [this]{
			if(m_fout.is_open()){
				m_fout.close();
			}
		};
		RunTask(task);
	}

	//启动
	//new 
	MpegtsHandler(int fd) {
		m_fd = (int)fd;
		ThreadStart(true);
	}
	
	//退出
	//delete
	virtual ~MpegtsHandler() {
		ThreadStop();
	}

	virtual void  ThreadPrepare(){
		m_fifo.Init(BUFFER_SIZE * 10);
		JNIEnv *env = Android_JNI_GetEnv();
		env->CallVoidMethod(s_ctx.jObj, s_ctx.cbEvent, m_fd, 1);
		LOGE("Accpet Socket [%d]", m_fd);
		m_jarrayBuf = env->NewByteArray(BUFFER_SIZE);
		m_pTsDemuxer = TSDemuxCreate();
	}

	virtual void  ThreadPost(){
		if(m_fout.is_open()){
			m_fout.close();
		}
		JNIEnv *env = Android_JNI_GetEnv();
		env->CallVoidMethod(s_ctx.jObj, s_ctx.cbEvent, m_fd, 0);
		LOGE("close Socket [%d]", m_fd);
		if(m_pTsDemuxer){
			TSDemuxDestroy(m_pTsDemuxer);
			m_pTsDemuxer = nullptr;
		}
	}

	//数据处理线程
	virtual  void ThreadProcess() {
        uint8_t buf[TS_LEN];
        if (m_fifo.Size() >= TS_LEN) {
            m_fifo.Read(buf, TS_LEN);
			if(m_fout.is_open()){
				m_fout.write((const char*)buf, TS_LEN);//Record TS Data
			}
            int ret = TSDemuxWriteData(m_pTsDemuxer, buf, TS_LEN);
            uint8_t* pOut = nullptr;
            int out_size = 0;
            if (ret == TS_TYPE_VIDEO) {
                if (!m_bGetExtra) { //SPS+PPS
                    TSDemuxGetExtraData(m_pTsDemuxer, &pOut, &out_size);
                    if (pOut != nullptr && out_size != 0) {
                        m_bGetExtra = true;
                       OnData(1, pOut, out_size);
		LOGE("%s --- OnVideoExtra %d",__FUNCTION__,out_size);
                    }
                }
                if(m_bGetExtra){ //正常的H264
                    TSDemuxGetVideoData(m_pTsDemuxer, &pOut, &out_size);
                    if(out_size > 0){
                       OnData(1, pOut, out_size);

		LOGE("%s --- OnVideoData %d",__FUNCTION__,out_size);
                    }
                }
            } else if (ret == TS_TYPE_AUDIO) {
                TSDemuxGetAudioData(m_pTsDemuxer, &pOut, &out_size);
                OnData(0, pOut, out_size);

		LOGE("%s --- OnAudoData %d",__FUNCTION__,out_size);
            }
        }else {
            SLEEP_MS(1);
        }
	}
};


//接收服务端
class  PCCastRecv :public WXThread {
	int m_fdListen = -1;//监听socket
	std::map<int, MpegtsHandler*>m_mapHandler;//不同的接收节点
	fd_set m_fds;
	WXDataBuffer m_buf;

	WXLocker m_mutex;

	//Running in the m_thread!
	void RemoveHandler(int fd) {
		WXTask task = [this, fd]{
			if (m_mapHandler.count(fd)) {
				FD_CLR(fd, &m_fds);
				MpegtsHandler* client = m_mapHandler[fd];
				delete client;
				m_mapHandler.erase(fd);
				FD_CLR(fd, &m_fds);
				LOGE("close socket = %d", (int)fd);
			}
        };
        RunTask(task);
	}

	void AddHandler(int fd){
		if (!m_mapHandler.count(fd)){
			LOGE("accept socket = %d", (int)fd);
			m_mapHandler[fd] = new MpegtsHandler(fd);
			FD_SET(fd, &m_fds);
		}
	}
public:
	virtual void ThreadPrepare() {
		m_buf.Init(nullptr,  TCP_SIZE);
	}
	virtual void ThreadProcess() {
 		fd_set read_fds = m_fds;
        timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 0;
        int ret = ::select(1024, &read_fds, nullptr, nullptr, &tv);
        if(ret > 0){
			LOGE("select ..... %d",ret);
            if(FD_ISSET(m_fdListen, &read_fds)){
                //new socket
                struct sockaddr client_addr;
                socklen_t cAddrLen = sizeof(struct sockaddr);
                int fd = ::accept(m_fdListen, (sockaddr*)&client_addr, &cAddrLen);
                if (fd >= 0) {
		AddHandler(fd);
		LOGE("accept socket %d",fd);
                }
            }
            for (auto obj : m_mapHandler) {
                int fd = obj.first;
                if(FD_ISSET(fd, &read_fds)){
                    while (true) {
                        MpegtsHandler*client = obj.second;
                        int length = (int)::recv(fd, m_buf.m_pBuf, m_buf.m_iBufSize, 0);
                        if(length == m_buf.m_iBufSize){
                            client->Write(m_buf.m_pBuf, length);
                            continue;//Read Continue
                        }else if(length < m_buf.m_iBufSize && length > 0){
                            client->Write(m_buf.m_pBuf, length);
                        }else if(length <0 || (length == 0 && errno != EINTR)){
                            RemoveHandler(fd);//close socket , remove handler
                        }
                        break;
                    }
                }
            }
        }else if(ret == 0){
            SLEEP_MS(1);
        }else{//Error
            m_bThreadStop = true;
        }
	}
	virtual void ThreadPost() {
		for (auto obj : m_mapHandler) {
			delete obj.second;
			::close(obj.first);
		}
		m_mapHandler.clear();
		if (m_fdListen != -1) {
			::close(m_fdListen);
			m_fdListen = -1;
			LOGE("::close(m_fdListen)");
		}
	}

	int  Start(int nPort) {
		WXAutoLock al(m_mutex);
		LOGE("Tcp Server[%d] AAAA", nPort);
		m_fdListen = ::socket(PF_INET, SOCK_STREAM, 0);
		if (m_fdListen < 0) {
			m_fdListen = -1;
			LOGE("Create socket error");
			return 0;
		}
		LOGE("Tcp Server[%d] BBBB", nPort);
		//TCP 服务 快速启动
		int enable = 1;
		::setsockopt(m_fdListen, SOL_SOCKET, MSG_NOSIGNAL , (const void*)&enable, sizeof(enable));
		::setsockopt(m_fdListen, IPPROTO_TCP, TCP_NODELAY, (const char*)&enable, sizeof(enable));
		int buf_size = TCP_SIZE;
		::setsockopt(m_fdListen, IPPROTO_TCP, SO_RCVBUF, (const char*)&buf_size, sizeof(buf_size));

		LOGE("Tcp Server[%d] CCCCC", nPort);
		//绑定本机端口
		struct sockaddr_in server_addr;
		//清空结构体
		memset(&server_addr, 0, sizeof(server_addr));
		server_addr.sin_family = PF_INET;
		//Bind to all address
		server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		//Convert port to network byte order
		server_addr.sin_port = htons(nPort);

		int ret = ::bind(m_fdListen, (sockaddr *)&server_addr, sizeof(server_addr));
		if (ret < 0) {
			LOGE("Tcp bind[%d] ret=%d Error", nPort, ret);
			return 0;
		}
		LOGE("Tcp Server[%d] DDDDD", nPort);
		FD_ZERO(&m_fds);//init fds
        		FD_SET(m_fdListen, &m_fds); // add server socket to fds
		ret = ::listen(m_fdListen, 1);
		if (ret < 0) {
			LOGE("Tcp listen[%d] ret=%d Error", nPort, ret);
			return 0;
		}
		LOGE("Tcp Server[%d] EEEEE", nPort);
		ThreadStart(true);
		LOGE("Tcp Server[%d] OK", nPort);
		return 1;
	}

	void Stop() {
		ThreadStop();
	}
};


JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env;
    memset(&s_ctx, 0, sizeof(s_ctx));
	s_ctx.jVM = vm;

    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR; // JNI version not supported.
    }

    LOGE("JNI_OnLoad FindClass");
    //初始化对象
    jclass clz = env->FindClass("com/apowersoft/WXMedia/PCCastRecv");

    LOGE("JNI_OnLoad NewGlobalRef");
	s_ctx.jClz = (jclass) env->NewGlobalRef(clz);
    
    LOGE("JNI_OnLoad GetMethodID OnVideoData");
	s_ctx.cbData  = env->GetMethodID(clz,  "OnData",  "(II[BI)V");
    
    LOGE("JNI_OnLoad GetMethodID OnEvent");
	s_ctx.cbEvent = env->GetMethodID(clz,  "OnEvent",  "(II)V");
    
    LOGE("JNI_OnLoad OK---");
    
    signal(SIGPIPE, SIG_IGN);//避免某端退出的异常

    return JNI_VERSION_1_6;
}


extern "C" JNIEXPORT jlong JNICALL Java_com_apowersoft_WXMedia_PCCastRecv_Start(JNIEnv *env, jobject _obj, int nPort) {
	LOGE("%s ++++",__FUNCTION__);
	PCCastRecv *recv = new PCCastRecv;
	if (recv->Start(nPort)) {
		s_ctx.jObj = env->NewGlobalRef(_obj);//不能直接赋值(g_obj = obj)
		LOGE("%s [%d] +++++ OK",__FUNCTION__,nPort);
		return  (jlong)(void*)recv;
	}
	LOGE("%s -++++- Error",__FUNCTION__);
	delete recv;
	return 0;
}


extern "C" JNIEXPORT void JNICALL Java_com_apowersoft_WXMedia_PCCastRecv_Stop(JNIEnv *env, jobject _obj, jlong jHandle) {
	LOGE("%s ---",__FUNCTION__);
	if (jHandle) {
		void *ptr = (void *)jHandle;
		PCCastRecv *recv = (PCCastRecv*)ptr;
		recv->Stop();
		s_ctx.jObj = NULL;
		delete recv;
	}
}