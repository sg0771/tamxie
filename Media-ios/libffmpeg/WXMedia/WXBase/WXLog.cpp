/*
C++  Log File 
*/

#include  "WXBase.h"
#include  <stdarg.h> 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>

#ifndef _WIN32
#include <malloc/malloc.h>
#define  RENAME rename
int _vscprintf(const char * format, va_list pargs) {
	int retval;
	va_list argcopy;
	va_copy(argcopy, pargs);
	retval = vsnprintf(NULL, 0, format, argcopy);
	va_end(argcopy);
	return retval;
}
#else
#include <malloc.h>
#define  RENAME MoveFileA
#endif

#include <queue>
class WXLogImpl :public WXLocker,public WXThread {
	WXString m_strFileName;
	bool m_bInit = false;
	FILE *m_pLog = nullptr;
	std::queue<WXString> m_queue;

	void RenameImpl() {
		struct stat st;
		int ret = stat(m_strFileName.c_str(), &st);
		if (ret == 0 && st.st_size > 1 * 1000 * 1000) {
			int rename2 = 1;
			while (1) {
				WXString wxstr;
				wxstr.Format("%s.%d", m_strFileName.c_str(), rename2);
				struct stat st2;
				int ret2 = stat(wxstr.c_str(), &st2);
				if (ret2 == 0) {
					rename2++;
					continue;
				}else {
					RENAME(m_strFileName.c_str(), wxstr.c_str()); //
					break;
				}
			}
			return;
		}
	}

	WXLocker m_lockWrite;

	const char *szRN = "\r\n";
public:
	WXLogImpl() {}
	~WXLogImpl() {
		DeInit();
	}

	virtual void ThreadRun() {
		while (!m_bThreadStop){
			{
				WXAutoLock al2(&m_lockWrite);
				while(!m_queue.empty()){
					WXString wxstr = m_queue.front();
					const char *szText = wxstr.c_str();
					fwrite(szText, strlen(szText), 1, m_pLog);				
					m_queue.pop();
				}				
			}
			WXUtils::SleepMs(100);
		}
		if (m_pLog) {
			fclose(m_pLog);
			m_pLog = nullptr;
		}
		m_bInit = false;
	}

	void DeInit() {
		WXAutoLock al(this);
		if (!m_bThreadStop)
			ThreadStop();
	}

	void Init(LPCTSTR filename) {
		WXAutoLock al(this);
		if (m_bInit)return;
		m_strFileName = filename;
		InitImpl(m_strFileName.c_str());
	}

	void InitImpl(const char *filename) {
		WXAutoLock al(this);
		if (m_bInit)return;
		m_strFileName.Format("%s",filename);
		RenameImpl();//
		m_pLog = fopen(m_strFileName.c_str(), "wb");
		if (m_pLog) {
			m_bInit = true;
			setbuf(m_pLog, NULL);
			ThreadStart();
		}
	}
	
	void Write(const char *szMsg) {
		WXAutoLock al(this);
		if (!m_bInit)
			return;

		WXString wxstr;
		time_t t = time(NULL);
		tm* local = localtime(&t);
		wxstr.Format("%04d-%02d-%02d %02d:%02d:%02d - ",
			local->tm_year + 1900, local->tm_mon + 1, local->tm_mday,
			local->tm_hour, local->tm_min, local->tm_sec);

		WXString wxstr1;
		wxstr1.Format("%s\r\n",szMsg);
		wxstr += wxstr1;
		WXAutoLock al2(&m_lockWrite);
		m_queue.push(wxstr);
	}
};

WXLog::WXLog() {
	m_impl = new WXLogImpl;
}

WXLog::~WXLog() {
	SAFE_DELETE(m_impl);
}

void WXLog::Init(LPCTSTR szFileName) {
	m_impl->Init(szFileName);
}

void WXLog::Write(const char *format, ...) {
#ifdef _WIN32
	va_list marker = NULL;
#else
    va_list marker;
#endif
    va_start(marker, format);
	size_t length = _vscprintf(format, marker);
	char *szMsg = new char[length + 1];
	vsprintf(szMsg, format, marker);
	va_end(marker);
	m_impl->Write(szMsg);
	delete[]szMsg;
}

