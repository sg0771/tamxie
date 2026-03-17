/*
TCP发送端
*/

#include "WXTcp.h"

//同步TCP发送
class TcpClient {
	WXLocker m_mutex;
public:
	struct sockaddr_in m_serverAddr;
	int m_sockfd = 0;
	int m_bConnect = 0;


	int  Connect(const char* strIP, int nPort) {
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

		//有数据就发，如果数据量大于默认缓冲区就分段发
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

	enum {TCP_SLICE = 1460};

	void WriteData(uint8_t type, uint8_t* buf, int buf_size) {
		WXAutoLock al(m_mutex);
		if (m_bConnect) {
			uint8_t tmp[4];
			tmp[0] = type;
			tmp[1] = (buf_size >> 16) & 0xff;
			tmp[2] = (buf_size >> 8) & 0xff;
			tmp[3] = (buf_size & 0xff);
			::send(m_sockfd, tmp, 4, 0);
			//::send(m_sockfd, buf, buf_size, 0);
			int nPos = 0;
			while (true) {
				int nextPos = nPos + TCP_SLICE;
				if (nextPos < buf_size) {
					::send(m_sockfd,buf + nPos, TCP_SLICE,0);
					nPos = nextPos;
				}else {
					::send(m_sockfd, buf + nPos, buf_size - nPos,0);
					break;
				}
			}
		}
	}
	
	void WriteAudioConfig(int nSampleRate,int nChannels){
		WXAutoLock al(m_mutex);
		if (m_bConnect) {
			uint8_t tmp[16];
			tmp[0] = TYPE_AUDIO_SAMPLERATE;
			tmp[1] = 0;
			tmp[2] = 0;
			tmp[3] = 4;
			tmp[4] = 0;
			tmp[5] = (nSampleRate >> 16) & 0xff;
			tmp[5] = (nSampleRate >> 8) & 0xff;
			tmp[7] = (nSampleRate >> 0) & 0xff;		
			
			tmp[8] = TYPE_AUDIO_CHANNEL;
			tmp[9] = 0;
			tmp[10] = 0;
			tmp[11] = 4;
			tmp[12] = 0;
			tmp[13] = 0;
			tmp[14] = (nChannels >> 8) & 0xff;
			tmp[15] = (nChannels >> 0) & 0xff;
			int ret = ::send(m_sockfd, tmp, 16, 0);
			if(ret < 0){
				LOGE("WriteAudioConfig ret = %d", ret);
			}
		}
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
	return (jlong)obj;
}

extern "C" JNIEXPORT void JNICALL Java_com_apowersoft_WXMedia_TcpClient_Disconnect(JNIEnv *env, jobject  _obj, jlong handler) {
	if (handler) {
		TcpClient * obj = (TcpClient *)handler;
		obj->Disconnect();
		delete obj;
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

