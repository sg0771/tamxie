//-------------------------------------------
#include "WXTcp_impl.h"
#include "Opus_impl.h"


JniContext s_TcpServerCtx;

void   InitTcpServer() {
	JNIEnv *env = Android_JNI_GetEnv();
	memset(&s_TcpServerCtx, 0, sizeof(s_TcpServerCtx));

	LOGE("InitTcpServer FindClass");
	jclass clz = env->FindClass("com/apowersoft/WXMedia/TcpServer");
	LOGE("InitTcpServer GetMethodID OnData");
	s_TcpServerCtx.cbData = env->GetMethodID(clz, "OnData", "(I[B)V");  // void  OnData(int fd, int type, byte[] buf)
	LOGE("InitTcpServer GetMethodID OnEvent");
	s_TcpServerCtx.cbEvent = env->GetMethodID(clz, "OnEvent", "(II)V");  // void  OnData(int fd, int type)
}

void  OnWXTcpServer(void* ctx, int fd, int type, uint8_t* buf, int buf_size);
//接收服务端
class  TcpServer {
	WXTcpServer m_server;
public:
	void  OnData(int fd, int type, uint8_t* buf, int buf_size) {

	}
	void  OnEvent(int fd, int type) {

	}
public:
	int  Init(int nPort) {
		m_server.SetSink(this, OnWXTcpServer);
		return m_server.Init(nPort);
	}
	void CloseChannel(int fd) {  //关闭指定通道数据
		m_server.CloseChannel(fd);
	}
	void Deinit() {
		m_server.Deinit();
	}
};

void  OnWXTcpServer(void* ctx, int fd, int type, uint8_t* buf, int buf_size) {
	TcpServer* pThis = (TcpServer*)ctx;
	if (buf == NULL) {
		pThis->OnEvent(fd,type);
	}
	else {
		pThis->OnData(fd, type,buf, buf_size);
	}
}

extern "C" JNIEXPORT jlong JNICALL Java_com_apowersoft_WXMedia_TcpServer_Init
(JNIEnv *env, jobject _obj, int nPort) {
	LOGE("%s obj=%08x---",__FUNCTION__,(long)_obj);
	TcpServer *recv = new TcpServer;
	if (recv->Init(nPort)) {
		s_TcpServerCtx.jObj = env->NewGlobalRef(_obj);//不能直接赋值(g_obj = obj)
		return  (jlong)(void*)recv;
	}
	delete recv;
	return 0;
}

extern "C" JNIEXPORT void JNICALL Java_com_apowersoft_WXMedia_TcpServer_CloseChannel
(JNIEnv *env, jobject _obj, jlong jHandle, int fd) {
	LOGE("%s obj=%08x---",__FUNCTION__,(long)_obj);
	if (jHandle) {
		void *ptr = (void *)jHandle;
		TcpServer *recv = (TcpServer*)ptr;
		recv->CloseChannel(fd);
		delete recv;
	}
}

extern "C" JNIEXPORT void JNICALL Java_com_apowersoft_WXMedia_TcpServer_Deinit
(JNIEnv *env, jobject _obj, jlong jHandle) {
	LOGE("%s obj=%08x---",__FUNCTION__,(long)_obj);
	if (jHandle) {
		void *ptr = (void *)jHandle;
		TcpServer *recv = (TcpServer*)ptr;
		recv->Deinit();
		s_TcpServerCtx.jObj = NULL;
		delete recv;
	}
}