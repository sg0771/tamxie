#include "WXBase.h"

#import <Foundation/Foundation.h>
#include <string>
#define LCPTSTR const char*

class WXStringImpl{
public:
    enum    { MAX_LENGTH = 4096};
    
    std::string m_string = "";
    NSString *m_nstring = @"";
    
    void Init(LCPTSTR sz) {
        assert(sz != NULL);
        m_string = sz;
        m_nstring = [NSString stringWithUTF8String:m_string.c_str()];
    }

    void Init(const WXString& str) {
        Init(str.c_str());
    }
    
    
    void Cat(WXStringImpl str, LPCTSTR slipt){
        if (str.length() != 0) {
            if (this->length() != 0)
                m_string += slipt;
            m_string += str.c_str();
            Init(m_string.c_str());
        }
    }
    
    LPCTSTR  c_str() const{
        return [m_nstring UTF8String];
    }
    
    LPCTSTR  str() const{
        return [m_nstring UTF8String];
    }
    
    const int length() const {
        return (int)[m_nstring length];
    }
    
    LPCTSTR Left(int n){
        return [[m_nstring substringFromIndex:(length() - n)] UTF8String];
    }
    
    WXStringImpl& operator+=(const WXStringImpl& str) {
        m_string += str.c_str();
        Init(m_string.c_str());
        return *this;
    }
    
    WXStringImpl& operator+=(LPCTSTR sz) {
        std::string str = m_string;
        str += sz;
        Init(str.c_str());
        return *this;
    }
    
    WXStringImpl& operator=(const WXStringImpl& str) {
        Init(str.c_str());
        return *this;
    }
    
    WXStringImpl& operator=(LPCTSTR sz) {
        Init(sz);
        return *this;
    }
    
    
    bool operator==(const WXStringImpl& str) const{
        return (strcmp(m_string.c_str(), str.c_str()) == 0);
    }
    
    bool operator==(LPCTSTR sz) const{
        return (strcmp(m_string.c_str(), sz) == 0);
    }
    
    bool operator!=(const WXStringImpl& str) const{
        return (strcmp(m_string.c_str(), str.c_str()));
    }
    
    bool operator!=(LPCTSTR sz) const{
        return (strcmp(m_string.c_str(), sz));
    }
};

void WXString::Format(LPCTSTR _Format, ...){
    char *tmp = new char[WXStringImpl::MAX_LENGTH];
    memset(tmp,0, WXStringImpl::MAX_LENGTH);
    va_list args;
    va_start(args, _Format);
    vsnprintf(tmp, WXStringImpl::MAX_LENGTH, _Format, args);
    va_end(args);
    m_impl->Init(tmp);
    delete []tmp;
}


WXString::WXString(){
    m_impl = new WXStringImpl;
}

WXString::~WXString(){
    SAFE_DELETE(m_impl);
}

WXString::WXString(const WXString& str) {
    m_impl = new WXStringImpl;
	m_impl->Init(str.c_str());
}

WXString::WXString(LPCTSTR wsz){
    m_impl = new WXStringImpl;
    m_impl->Init(wsz);
}


void WXString::Cat(WXString str, LPCTSTR szSlipt){
    m_impl->Cat(*str.m_impl, szSlipt);
}

const char* WXString::c_str() const{
    return m_impl->c_str();
}

LPCTSTR WXString::str() const{
    return m_impl->c_str();
}

LPCTSTR  WXString::Left(int n){
    return m_impl->Left(n);
}

const int  WXString::length() const{
    return m_impl->length();
}

WXString& WXString::operator=(const WXString& str){
    m_impl->Init(str);
    return *this;
}

WXString& WXString::operator=(LPCTSTR sz){
    m_impl->Init(sz);
    return *this;
}


WXString& WXString::operator+=(const WXString& str){
    *this->m_impl += (*str.m_impl);
    return *this;
}

WXString& WXString::operator+=(LPCTSTR sz){
    *this->m_impl += sz;
    return *this;
}


bool WXString::operator==(const WXString& str) const{
    return m_impl == str.m_impl;
}

bool WXString::operator==(LPCTSTR sz) const{
    return *m_impl == sz;
}

bool WXString::operator!=(const WXString& str) const{
    return m_impl != str.m_impl;
}

bool WXString::operator!=(LPCTSTR sz) const{
    return *m_impl != sz;
}
