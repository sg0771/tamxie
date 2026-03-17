//-------------------------------------------

#include "WXTcp.h"

// jni上下文
typedef struct jni_context {
    JavaVM *jVM;//虚拟机
    jclass jClz;//类
    jobject jObj;
} JniContext;
static  JniContext g_ctx;

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env;
    memset(&g_ctx, 0, sizeof(g_ctx));

    g_ctx.jVM = vm;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR; // JNI version not supported.
    }

    //初始化对象
    jclass clz = env->FindClass("com/apowersoft/WXMedia/TcpServer");
    g_ctx.jClz = (jclass) env->NewGlobalRef(clz);
    jmethodID jMID = env->GetMethodID(g_ctx.jClz, "<init>", "()V");
    jobject handler = env->NewObject(g_ctx.jClz, jMID);
    g_ctx.jObj = env->NewGlobalRef(handler);

    return JNI_VERSION_1_6;
}

//获取env
JNIEnv *Android_JNI_GetEnv(void) {
    JNIEnv *env;
    int status = g_ctx.jVM->AttachCurrentThread(&env, NULL);
    if (status < 0) {
        LOGE("failed to attach current thread");
        return 0;
    }
    return env;
}

//协议层使用的回调函数，此处C++调用Java层方法，实现回调过程
static  void OnVideoData(const char *buf, int bufSize) {
    //LOGE("Android JniCallBack WXSLRecordOnWrite");
    JNIEnv *env = Android_JNI_GetEnv();
    jbyteArray jarrayBuf = env->NewByteArray(bufSize);
    if (jarrayBuf) {
        env->SetByteArrayRegion(jarrayBuf, 0, bufSize, (const jbyte *) buf);
    }
    jmethodID cbFunc = env->GetMethodID(g_ctx.jClz, "OnVideoData", "([BI)V");
    if (!cbFunc) {
        LOGE("Failed to retrieve OnVideoData() methodID @ line %d", __LINE__);
        return;
    }
    env->CallVoidMethod(g_ctx.jObj, cbFunc, jarrayBuf);
    if (jarrayBuf) {
        env->DeleteLocalRef(jarrayBuf);
    }
}


static  void OnAudioData(const char *buf, int bufSize) {
    //LOGE("Android JniCallBack OnAudioData");
    JNIEnv *env = Android_JNI_GetEnv();
    jbyteArray jarrayBuf = env->NewByteArray(bufSize);
    if (jarrayBuf) {
        env->SetByteArrayRegion(jarrayBuf, 0, bufSize, (const jbyte *) buf);
    }
    jmethodID cbFunc = env->GetMethodID(g_ctx.jClz, "OnAudioData", "([BI)V");
    if (!cbFunc) {
        LOGE("Failed to retrieve OnAudioData() methodID @ line %d", __LINE__);
        return;
    }
    env->CallVoidMethod(g_ctx.jObj, cbFunc, jarrayBuf);
    if (jarrayBuf) {
        env->DeleteLocalRef(jarrayBuf);
    }
}
//处理节点
class TcpServer;
class WTcpNode:public WXThread {
public:
	void DoData(int type, uint8_t *buf, int buf_size) {
		//回调Java方法
		if (type == TYPE_VIDEO_H264) {
			//回调视频数据 buf buf_size
			OnVideoData((const char*)buf,  buf_size);
		}else if (type == TYPE_AUDIO_OPUS) {
			//音频解码器
			uint8_t* bufPcm = nullptr;
			int pcm_size = 0;
			int ret = m_opusDecoder.DecodeFrame(buf, buf_size, &bufPcm, &pcm_size);
			if (ret) {
				OnAudioData((const char*)bufPcm,  pcm_size);
			}
		}
		else if (type == TYPE_AUDIO_SAMPLERATE) {
			m_nSampleRate = buf[1] * 256 * 256 + buf[2] * 256 + buf[3];
		}
		else if (type == TYPE_AUDIO_CHANNEL) {
			m_nChannel = buf[3];
			m_opusDecoder.Open(m_nSampleRate, m_nChannel);
		}
	}
public:
	int m_fd = 0;
	long m_uid;
	WXFifo  m_fifo;
	TcpServer *m_ctx = nullptr;

	WXOpusDecoder m_opusDecoder;
	int m_nSampleRate = 0;
	int m_nChannel = 0;

	WXDataBuffer m_bufData;

	int m_iType = 0;//数据类型
	int m_iBufSize = 0;//数据区长度

	void Open(TcpServer *ctx, int fd, long uid) {
		m_ctx = ctx;
		m_fd = fd;
		m_uid = uid;
		m_fifo.Init(1920*1080*5);
		m_bufData.Init(nullptr, 1920 * 1080);
		std::thread threadRecv(&WTcpNode::ThreadRecv, this);
		threadRecv.detach();
		ThreadStart(0);
	}
	void Close() {
		::close(m_fd);//关闭远端Socket
		ThreadStop();
		m_opusDecoder.Close();
	}
	void  ThreadRecv() {//数据接收线程
		while (!m_bThreadStop) {
			uint8_t buf[2048];
			int ret = ::recv(m_fd, (char*)buf, 2048, 0);
			if (ret < 0) {  //对方异常退出
				LOGE("threadRecv will be Stop!");
				m_bThreadStop = 1;
				break;
			}
			else if (ret > 0) { //OK
				m_fifo.Write(buf, ret);//接收数据，解析回调！
			}
			else if (ret == 0) {
				usleep(5000);
			}
		}
	}
	virtual  void ThreadProcess() { //输出处理线程函数
		if (m_fifo.Size() == 0) {
			usleep(5 * 1000);//暂停5ms
			return;
		}
		if (m_iBufSize == 0) { //重新解析新的一帧数据
			if (m_fifo.Size() < 4) {
				return;//数据区不足
			}
			uint8_t tmp[4];
			m_fifo.Read(tmp, 4);//获取4个字节
			m_iType = tmp[0]; //数据类型
			m_iBufSize = tmp[1] * 65536 + tmp[2] * 256 + tmp[3];//数据长度
		}
		if (m_iBufSize != 0) { //已经读取数据长度
			if (m_fifo.Size() >= m_iBufSize) {
				m_fifo.Read(m_bufData.m_pBuf, m_iBufSize); //实际数据
				//回调数据处理
				DoData(m_iType, m_bufData.m_pBuf, m_iBufSize);
				m_iBufSize = 0;//重置
			}
		}
	}
};


//接收服务端
class  TcpServer {
	int m_fdListen = 0;//监听socket
	int m_nPort = 0;
	std::map<long, WTcpNode*>m_mapHandle;//不同的接收节点
public:
	int  Init(int nPort) {
		m_nPort = nPort;
		m_fdListen = ::socket(PF_INET, SOCK_STREAM, 0);
		if (m_fdListen <= 0) {
			m_fdListen = 0;
			LOGE("Create socket error");
			return 0;
		}

		//TCP 服务 快速启动
		const int on = 1;
		::setsockopt(m_fdListen, SOL_SOCKET, SO_REUSEADDR, (const char*)&on, sizeof(on));

		//绑定本机端口
		struct sockaddr_in server_addr;
		//清空结构体
		memset(&server_addr, 0, sizeof(server_addr));
		server_addr.sin_family = PF_INET;
		//Bind to all address
		server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		//Convert port to network byte order
		server_addr.sin_port = htons(m_nPort);

		int ret = ::bind(m_fdListen, (sockaddr *)&server_addr, sizeof(server_addr));
		if (ret < 0) {
			LOGE("WXTcpRecv %s ::bind(m_sock, (sockaddr *)&server_addr, sizeof server_addr) ret=%d ", __FUNCTION__, ret);
			return 0;
		}

		ret = ::listen(m_fdListen, 1);
		if (ret < 0) {
			LOGE("WXTcpRecv %s Error ::listen(m_sock, 1) ret=%d ", __FUNCTION__, ret);
			return 0;
		}

		int enable = 1;
		ret = ::setsockopt(m_fdListen, IPPROTO_TCP, TCP_NODELAY, (const char*)&enable, sizeof(enable));
		if (ret < 0) {
			LOGE("WXTcpRecv %s Error ::setsockopt(m_sock, IPPROTO_TCP, TCP_NODELAY, (const char*)&enable, sizeof(enable)) ret=%d ", __FUNCTION__, ret);
			return 0;
		}

		//监听线程函数
		std::thread threadListen([this] {
			LOGE("Start TCP Listen Thread");
			while (true) {
				struct sockaddr_in client_addr;
				socklen_t cAddrLen = sizeof(client_addr);
				int client_fd = ::accept(m_fdListen, (struct sockaddr *)&client_addr, &cAddrLen);//新的客户端
				if (client_fd <= 0) {  //外部关闭监听线程m_fdListen
					break;
				}
				else { //新连接创建
					long uid = client_addr.sin_addr.s_addr;//区别符号
					WTcpNode *node = new WTcpNode;
					node->Open(this, client_fd, uid);
					m_mapHandle[uid] = node;
				}
			}
			LOGE("Start TCP Listen Thread");
			m_fdListen = 0;
		});
		threadListen.detach();
		return 1;
	}
	void CloseChannel(long uid) {  //关闭指定通道数据
		if (m_mapHandle.count(uid)) {
			m_mapHandle[uid]->Close();
			m_mapHandle[uid] = 0;
			m_mapHandle.erase(uid);
			LOGE("Stop TCP Recv [%ld] Thread", uid);
		}
	}
	void Deinit() {
		for (auto obj : m_mapHandle) {
			obj.second->Close();
			delete obj.second;
			m_mapHandle.erase(obj.first);
		}

		if (m_fdListen) {
			::close(m_fdListen);
			m_fdListen = 0;
			LOGE("::closesocket(m_sock)");
		}
	}
};


extern "C" JNIEXPORT jlong JNICALL Java_com_apowersoft_WXMedia_TcpServer_Init
(JNIEnv *env, jobject _obj, int nPort) {
	TcpServer *recv = new TcpServer;
	if (recv->Init(nPort)) {
		return  (jlong)(void*)recv;
	}
	delete recv;
	return 0;
}

extern "C" JNIEXPORT void JNICALL Java_com_apowersoft_WXMedia_TcpServer_CloseChannel
(JNIEnv *env, jobject _obj, jlong jHandle, jlong uid) {
	if (jHandle) {
		void *ptr = (void *)jHandle;
		TcpServer *recv = (TcpServer*)ptr;
		recv->CloseChannel(uid);
		delete recv;
	}
}

extern "C" JNIEXPORT void JNICALL Java_com_apowersoft_WXMedia_TcpServer_Deinit
(JNIEnv *env, jobject _obj, jlong jHandle) {
	if (jHandle) {
		void *ptr = (void *)jHandle;
		TcpServer *recv = (TcpServer*)ptr;
		recv->Deinit();
		delete recv;
	}
}