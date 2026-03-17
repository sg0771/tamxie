
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
#ifndef ANDROID
#include  <codecvt>
#endif
#include  <functional>
#include  <condition_variable>
#include  <atomic>

#ifdef _WIN32

#include <Windows.h>
#include <tchar.h>
#include <timeapi.h>
#include <time.h>
#pragma warning(disable : 4068)

#define WXCTSTR const wchar_t*
#define WXTSTR  wchar_t*
#define  RENAME MoveFileA

#else //__APPLE__
#include <sys/time.h>
#include <strings.h> //memcpy
#define  RENAME rename
#define _T(x)       x
#define _TEXT(x)    x
using namespace std;
#define MAX_PATH  1024
#define HWND void*
#define DLL_API
#define WXCTSTR const char*
#define WXTSTR  char*
#ifdef __cplusplus
#define EXTERN_C extern "C"
#else
#define EXTERN_C
#endif
#endif


class WXUtils {
public:
	static  WXTSTR  Strcpy(WXTSTR str1, WXCTSTR str2) {
#ifndef WIN32
		return strcpy(str1, str2);
#else
		return wcscpy(str1, str2);
#endif
	}

	static int  Strcmp(WXCTSTR str1, WXCTSTR str2) {
#ifndef WIN32
		return strcasecmp(str1, str2);
#else
		return _wcsicmp(str1, str2);
#endif
	}

	static int  Strlen(WXCTSTR str) {
#ifndef WIN32
		return (int)strlen(str);
#else
		return wcslen(str);
#endif
	}

	static WXCTSTR  Strdup(WXCTSTR str) {
#ifndef WIN32
		return strdup(str);
#else
		return _wcsdup(str);
#endif
	}
};

//WXMedia 内部log
#ifdef _WX_EXPORT
EXTERN_C void  WXLogA(const char* format, ...);
EXTERN_C void  WXLogW(const wchar_t* format, ...);

EXTERN_C void* wx_malloc(size_t size);
EXTERN_C void  wx_free(void* ptr);
EXTERN_C void* wx_realloc(void* ptr, size_t size);
#else
static void  WXLogA(const char* format, ...) {}
static void  WXLogW(const wchar_t* format, ...) {}
static void* wx_malloc(size_t size) {
	return malloc(size);
}
static void  wx_free(void* ptr) {
	free(ptr);
}
static void* wx_realloc(void* ptr, size_t size) {
	return realloc(ptr, size);
}
#endif

#define WXTask      std::function<void()>
#define WXLocker    std::recursive_mutex //递归锁
#define WXAutoLock  std::lock_guard<WXLocker>  //自动锁
#define WXMutex     std::mutex
#define WXLockMutex std::unique_lock<WXMutex> 
#define WXCond      std::condition_variable
#define SLEEP(sec)  std::this_thread::sleep_for(std::chrono::seconds(sec));
#define SLEEPMS(ms) std::this_thread::sleep_for(std::chrono::milliseconds(ms));

static void WXCond_Wait(WXCond* cond, int* bFlag) {
	if (cond) {
		WXMutex mutex;
		WXLockMutex lock(mutex);//定义独占锁
		cond->wait(lock, [bFlag] {
			if (bFlag) {
				return *bFlag == 1;//false 表示被堵塞, true表示不被堵塞
			}
			return false;//被堵塞
			});//阻塞到cond执行notify
	}
}

static void WXCond_Notify(WXCond* cond, int* bFlag, int bAll) {
	if (cond) {
		if (bFlag) {
			*bFlag = 1;
		}
		bAll ? cond->notify_all() : cond->notify_one();
	}
}


#ifdef __APPLE__
#include <malloc/malloc.h>
#define  RENAME rename
static int _vscprintf(const char* format, va_list pargs) {
	int retval;
	va_list argcopy;
	va_copy(argcopy, pargs);
	retval = vsnprintf(nullptr, 0, format, argcopy);
	va_end(argcopy);
	return retval;
}
#endif

#ifdef ANDROID
#include <malloc.h>
#define  RENAME rename
static int _vscprintf(const char* format, va_list pargs) {
	int retval;
	va_list argcopy;
	va_copy(argcopy, pargs);
	retval = vsnprintf(nullptr, 0, format, argcopy);
	va_end(argcopy);
	return retval;
}
#endif

#ifdef linux
#include <malloc.h>
#define  RENAME rename
static int _vscprintf(const char* format, va_list pargs) {
	int retval;
	va_list argcopy;
	va_copy(argcopy, pargs);
	retval = vsnprintf(nullptr, 0, format, argcopy);
	va_end(argcopy);
	return retval;
}

#endif

//内存数据管理
class WXDataBuffer {
	std::shared_ptr<uint8_t> m_pBuf;
public:
	int     m_iBufSize = 0;
	int     m_iPos = 0;
	int64_t m_pts = 0;
	int64_t extra1 = 0;
	int64_t extra2 = 0;

	int     m_iRealSize = 0;
public:
	uint8_t* GetBuffer() {
		return m_pBuf.get();
	}
	void Init(uint8_t* buf, int size, int64_t pts = 0) {
		if (size <= 0)
			return;
		m_pts = pts;
		if (m_iRealSize < size) { //当前内存不足，进行扩容
			m_pBuf = nullptr;
			m_pBuf = std::shared_ptr<uint8_t>((uint8_t*)wx_malloc(size),
				[](uint8_t* p) { if (p) { wx_free(p); p = nullptr; }});
			m_iRealSize = size;//实际长度
			m_iBufSize = size;
		}
		else {
			m_iBufSize = size;//只改变有效长度
		}

		if (buf != nullptr) { //拷贝数据
			memcpy(m_pBuf.get(), buf, size);
		}
		else { //初始化为0
			memset(m_pBuf.get(), 0, size);
		}
	}

	WXDataBuffer() {}

	WXDataBuffer(uint8_t* buf, int size) {
		Init(buf, size);
	}

	virtual ~WXDataBuffer() {
		m_pBuf = nullptr;
		m_iBufSize = 0;
		m_iRealSize = 0;
	}
};

class WXString {
	std::string  m_strUTF8 = "";//ffmpeg 需要

	void InitA(const char* sz) {
		m_strUTF8 = sz;
	}


public:
	void Format(const char* format, ...) {
		char    szMsg[4096];
		memset(szMsg, 0, 4096);
		va_list marker;
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

    
    WXString(const char* sz) {
        InitA(sz);
    }
    
	const int  length() const {
		return (int)m_strUTF8.length();
	}


	const char* str()const {
		return c_str();
	}
	const char* Left(int n) {
		return m_strUTF8.c_str() + (m_strUTF8.length() - n);
	}

	const char* c_str() const {
		return m_strUTF8.c_str();
	}


public://WXString

	WXString& operator+=(const WXString& wxstr) {
		std::string str = m_strUTF8;
		str += wxstr.c_str();
		InitA(str.c_str());
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
public://const char*
    WXString& operator+=(const char* wsz) {
        std::string wstr = m_strUTF8;
        wstr += wsz;
        InitA(wstr.c_str());
        return *this;
    }
    bool operator==(const char* sz) const {
        const char* sz1 = this->c_str();
        return (WXUtils::Strcmp(sz1, sz) == 0);
    }
    bool operator!=(const char* sz) const {
        const char* sz1 = this->c_str();
        return (WXUtils::Strcmp(sz1, sz) != 0);
    }
    void Cat(WXString wxstr, const char* wszSlipt) {
        //添加一个间隔符来拼接字符串
        if (wxstr.length() != 0) {
            std::string wstr = m_strUTF8;
            if (length() != 0)
                wstr += wszSlipt;
            wstr += wxstr.c_str();
            InitA(wstr.c_str());
        }
    }
};

//成员应该为指针类型！
template <class PtrT>
class ThreadSafeQueue {
protected:
	WXLocker m_lock;
	std::queue<PtrT> m_queue;
public:
	bool Empty() {
		WXAutoLock al(m_lock);
		return m_queue.empty();
	}
	PtrT Pop() {
		PtrT obj = nullptr;
		{
			WXAutoLock al(m_lock);
			if (!m_queue.empty()) {
				obj = m_queue.front();
				m_queue.pop();
			}
		}
		return obj;
	}
	int Size()
	{
		WXAutoLock al(m_lock);
		return (int)m_queue.size();
	}
	void Push(PtrT obj) {
		WXAutoLock al(m_lock);
		m_queue.push(obj);
	}
	void Flush() {
		WXAutoLock al(m_lock);
		while (!m_queue.empty()) {
			PtrT obj = m_queue.front();
			m_queue.pop();
			delete obj;
		}
	}
};

//线程类
class WXTrace {
public:
	int m_nMaxError = 5;//最多错误次数，超过之后内部log不执行
	int m_nError = 0;
	void  LogW(const wchar_t* format, ...) {

	}
	void  LogA(const char* format, ...) {

	}
};

class WXThread :public WXTrace {

	WXString m_strThreadName = _T("WXThread");

	bool m_bUseTask = false; //sync task
	ThreadSafeQueue<WXTask>m_queueTask;

	std::thread::id m_id;

	bool m_bThreadStop =true; 
	std::shared_ptr<std::thread> m_thread = nullptr;

	WXCond* m_condWait = nullptr;//wait start
	int* m_bWaitFlag = nullptr;

	void WaitQueueEmpty() {
		WXMutex mutex;
		WXLockMutex lock(mutex);
		WXCond m_cond;//queue empty notify
		m_cond.wait(lock, [this] {
			return m_queueTask.Empty(); //
			});
		return;
	}

	void TaskRunning() {
		if (m_bUseTask) { //sync task
			while (!Empty()) {
				WXTask task = m_queueTask.Pop();
				if (Empty()) {
					task();//执行队列里面的任务
					break;
				}
				else {
					task(); //执行队列里面的任务
				}
			}
		}
	}

	bool Empty() {
		return m_queueTask.Empty();
	}

public:
	//线程循环前的初始化操作，运行这个后向 ThreadStart 发送启动成功消息
	virtual  void ThreadPrepare() {
		//this->LogA("WXThread [%ws] ThreadPrepare", m_strThreadName.str());
	}


	virtual  void ThreadWait() {
		//this->LogA("WXThread [%ws] ThreadWait", m_strThreadName.str());
	}

	//线程循环函数,必须实现
	virtual  void ThreadProcess() = 0;

	//线程循环结束后的退出处理
	virtual  void ThreadPost() {
		this->LogW(L"WXThread [%ws] ThreadPost", m_strThreadName.str());
	}
public:
	bool IsRunning() {
		return m_thread != nullptr;
	}
	//功能: 在执行线程中执行任务
	//task: 任务
	//bSync: 是否等待到执行完毕
	void RunTask(WXTask task, int bSync = 0) {
		if (!m_bThreadStop && m_bUseTask) {
			if (std::this_thread::get_id() == m_id) {
				task();//直接运行
			}
			else {
				if (bSync)
					WaitQueueEmpty();
				m_queueTask.Push(task);
				if (bSync)
					WaitQueueEmpty();
			}
		}
	}


	void ThreadWillStop() {
		m_bThreadStop = true;
	}

	void ThreadSetCond(WXCond* condWait, int* bFlag) {
		m_condWait = condWait;//等待启动
		m_bWaitFlag = bFlag;
	}
	WXCond* GetCond() {
		return m_condWait;
	}
	int* GetFlag() {
		return m_bWaitFlag;
	}
	void ThreadSetName(const char* wszName) {
		m_strThreadName = wszName;
	}
	bool ThreadStart(bool bUseTask = false) {
		if (m_thread)
			return true;//已经启动

		m_bThreadStop = false;
		m_bUseTask = bUseTask;

		WXCond condStart;//启动消息
		int bStartFlag = 0;
		m_thread = std::shared_ptr<std::thread>(new std::thread([this, &condStart, &bStartFlag] {
			m_id = std::this_thread::get_id();//线程ID

			ThreadPrepare(); //线程资源初始化操作
			WXCond_Notify(&condStart, &bStartFlag, 0);//通知ThreadStart线程已经启动

			if (nullptr != m_condWait) {//如果已经注册了外部信号量，堵塞等待
				WXCond_Wait(m_condWait, m_bWaitFlag);
			}
			ThreadWait();//线程激活

			while (!m_bThreadStop) { //线程循环
				TaskRunning(); //执行队列上的任务
				ThreadProcess(); //自定义线程函数
			}

			ThreadPost(); //线程退出操作

			}), [](std::thread* thread) {
				//删除线程的处理
				if (thread) {
					if (thread->joinable()) {
						thread->join();
					}
					delete thread;
				}
				thread = nullptr;
			});

		//等待线程函数执行完ThreadPrepare
		//如果已经注册外部信号量
		//使用WXCond_Notify 来激活线程函数
		WXCond_Wait(&condStart, &bStartFlag);
		return true;
	}

	void ThreadStop() {
		m_bThreadStop = true;//结束线程循环
		if (m_condWait) {
			WXCond_Notify(m_condWait, m_bWaitFlag, 1);//避免线程堵塞，激活一下
		}
		m_thread = nullptr;//结束线程
	}
};



//单向队列
class WXFifo {
public:
	volatile int m_bEnable = 1;//是否可写
	WXDataBuffer m_dataBuffer;
	int64_t m_nTotalSize = 384000;
	int64_t m_nPosRead = 0;//读位置
	int64_t m_nPosWrite = 0;//写位置
	uint8_t m_last = 0;
	WXLocker m_mutex;
public:
	inline int64_t Size() {
		WXAutoLock al(m_mutex);
		return m_nPosWrite - m_nPosRead;
	}
public:
	WXFifo(int totolsize = 192000) {
		WXAutoLock al(m_mutex);
		m_dataBuffer.Init(nullptr, totolsize);
		m_nTotalSize = totolsize;
	}

	void Init(int totolsize = 192000) {
		WXAutoLock al(m_mutex);
		m_dataBuffer.Init(nullptr, totolsize);
		m_nTotalSize = totolsize;
	}

	void Reset() {
		WXAutoLock al(m_mutex);
		m_nPosRead = 0;
		m_nPosWrite = 0;
		memset(m_dataBuffer.GetBuffer(), 0, m_dataBuffer.m_iBufSize);
	}

	virtual ~WXFifo() {
		WXAutoLock al(m_mutex);
		m_nPosRead = 0;
		m_nPosWrite = 0;
	}

	void Write(uint8_t* pBuf, int nSize) { //写数据
		WXAutoLock al(m_mutex);
		if (m_bEnable) {
			int64_t newSize = m_nPosWrite - m_nPosRead + nSize;//可写区域
			if (newSize < m_nTotalSize) { //数据区可写
				int64_t posWirte = m_nPosWrite % m_nTotalSize;//实际写入位置
				int64_t posLeft = m_nTotalSize - posWirte;//在实际位置上buffer还剩多少可以写入的空间
				if (posLeft >= nSize) { //数据区足够
					memcpy(m_dataBuffer.GetBuffer() + posWirte, pBuf, nSize);
				}
				else {  //有部分数据需要写到RingBuffer的头部
					memcpy(m_dataBuffer.GetBuffer() + posWirte, pBuf, posLeft);//写到RingBuffer尾部
					memcpy(m_dataBuffer.GetBuffer(), pBuf + posLeft, nSize - posLeft);//写到RingBuffer头部
				}
				m_nPosWrite += nSize;//更新写位置
			}
		}
	}

	int Read(uint8_t* pBuf, int nSize) {//读数据
		WXAutoLock al(m_mutex);
		int64_t nowSize = m_nPosWrite - m_nPosRead;//可读区域
		if (nSize && nowSize >= nSize) { //数据足够读
			if (pBuf) {
				int64_t posRead = m_nPosRead % m_nTotalSize;//实际读取位置
				int64_t posLeft = m_nTotalSize - posRead;//在实际位置上buffer还剩多少可以写入的空间
				if (posLeft >= nSize) { //数据区足够
					memcpy(pBuf, m_dataBuffer.GetBuffer() + posRead, (size_t)nSize);
				}
				else {  //有部分数据需要写到RingBuffer的头部
					memcpy(pBuf, m_dataBuffer.GetBuffer() + posRead, (size_t)posLeft);//从尾部拷贝数据
					memcpy(pBuf + posLeft, m_dataBuffer.GetBuffer(), (size_t)(nSize - posLeft));//从头部拷贝数据
				}
			}
			m_nPosRead += nSize;
			m_last = pBuf[nSize - 1];
			return nSize;
		}
		return 0;
	}

	int Read2(uint8_t* buf, int size) {//buf=nullptr Skip
		WXAutoLock al(m_mutex);
		memset(buf, m_last, size);
		int64_t nowSize = m_nPosWrite - m_nPosRead;//有效数据区
		if (nowSize >= size) {
			return Read(buf, size);
		}
		else if (nowSize > 0) {
			int ret = Read(buf, nowSize);
			return ret;
		}
		return 0;
	}

	int Read3(uint8_t* buf, int size) {//buf=nullptr Skip
		WXAutoLock al(m_mutex);
		memset(buf, 0, size);
		int64_t nowSize = m_nPosWrite - m_nPosRead;//有效数据区
		if (nowSize >= size) {
			return Read(buf, size);
		}
		else if (nowSize > 0) {
			int ret = Read(buf, nowSize);
			return ret;
		}
		return 0;
	}
};

#endif

