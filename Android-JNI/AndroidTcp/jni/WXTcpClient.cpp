/*
基于libevent2 的 TCP发送端
*/
#include <jni.h>
#include "WXBase.h"
#include <event2/event.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/thread.h>
#include <linux/in.h>
#include <linux/tcp.h>
#include <sys/endian.h>
#include <arpa/inet.h>

static void socket_event_cb(bufferevent *bev, short events, void *arg);
static void socket_read_cb(bufferevent *bev, void *arg);

class WXTcpClient {
public:
	WXLocker m_mutex;
	struct event_base* m_base = NULL;
	struct bufferevent* m_bev = NULL;
	int   m_bConnect = 0;
	std::thread *m_thread = NULL;
public:
	virtual ~WXTcpClient() { Stop(); }
	WXTcpClient() {
		//m_dataBuf.Init(NULL, 255);
	}
	//------------------------------------------------------------
	int Start(const char *szIP, int nPort) { //注意加锁
		WXAutoLock al(m_mutex);

		evthread_use_pthreads();

		m_base = event_base_new();
		evthread_make_base_notifiable(m_base);

		// set TCP_NODELAY to let data arrive at the server side quickly
		evutil_socket_t fd = socket(AF_INET, SOCK_STREAM, 0);
		int bKeepAlive = 1;
		::setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (char*)&bKeepAlive, sizeof(bKeepAlive));
		int enable = 1;
		::setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (const char*)&enable, sizeof(enable));

		m_bev = bufferevent_socket_new(m_base, fd, BEV_OPT_CLOSE_ON_FREE | BEV_OPT_THREADSAFE);
		bufferevent_setcb(m_bev, socket_read_cb, NULL, socket_event_cb, this); // For client, we don't need callback function
		bufferevent_enable(m_bev, EV_READ | EV_PERSIST);

		struct sockaddr_in m_address;
		memset(&m_address, 0, sizeof(m_address));
		m_address.sin_family = AF_INET;
		m_address.sin_addr.s_addr = inet_addr(szIP);
		m_address.sin_port = htons(nPort);
		int ret = bufferevent_socket_connect(m_bev, (struct sockaddr*)&m_address, sizeof(struct sockaddr_in));
		if (ret != 0) {
			if (m_bev) {
				bufferevent_free(m_bev);
				m_bev = nullptr;
			}
			if (m_base) {
				event_base_free(m_base);
				m_base = NULL;
			}
			return 0;
		}
		else {
			//连接成功的回调！
			m_bConnect = 1;//没有消息回调就强制通行
			m_thread = new std::thread([this] {
				event_base_dispatch(m_base);
				if (m_bev) {
					bufferevent_free(m_bev);
					m_bev = nullptr;
				}
				if (m_base) {
					event_base_free(m_base);
					m_base = NULL;
				}
			});
		}
		return 1;
	}

	//发送多媒体数据
	void WriteData(const void *data, int data_size) { //注意加锁
		WXAutoLock al(m_mutex);
		if (m_base && m_bConnect) {
			bufferevent_write(m_bev, &data_size, 4); //发送多媒体数据
			bufferevent_write(m_bev, data, data_size); //发送多媒体数据
		}
	}

	void Stop() { //注意加锁
		WXAutoLock al(m_mutex);
		if (m_base) {
			event_base_loopbreak(m_base);
			if(m_thread){
				m_thread->join();
				delete m_thread;
				m_thread = NULL;
			}
		}
	}
};

//连接状态回调函数
static void socket_event_cb(bufferevent *bev, short events, void *arg) {
	WXTcpClient *sev = (WXTcpClient *)arg;
	if (sev->m_bev == bev) {
		bool bStop = false;
		if (events & BEV_EVENT_EOF) {
			bStop = true;
		}
		else if (events & BEV_EVENT_ERROR) {
			bStop = true;
		}
		else if (events & BEV_EVENT_READING) {
			bStop = true;
		}
		else if (events & BEV_EVENT_WRITING) {
			bStop = true;
		}
		if (bStop) {
			bufferevent_free(sev->m_bev);
			sev->m_bev = nullptr;
		}
	}
}

//读数据回调函数
//主要处理文字信息
static void socket_read_cb(bufferevent *bev, void *arg) {

}

// ---------------------------------------------------------------------------------------
extern "C" JNIEXPORT jlong JNICALL Java_com_apowersoft_WXMedia_WXTcp_Start
  (JNIEnv *env, jobject obj, jstring  strIP, jint  nPort){
  	WXTcpClient *client = new WXTcpClient;
  	char*   szIP   =   (char*)env->GetStringUTFChars(strIP,0);  
	int bRet = client->Start(szIP, nPort);
	if (!bRet) {
		delete client;
		return 0;
	}
	return (jlong)client;
  }

extern "C" JNIEXPORT void JNICALL Java_com_apowersoft_WXMedia_WXTcp_WriteData
  (JNIEnv *env, jobject obj, jlong handler, jbyteArray  data, jint  data_size){
    	if (handler) {
		WXTcpClient* client = (WXTcpClient*)handler;
		uint8_t *pData = (uint8_t *)(env->GetByteArrayElements(data, 0));
		client->WriteData(pData, data_size);
		env->ReleaseByteArrayElements(data, (jbyte*)pData, 0);
	}
  }

extern "C" JNIEXPORT void JNICALL Java_com_apowersoft_WXMedia_WXTcp_Stop
  (JNIEnv *env, jobject  obj, jlong handler){
  	if (handler) {
		WXTcpClient* client = (WXTcpClient*)handler;
		client->Stop();
		delete client;
	}
  }

