/*
WXBase 基础库，基于C++11
*/
#ifndef _WX_BASE_H_
#define _WX_BASE_H_

#include  <stdint.h>
#include  <stdarg.h>
#include  <thread>
#include  <mutex>
#include  <queue>
#include  <fstream>

#ifdef _WIN32
#include <Windows.h>
#define  RENAME MoveFileA
#include <tchar.h>
#else
#define  RENAME rename
#define _T(x)       x
#define _TEXT(x)    x
using namespace std;
#endif

//递归锁
#define WXLocker    std::recursive_mutex

//自动锁
#define WXAutoLock  std::lock_guard<WXLocker>

//内存数据管理
class WXDataBuffer {
public:
	uint8_t *m_pBuf = nullptr;
	int     m_iBufSize = 0;
	int     m_iPos = 0;
	int64_t extra1 = 0;
	int64_t extra2 = 0;
public:
	void Init(uint8_t *buf, int size) {
		if (m_pBuf)delete[]m_pBuf;
		m_pBuf = new uint8_t[size];
		if (buf != nullptr) {
			memcpy(m_pBuf, buf, size);
		}else {
			memset(m_pBuf, 0, size);
		}
		m_iBufSize = size;
	}

	WXDataBuffer() {}

	WXDataBuffer(uint8_t *buf, int size) {
		Init(buf, size);
	}

	virtual ~WXDataBuffer() {
		if (m_pBuf) {
			delete[]m_pBuf;
			m_pBuf = nullptr;
			m_iBufSize = 0;
		}
	}
};

class WXString {
	std::string  m_strUTF8 = "";//ffmpeg 需要
	//Acsi
	void InitA(const char *sz) {
		m_strUTF8 = sz;
	}

public:
	void Format(const char * format, ...) {
		char    szMsg[4096];
		memset(szMsg, 0, 4096);
		va_list marker
#ifdef  _WIN32
        = nullptr
#endif
        ;
		va_start(marker, format);
		vsprintf(szMsg, format, marker);
		va_end(marker);
		InitA(szMsg);
	}

public:
	WXString() {}

	WXString(const WXString& wxstr) {
		InitA(wxstr.str());
	}
	const int  length() const {
		return (int)m_strUTF8.length();
	}
    

    const char* str()const  {
        return c_str();
    }
    const char* Left(int n) {
        return m_strUTF8.c_str() + (m_strUTF8.length() - n);
    }

  
	const char*  c_str() const {
		return m_strUTF8.c_str();
	}


public://WXString

	WXString& operator+=(const WXString& wxstr) {
		std::string wstr = m_strUTF8.c_str();
		wstr += wxstr.str();
		InitA(wstr.c_str());
		return *this;
	}
	bool operator==(const WXString& wxstr) const {
		const char* wsz1 = this->str();
		const char* wsz2 = wxstr.str();
		return (strcasecmp(wsz1, wsz2) == 0);
	}
	bool operator!=(const WXString& wxstr) const {
		const char* wsz1 = this->str();
		const char* wsz2 = wxstr.str();
		return (strcasecmp(wsz1, wsz2) != 0);
	}
};

//线程类
class WXThread {
	std::thread *m_thread = nullptr;
protected:
	volatile bool m_bThreadStop = true;
public:
	virtual  void ThreadPrepare() {}//线程循环前的初始化
	virtual  void ThreadProcess() = 0;//线程循环函数,必须实现
	virtual  void ThreadPost() {}//线程循环结束后的退出处理
public:
	bool ThreadStart() {
		if (!m_bThreadStop) return false;//已经启动
		m_bThreadStop = false;
		m_thread = new std::thread([this] {
			ThreadPrepare();
			while (!m_bThreadStop) {
				ThreadProcess();//加个异常处理
			}
			ThreadPost();
		});
		return true;
	}
	void ThreadStop() {
		m_bThreadStop = true;
		if (m_thread) {
			m_thread->join();
			delete m_thread;
			m_thread = nullptr;
		}
	}
};
//日志类
class WXLogInstance{
	WXLocker m_mutex;
	WXString m_strFileName;
	std::ofstream m_fout;
public:
	WXLogInstance() {}

	virtual ~WXLogInstance() {
		Close();
	}

	void Close() {
		WXAutoLock al(m_mutex);
		if (m_fout.is_open()) {
			m_fout.close();
		}
	}

	bool Open(const char* strFileName) {
		WXAutoLock al(m_mutex);
		Close();
		m_strFileName.Format(strFileName);
		m_fout.open(m_strFileName.c_str(), std::ios::app | std::ios::binary);
		return m_fout.is_open();
	}

    void Write(const char* szMsg) {
		WXAutoLock al(m_mutex);
		if (m_fout.is_open()) {
			m_fout.write(szMsg, sizeof(char)*strlen(szMsg));
		}
	}

	void Write(WXString strMsg) {
		WXAutoLock al(m_mutex);
		Write(strMsg.str());
	}
};

//单向队列
class WXFifo {
public:
	volatile int m_bEnable = 1;//是否可写
	WXDataBuffer m_dataBuffer;
	int64_t m_nTotalSize = 192000;
	int64_t m_nPosRead = 0;//读位置
	int64_t m_nPosWrite = 0;//写位置
public:
	inline int64_t Size() { return m_nPosWrite - m_nPosRead; }
public:
	WXFifo(int totolsize = 192000) {
		m_dataBuffer.Init(nullptr, totolsize);
		m_nTotalSize = totolsize;
	}

	void Init(int totolsize = 192000) {
		m_dataBuffer.Init(nullptr, totolsize);
		m_nTotalSize = totolsize;
	}

	void Reset() {
		m_nPosRead = 0;
		m_nPosWrite = 0;
		memset(m_dataBuffer.m_pBuf, 0, m_dataBuffer.m_iBufSize);
	}

	virtual ~WXFifo() {
		m_nPosRead = 0;
		m_nPosWrite = 0;
	}

	void Write(uint8_t *pBuf, int nSize) { //写数据
		if (m_bEnable) {
			int64_t newSize = m_nPosWrite - m_nPosRead + nSize;//可写区域
			if (newSize < m_nTotalSize) { //数据区可写
				int64_t posWirte = m_nPosWrite % m_nTotalSize;//实际写入位置
				int64_t posLeft = m_nTotalSize - posWirte;//在实际位置上buffer还剩多少可以写入的空间
				if (posLeft >= nSize) { //数据区足够
					memcpy(m_dataBuffer.m_pBuf + posWirte, pBuf, nSize);
				}else {  //有部分数据需要写到RingBuffer的头部
					memcpy(m_dataBuffer.m_pBuf + posWirte, pBuf, posLeft);//写到RingBuffer尾部
					memcpy(m_dataBuffer.m_pBuf, pBuf + posLeft, nSize - posLeft);//写到RingBuffer头部
				}
				m_nPosWrite += nSize;//更新写位置
			}
		}
	}

	int Read(uint8_t *pBuf, int nSize) {//读数据
		int64_t nowSize = m_nPosWrite - m_nPosRead;//可读区域
		if (nSize && nowSize >= nSize) { //数据足够读
			if (pBuf) {
				int64_t posRead = m_nPosRead % m_nTotalSize;//实际读取位置
				int64_t posLeft = m_nTotalSize - posRead;//在实际位置上buffer还剩多少可以写入的空间
				if (posLeft >= nSize) { //数据区足够
					memcpy(pBuf, m_dataBuffer.m_pBuf + posRead, nSize);
				}
				else {  //有部分数据需要写到RingBuffer的头部
					memcpy(pBuf, m_dataBuffer.m_pBuf + posRead, posLeft);//从尾部拷贝数据
					memcpy(pBuf + posLeft, m_dataBuffer.m_pBuf, nSize - posLeft);//从头部拷贝数据
				}
			}
			m_nPosRead += nSize;
			return nSize;
		}
		return 0;
	}

	int Read2(uint8_t *buf, int size) {//buf=nullptr Skip
		memset(buf, 0, size);
		int64_t nowSize = m_nPosWrite - m_nPosRead;//有效数据区
		if (nowSize >= size) {
			return Read(buf, size);
		}else if(nowSize > 0){
			return Read(buf, nowSize);
		}
		return 0;
	}
};

#endif
