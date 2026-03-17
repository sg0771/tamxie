/*
TCP发送端
*/

#include "WXTcp_impl.h"
#include "Opus_impl.h"

JniContext s_TcpClientCtx;
void   InitTcpClient() {

	return;//暂时不用这里的回调

	JNIEnv *env = Android_JNI_GetEnv();
	LOGE("InitTcpClient FindClass");
	jclass clz = env->FindClass("com/apowersoft/WXMedia/TcpClient");
	
	//可能不用
	LOGE("InitTcpClient GetMethodID OnData");
	s_TcpClientCtx.cbData = env->GetMethodID(clz, "OnData", "(I[B)V");  // void  OnData(int type, byte[] buf)

	//可能不用
	LOGE("InitTcpClient GetMethodID OnEvent");
	s_TcpClientCtx.cbEvent = env->GetMethodID(clz, "OnEvent", "(II)V");  // void  OnData(int type)
}


//同步TCP发送
class TcpClient {
	WXTcpClient m_tcpClient;
public:
	int  Connect(const char* strIP, int nPort) {
		return m_tcpClient.Connect(strIP, nPort, 0);
	}

	void Disconnect() {
		m_tcpClient.Disconnect();
	}

	void WriteData(uint8_t type, uint8_t* buf, int buf_size) {
		m_tcpClient.SendData(type, buf, buf_size);
	}
	
	void WriteAudioConfig(int nSampleRate,int nChannels){
		m_tcpClient.SendType(TYPE_AUDIO_SAMPLERATE, nChannels);
		m_tcpClient.SendType(TYPE_AUDIO_CHANNEL, nChannels);
	}
};

extern "C" JNIEXPORT jlong JNICALL Java_com_apowersoft_WXMedia_TcpClient_Connect
(JNIEnv *env, jobject _obj, jstring  strIP, jint  nPort) {
	TcpClient *obj = new TcpClient();
	char*   szIP = (char*)env->GetStringUTFChars(strIP, 0);
	int ret = obj->Connect(szIP, nPort);
	if (ret == 0) {
		delete obj;
		return 0;
	}
	s_TcpClientCtx.jObj = env->NewGlobalRef(_obj);//不能直接赋值(g_obj = obj)
	return (jlong)obj;
}

extern "C" JNIEXPORT void JNICALL Java_com_apowersoft_WXMedia_TcpClient_Disconnect(JNIEnv *env, jobject  _obj, jlong handler) {
	if (handler) {
		TcpClient * obj = (TcpClient *)handler;
		obj->Disconnect();
		delete obj;
		s_TcpClientCtx.jObj = 0;
	}
}

extern "C" JNIEXPORT void JNICALL Java_com_apowersoft_WXMedia_TcpClient_WriteH264
(JNIEnv *env, jobject _obj, jlong handler, jbyteArray  data, jint  data_size) {
	if (handler) {
		TcpClient * obj = (TcpClient *)handler;
		uint8_t *pData = (uint8_t *)(env->GetByteArrayElements(data, 0));
		obj->WriteData(TYPE_VIDEO_H264, pData, data_size);
		env->ReleaseByteArrayElements(data, (jbyte*)pData, 0);
	}
}

extern "C" JNIEXPORT void JNICALL Java_com_apowersoft_WXMedia_TcpClient_WriteH265
(JNIEnv *env, jobject _obj, jlong handler, jbyteArray  data, jint  data_size) {
	if (handler) {
		TcpClient * obj = (TcpClient *)handler;
		uint8_t *pData = (uint8_t *)(env->GetByteArrayElements(data, 0));
		obj->WriteData(TYPE_VIDEO_H265, pData, data_size);
		env->ReleaseByteArrayElements(data, (jbyte*)pData, 0);
	}
}

//发送音频参数，opus解码器初始化需要
extern "C" JNIEXPORT void JNICALL Java_com_apowersoft_WXMedia_TcpClient_WriteAudioConfig
(JNIEnv *env, jobject _obj, jlong handler, jint  sampleRate, jint  channels) {
	if (handler) {
		TcpClient * obj = (TcpClient *)handler;
		obj->WriteAudioConfig(sampleRate, channels);
	}
}


extern "C" JNIEXPORT void JNICALL Java_com_apowersoft_WXMedia_TcpClient_WriteAAC
(JNIEnv *env, jobject _obj, jlong handler, jbyteArray  data, jint  data_size) {
	if (handler) {
		TcpClient * obj = (TcpClient *)handler;
		uint8_t *pData = (uint8_t *)(env->GetByteArrayElements(data, 0));
		obj->WriteData(TYPE_AUDIO_AAC, pData, data_size);
		env->ReleaseByteArrayElements(data, (jbyte*)pData, 0);
	}
}

extern "C" JNIEXPORT void JNICALL Java_com_apowersoft_WXMedia_TcpClient_WriteOpus
(JNIEnv *env, jobject _obj, jlong handler, jbyteArray  data, jint  data_size) {
	if (handler) {
		TcpClient * obj = (TcpClient *)handler;
		uint8_t *pData = (uint8_t *)(env->GetByteArrayElements(data, 0));
		obj->WriteData(TYPE_AUDIO_OPUS, pData, data_size);
		env->ReleaseByteArrayElements(data, (jbyte*)pData, 0);
	}
}

