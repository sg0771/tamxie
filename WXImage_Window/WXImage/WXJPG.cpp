#include <stdio.h>
#include <math.h>
#include <setjmp.h>
#include <memory>
#include <stdint.h>

#include <chrono>
#include <ctime>

#include "WXImage.h"
#include "WXImageBase.h" 
#include "FreeImage/FreeImage.h"
#include <stdarg.h>
#include <mutex>
#include <chrono>
#include <ctime>

#include <iostream>

extern uint8_t* wx_tmp[3000];

//JPEG等图像解码时是否使用LCMS2库转成正常的不带IccProfile 数据的 RGB图像
int g_bUseIccParser = 1;
void UseIccParser(int b) {
	g_bUseIccParser = b;
}

#ifndef _WIN32

static std::string   wx_exec(const char* cmd) {
	std::array<char, 128> buffer;
	std::string result;
	std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
	if (!pipe) {
		throw std::runtime_error("popen() failed!");
	}
	while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
		result += buffer.data();
	}
	return result;
}

static std::string   wx_GetMACAddress() {
	std::string result = wx_exec("cat /sys/class/net/$(ip route show default | awk '/default/ {print $5}')/address");
	return result;
}


#endif

#define WXLocker    std::recursive_mutex
#define WXAutoLock  std::lock_guard<WXLocker>
static WXLocker gLock;
static int64_t s_nMaxLog = 20000;//Max log number
static int64_t s_nCountLog = 0;//log number

//库有效期

//结束时间
static int s_end_year = 2024;
static int s_end_mon = 8;
static int s_end_day = 31;

//开始时间
static int s_begin_year = 2024;
static int s_begin_mon = 6;
static int s_begin_day = 19;


static int s_bUse = 0;

static void  WXImageLog(FREE_IMAGE_FORMAT fif, const char* msg) {
#ifdef _ANDROID
	if (s_bUse) {
		printf("%s ++ ", msg);
	}
#else
	if (s_bUse)
		printf("%s", msg);
#endif
}

//設置Key，成功返回1，失敗返回0
WXIMAGE_API int WXImage_SetKey(const char* key, int length) {
	//key OpenSSL
	//
	return 0;
//	return (int)wx_tmp[rand()%3000][15];
}

static int s_nTimeout=0;
bool IsTimeout(int year, int mouth, int data) {
	return false;
#ifndef _WIN32
	try {
		std::string strMacA= "ac:1f:6b:fc:b2:30"; //V100 开发机器 intel
		std::string strMacB = "6a:32:54:01:b1:1e"; //兴图新科编译板卡 arm64
		std::string strMacC = "56:53:bd:8d:57:55"; //兴图新科盒子 arm64

		std::string strMac = wx_GetMACAddress();
		int result = strncasecmp(strMac.c_str(), strMacA.c_str(), 16);
		if (result == 0) {
			return false;
		}else{
			result = strncasecmp(strMac.c_str(), strMacB.c_str(), 16);

			if (result == 0) {
				return false;
			}else{
				result = strncasecmp(strMac.c_str(), strMacC.c_str(), 16);

				if (result == 0) {
					return false;
				}else{
					s_nTimeout++;
					//std::cout << "Time Out A "<< std::endl;//Mac
					return true;
				}
			}
		}

	}
	catch (const std::exception& e) {
		exit(-1);
	}

#endif

	// 使用 std::chrono 获取当前时间点
	auto now = std::chrono::system_clock::now();

	// 将时间点转换为 time_t 类型
	std::time_t time1 = std::chrono::system_clock::to_time_t(now);

	// 将 time_t 转换为 tm 结构，使用 localtime 函数
	std::tm date1_tm = *std::localtime(&time1);

	// 定义两个日期
	//超过这个时间返回超时
	std::tm date2_tm = { 0, 0, 0, s_end_day, s_end_mon-1,  s_end_year - 1900 }; //结束时间 2024-08-31

	//小于这个时间返回超时
	std::tm date3_tm = { 0, 0, 0, s_begin_day, s_begin_mon - 1, s_begin_year - 1900 }; //结束时间 



	// 将 time_t 转换为 std::chrono::system_clock::time_point
	auto tp1 = std::chrono::system_clock::from_time_t(time1);
	// 将 tm 结构转换为 time_t
	std::time_t time2 = std::mktime(&date2_tm);
	auto tp2 = std::chrono::system_clock::from_time_t(time2);

	// 将 tm 结构转换为 time_t
	std::time_t time3 = std::mktime(&date3_tm);
	auto tp3 = std::chrono::system_clock::from_time_t(time3);


	// 比较时间点
	if (tp1 > tp2) {
		s_nTimeout++;
		//std::cout << "Time Out B "<< std::endl;
		return true;
	}

	if (tp1 < tp3) {
		s_nTimeout++;
		//std::cout << "Time Out C  "<< std::endl;
		return true;
	}
	return false;
}


WXIMAGE_API void WXImage_InitLibrary() {
	if (IsTimeout(100, 200, 300)) {
		exit(-1);
	}
	FreeImage_Initialise(FALSE);
}

WXIMAGE_API void WXImage_SetLog(int bUse) {
	s_bUse = bUse;
	if (s_bUse) {
		FreeImage_SetOutputMessage(WXImageLog);
	}else {
		FreeImage_SetOutputMessage(nullptr); 
	}
}

EXTERN_C void  WXLogA(const char* format, ...) {
	WXAutoLock al(gLock);
	if (s_bUse) {
		s_nCountLog++;
		if (s_nCountLog >= s_nMaxLog) {
			s_bUse = false;
			return;
		}
		static char    szMsg[4096];
		memset(szMsg, 0, 4096);
		va_list marker
#ifdef _WIN32
			= nullptr
#endif
			;
		va_start(marker, format);
		vsprintf(szMsg, format, marker);
		va_end(marker);
		
		printf("++ %s", szMsg);
	}
}

#ifdef _WIN32
EXTERN_C void  WXLogW(const wchar_t* format, ...) {
	WXAutoLock al(gLock);
	if (s_bUse) {
		s_nCountLog++;
		if (s_nCountLog >= s_nMaxLog) {
			s_bUse = false;
			return;
		}

		wchar_t wszMsg[4096];
		memset(wszMsg, 0, 4096 * 2);
		va_list marker
#ifdef _WIN32
			= nullptr
#endif
			;
		va_start(marker, format);
		vswprintf(wszMsg,
#ifndef _WIN32
			4096,
#endif
			format, marker);

		va_end(marker);
		wprintf(wszMsg);
	}
}
#endif

WXIMAGE_API void* HandlerCreate() {
	DataBuffer* obj = new DataBuffer;
	return (void*)obj;
}

WXIMAGE_API int    HandlerGetSize(void* handle) {
	if (handle) {
		DataBuffer* obj = (DataBuffer*)handle;
		return obj->GetSize();
	}
	return 0;
}

WXIMAGE_API int   HandlerGetData(void* handle, uint8_t* buf) {
	if (handle) {
		DataBuffer* obj = (DataBuffer*)handle;
		return obj->Read(buf);
	}
	return 0;
}

WXIMAGE_API int   HandlerDestroy(void* handle) {
	if (handle) {
		DataBuffer* obj = (DataBuffer*)handle;
		delete obj;
	}
	return 0;
}

WXIMAGE_API int   Image_Encode(void* handle, int target_type, int quality,
	int rgb_type, const uint8_t* rgb_buffer, int width, int height, int stride,
	int DotsPerMeterX, int DotsPerMeterY, const uint8_t* icc_data /*= NULL*/, int icc_size/* = 0*/) {
	return  Image_Encode2( handle,  target_type,  quality,
		 rgb_type,  rgb_buffer,  width,  height,  stride,
		 DotsPerMeterX,  DotsPerMeterY, icc_data,  icc_size,  NULL);
}

//功能:图像内存编码
//返回值表示编码长度
//[target_type]: 输出文件压缩类型，参加WXIMAGE_TYPE_等值
//[quality]: 在JPEG/WEBP表示编码质量，默认75,范围0-100
//在PNG 中表示zlib 压缩系数 10 表示 9 ， 100 表示1， 一般来说quality越大文件越大
//也就是 (level +1)*10
//[rgb_type] :表示rgb数据类型
//[rgb_buffer]： 输入数据指针
//[width] :输入分辨率宽度
//[height]: 输入分辨率高度
//[stride]: 每行数据字节数量
//[DotsPerMeterX]: DPI信息，72DPI对应 2835，新增参数
//[DotsPerMeterY]: DPI信息，72DPI对应 2835，新增参数
//[icc_data] : ICC Profile 数据
//[icc_size] : ICC Profile 数据长度
//[bitmap]: 原图像信息，可能含有EXIF等
WXIMAGE_API int   Image_Encode2(void* handle, int target_type, int quality,
	int rgb_type, const uint8_t* rgb_buffer, int width, int height, int stride,
	int DotsPerMeterX, int DotsPerMeterY, const uint8_t* icc_data /*= NULL*/, int icc_size/* = 0*/, void* bitmap/*= NULL*/) {

	int err = 0;
	if (handle == NULL) {
		WXLogA( "%s handle=NULL  failed \n", __FUNCTION__);
		err = WXIMAGE_STATUS_ERROR_HANDLE;
	}
	else if (quality < 0 || quality > 100) { //mozjpeg
		WXLogA( "%s quality=%d  failed \n", __FUNCTION__, quality);
		err = WXIMAGE_STATUS_ERROR_QUALITY;
	}
	else if (rgb_type != 1 && rgb_type != 3 && rgb_type != 4) {
		WXLogA( "%s rgb_type=%d  failed \n", __FUNCTION__, rgb_type);
		err = WXIMAGE_STATUS_ERROR_RGB_TYPE;
	}
	else if (rgb_buffer == NULL) {
		WXLogA( "%s rgb_buffer=NULL  failed\n", __FUNCTION__);
		err = WXIMAGE_STATUS_ERROR_RGB_BUFFER;
	}
	else if (width <= 0 ) {
		WXLogA( "Size Error %s Size=[%d,%d] failed\n", __FUNCTION__, width, height);
		err = WXIMAGE_STATUS_ERROR_DST_WIDTH;
	}
	else if (height <= 0) {
		WXLogA("Size Error %s Size=[%d,%d] failed\n", __FUNCTION__, width, height);
		err = WXIMAGE_STATUS_ERROR_DST_HEIGHT;
	}
	else if (stride < width * rgb_type) {
		WXLogA( " Encode Erro %s stride=%d width=%d type=%d failed\n", __FUNCTION__,
			stride, width, rgb_type);
		err = WXIMAGE_STATUS_ERROR_DST_STRIDE;
	}
	else if (icc_data != 0 && icc_size <= 0) {
		WXLogA( "Icc profile error %s icc_data=%p icc_size=%d  failed \n",
			__FUNCTION__, icc_data, icc_size);
		err = WXIMAGE_STATUS_ERROR_ICC;
	}
	else if (target_type > WXIMAGE_TYPE_MOZJPEG || target_type < WXIMAGE_TYPE_JPEG) {
		WXLogA( "%s nImgType=%d  failed\n", __FUNCTION__, target_type);
		err = WXIMAGE_STATUS_ERROR_DST_IMAGETYPE;
	}
	else if (DotsPerMeterX <= 0 || DotsPerMeterY <= 0) { //DPI  Ϣ   ô   
		if (DotsPerMeterX == 0 && DotsPerMeterY == 0) {
			WXLogA("%s dpiX=%d dpiY=%d failed change Default\n", __FUNCTION__, DotsPerMeterX, DotsPerMeterY);
			DotsPerMeterX = 2835;
			DotsPerMeterY = 2835;
		}

		//err = WXIMAGE_STATUS_ERROR_DPI;
	}

	if (err != 0) {
		WXLogA( "ERROR=%d\n", err);
		return err;
	}

	FIBITMAP *dib = FreeImage_Allocate(width, height, rgb_type * 8);

	uint8_t *pBuf = FreeImage_GetBits(dib);
	int nStride = FreeImage_GetPitch(dib);
	for (int i = 0; i < height; i++){
		memcpy(pBuf + i * nStride, rgb_buffer + (height - 1 - i) * stride, width * rgb_type); 
	}

	if (target_type == WXIMAGE_TYPE_JPEG && rgb_type == TYPE_BGRA) {
		FIBITMAP *new_dib = FreeImage_ConvertTo24Bits(dib);
		FreeImage_Unload(dib);
		dib = new_dib;
	}

	if (target_type == WXIMAGE_TYPE_WEBP && rgb_type == TYPE_GRAY) {
		FIBITMAP *new_dib = FreeImage_ConvertTo24Bits(dib);
		FreeImage_Unload(dib);
		dib = new_dib;
	}

	if (icc_data != nullptr && icc_size > 0) {
		FreeImage_CreateICCProfile(dib, (void*)icc_data, icc_size);
	}

	FreeImage_SetDotsPerMeterX(dib, DotsPerMeterX);
	FreeImage_SetDotsPerMeterY(dib, DotsPerMeterY);

	if (bitmap) {
		FreeImage_CloneMetadata(dib, (FIBITMAP*)bitmap);//
	}

	FREE_IMAGE_FORMAT format = FIF_JPEG;

	int _level = quality;
	if (target_type == WXIMAGE_TYPE_JPEG) {
		format = FIF_JPEG;
	}
	if (target_type == WXIMAGE_TYPE_MOZJPEG) {
		format = FIF_JPEG;
		_level = 100 + quality;
	}
	else if (target_type == WXIMAGE_TYPE_PNG) {
		format = FIF_PNG;
		_level = (100 - quality) / 10;
		if (_level == 0)
			_level = 1;
		if (_level > 9)
			_level = 9;
	}
	else if (target_type == WXIMAGE_TYPE_WEBP) {
		format = FIF_WEBP;
	}

	dib->m_stream = FreeImage_OpenMemory(NULL, 0);
	int ret = FreeImage_SaveToMemory(format, dib, dib->m_stream, _level);
	BYTE *mem_buffer = NULL;
	DWORD size_in_bytes = 0;
	if (ret) {
		// Load image data
		FreeImage_AcquireMemory(dib->m_stream, &mem_buffer, &size_in_bytes);
		if (size_in_bytes > 0) {
			DataBuffer * obj = (DataBuffer*)handle;
			obj->ReSize(size_in_bytes);
			obj->SetPos(size_in_bytes);
			memcpy(obj->GetBuffer(), mem_buffer, size_in_bytes);
		}
	}
	FreeImage_Unload(dib);
	return size_in_bytes;
}



//重写RGBtoJPG编码
WXIMAGE_API int  RGBtoJPG(void* handle, int quality,
	int rgb_type, const uint8_t *rgb_buffer, int width, int height, int stride,
	const uint8_t *icc_data /*= NULL*/, int icc_size/* = 0*/) {

	if (IsTimeout(0,100,200)) {
		return WXIMAGE_STATUS_TIMEOUT;
	}
	int err = 0;
	if (handle == NULL) {
		WXLogA("%s handle=NULL  failed \n", __FUNCTION__);
		err = WXIMAGE_STATUS_ERROR_HANDLE;
	}
	else if (quality < 0 || quality > 200) {
		WXLogA("%s quality=%d  failed \n", __FUNCTION__, quality);
		err = WXIMAGE_STATUS_ERROR_QUALITY;
	}
	else if (rgb_type != 1 && rgb_type != 3 && rgb_type != 4) {
		WXLogA("%s rgb_type=%d  failed \n", __FUNCTION__, rgb_type);
		err = WXIMAGE_STATUS_ERROR_RGB_TYPE;
	}
	else if (rgb_buffer == NULL) {
		WXLogA("%s rgb_buffer=NULL  failed\n", __FUNCTION__);
		err = WXIMAGE_STATUS_ERROR_RGB_BUFFER;
	}
	else if (width <= 0) {
		WXLogA("Size Error %s Size=[%d,%d] failed\n", __FUNCTION__, width, height);
		err = WXIMAGE_STATUS_ERROR_DST_WIDTH;
	}
	else if (height <= 0) {
		WXLogA("Size Error %s Size=[%d,%d] failed\n", __FUNCTION__, width, height);
		err = WXIMAGE_STATUS_ERROR_DST_HEIGHT;
	}
	else if (stride < width * rgb_type) {
		WXLogA(" Encode Erro %s stride=%d width=%d type=%d failed\n", __FUNCTION__,
			stride, width, rgb_type);
		err = WXIMAGE_STATUS_ERROR_DST_STRIDE;
	}
	else if (icc_data != 0 && icc_size <= 0) {
		WXLogA("Icc profile error %s icc_data=%p icc_size=%d  failed \n",
			__FUNCTION__, icc_data, icc_size);
		err = WXIMAGE_STATUS_ERROR_ICC;
	}
	//else if (target_type > WXIMAGE_TYPE_WEBP || target_type < WXIMAGE_TYPE_JPEG) {
	//	WXLogA("%s nImgType=%d  failed\n", __FUNCTION__, target_type);
	//	err = WXIMAGE_STATUS_ERROR_DST_IMAGETYPE;
	//}
	//else if (DotsPerMeterX <= 0 || DotsPerMeterY <= 0) { //DPI  Ϣ   ô   
	//	WXLogA("%s dpiX=%d dpiY=%d failed\n", __FUNCTION__, DotsPerMeterX, DotsPerMeterY);
	//	err = WXIMAGE_STATUS_ERROR_DPI;
	//}

	if (err != 0) {
		WXLogA("ERROR=%d\n", err);
		return err;
	}

	int outsize = Image_Encode(handle, WXIMAGE_TYPE_JPEG, quality,
		rgb_type, rgb_buffer, width, height, stride, 2835, 2835,
		icc_data, icc_size);
	return outsize;
}


//重写RGBtoPNG编码
WXIMAGE_API int   RGBtoPNG(void* handle, int level,
	int rgb_type, const uint8_t* rgb_buffer, int width, int height, int stride,
	const uint8_t* icc_data /*= NULL*/, int icc_size/* = 0*/)//ICC数据
{

	if (IsTimeout(0, 100, 200)) {
		return WXIMAGE_STATUS_TIMEOUT;
	}
	int err = 0;
	if (handle == NULL) {
		WXLogA("%s handle=NULL  failed \n", __FUNCTION__);
		err = WXIMAGE_STATUS_ERROR_HANDLE;
	}
	else if (level < 0 || level > 100) {
		WXLogA("%s level=%d  failed \n", __FUNCTION__, level);
		err = WXIMAGE_STATUS_ERROR_QUALITY;
	}
	else if (rgb_type != 1 && rgb_type != 3 && rgb_type != 4) {
		WXLogA("%s rgb_type=%d  failed \n", __FUNCTION__, rgb_type);
		err = WXIMAGE_STATUS_ERROR_RGB_TYPE;
	}
	else if (rgb_buffer == NULL) {
		WXLogA("%s rgb_buffer=NULL  failed\n", __FUNCTION__);
		err = WXIMAGE_STATUS_ERROR_RGB_BUFFER;
	}
	else if (width <= 0) {
		WXLogA("Size Error %s Size=[%d,%d] failed\n", __FUNCTION__, width, height);
		err = WXIMAGE_STATUS_ERROR_DST_WIDTH;
	}
	else if (height <= 0) {
		WXLogA("Size Error %s Size=[%d,%d] failed\n", __FUNCTION__, width, height);
		err = WXIMAGE_STATUS_ERROR_DST_HEIGHT;
	}
	else if (stride < width * rgb_type) {
		WXLogA(" Encode Erro %s stride=%d width=%d type=%d failed\n", __FUNCTION__,
			stride, width, rgb_type);
		err = WXIMAGE_STATUS_ERROR_DST_STRIDE;
	}
	else if ((icc_data != 0 && icc_size <= 0)) {
		WXLogA("Icc profile error %s icc_data=%p icc_size=%d  failed \n",
			__FUNCTION__, icc_data, icc_size);
		err = WXIMAGE_STATUS_ERROR_ICC;
	}
	//else if (target_type > WXIMAGE_TYPE_WEBP || target_type < WXIMAGE_TYPE_JPEG) {
	//	WXLogA("%s nImgType=%d  failed\n", __FUNCTION__, target_type);
	//	err = WXIMAGE_STATUS_ERROR_DST_IMAGETYPE;
	//}
	//else if (DotsPerMeterX <= 0 || DotsPerMeterY <= 0) { //DPI  Ϣ   ô   
	//	WXLogA("%s dpiX=%d dpiY=%d failed\n", __FUNCTION__, DotsPerMeterX, DotsPerMeterY);
	//	err = WXIMAGE_STATUS_ERROR_DPI;
	//}

	if (err != 0) {
		WXLogA("ERROR=%d\n", err);
		return err;
	}

	int outsize = Image_Encode(handle, WXIMAGE_TYPE_PNG, level,
		rgb_type, rgb_buffer, width, height, stride, 2835, 2835,
		icc_data, icc_size);
	return outsize;
}


#ifdef _WIN32
WXIMAGE_API int WXImage_WriteFileU(const wchar_t* strOutput, uint8_t* buf, int length) {
	if (IsTimeout(0,100,200)) {
		return WXIMAGE_STATUS_TIMEOUT;
	}
	FilePtr fout = new_FilePtr(strOutput, FALSE);
	if (fout) {
		fseek(fout.get(), 0, SEEK_SET);
		int ret = (int)fwrite(buf, 1, length, fout.get());
		//fclose(fout);
		return ret;
	}
	return 0;
}
#endif
WXIMAGE_API int WXImage_WriteFile(const char* strOutput, uint8_t* buf, int length) {
	if (IsTimeout(0,100,200)) {
		return WXIMAGE_STATUS_TIMEOUT;
	}
	FilePtr fout = new_FilePtr(strOutput, FALSE);
	if (fout) {
		fseek(fout.get(), 0, SEEK_SET);
		int ret = (int)fwrite(buf, 1, length, fout.get());
		//fclose(fout);
		return ret;
	}
	return 0;
}



WXIMAGE_API int       WXImage_Encode(void *ptr, int type, int quality) {
	if (IsTimeout(0,100,200)) {
		return WXIMAGE_STATUS_TIMEOUT;
	}
	if (ptr) {
		FIBITMAP *dib = (FIBITMAP*)ptr;
		if (dib->m_stream) {
			FreeImage_CloseMemory(dib->m_stream);
			dib->m_stream = NULL;
		}
		FREE_IMAGE_FORMAT format = FIF_JPEG;

		int level = quality;
		if (type == 0) {
			format = FIF_JPEG;
		}
		else if (type == 1) {
			format = FIF_PNG;
			level = (quality - 10) / 10;
			if (level == 0)
				level = 1;
			if (level > 9)
				level = 9;
		}
		else if (type == 2) {
			format = FIF_WEBP;
		}
		else {
			return -1;
		}

		dib->m_stream = FreeImage_OpenMemory(NULL, 0);
		int ret = FreeImage_SaveToMemory(format, dib, dib->m_stream, 0); 
		if (ret) {
			// Load image data
			BYTE *mem_buffer = NULL;
			DWORD size_in_bytes = 0;
			FreeImage_AcquireMemory(dib->m_stream, &mem_buffer, &size_in_bytes);
			return (int)size_in_bytes;
		}
	}
	return 0;
}

WXIMAGE_API uint8_t*  WXImage_GetEncData(void *ptr) {
	if (ptr) {
		FIBITMAP *dib = (FIBITMAP*)ptr;
		if (dib->m_stream) {
			// Load JPEG data
			BYTE *mem_buffer = NULL;
			DWORD size_in_bytes = 0;
			FreeImage_AcquireMemory(dib->m_stream, &mem_buffer, &size_in_bytes);
			return mem_buffer;
		}
	}
	return NULL;
}



#ifdef _WIN32
WXIMAGE_API void*     WXImage_LoadFromFileU(const wchar_t* wszName) {
	int src_size = 0;
	std::shared_ptr<uint8_t>src_buffer = _ReadFile(wszName, &src_size);
	if (src_size) {
		void *dib = WXImage_Load(src_buffer.get(), src_size);
		src_buffer = nullptr;
		return dib;
	}
	return NULL;
}
#endif

WXIMAGE_API void* WXImage_LoadFromFile(const char* szName) {
	int src_size = 0;
	std::shared_ptr<uint8_t>src_buffer = _ReadFile(szName, &src_size);
	if (src_size) {
		void* dib = WXImage_Load(src_buffer.get(), src_size);
		printf("WXImage_Load = %p\r\n", dib);
		src_buffer = nullptr;
		return dib;
	}
	return NULL;
}


WXIMAGE_API void *    WXImage_Load(uint8_t *data, int size) {
	if (data == NULL || size <= 0)
		return NULL;

	FIBITMAP* bitmap = NULL;
	FREE_IMAGE_FORMAT src_format = FIF_UNKNOWN;
	FIMEMORY* stream = FreeImage_OpenMemory(data, size);
	if (stream) {
		src_format = FreeImage_GetFileTypeFromMemory(stream, 0);
		if (src_format != FIF_UNKNOWN) {
			bitmap = FreeImage_LoadFromMemory(src_format, stream, 0);
		}
		FreeImage_CloseMemory(stream);
	}
	if (bitmap) {
		bitmap->m_srcFormat = src_format;
		BITMAPINFO *bi = (BITMAPINFO*)FreeImage_GetInfo(bitmap);
		int height = bi->bmiHeader.biHeight;
		if (height > 0) {
			int nHeight = height;
			bi->bmiHeader.biHeight = -bi->bmiHeader.biHeight;
			int Pitch = FreeImage_GetPitch(bitmap);
			uint8_t *pData = FreeImage_GetBits(bitmap);
			uint8_t *tmp = (uint8_t *)malloc(Pitch);
			for (int i = 0; i < nHeight / 2; i++){
				memcpy(tmp, pData + i * Pitch, Pitch);
				memcpy(pData + i * Pitch, pData + (nHeight - 1 - i) * Pitch, Pitch);
				memcpy(pData + (nHeight - 1 - i) * Pitch, tmp, Pitch);
			}
			free(tmp);
		}
	}
	return (void*)bitmap;
}



#ifdef _WIN32
WXIMAGE_API int  WXImage_SaveU(void *ptr, const wchar_t *wszFileName, int type, int quality) {
	if (IsTimeout(0,100,200)) {
		return WXIMAGE_STATUS_TIMEOUT;
	}
	if (ptr) {
		FIBITMAP *dib = (FIBITMAP*)ptr;
		int rgb_type = WXImage_GetChannel(ptr);

		FREE_IMAGE_FORMAT format = FIF_JPEG;
		int level = quality;
		if (type == 0) {
			format = FIF_JPEG;
			if (rgb_type == TYPE_BGRA) {
				FIBITMAP *new_dib = (FIBITMAP*)FreeImage_ConvertTo24Bits(dib);
				FreeImage_CopyIccProfile(new_dib, dib);
				int ret = FreeImage_SaveU(format, new_dib, wszFileName, level);
				FreeImage_Unload(new_dib);
				return ret;
			}
		}else if (type == 1) {
			format = FIF_PNG;
			level = (quality - 10) / 10;
			if (level == 0)
				level = 1;
			if (level > 9)
				level = 9;
			return FreeImage_SaveU(format, dib, wszFileName, level);
		}
		else if (type == 2) {
			format = FIF_WEBP;
			return FreeImage_SaveU(format, dib, wszFileName, level);
		}
		else {
			return -1;
		}
	}
	return 0;
}
#endif

WXIMAGE_API int WXImage_Save(void* ptr, const char* szFileName, int type, int quality) {
	if (IsTimeout(0,100,200)) {
		return WXIMAGE_STATUS_TIMEOUT;
	}
	if (ptr) {
		FIBITMAP* dib = (FIBITMAP*)ptr;
		FREE_IMAGE_FORMAT format = FIF_JPEG;
		int level = quality;
		if (type == 0) {
			format = FIF_JPEG;
		}
		else if (type == 1) {
			format = FIF_PNG;
			level = (quality - 10) / 10;
			if (level == 0)
				level = 1;
			if (level > 9)
				level = 9;
		}
		else if (type == 2) {
			format = FIF_WEBP;
		}
		else {
			return -1;
		}
		return FreeImage_Save(format, dib, szFileName, level);
	}
	return 0;
}


WXIMAGE_API uint8_t*  WXImage_GetIccData(void *ptr) {
	if (ptr) {
		FIBITMAP *dib = (FIBITMAP*)ptr;
		if (dib->m_bSrcCMYK) {
			return NULL;
		}
		FIICCPROFILE *src_iccProfile = FreeImage_GetICCProfile(dib);
		if (src_iccProfile) {
			return (uint8_t*)src_iccProfile->data;
		}
	}
	return NULL;
}

WXIMAGE_API int       WXImage_GetIccSize(void *ptr) {
	if (ptr) {
		FIBITMAP *dib = (FIBITMAP*)ptr;
		if (dib->m_bSrcCMYK) {
			return 0;
		}
		FIICCPROFILE *src_iccProfile = FreeImage_GetICCProfile(dib);
		if (src_iccProfile) {
			return src_iccProfile->size;
		}
	}
	return 0;
}

WXIMAGE_API void *    WXImage_GetInfo(void *ptr) {
	if (ptr) {
		FIBITMAP *dib = (FIBITMAP*)ptr;
		return (void*)FreeImage_GetInfo(dib);
	}
	return NULL;
}

WXIMAGE_API int       WXImage_Unload(void *ptr) {
	if (ptr) {
		FIBITMAP *dib = (FIBITMAP*)ptr;
		FreeImage_Unload(dib);
		return 1;
	}
	return 0;
}

WXIMAGE_API int       WXImage_GetWidth(void *ptr) {
	if (ptr) {
		FIBITMAP *dib = (FIBITMAP*)ptr;
		return FreeImage_GetWidth(dib);
	}
	return 0;
}

WXIMAGE_API int       WXImage_GetHeight(void *ptr) {
	if (ptr) {
		FIBITMAP *dib = (FIBITMAP*)ptr;
		return FreeImage_GetHeight(dib);
	}
	return 0;
}

WXIMAGE_API int       WXImage_GetChannel(void *ptr) {
	if (ptr) {
		FIBITMAP *dib = (FIBITMAP*)ptr;
		return FreeImage_GetBPP(dib) / 8;
	}
	return 0;
}

WXIMAGE_API int       WXImage_GetPitch(void *ptr) {
	if (ptr) {
		FIBITMAP *dib = (FIBITMAP*)ptr;
		return FreeImage_GetPitch(dib);
	}
	return 0;
}

WXIMAGE_API uint8_t*     WXImage_GetBits(void *ptr) {
	if (ptr) {
		FIBITMAP *dib = (FIBITMAP*)ptr;
		return FreeImage_GetBits(dib);
	}
	return nullptr;
}


WXIMAGE_API int   WXImage_GetDotsPerMeterX(void* ptr) {
	if (ptr) {
		FIBITMAP* dib = (FIBITMAP*)ptr;
		int res = FreeImage_GetDotsPerMeterX(dib);
		if (res == 0)
			res = 2835;
		return res;
	}
	return 0;
}
WXIMAGE_API int   WXImage_GetDotsPerMeterY(void* ptr) {
	if (ptr) {
		FIBITMAP* dib = (FIBITMAP*)ptr;
		int res = FreeImage_GetDotsPerMeterY(dib);
		if (res == 0)
			res = 2835;
		return res;
	}
	return 0;
}
WXIMAGE_API void  WXImage_SetDotsPerMeterX(void* ptr, int res) {
	if (ptr) {
		FIBITMAP* dib = (FIBITMAP*)ptr;
		FreeImage_SetDotsPerMeterX(dib, res);
	}
}
WXIMAGE_API void  WXImage_SetDotsPerMeterY(void* ptr, int res) {
	if (ptr) {
		FIBITMAP* dib = (FIBITMAP*)ptr;
		FreeImage_SetDotsPerMeterY(dib, res);
	}
}


WXIMAGE_API int   WXImage_GetImageType(void* ptr) {
	if (ptr) {
		FIBITMAP* dib = (FIBITMAP*)ptr;
		int format = dib->m_srcFormat;
		if (format == FIF_JPEG) {
			return WXIMAGE_TYPE_JPEG;
		}else  if (format == FIF_PNG) {
			return WXIMAGE_TYPE_PNG;
		}else   if (format == FIF_WEBP) {
			return WXIMAGE_TYPE_WEBP;
		}
	}
	return WXIMAGE_TYPE_UNKNOWN;
}
//------------------------------------------------------------------


 //获取Image buffer 中的 icc profile
WXIMAGE_API int ReadICC(uint8_t* icc_data, const uint8_t* buffer, int length) {
	if (IsTimeout(0, 100, 200)) {
		return WXIMAGE_STATUS_TIMEOUT;
	}
	int ret = 0;
	//从内存buffer解码图像
	std::shared_ptr<void>bitmap = std::shared_ptr<void>(
		WXImage_Load((uint8_t*)buffer, length),
		[](void* p) {  if (p) { WXImage_Unload(p); p = nullptr; } });

	if (bitmap.get() == nullptr) {
		WXLogA("%s WXIMAGE_STATUS_ERROR_INPUT_DECODE\r\n", __FUNCTION__);
		return WXIMAGE_STATUS_ERROR_INPUT_DECODE; //解码错误
	}
	else {
		uint8_t* _pIccData = WXImage_GetIccData(bitmap.get());//ICC 数据
		int _nIccSize = WXImage_GetIccSize(bitmap.get());//ICC 数据

		if (_nIccSize > 0 && icc_data) {
			memcpy(icc_data, _pIccData, _nIccSize);
		}
		ret = _nIccSize;
	}
	return ret;
}