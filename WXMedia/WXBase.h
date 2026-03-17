/*
WXTcpBase 基础库，基于C++11
*/
#ifndef __WX_BASE_H_
#define __WX_BASE_H_

#include <windows.h>
#include <stdint.h>
#include <stdarg.h>
#include <thread>
#include <mutex>
#include <queue>
#include <fstream>
#include <tchar.h>
#include <wchar.h>

//递归锁
#define WXTcpLocker    std::recursive_mutex

//自动锁
#define WXTcpAutoLock std::lock_guard<WXTcpLocker>

//内存数据管理
class WXTcpDataBuffer {
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
		}
		else {
			memset(m_pBuf, 0, size);
		}
		m_iBufSize = size;
	}

	WXTcpDataBuffer() {}

	WXTcpDataBuffer(uint8_t *buf, int size) {
		Init(buf, size);
	}

	virtual ~WXTcpDataBuffer() {
		if (m_pBuf) {
			delete[]m_pBuf;
			m_pBuf = nullptr;
			m_iBufSize = 0;
		}
	}
};

//线程类
class WXTcpThread {
	std::thread *m_thread = nullptr;
protected:
	volatile bool m_bThreadStop = true;
public:
	void Run() {
		ThreadPrepare();
		while (!m_bThreadStop) {
			ThreadProcess();//加个异常处理
		}
		ThreadPost();
	}

	bool ThreadStart() {
		if (!m_bThreadStop) return false;//已经启动
		m_bThreadStop = false;
		m_thread = new std::thread(&WXTcpThread::Run, this);
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
	inline bool Processing() { return !m_bThreadStop; }
public:
	virtual  void ThreadPrepare() {}//线程循环前的初始化
	virtual  void ThreadProcess() = 0;//线程循环函数
	virtual  void ThreadPost() {}//线程循环结束后的退出处理
};

//日志类
class WXTcpLogInstance {
	WXTcpLocker m_mutex;
	std::wstring m_strFileName;
	std::ofstream m_pfOut;
public:
	WXTcpLogInstance() {}

	virtual ~WXTcpLogInstance() {
		Close();
	}

	void Close() {
		WXTcpAutoLock al(m_mutex);
		if (m_pfOut.is_open()) {
			m_pfOut.close();
		}
	}

	bool Open(const wchar_t* strFileName) {
		WXTcpAutoLock al(m_mutex);
		Close();
		m_strFileName = strFileName;
		m_pfOut.open(m_strFileName.c_str(), std::ios::app | std::ios::binary);
		if (m_pfOut.is_open()) {
			uint8_t headText[2] = { 0xff, 0xfe };
			m_pfOut.write((const char*)headText, 2);
			return true;
		}
		return m_pfOut.is_open();
	}

	void Write(const wchar_t* wszMsg) {
		WXTcpAutoLock al(m_mutex);
		if (m_pfOut.is_open()) {
			m_pfOut.write((const char*)wszMsg, sizeof(wchar_t)*wcslen(wszMsg));
			m_pfOut.flush();
		}
	}
};

//单向队列
class WXTcpFifo {
public:
	volatile int m_bEnable = 1;//是否可写
	WXTcpDataBuffer m_dataBuffer;
	int64_t m_nTotalSize = 176400;
	int64_t m_nPosRead = 0;//读位置
	int64_t m_nPosWrite = 0;//写位置
public:
	inline int64_t Size() { return m_nPosWrite - m_nPosRead; }
public:
	WXTcpFifo() {
		m_dataBuffer.Init(nullptr, 176400);
		m_nTotalSize = 176400;
	}

	virtual ~WXTcpFifo() {
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
				}
				else {  //有部分数据需要写到RingBuffer的头部
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
			int64_t posRead = m_nPosRead % m_nTotalSize;//实际读取位置
			int64_t posLeft = m_nTotalSize - posRead;//在实际位置上buffer还剩多少可以写入的空间
			if (posLeft >= nSize) { //数据区足够
				if (pBuf)
					memcpy(pBuf, m_dataBuffer.m_pBuf + posRead, nSize);
			}
			else {  //有部分数据需要写到RingBuffer的头部
				if (pBuf) {
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
		}else if (nowSize > 0) {
			return Read(buf, nowSize);
		}
		return 0;
	}
};


//Win32字符串操作
class WXTcpString {
	std::string  m_strUTF8 = "";
	std::wstring m_strUnicode = L"";

	int m_nLength = 0;
	enum { MAX_LENGTH = 4096 };
	char m_szUTF8[MAX_LENGTH];
	char m_szANSI[MAX_LENGTH];
	wchar_t m_wszUnicode[MAX_LENGTH];


	void ANSIToUnicode(const char *sz) { //从char*初始化
		strcpy(m_szANSI, sz);//Copy to ANSI
		int len = MultiByteToWideChar(CP_ACP, 0, sz, -1, NULL, 0);
		memset(m_wszUnicode, 0, sizeof(wchar_t) * MAX_LENGTH);
		MultiByteToWideChar(CP_ACP, 0, sz, -1, (LPWSTR)m_wszUnicode, len);
	}

	void UnicodeToANSI(const wchar_t* wsz) { //从wchar_tc初始化
		wcscpy(m_wszUnicode, wsz);//Copy To Unicode
		int len = WideCharToMultiByte(CP_ACP, 0, wsz, -1, NULL, 0, NULL, NULL);
		memset(m_szANSI, 0, sizeof(char) * MAX_LENGTH);
		WideCharToMultiByte(CP_ACP, 0, wsz, -1, m_szANSI, len, NULL, NULL);
	}

	void UnicodeToUTF8() { //Convert To UTF8
		m_nLength = WideCharToMultiByte(CP_UTF8, 0, m_wszUnicode, -1, NULL, 0, NULL, NULL);
		memset(m_szUTF8, 0, sizeof(char) * MAX_LENGTH);
		WideCharToMultiByte(CP_UTF8, 0, m_wszUnicode, -1, m_szUTF8, m_nLength, NULL, NULL);
	}
public:
	void Init(const char *sz) {
		ANSIToUnicode(sz);
		UnicodeToUTF8();
		m_strUTF8 = m_szUTF8;
		m_strUnicode = m_wszUnicode;
	}

	void Init(const wchar_t* wsz) {
		UnicodeToANSI(wsz);
		UnicodeToUTF8();
		m_strUTF8 = m_szUTF8;
		m_strUnicode = m_wszUnicode;
	}
public:
	void Init(const WXTcpString& wxstr) {
		Init(wxstr.str());
	}
public:
	void Format(const wchar_t * format, ...) {
		wchar_t wszMsg[4096];
		memset(wszMsg, 0, 4096 * 2);
		va_list marker = nullptr;
		va_start(marker, format);
		vswprintf(wszMsg, format, marker);
		va_end(marker);
		Init(wszMsg);
	}
public:
	WXTcpString() {}

	WXTcpString(const WXTcpString& wxstr) {
		Init(wxstr.str());
	}

	WXTcpString(const wchar_t* wsz) {
		Init(wsz);
	}

	const int  length() const {
		return m_strUnicode.length();
	}
	const wchar_t* str()const {
		return m_strUnicode.c_str();
	}
	const char*  c_str() const {
		return m_strUTF8.c_str();
	}
	const wchar_t* Left(int n) {
		return m_strUnicode.c_str() + (length() - n);
	}

public://WXTcpString

	WXTcpString& operator+=(const WXTcpString& wxstr) {
		std::wstring wstr = m_wszUnicode;
		wstr += wxstr.str();
		Init(wstr.c_str());
		return *this;
	}
	bool operator==(const WXTcpString& wxstr) const {
		const wchar_t* wsz1 = this->str();
		const wchar_t* wsz2 = wxstr.str();
		return (wcsicmp(wsz1, wsz2) == 0);
	}
	bool operator!=(const WXTcpString& wxstr) const {
		const wchar_t* wsz1 = this->str();
		const wchar_t* wsz2 = wxstr.str();
		return (wcsicmp(wsz1, wsz2) != 0);
	}

public://const wchar_t*
	WXTcpString& operator+=(const wchar_t*  wsz) {
		std::wstring wstr = m_wszUnicode;
		wstr += wsz;
		Init(wstr.c_str());
		return *this;
	}
	bool operator==(const wchar_t* wsz) const {
		const wchar_t* wsz1 = this->str();
		return (wcscmp(wsz1, wsz) == 0);
	}
	bool operator!=(const wchar_t* wsz) const {
		const wchar_t* wsz1 = this->str();
		return (wcscmp(wsz1, wsz) != 0);
	}
};


#endif
