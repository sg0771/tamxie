/*
PC 投屏 SDK
*/


#include "WXBase.h"
#include <map>

WXMEDIA_API void WXTaskPost(WXTask task);


#include <dispatch/dispatch.h>

WXMEDIA_API void WXTaskPost(WXTask task){
        dispatch_sync(dispatch_get_main_queue(), ^{
            task();
        });
}
class  PCCastRecv : public WXThread {
	class TcpClient :public WXThread {
		SOCKET m_fd = INVALID_SOCKET;
		WXFifo m_fifo;
		std::thread* m_threadFunc = nullptr;
		void* m_pTsDemuxer = nullptr;

		std::ofstream m_fout;
		bool m_bTemp = false;
		WXString m_strName = L"";
		WXString m_strTemp = L"";
		PCCastRecv* m_ctx = nullptr;

		WXDataBuffer m_buf;

		void ThreadFunc() {
			bool bGetExtra = false;
            
            if(m_ctx && m_ctx->m_cbFunc)
                m_ctx->m_cbFunc(m_ctx->m_pSink, m_fd, TYPE_CONNECT, nullptr,0);
			while (!m_bThreadStop) {
				const int TS_LEN = 188;
				uint8_t buf[188];
				if (m_fifo.Size() >= TS_LEN) {
					m_fifo.Read(buf, TS_LEN);
					int ret = TSDemuxWriteData(m_pTsDemuxer, buf, TS_LEN);
					uint8_t* pOut = nullptr;
					int out_size = 0;
					if (ret == TS_TYPE_VIDEO) {
						if (!bGetExtra) { //SPS+PPS
							TSDemuxGetExtraData(m_pTsDemuxer, &pOut, &out_size);
							if (pOut!=nullptr && out_size != 0) {
								bGetExtra = true;
                               // printf("Recv H264 Ertra[%d]\r\n",out_size);
								//m_handler.OnH264Data(pOut, out_size);
                                if(m_ctx && m_ctx->m_cbFunc)
                                    m_ctx->m_cbFunc(m_ctx->m_pSink, m_fd, TS_TYPE_VIDEO, pOut, out_size);
							}
						}
						
						if(bGetExtra){ //正常的H264
							TSDemuxGetVideoData(m_pTsDemuxer, &pOut, &out_size);
                            if(out_size > 0){
                                
                               // printf("Recv H264 Data[%d]\r\n",out_size);
                                if(m_ctx && m_ctx->m_cbFunc)
                                    m_ctx->m_cbFunc(m_ctx->m_pSink, m_fd, TS_TYPE_VIDEO, pOut, out_size);
                            }
								//m_handler.OnH264Data(pOut, out_size);
						}
					}
					else if (ret == TS_TYPE_AUDIO) {
						TSDemuxGetAudioData(m_pTsDemuxer, &pOut, &out_size);
						//m_handler.OnAacData(pOut, out_size);
                        printf("Recv [%d] AAC Data[%d]\r\n",(int)m_fd, out_size);
                        if(m_ctx && m_ctx->m_cbFunc)
                            m_ctx->m_cbFunc(m_ctx->m_pSink, m_fd, TS_TYPE_AUDIO, pOut, out_size);
					}
				}
				else {
					usleep(1000);
				}
			}
            //onDisconnect
			//m_handler.Deinit();
            if(m_ctx && m_ctx->m_cbFunc)
                m_ctx->m_cbFunc(m_ctx->m_pSink, m_fd, TYPE_DISCONNECT, nullptr,0);
            
            PCCastRecv *ctx = m_ctx;
            SOCKET fd = m_fd;
            WXTask task = [ctx,fd]{
                if(ctx){
                    ctx->RemoveChannel(fd);
                }
            };
            WXTaskPost(task);
		}
	public:

		TcpClient(SOCKET fd, PCCastRecv* ctx) {
			m_ctx = ctx;
			m_fd = fd;
			m_buf.Init(nullptr, 16384);
			//m_handler.Init(fd, ctx);
			m_pTsDemuxer = TSDemuxCreate();
			ThreadStart(true);
		}

		virtual~TcpClient() {
			ThreadStop();
		}

		//线程循环前的初始化
		virtual  void ThreadPrepare() {
			m_threadFunc = new std::thread(&TcpClient::ThreadFunc, this);
		}

		void RecordStart(WXCTSTR wszName) {
			WXTask task = [this, wszName] {
				if (m_fout.is_open()) {
					m_fout.close();
					if (m_bTemp) {
						m_bTemp = false;
						//WXFfmpegConvertVideoFast(m_strTemp.str(), m_strName.str());
						//::DeleteFile(m_strTemp.str());
					}
				}
				m_strName = wszName;
                const char* wszExt = m_strName.Left(3);
				if (strcasecmp(wszExt, _T(".ts")) == 0) {
					m_bTemp = false;
					m_fout.open(wszName, std::iostream::binary);
				}
				else {
					m_strTemp = wszName;
					m_strTemp += _T(".ts");
					m_bTemp = true;
					m_fout.open(m_strTemp.str(), std::iostream::binary);
				}

			};
			RunTask(task);
		}

		void RecordStop() {

			WXTask task = [this] {
				if(m_fout.is_open())
					m_fout.close();

				//if (m_bTemp) {
					//m_bTemp = FALSE;
					//WXFfmpegConvertVideoFast(m_strTemp.str(), m_strName.str());
					//::DeleteFile(m_strTemp.str());
				//}
			};
			RunTask(task);

		}

		//线程循环函数,必须实现
		virtual  void ThreadProcess() {
			int length = ::recv(m_fd, (char*)m_buf.m_pBuf, (int)m_buf.m_iBufSize, 0);
			if (length < 0) {
				m_bThreadStop = true;
			}
			else if (length > 0) {
				m_fifo.Write(m_buf.m_pBuf, length);
				if (m_fout.is_open()) {
					m_fout.write((const char*)m_buf.m_pBuf, length);
				}
			}
			else if (length == 0) {
				if (errno == EINTR) {
					usleep(1000);
				}else {
					m_bThreadStop = true;
				}
			}
		}

		//线程循环结束后的退出处理
		virtual  void ThreadPost() {
			
			if (m_fout.is_open())
				m_fout.close();

			if (m_bTemp) {
				m_bTemp = false;
				//WXFfmpegConvertVideoFast(m_strTemp.str(), m_strName.str());
				//::DeleteFile(m_strTemp.str());
			}

			if (m_threadFunc) {
				m_threadFunc->join();
				delete m_threadFunc;
				m_threadFunc = nullptr;
			}

			PCCastRecv* ctx = m_ctx;
			SOCKET fd = m_fd;
			WXTask task = [ctx, fd] {
				ctx->RemoveChannel(fd);
			};
			WXTaskPost(task);

			if (m_fd != INVALID_SOCKET) {
				close(m_fd);
				m_fd = INVALID_SOCKET;
			}
			if (m_pTsDemuxer) {
				TSDemuxDestroy(m_pTsDemuxer);
				m_pTsDemuxer = nullptr;
			}
		}
	};
	std::map<SOCKET, TcpClient*>m_mapClinet;//接收服务端

	WXLocker m_lckTcp;
	SOCKET m_fdListen = 0;//监听socket

	//回调，申请窗口
	void* m_pSink = nullptr;
    PCCastOnData m_cbFunc = nullptr;

	virtual void ThreadProcess() { //监听事件
        
#ifdef _WIN32
		SOCKADDR_IN client_addr;
		int cAddrLen = sizeof(client_addr);
#else
        struct sockaddr client_addr;
        socklen_t cAddrLen = sizeof(struct sockaddr);
#endif
		SOCKET new_fd = ::accept(m_fdListen, (sockaddr*)&client_addr, &cAddrLen);
		if (new_fd != INVALID_SOCKET) {
			m_mapClinet[new_fd] = new TcpClient(new_fd, this);
		}
	}

	virtual void ThreadPost() {
		for (auto obj : m_mapClinet) {
			if (obj.first != 0 && obj.second != 0) {
				close(obj.first);
			}
			if (m_mapClinet[obj.first]) {
				m_mapClinet[obj.first]->ThreadStop();
				m_mapClinet.erase(obj.first);
			}
		}
	}

public:
	WXLocker m_mutex;
	int  Start(int nPort, void* pSink, PCCastOnData cbFunc) {
		WXAutoLock al(m_mutex);
		m_pSink = pSink;
		m_cbFunc = cbFunc;
		m_fdListen = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (m_fdListen == INVALID_SOCKET) {
			m_fdListen = 0;
			return 0;
		}
		int enable = 1;
		::setsockopt(m_fdListen, SOL_SOCKET, SO_REUSEADDR, (const char*)&enable, sizeof(enable));
		::setsockopt(m_fdListen, IPPROTO_TCP, TCP_NODELAY, (const char*)&enable, sizeof(enable));
        
        struct sockaddr_in server_addr;
        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = PF_INET;
        server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        server_addr.sin_port = htons((unsigned short)nPort);
        
		int ret = ::bind(m_fdListen, (sockaddr*)&server_addr, sizeof(server_addr));
		if (ret < 0) {
			return 0;
		}
		ret = ::listen(m_fdListen, 1);
		if (ret < 0) {
			return 0;
		}
		ThreadStart();
		return 1;
	}

	void Stop() {
		WXAutoLock al(m_mutex);
		if (m_fdListen) {
			close(m_fdListen);
		}
		ThreadStop();
	}

	//CallBack
	//关闭通道
	void RemoveChannel(SOCKET fd) {
		WXAutoLock al(m_mutex);
		if (m_mapClinet.count(fd)) {
			TcpClient* client = m_mapClinet[fd];
			client->ThreadStop();
			m_mapClinet.erase(fd);
			delete client;
		}
	}

	void RecordStart(SOCKET fd, WXCTSTR wszName) {
		WXAutoLock al(m_mutex);
		if (m_mapClinet.count(fd)) {
			m_mapClinet[fd]->RecordStart(wszName);
		}
	}
	void RecordStop(SOCKET fd) {
		WXAutoLock al(m_mutex);
		if (m_mapClinet.count(fd)) {
			m_mapClinet[fd]->RecordStop();
		}
	}
};

//-------------------  WXAPI ----------------------------
WXMEDIA_API void  PCCastInit(const wchar_t* logfile) {

}

WXMEDIA_API void  PCCastRecvRecordStart(void* ptr, uint64_t uid, WXCTSTR wszName) {
	if (ptr) {
		PCCastRecv* obj = (PCCastRecv*)ptr;
		obj->RecordStart(uid, wszName);
	}
}

WXMEDIA_API void  PCCastRecvRecordStop(void* ptr, uint64_t uid) {
	if (ptr) {
		PCCastRecv* obj = (PCCastRecv*)ptr;
		obj->RecordStop(uid);
	}
}

WXMEDIA_API void* PCCastRecvStart(int nPort, void* pSink, PCCastOnData cbSize) {
	PCCastRecv* obj = new PCCastRecv;
	if (!obj->Start(nPort, pSink, cbSize)) {
		delete obj;
		return nullptr;
	}
	return (void*)obj;
}

WXMEDIA_API void  PCCastRecvStop(void* ptr) {
	if (ptr) {
		PCCastRecv* obj = (PCCastRecv*)ptr;
		obj->Stop();
	}
}
