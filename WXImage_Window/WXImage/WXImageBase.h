/*

PNG 压缩

*/
#ifndef _RWPNG_H_
#define _RWPNG_H_

#include <stdint.h>
#include <string.h>
#include "libyuv.h"
#include <map>
#include <memory>
#include <string>
#include  "WXImage.h"
#include <sys/stat.h>

#ifndef FALSE                   /* in case these macros already exist */
#define FALSE   0               /* values of boolean */
#endif
#ifndef TRUE
#define TRUE    1
#endif

bool IsTimeout(int year, int mouth, int data);
EXTERN_C void  WXLogA(const char* format, ...);
EXTERN_C void  WXLogW(const wchar_t* format, ...);

#ifdef _WIN32

#include <locale>
#include <codecvt>
static std::string UTF16ToUTF8(const wchar_t* wszUTF16) {
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::wstring wstrUTF16 = wszUTF16 ? wszUTF16 : L"";
	std::string strUTF8 = converter.to_bytes(wstrUTF16);
	return strUTF8;
}

static std::wstring UTF8ToUTF16(const char* szUTF8) {
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::string strUTF8 = szUTF8 ? szUTF8 : "";
	std::wstring strUTF16 = converter.from_bytes(strUTF8);
	return strUTF16;
}

static std::wstring strRight(std::wstring str, int nPos) {
	size_t length = str.length();
	return str.substr(length - nPos, nPos);
}

static int GetImageTypeFromName(const wchar_t* strName) {
	std::wstring str = strName;
	std::wstring str4 = strRight(str, 4);
	std::wstring str5 = strRight(str, 5);
	if (wcsicmp(str4.c_str(), L".jpg") == 0 || wcsicmp(str5.c_str(), L".jpeg") == 0)
		return WXIMAGE_TYPE_JPEG;
	
	else if (wcsicmp(str4.c_str(), L".png") == 0)
		return WXIMAGE_TYPE_PNG;
	
	else if (wcsicmp(str5.c_str(), L".webp") == 0)
		return WXIMAGE_TYPE_WEBP;

	return WXIMAGE_TYPE_UNKNOWN;
}
#else
#define  stricmp strcasecmp
#endif


static std::string strRight(std::string str, int nPos) {
	size_t length = str.length();
	return str.substr(length - nPos, nPos);
}

static int GetImageTypeFromName(const char* strName) {
	std::string str = strName;
	std::string str4 = strRight(str, 4);
	std::string str5 = strRight(str, 5);
	if (stricmp(str4.c_str(), ".jpg") == 0 || stricmp(str5.c_str(), ".jpeg") == 0)
		return WXIMAGE_TYPE_JPEG;
	else if (stricmp(str4.c_str(), ".png") == 0)
		return WXIMAGE_TYPE_PNG;
	else if (stricmp(str5.c_str(), ".webp") == 0)
		return WXIMAGE_TYPE_WEBP;
	return WXIMAGE_TYPE_UNKNOWN;
}


static const char*  arrImageType[]{
	"original","jpg", "png", "webp"
};

static const wchar_t* arrImageTypeW[]{
	L"original",L"jpg", L"png", L"webp"
};

typedef std::shared_ptr<uint8_t> U8Ptr;
static U8Ptr new_u8_ptr(size_t  size) {
	return U8Ptr(new uint8_t[size],
		[](uint8_t* p) {
			if (p) {
				delete[]p; p = nullptr;
			}
		}
	);
}
typedef std::shared_ptr<FILE> FilePtr;

#ifdef  _WIN32
static FilePtr new_FilePtr(const wchar_t* strName, int bRead) {
	FilePtr ptr = FilePtr(_wfopen(strName, bRead ? L"rb" : L"wb"),
		[](FILE* file) {
			if (file) {
				fclose(file);
				file = nullptr;
			}
		}
	);
	return ptr;
}
#endif //  _WIN32

static FilePtr new_FilePtr(const char* strName, int bRead) {
	FilePtr ptr = FilePtr(fopen(strName, bRead ? "rb" : "wb"),
		[](FILE* file) {
			if (file) {
				fclose(file);
				file = nullptr;
			}
		}
	);
	return ptr;
}


static std::shared_ptr<uint8_t>  _ReadFile(const char* szName, int* length) {
	 // 创建一个stat结构体变量
   	struct stat fileStat;
    	* length = 0;
    	// 使用stat函数获取文件信息
    	if (stat(szName, &fileStat) == 0) {
        	// 使用st_size获取文件大小，单位为字节
        * length= fileStat.st_size;
    	} else{
		return nullptr;
	}
	FilePtr fin = new_FilePtr(szName, TRUE);
	if (fin) {
		U8Ptr pData = new_u8_ptr(* length);
		fread(pData.get(), 1, * length, fin.get());
		return pData;
	}
	return nullptr;
}

#ifdef _WIN32
static std::shared_ptr<uint8_t>  _ReadFile(const wchar_t* wszName, int* length) {
	FilePtr fin = new_FilePtr(wszName, TRUE);
	if (fin) {
		fseek(fin.get(), 0, SEEK_END);
		int size = (int)ftell(fin.get());
		if (size) {
			U8Ptr pData = new_u8_ptr(size);
			fseek(fin.get(), 0, SEEK_SET);
			fread(pData.get(), 1, size, fin.get());
			*length = size;
			return pData;
		}
	}
	return nullptr;
}

#endif

class DataBuffer {
	int m_bImplData = false;
	uint8_t* m_pBuf = nullptr;//数据区
	int m_nTotalSize = 0;//由底层申请的buffer
	int m_nSize = 0;//有效长度
public:

	virtual ~DataBuffer() {
		if (m_bImplData  && m_pBuf) {
			delete[]m_pBuf;
			m_pBuf = nullptr;
		}
	}

	//根据外部数据初始化
	void Init(uint8_t *data, int data_size) {
		m_pBuf = data;
		m_nSize = data_size;
		m_nTotalSize = data_size;
		m_bImplData = false;
	}

	//
	void ReSize(int new_size) {
		if (new_size > m_nTotalSize) {
			if (m_pBuf) {
				delete []m_pBuf;
				m_pBuf = nullptr;
			}
			m_bImplData = true;
			m_pBuf = new uint8_t[new_size];
			m_nTotalSize = new_size;
		}
		m_nSize = 0;
	}
	int GetSize() {
		return m_nSize;
	}

	uint8_t* GetBuffer() {
		return m_pBuf;
	}

	uint8_t** GetPtr() {
		return &m_pBuf;
	}

	void SetPos(int pos) {
		m_nSize = pos;
	}
	int Read(uint8_t* buf) {
		if (m_pBuf && m_nSize > 0) {
			memcpy(buf, m_pBuf, m_nSize);
			return 1;
		}
		return 0;
	}
};




#ifndef Z_BEST_COMPRESSION
#define Z_BEST_COMPRESSION 9
#endif

#ifndef Z_BEST_SPEED
#define Z_BEST_SPEED 1
#endif

struct png_write_state {
	uint8_t* buffer = nullptr;//足够长的bufffer
	int pos = 0;
};

#include "libpng/libpng.h"
static void user_write_data(png_structp png_ptr, png_bytep data, png_size_t length) {
	struct png_write_state* write_state = (struct png_write_state*)png_get_io_ptr(png_ptr);
	memcpy(write_state->buffer + write_state->pos, data, length);
	write_state->pos += (int)length;
}

static void user_flush_data(png_structp png_ptr)
{
	// libpng never calls this :(
}

#endif