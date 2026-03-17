/*
基于libevent 和 C++11 的 TCP 服务端和客户端
*/

#ifndef _WXTCP_IMPL_H_
#define _WXTCP_IMPL_H_

#include <WXBase.h>
#ifdef _WIN32
#include <winsock2.h>
#include <Windows.h>
#endif
#define _EVENT_HAVE_PTHREADS
#include <event2/event.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/thread.h>

#ifdef ANDROID
#include <jni.h>
#define   LOG_TAG  "TamTcp"
#define   LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)  
#include <android/log.h>  
#include <linux/tcp.h>


#include <string.h>
#include <unistd.h>

#include <sys/socket.h>  
#include <netinet/in.h> 
#include <arpa/inet.h>

typedef void(*OnClientEvent)(void*, int type);
typedef void(*OnClientData)(void*, uint8_t*, int);
typedef void(*OnServerEvent)(void*, uint64_t uid, int type);//消息回调
typedef void(*OnServerData)(void*, uint64_t uid, uint8_t*, int);//数据回调


JNIEnv *Android_JNI_GetEnv(void);
void   InitTcpClient();
void   InitTcpServer();

// jni上下文
typedef struct jni_context {
	jobject jObj;//对象
	jmethodID cbData;//数据回调
	jmethodID cbEvent;//消息回调，主要是远端关闭导致的本端关闭
} JniContext;
extern  JniContext s_TcpServerCtx;
extern  JniContext s_TcpClientCtx;
#endif

//音视频数据包结构
//4个字节， 第一个字节表示数据类型
//2-4 字节表示数据长度，网络顺序
//主要类型
#define TYPE_VIDEO_H264 0x00
#define TYPE_VIDEO_H265 0x01
#define TYPE_VIDEO_VP8  0x02  //声网直播？

//视频参数
#define TYPE_VIDEO_WIDTH   0x10  
#define TYPE_VIDEO_HEIGHT  0x11  
#define TYPE_VIDEO_FPS     0x12  


//音频参数
#define TYPE_AUDIO_SAMPLERATE   0x70
#define TYPE_AUDIO_CHANNEL      0x71

#define TYPE_AUDIO_PCM   0x80
#define TYPE_AUDIO_AAC   0x81
#define TYPE_AUDIO_OPUS  0x82 //声网音频格式

#define STAT_CONNECT     0x90  //连接成功
#define STAT_DISCONNECT  0x91   //连接断开 
//Tcp 回调，buf=NULL,buf_szie=0 表示消息回调
//fd 不为零表示接收端指定fd的回调
typedef void(*OnWXTcp)(void* ctx, int fd, int type, uint8_t* buf, int buf_size);

static void TcpClient_socket_event_cb(bufferevent *bev, short events, void *arg);//事件回调
static void TcpClient_socket_read_cb(bufferevent *bev, void *arg);//数据回调

class WXTcpClient :public WXThread {

	WXLocker m_mutex;
	event_base *m_base = NULL;
	void *m_pSink = nullptr;//回调对象
	OnWXTcp  m_cb = nullptr;

	int m_fd = 0;
	bufferevent *m_pBev = nullptr;
	std::thread *m_threadTcp = nullptr;

	//接收
	int  m_bUseRecv = 0;
	WXFifo m_fifo;
	int m_iType = 0;
	int m_iBufSize = 0;
	WXDataBuffer m_bufData;

public:
	virtual void ThreadProcess() {
		if (m_fifo.Size() == 0)
			SleepMs(10);

		uint8_t tmp[4];
		while (!m_bThreadStop) {
			if (m_iBufSize == 0) { //重新解析新的一帧数据
				if (m_fifo.Size() < 4) {
					return;//数据区不足
				}
				m_fifo.Read(tmp, 4);//获取4个字节
				m_iType = tmp[0];
				m_iBufSize = tmp[1] * 65536 + tmp[2] * 256 + tmp[3];
			}
			if (m_iBufSize != 0) {
				if (m_fifo.Size() < m_iBufSize) {
					SleepMs(10);
					return;//数据区不足
				}
				m_fifo.Read(m_bufData.m_pBuf, m_iBufSize);
				if (this->m_cb) {
					this->m_cb(this->m_pSink, m_fd, m_iType, m_bufData.m_pBuf, m_iBufSize);//数据回调出去
				}
				m_iBufSize = 0;//重置
				continue;
			}
		}
	}
	virtual void ThreadTcp() {
		//StopImpl 解开
		event_base_dispatch(m_base);//堵塞在这里！，直到外部 event_base_loopbreak

		{
			WXAutoLock al(m_mutex);
			//清理
			if (m_pBev) {
				bufferevent_free(m_pBev);
				m_pBev = nullptr;
			}
			if (m_base) {
				event_base_free(m_base);
				m_base = NULL;
			}
		}
		if (this->m_cb)
			this->m_cb(this->m_pSink, 0, STAT_DISCONNECT, NULL, 0);
	}
	void StopImpl() {
		{
			WXAutoLock al(m_mutex);
			if (m_base) {
				event_base_loopbreak(m_base);//发送断开指令
			}	
		}

		if (m_threadTcp) {
			m_threadTcp->join();
			delete m_threadTcp;
			m_threadTcp = NULL;
		}

		if(m_bUseRecv)
			ThreadStop();//退出接收线程
	}

	//TCP 发送
	enum { TCP_SLICE = 1460 };
	void SendRaw(uint8_t *buf, int buf_size) {
		WXAutoLock al(m_mutex);
		//分块1460 发送
		if (m_pBev) {
			//bufferevent_write(m_pBev, buf, buf_size);
			int nPos = 0;
			while (true) {
				int nextPos = nPos + TCP_SLICE;
				if (nextPos < buf_size) {
					bufferevent_write(m_pBev, buf + nPos, TCP_SLICE);
					nPos = nextPos;
				}
				else {
					bufferevent_write(m_pBev, buf + nPos, buf_size - nPos);
					break;
				}
			}
		}
	}

public:
	//消息回调
	void _socket_event_cb(bufferevent *bev, short events) {
		bool bStop = false;
		if (events & BEV_EVENT_EOF) {
			bStop = true;
		}else if (events & BEV_EVENT_ERROR) {
			bStop = true;
		}else if (events & BEV_EVENT_READING) {
			bStop = true;
		}else if (events & BEV_EVENT_WRITING) {
			bStop = true;
		}if (bStop) {
			this->StopImpl();
		}
	}

	//数据回调
	void _socket_read_cb(bufferevent *bev) {
		uint8_t RecvBuf[4096];
		memset(RecvBuf, 0, 4096);
		size_t RecvLength = bufferevent_read(bev, RecvBuf, 4096);
		if (RecvLength) {
			m_fifo.Write(RecvBuf, RecvLength);
		}
	}

public:

	void SetSink(void* pSink, OnWXTcp cb) {
		WXAutoLock al(m_mutex);
		m_pSink = pSink;
		m_cb = cb;
	}

	int  Connect(WXCTSTR strIP, int nPort, int bRecv = 0) { //注意加锁
		WXAutoLock al(m_mutex);
		#ifdef _WIN32
				evthread_use_windows_threads();
		#else
				evthread_use_pthreads();
		#endif
		m_base = event_base_new();
		evthread_make_base_notifiable(m_base);

		// set TCP_NODELAY to let data arrive at the server side quickly
		evutil_socket_t fd = ::socket(AF_INET, SOCK_STREAM, 0);
		int bKeepAlive = 1;
		::setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (const char*)&bKeepAlive, sizeof(bKeepAlive));

		int enable = 1;
		::setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (const char*)&enable, sizeof(enable));

		m_pBev = bufferevent_socket_new(m_base, fd, BEV_OPT_CLOSE_ON_FREE | BEV_OPT_THREADSAFE);
		bufferevent_setcb(m_pBev, TcpClient_socket_read_cb, NULL, TcpClient_socket_event_cb, this); // For client, we don't need callback function
		bufferevent_enable(m_pBev, EV_READ | EV_PERSIST);

		struct sockaddr_in m_address;
		memset(&m_address, 0, sizeof(m_address));
		m_address.sin_family = AF_INET;
#ifdef WIN32
		WXString wstrIP = strIP;
#else
		WXString wstrIP;
		wstrIP.Format("%s",strIP);
#endif
		const char*szIP = wstrIP.c_str();
		m_address.sin_addr.s_addr = inet_addr(szIP);
		m_address.sin_port = htons(nPort);
		int ret = bufferevent_socket_connect(m_pBev, (struct sockaddr*)&m_address, sizeof(struct  sockaddr_in));//连接对方IP+Port
		if (ret != 0) { //连不上服务器
			if (m_pBev) {
				bufferevent_free(m_pBev);
				m_pBev = nullptr;
			}

			if (m_base) {
				event_base_free(m_base);
				m_base = NULL;
			}
			return 0;
		}
		else { //连上服务器
			m_fd = (int)fd;
			
			if (bRecv) {
				m_bUseRecv = bRecv;
				ThreadStart();//接收数据的线程
			}

			m_threadTcp = new std::thread(&WXTcpClient::ThreadTcp,this);
		}
		return 1;
	}

	void Disconnect() { //注意加锁
		StopImpl();//中断发送线程
	}

	//发送参数
	void SendType(int type, int value) {
		WXAutoLock al(m_mutex);
		uint8_t tmp[8];
		tmp[0] = type;
		tmp[1] = 0;
		tmp[2] = 0;
		tmp[3] = 4;
		tmp[4] = (value >> 24) & 0xff;
		tmp[5] = (value >> 16) & 0xff;
		tmp[6] = (value >> 8) & 0xff;
		tmp[7] = (value & 0xff);
		SendRaw(tmp, 8);
	}

	void SendData(int type, uint8_t *buf, int buf_size) {
		WXAutoLock al(m_mutex);
		uint8_t tmp[4];
		tmp[0] = type;
		tmp[1] = (buf_size >> 16) & 0xff;
		tmp[2] = (buf_size >> 8) & 0xff;
		tmp[3] = (buf_size & 0xff);
		SendRaw(tmp, 4);
		SendRaw(buf, buf_size);
	}
};

//连接状态回调函数
static void TcpClient_socket_event_cb(bufferevent *bev, short events, void *arg) {
	WXTcpClient *obj = (WXTcpClient *)arg;
	obj->_socket_event_cb(bev, events);
}

//读数据回调函数
static void TcpClient_socket_read_cb(bufferevent *bev, void *arg) {
	WXTcpClient *obj = (WXTcpClient *)arg;
	obj->_socket_read_cb(bev);
}

//------------------------------------------------------------------------------------------
static void WXTcpServer_socket_read_cb(bufferevent *bev, void *arg);
static void WXTcpServer_socket_event_cb(bufferevent *bev, short events, void *arg);
static void WXTcpServer_listener_cb(evconnlistener *listener, evutil_socket_t fd, sockaddr *sock, int socklen, void *arg);

class WXTcpServer :public WXThread {
public:
	//连接通道
	class WXTcpChannel : public WXThread {
		WXLocker m_mutex;
		int m_fd = 0;
		bufferevent *m_pBev = nullptr;
		WXTcpServer *m_pServer = nullptr;
		WXFifo m_fifo;
		int m_iType = 0;
		int m_iBufSize = 0;
		WXDataBuffer m_bufData;

	public:
		virtual void ThreadProcess() {
			if (m_fifo.Size() == 0)
				SleepMs(10);

			uint8_t tmp[4];
			while (!m_bThreadStop) {
				if (m_iBufSize == 0) { //重新解析新的一帧数据
					if (m_fifo.Size() < 4) {
						return;//数据区不足
					}
					m_fifo.Read(tmp, 4);//获取4个字节
					m_iType = tmp[0];
					m_iBufSize = tmp[1] * 65536 + tmp[2] * 256 + tmp[3];
				}
				if (m_iBufSize != 0) {
					if (m_fifo.Size() < m_iBufSize) {
						SleepMs(10);
						return;//数据区不足
					}
					m_fifo.Read(m_bufData.m_pBuf, m_iBufSize);
					if (this->m_pServer && m_pServer->m_cb) {
						m_pServer->m_cb(m_pServer->m_pSink, m_fd, m_iType, m_bufData.m_pBuf, m_iBufSize);//数据回调出去
					}
					m_iBufSize = 0;//重置
					continue;
				}
			}
		}
	public:
		//读数据回调函数
		void _socket_read_cb(bufferevent *bev) {
			uint8_t RecvBuf[4096];
			memset(RecvBuf, 0, 4096);
			size_t RecvLength = bufferevent_read(bev, RecvBuf, 4096);//len
			if (RecvLength) {
				m_fifo.Write(RecvBuf, RecvLength);
			}
		}

		//事件回调函数
		void _socket_event_cb(bufferevent *bev, short events) {
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
			if (bStop) { //通知上层，关闭指定通道
				WXAutoLock al(m_mutex);
				if (this->m_pServer && m_pServer->m_cb) {
					m_pServer->m_cb(m_pServer->m_pSink, m_fd, STAT_DISCONNECT, NULL, 0);//数据回调出去
				}
			}
		}

	public:

		void Close() {
			WXAutoLock al(m_mutex);
			ThreadStop();
			if (m_pBev) {
				bufferevent_free(m_pBev);
				m_pBev = nullptr;
			}
		}

		//发送多媒体数据
		void SendData(int type, uint8_t *data, int data_size) { //发送数据
			WXAutoLock al(m_mutex);
			if (m_pBev) {
				uint8_t tmp[4];
				tmp[0] = type;
				tmp[1] = (data_size >> 16) & 0xff;
				tmp[2] = (data_size >> 8) & 0xff;
				tmp[3] = (data_size & 0xff);
				bufferevent_write(m_pBev, tmp, 4);
				bufferevent_write(m_pBev, data, data_size);
			}
		}

		void Open(int fd, WXTcpServer *server, bufferevent *bev) {
			WXAutoLock al(m_mutex);
			m_pBev = bev;
			m_pServer = server;
			m_fd = fd;
			m_fifo.Init(1920 * 1080 * 5);
			m_bufData.Init(nullptr, 1920 * 1080);
			ThreadStart();
		}
	};
public:
	WXLocker m_mutex;

	event_base *m_base = NULL;
	evconnlistener *m_listener = NULL;

	void *m_pSink = nullptr;  //数据回调对象
	OnWXTcp  m_cb = nullptr; //数据通道数据回调函数

	std::map<int, WXTcpChannel*>m_mapChannel;//TCP 通道处理

	void cbListen(evconnlistener *listener, evutil_socket_t fd, sockaddr *sock, int socklen) {
		int enable = 1;
		::setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (const char*)&enable, sizeof(enable));

		//sockaddr_in *sin = (sockaddr_in *)sock;
		//char *szIP = inet_ntoa(sin->sin_addr);

		WXTcpChannel *Channel = new WXTcpChannel;

		bufferevent *pBufEvent = bufferevent_socket_new(this->m_base, fd, BEV_OPT_CLOSE_ON_FREE | BEV_OPT_THREADSAFE);
		bufferevent_setcb(pBufEvent, WXTcpServer_socket_read_cb, NULL, WXTcpServer_socket_event_cb, Channel);
		bufferevent_enable(pBufEvent, EV_READ | EV_PERSIST);

		int ufd = (int)fd;
		Channel->Open(ufd, this, pBufEvent);

		if (m_cb) {
			m_cb(m_pSink, ufd, STAT_CONNECT, NULL, 0);//连接A
		}

		WXAutoLock al(this->m_mutex);
		this->m_mapChannel[ufd] = Channel;//Map channel
	}

public:
	virtual  void ThreadPrepare() {
		//WXLogWriteNewW(L"WXTcpServer %ws Begin", __FUNCTIONW__);
		event_base_dispatch(m_base);//监听连接
	}//线程循环前的初始化

	virtual  void ThreadProcess() {
		//--------------------------
	}

	virtual  void ThreadPost() {
		//断开连接
		if (m_listener) {
			evconnlistener_free(m_listener);
			m_listener = NULL;
		}

		if (m_base) {
			event_base_free(m_base);
			m_base = NULL;
		}
		//WXLogWriteNewW(L"WXTcpServer %ws End", __FUNCTIONW__);
	}//线程循环结束后的退出处理
public:
	void SetSink(void *pSink, OnWXTcp cb) {
		WXAutoLock al(m_mutex);
		m_pSink = pSink;
		m_cb = cb;
	}

	//启动监听端口
	int  Init(int nPort) {
		WXAutoLock al(m_mutex);
		//WXLogWriteNewW(L"WXTcpServer %ws", __FUNCTIONW__);
		sockaddr_in sin;
		memset(&sin, 0, sizeof(sockaddr_in));
		sin.sin_family = AF_INET;
		sin.sin_port = htons(nPort);
		#ifdef _WIN32
				evthread_use_windows_threads();
		#else
				evthread_use_pthreads();
		#endif
		m_base = event_base_new();
		evthread_make_base_notifiable(m_base);
		m_listener = evconnlistener_new_bind(m_base,
			WXTcpServer_listener_cb, this,
			LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE,
			10, (sockaddr*)&sin, sizeof(sockaddr_in)); //连接回调
		if (m_listener == NULL) { //失败了
			event_base_free(m_base);
			m_base = NULL;
			return 0;
		}
		ThreadStart();
		return 1;
	}

	//关闭所有连接，关闭监听端口
	void Deinit() {
		WXAutoLock al(m_mutex);
		//WXLogWriteNewW(L"WXTcpServer %ws", __FUNCTIONW__);
		if (m_base) {
			event_base_loopbreak(m_base);
			ThreadStop();
		}
	}

	//主动关闭某个通道
	void CloseChannel(int uid) {
		WXAutoLock al(m_mutex);
		//WXLogWriteNewW(L"WXTcpServer %ws", __FUNCTIONW__);
		if (m_mapChannel.count(uid) != 0) {
			WXTcpChannel *Channel = m_mapChannel[uid];
			Channel->Close();
			delete Channel;
			m_mapChannel.erase(uid);
		}
	}
};

//读数据回调函数
static void WXTcpServer_socket_read_cb(bufferevent *bev, void *channel) {
	WXTcpServer::WXTcpChannel *obj = (WXTcpServer::WXTcpChannel *)channel;
	obj->_socket_read_cb(bev);
}

//事件回调函数
static void WXTcpServer_socket_event_cb(bufferevent *bev, short events, void *channel) {
	WXTcpServer::WXTcpChannel *obj = (WXTcpServer::WXTcpChannel *)channel;
	obj->_socket_event_cb(bev, events);
}

//监听回调函数
//收到新的socket链接
static void WXTcpServer_listener_cb(evconnlistener *listener, evutil_socket_t fd, sockaddr *sock, int socklen, void *server) {
	WXTcpServer *pServer = (WXTcpServer *)server;
	pServer->cbListen(listener, fd, sock, socklen);
}

#endif //_WXTCP_IMPL_H_
