
#include "WXBase.h"
#include <string.h>
#include <stdarg.h>

#ifndef _WIN32
#include <sys/time.h>
#include <stdio.h>
static int _vscprintf(const char * format, va_list pargs) {
    int retval;
    va_list argcopy;
    va_copy(argcopy, pargs);
    retval = vsnprintf(NULL, 0, format, argcopy);
    va_end(argcopy);
    return retval;
}
#endif


static WXLocker gLock;
static WXLog *s_log = nullptr;
WXLog * WXUtils::GetLog() {
	return s_log;
}

void WXUtils::SetLogFile(LPCTSTR wszFileName) {
	WXAutoLock al(&gLock);
	WXString strFileName = wszFileName;
	if (s_log == nullptr)
		s_log = new WXLog;
	s_log->Init(strFileName.str());
}


int log_level = 0;
extern "C" void  WXLogWriteNew(const char *format, ...) {
    
    if (s_log) {
        va_list marker
#ifdef _WIN32
        = NULL
#endif
        ;
        va_start(marker, format);
        size_t length = _vscprintf(format, marker);
        char *szMsg = new char[length + 1];
        vsprintf(szMsg, format, marker);
        va_end(marker);
        s_log->Write(szMsg);
        delete[]szMsg;
    }
}



LPTSTR  WXUtils::Strcpy(LPTSTR str1, LPCTSTR str2) {
#ifndef _WIN32
	return strcpy(str1, str2);
#else
	return wcscpy(str1, str2);
#endif
}


int  WXUtils::Strcmp(LPCTSTR str1, LPCTSTR str2) {
#ifndef _WIN32
	return strcasecmp(str1, str2);
#else
	return _wcsicmp(str1, str2);
#endif
}


int  WXUtils::Strlen(LPCTSTR str) {
#ifndef _WIN32
	return (int)strlen(str);
#else
	return wcslen(str);
#endif
}

LPCTSTR  WXUtils::Strdup(LPCTSTR str) {
#ifndef _WIN32
	return strdup(str);
#else
	return _wcsdup(str);
#endif
}



int64_t WXUtils::GetTimeMs() {
#ifdef _WIN32
	return ::timeGetTime();
#else
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000 + tv.tv_usec / 1000;
#endif
}

void WXUtils::SleepMs(int ms) {
	if (ms <= 0)return;
#ifdef _WIN32
	::timeBeginPeriod(1);
	::Sleep(ms); 
	::timeEndPeriod(1);
#else
	struct timeval delay;
	delay.tv_sec = 0;
	delay.tv_usec = ms * 1000;
	select(0, NULL, NULL, NULL, &delay);
#endif
}
