/*
TCP发送端
*/

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

#define   LOG_TAG  "TamTcp"
#define   LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)  

//递归锁
#define WXLocker    std::recursive_mutex

//自动锁
#define WXAutoLock  std::lock_guard<WXLocker>

//音视频数据包结构
//4个字节， 第一个字节表示数据类型
//2-4 字节表示数据长度，网络顺序
//主要类型
#define TYPE_VIDEO_H264 0x00
#define TYPE_VIDEO_H265 0x01

#define TYPE_AUDIO_PCM 0x10
#define TYPE_AUDIO_AAC 0x11

//同步TCP发送
class TcpClient {
	WXLocker m_mutex;
public:
	struct sockaddr_in m_serverAddr;
	int m_sockfd = 0;
	int m_bConnect = 0;
	TcpClient() {

	}
	virtual ~TcpClient() {
		Disconnect();
	}

	int Connect(const char* strIP, int nPort) {
		WXAutoLock al(m_mutex);
		LOGE("step1 Create Socket");
		m_sockfd = socket(AF_INET, SOCK_STREAM, 0);    //ipv4,TCP数据连接  
		if (m_sockfd < 0) {
			LOGE("Create socket error");
			return 0;
		}

		int bKeepAlive = 1;
		int ret = ::setsockopt(m_sockfd, SOL_SOCKET, SO_KEEPALIVE, (char*)&bKeepAlive, sizeof(bKeepAlive));
		LOGE("%s %d  Set SO_KEEPALIVE = %d", __FUNCTION__, __LINE__, ret);

		int enable = 1;
		ret = ::setsockopt(m_sockfd, IPPROTO_TCP, TCP_NODELAY, (const char*)&enable, sizeof(enable));
		LOGE("%s %d  Set TCP_NODELAY = %d", __FUNCTION__, __LINE__, ret);

		//服务器地址 
		memset(&m_serverAddr, 0, sizeof(struct sockaddr_in));
		m_serverAddr.sin_family = AF_INET;
		m_serverAddr.sin_port = htons(nPort);
		LOGE("step2 inet_pton %s:%d", strIP, nPort);
		if (inet_pton(AF_INET, strIP, &m_serverAddr.sin_addr) < 0) {    //设置ip地址  
			LOGE("Set IP Port error");
			close(m_sockfd);
			m_sockfd = 0;
			return 0;
		}

		int connfd = connect(m_sockfd, (struct sockaddr*)&m_serverAddr, sizeof(struct sockaddr_in)); //连接到服务器  
		LOGE("step3 Connert Server");
		if (connfd < 0) {
			LOGE("step4 Connert Server Error");
			close(m_sockfd);
			m_sockfd = 0;
			return 0;
		}

		LOGE("step4 Connert Server OK");
		m_bConnect = 1;
		return 1;
	}

	void Disconnect() {
		WXAutoLock al(m_mutex);
		if (m_sockfd && m_bConnect) {
			LOGE("WXTcp_Disconnect");
			close(m_sockfd);
			m_sockfd = 0;
			m_bConnect = 0;
		}
	}

	void WriteData(uint8_t type, uint8_t* buf, int buf_size) {
		WXAutoLock al(m_mutex);
		if (m_bConnect) {
			uint8_t tmp[4];
			tmp[0] = type;
			tmp[1] = (buf_size >> 16) & 0xff;
			tmp[2] = (buf_size >> 8) & 0xff;
			tmp[3] = (buf_size & 0xff);
			::send(m_sockfd, tmp, 4, 0);
			::send(m_sockfd, buf, buf_size, 0);
		}
	}
};

extern "C" JNIEXPORT jlong JNICALL Java_com_apowersoft_WXMedia_TcpClient_Start
(JNIEnv *env, jobject _obj, jstring  strIP, jint  nPort) {
	TcpClient *obj = new TcpClient();
	char*   szIP = (char*)env->GetStringUTFChars(strIP, 0);
	int ret = obj->Connect(szIP, nPort);
	if (ret == 0) {
		delete obj;
		return 0;
	}
	return (jlong)obj;
}

extern "C" JNIEXPORT void JNICALL Java_com_apowersoft_WXMedia_TcpClient_SendH264
(JNIEnv *env, jobject _obj, jlong handler, jbyteArray  data, jint  data_size) {
	if (handler) {
		TcpClient * obj = (TcpClient *)handler;
		uint8_t *pData = (uint8_t *)(env->GetByteArrayElements(data, 0));
		obj->WriteData(TYPE_VIDEO_H264, pData, data_size);
		env->ReleaseByteArrayElements(data, (jbyte*)pData, 0);
	}
}

extern "C" JNIEXPORT void JNICALL Java_com_apowersoft_WXMedia_TcpClient_SendH265
(JNIEnv *env, jobject _obj, jlong handler, jbyteArray  data, jint  data_size) {
	if (handler) {
		TcpClient * obj = (TcpClient *)handler;
		uint8_t *pData = (uint8_t *)(env->GetByteArrayElements(data, 0));
		obj->WriteData(TYPE_VIDEO_H265, pData, data_size);
		env->ReleaseByteArrayElements(data, (jbyte*)pData, 0);
	}
}

extern "C" JNIEXPORT void JNICALL Java_com_apowersoft_WXMedia_TcpClient_SendAAC
(JNIEnv *env, jobject _obj, jlong handler, jbyteArray  data, jint  data_size) {
	if (handler) {
		TcpClient * obj = (TcpClient *)handler;
		uint8_t *pData = (uint8_t *)(env->GetByteArrayElements(data, 0));
		obj->WriteData(TYPE_AUDIO_AAC, pData, data_size);
		env->ReleaseByteArrayElements(data, (jbyte*)pData, 0);
	}
}

extern "C" JNIEXPORT void JNICALL Java_com_apowersoft_WXMedia_TcpClient_Stop(JNIEnv *env, jobject  _obj, jlong handler) {
	if (handler) {
		TcpClient * obj = (TcpClient *)handler;
		obj->Disconnect();
	}
}

