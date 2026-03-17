/*
WXBase 基础库，基于C++11
*/
#ifndef _WX_BASE_H_
#define _WX_BASE_H_

#include <stdint.h>


#ifndef SAFE_DELETE
#define SAFE_DELETE(p)  if(p){delete p;p=nullptr;}
#endif

#ifdef _WIN32
#include <Windows.h>
#include <tchar.h>
#pragma execution_character_set("utf-8")
#else
#define LPCTSTR const char*
#define LPTSTR  char*
#endif

#include <stdint.h>

class DataFrame {
public:
	uint8_t *m_pBuf = nullptr;
	int     m_iBufSize = 0;
	int     m_iPos = 0;
	int     extra1 = 0;
	int     extra2 = 0;
public:
	void Init(uint8_t *buf, int size);
	DataFrame();
	DataFrame(uint8_t *buf, int size);
	~DataFrame();
};


class WXLocker{
	void *m_impl = nullptr;
public:
	WXLocker();
	~WXLocker();
	void Lock();
	void Unlock();
};

class WXAutoLock{
	WXLocker * m_lock;
public:
	WXAutoLock(WXLocker * lock);
	~WXAutoLock();	
};


class WXThread {
	void *m_impl = nullptr;
	void ThreadFunction();
public:
	volatile bool m_bThreadStop = true;
	virtual  void ThreadRun() = 0;//必现实现的线程函数
	bool     ThreadStart();
	void     ThreadStop();	
};


class WXStringImpl;
class WXString{
public:
	WXStringImpl *m_impl = nullptr;
public:
	virtual ~WXString();
	WXString();
	WXString(const WXString& str);
	WXString(LPCTSTR wsz);

	void Format(const char * _Format, ...);//格式化字符串

	const char*   c_str() const;//ffmpeg(argv,argc) 需要的是 UTF8 的 char*

	LPCTSTR       str() const;
	LPCTSTR       Left(int n);
	const int     length() const;

	WXString& operator=(const WXString& str);
	WXString& operator=(LPCTSTR wsz);

	WXString& operator+=(const WXString& str);
	WXString& operator+=(LPCTSTR wsz);
	void Cat(WXString str, LPCTSTR szSlipt);

	bool operator==(const WXString& str) const;
	bool operator==(LPCTSTR wsz) const;

	bool operator!=(const WXString& str) const;
	bool operator!=(LPCTSTR wsz) const;
};


class WXLogImpl;
class WXLog :public WXLocker{
	WXLogImpl *m_impl = nullptr;
public:
	WXLog();
	~WXLog();
	void Init(LPCTSTR szFileName);
	void Write(const char *format, ...);
};

class WXUtils {
public:
	static int64_t  GetTimeMs();
	static void     SleepMs(int ms);
	static WXLog *  GetLog();
	static void     SetLogFile(LPCTSTR wszFileName);

	static LPTSTR   Strcpy(LPTSTR str1, LPCTSTR str2);
	static int      Strcmp(LPCTSTR str1, LPCTSTR str2);
	static int      Strlen(LPCTSTR str);
	static LPCTSTR  Strdup(LPCTSTR str);
};


class WXFifoImpl;
class WXFifo{
	WXFifoImpl *m_impl = nullptr;
public:
	WXFifo();
	~WXFifo();
public:
	int Size(); 
	int Write(uint8_t *buf, int size);
	int Read(uint8_t *buf, int size);
};

#endif
