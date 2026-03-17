/*
文件名:WXString.h
作者: Tam.Xie
时间: 2017-09-29
版本号:  1.0
描述: 基于 std::string 的字符串处理
*/
#ifndef __APPLE__

#include "WXBase.h"
#include <string>
#include <locale>
#include <codecvt>

class WXStringImpl {
	std::string m_string = "";
	std::wstring m_wstring = L"";
public:
	int Init(const char *sz) {
		if (sz == nullptr)return Init("");
		m_string = sz;
		std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
		m_wstring = conv.from_bytes(m_string);
		return m_wstring.length();
	}

	int Init(LPCTSTR wsz) {
		if (wsz == nullptr)return Init(L"");
		m_wstring = wsz;
		std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
		m_string = conv.to_bytes(m_wstring);
		return m_wstring.length();
	}

	int Init(const WXString& str) {
		return Init(str.str());
	}

	void Cat(WXStringImpl wxstr, LPCTSTR szSlipt) { //添加一个间隔符来拼接字符串
		if (wxstr.length() != 0) {
			if (length() != 0)m_wstring += szSlipt;
			m_wstring += wxstr.str();
			std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
			m_string = conv.to_bytes(m_wstring);
		}
	}

	const char*  c_str() const {
		return m_string.c_str();
	}

	LPCTSTR Left(int n) {
		return m_wstring.c_str() + (length() - n);
	}

	LPCTSTR str() const {
		return m_wstring.c_str();
	}

	const int  length() const {
		return m_wstring.length();
	}

	WXStringImpl& operator+=(const WXStringImpl& str) {
		m_wstring += str.str();
		Init(m_wstring.c_str());
		return *this;
	}


	WXStringImpl& operator+=(LPCTSTR wsz) {
		m_wstring += wsz;
		Init(m_wstring.c_str());
		return *this;
	}

	WXStringImpl& operator=(const WXStringImpl& str) {
		Init(str.str());
		return *this;
	}

	WXStringImpl& operator=(LPCTSTR wsz) {
		Init(wsz);
		return *this;
	}

	bool operator==(const WXStringImpl& str) const {
		const char* sz1 = c_str();
		const char* sz2 = str.c_str();
		return (strcmp(sz1, sz2) == 0);
	}

	bool operator==(LPCTSTR wsz) const {
		if (wsz == nullptr)return false;
		LPCTSTR wsz1 = str();
		return (wcscmp(wsz1, wsz) == 0);
	}

	bool operator!=(const WXStringImpl& str) const {
		const char* sz1 = c_str();
		const char* sz2 = str.c_str();
		return (strcmp(sz1, sz2) != 0);
	}

	bool operator!=(LPCTSTR wsz) const {
		if (wsz == nullptr)return true;
		LPCTSTR wsz1 = str();
		return (wcscmp(wsz1, wsz) != 0);
	}
};

WXString::~WXString() {
	SAFE_DELETE(m_impl);
}

WXString::WXString(){
	m_impl = new WXStringImpl;
}


WXString::WXString(LPCTSTR wsz) {
	m_impl = new WXStringImpl;
	m_impl->Init(wsz);
}

WXString::WXString(const WXString& str) {
	m_impl = new WXStringImpl;
	m_impl->Init(str.str());
}

void WXString::Format(const char * format, ...) {
	va_list marker = NULL;
	va_start(marker, format);
	size_t length = _vscprintf(format, marker);
	char *tmp = new char[length + 1];
	vsprintf(tmp, format, marker);
	va_end(marker);
	m_impl->Init(tmp);
	delete[]tmp;
}


const int WXString::length() const {
	return m_impl->length();
}

const char*    WXString::c_str() const {
	return m_impl->c_str();
}

LPCTSTR        WXString::str() const {
	return m_impl->str();
}

LPCTSTR WXString::Left(int n) {
	return m_impl->Left(n);
}

WXString&  WXString::operator=(const WXString& str) {
	*m_impl = str.str();
	return *this;
}
WXString&  WXString::operator=(LPCTSTR wsz) {
	*m_impl = wsz;
	return *this;
}

WXString&  WXString::operator+=(const WXString& str) {
	*m_impl += str.str();
	return *this;
}
WXString&  WXString::operator+=(LPCTSTR wsz) {
	*m_impl += wsz;
	return *this;
}
void  WXString::Cat(WXString str, LPCTSTR szSlipt) {
	return m_impl->Cat(*str.m_impl, szSlipt);
}

bool  WXString::operator==(const WXString& str) const {
	return *m_impl == *str.m_impl;
}
bool WXString:: operator==(LPCTSTR wsz) const {
	return *m_impl == wsz;
}

bool  WXString::operator!=(const WXString& str) const {
	return *m_impl != *str.m_impl;
}

bool  WXString::operator!=(LPCTSTR wsz) const {
	return *m_impl != wsz;
}

#endif
