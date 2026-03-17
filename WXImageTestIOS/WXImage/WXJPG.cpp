#include <stdio.h>
#include <math.h>
#include <setjmp.h>
#include <memory>
#include <stdint.h>

#include "./libjpeg/jpeglib.h"
#include "./libjpeg/jerror.h"
#include "./libjpeg/cdjpeg.h"

#include "WXImage.h"
#include "WXImageBase.h"  /* typedefs, common macros, public prototypes */
#include "FreeImage/FreeImage.h"
#include <stdarg.h>
#include <mutex>
//限制程序一次运行的最大日志数量，避免日志过多
//递归锁
#define WXLocker    std::recursive_mutex
//自动锁
#define WXAutoLock  std::lock_guard<WXLocker>
static WXLocker gLock;
static int64_t s_nMaxLog = 20000;//程序运行中最多的日志数量
static int64_t s_nCountLog = 0;//log 数量



static int s_bUse = 0;

static void __stdcall WXImageLog(FREE_IMAGE_FORMAT fif, const char* msg) {
	if(s_bUse)
		printf(msg);
}
WXIMAGE_API void WXImage_SetLog(int bUse) {
	s_bUse = bUse;
	if (s_bUse) {
		FreeImage_SetOutputMessage(WXImageLog); //设置内部日志
	}else {
		FreeImage_SetOutputMessage(nullptr); //取消内部日志
	}
}

EXTERN_C void  WXLogA(const char* format, ...) {
	WXAutoLock al(gLock);
	if (s_bUse) {
		s_nCountLog++;
		if (s_nCountLog >= s_nMaxLog) {
			s_bUse = false;//禁止写日志
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
		printf(szMsg);
	}
}

#ifdef _WIN32
EXTERN_C void  WXLogW(const wchar_t* format, ...) {
	WXAutoLock al(gLock);
	if (s_bUse) {
		s_nCountLog++;
		if (s_nCountLog >= s_nMaxLog) {
			s_bUse = false;//禁止写日志
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

//构造对象
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

//获取处理数据
WXIMAGE_API int   HandlerGetData(void* handle, uint8_t* buf) {
	if (handle) {
		DataBuffer* obj = (DataBuffer*)handle;
		return obj->Read(buf);
	}
	return 0;
}

//销毁对象
WXIMAGE_API int   HandlerDestroy(void* handle) {
	if (handle) {
		DataBuffer* obj = (DataBuffer*)handle;
		delete obj;
	}
	return 0;
}

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
//[DotsPerMeterX]: DPI信息，72DPI对应 2835
//[DotsPerMeterY]: DPI信息，72DPI对应 2835
//[icc_data] : ICC Profile 数据
//[icc_size] : ICC Profile 数据长度
WXIMAGE_API int   Image_Encode(void* handle, int target_type, int quality,
	int rgb_type, const uint8_t* rgb_buffer, int width, int height, int stride,
	int DotsPerMeterX, int DotsPerMeterY, const uint8_t* icc_data /*= NULL*/, int icc_size/* = 0*/) {
	//参数检查
	int err = 0;
	if (handle == NULL) {
		WXLogA( "%s handle=NULL  failed \n", __FUNCTION__);
		err = WXIMAGE_STATUS_ERROR_HANDLE;
	}
	else if (quality < 0 || quality > 100) {
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
	else if ((icc_data != 0 && icc_size <= 0) || (icc_data == 0 && icc_size > 0)) {
		WXLogA( "Icc profile error %s icc_data=%p icc_size=%d  failed \n",
			__FUNCTION__, icc_data, icc_size);
		err = WXIMAGE_STATUS_ERROR_ICC;
	}
	else if (target_type > WXIMAGE_TYPE_WEBP || target_type < WXIMAGE_TYPE_JPEG) {
		WXLogA( "%s nImgType=%d  failed\n", __FUNCTION__, target_type);
		err = WXIMAGE_STATUS_ERROR_DST_IMAGETYPE;
	}
	else if (DotsPerMeterX <= 0 || DotsPerMeterY <= 0) { //DPI信息设置错误
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


	//开始编码！！
	FIBITMAP *dib = FreeImage_Allocate(width, height, rgb_type * 8);

	uint8_t *pBuf = FreeImage_GetBits(dib);
	int nStride = FreeImage_GetPitch(dib);
	for (int i = 0; i < height; i++){
		memcpy(pBuf + i * nStride, rgb_buffer + (height - 1 - i) * stride, width * rgb_type);  //拷贝数据到新的 Bitmap
	}

	if (target_type == WXIMAGE_TYPE_JPEG && rgb_type == TYPE_BGRA) {
		//JPEG 不支持 RGB32 而是支持 CYMK
		FIBITMAP *new_dib = FreeImage_ConvertTo24Bits(dib);
		FreeImage_Unload(dib);
		dib = new_dib;
	}

	if (target_type == WXIMAGE_TYPE_WEBP && rgb_type == TYPE_GRAY) {
		//WEBP 不支持Gray！！
		FIBITMAP *new_dib = FreeImage_ConvertTo24Bits(dib);
		FreeImage_Unload(dib);
		dib = new_dib;
	}

	if (icc_data != nullptr && icc_size > 0) {
		FreeImage_CreateICCProfile(dib, (void*)icc_data, icc_size);
	}

	//设置DPI打印信息
	FreeImage_SetDotsPerMeterX(dib, DotsPerMeterX);
	FreeImage_SetDotsPerMeterY(dib, DotsPerMeterY);

	FREE_IMAGE_FORMAT format = FIF_JPEG;

	int _level = quality;
	if (target_type == WXIMAGE_TYPE_JPEG) {
		format = FIF_JPEG;
	}
	else if (target_type == WXIMAGE_TYPE_PNG) {
		//改成quality越大文件越大
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
	//设置level
	int ret = FreeImage_SaveToMemory(format, dib, dib->m_stream, _level); //编码到内存
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


void jpeg_error_exit(j_common_ptr cinfo)
{
	/* Always display the message */
	(*cinfo->err->output_message) (cinfo);
	/* Let the memory manager delete any temp files before we die */
	jpeg_destroy(cinfo);
}


//DPI 信息
WXIMAGE_API int  RGBtoJPG(void* handle, int quality,
	int rgb_type, const uint8_t *rgb_buffer, int width, int height, int stride,
	const uint8_t *icc_data /*= NULL*/, int icc_size/* = 0*/) {

	//参数检查
	//参数检查
	int err = 0;
	if (handle == NULL) {
		WXLogA("%s handle=NULL  failed \n", __FUNCTION__);
		err = WXIMAGE_STATUS_ERROR_HANDLE;
	}
	else if (quality < 0 || quality > 100) {
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
	else if ((icc_data != 0 && icc_size <= 0) || (icc_data == 0 && icc_size > 0)) {
		WXLogA("Icc profile error %s icc_data=%p icc_size=%d  failed \n",
			__FUNCTION__, icc_data, icc_size);
		err = WXIMAGE_STATUS_ERROR_ICC;
	}
	//else if (target_type > WXIMAGE_TYPE_WEBP || target_type < WXIMAGE_TYPE_JPEG) {
	//	WXLogA("%s nImgType=%d  failed\n", __FUNCTION__, target_type);
	//	err = WXIMAGE_STATUS_ERROR_DST_IMAGETYPE;
	//}
	//else if (DotsPerMeterX <= 0 || DotsPerMeterY <= 0) { //DPI信息设置错误
	//	WXLogA("%s dpiX=%d dpiY=%d failed\n", __FUNCTION__, DotsPerMeterX, DotsPerMeterY);
	//	err = WXIMAGE_STATUS_ERROR_DPI;
	//}

	if (err != 0) {
		WXLogA("ERROR=%d\n", err);
		return err;
	}

	DataBuffer* obj = (DataBuffer*)handle;
	obj->ReSize(width * height  * 4);

	struct jpeg_compress_struct toWriteInfo;
	struct jpeg_error_mgr errorMgr;
	jpeg_create_compress(&toWriteInfo);
	toWriteInfo.err = jpeg_std_error(&errorMgr);
	//注册失败的回调函数
	toWriteInfo.err->error_exit = jpeg_error_exit;

	unsigned long outsize = 0;
	jpeg_mem_dest(&toWriteInfo, obj->GetPtr(), &outsize);//输出到这里

	toWriteInfo.image_width = width;
	toWriteInfo.image_height = height;
	toWriteInfo.jpeg_width   = width;
	toWriteInfo.jpeg_height   = height;

	if (rgb_type == TYPE_GRAY) {
		toWriteInfo.input_components = 1;
		toWriteInfo.in_color_space = JCS_GRAYSCALE; 
	}
	else if (rgb_type == TYPE_BGR) {
		toWriteInfo.input_components = 3;
		toWriteInfo.in_color_space = JCS_EXT_BGR; 
	}

	jpeg_set_defaults(&toWriteInfo);
	toWriteInfo.optimize_coding = TRUE;//强制使用JPEG优化大小功能
	jpeg_set_quality(&toWriteInfo, quality, TRUE);	//设置压缩质量100表示100%
	jpeg_start_compress(&toWriteInfo, TRUE);
	//jpeg_write_icc_profile 要放在 jpeg_start_compress之后 ！！！
	if (icc_data && icc_size > 0) {
		jpeg_write_icc_profile(&toWriteInfo, icc_data, icc_size);
	}

	JSAMPROW row_pointer[1];	// 一行位图 
	for(int i = 0; i < height;i++){
		row_pointer[0] = (JSAMPROW)(rgb_buffer + i * stride);
		jpeg_write_scanlines(&toWriteInfo, row_pointer, 1);
	}
	jpeg_finish_compress(&toWriteInfo);
	jpeg_destroy_compress(&toWriteInfo);
	obj->SetPos(outsize);
	return outsize;
}


//读取JPG文件的ICC信息
//返回值为长度，数据格式错误返回-1
//不存在返回0
//icc_data 为NULL是不保存数据，只计算长度
EXTERN(int) jpeg_read_icc(uint8_t *icc_data, const uint8_t* jpeg_buffer,int length){
	//如果不是JPG格式的文件返回-1
	if (jpeg_buffer == NULL || 
		length <= 2 || 
		0xFF != (unsigned char)jpeg_buffer[0] ||
		0xD8 != (unsigned char)jpeg_buffer[1])
	{
		return -1;
	}
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);
	jpeg_mem_src(&cinfo, (unsigned char*)jpeg_buffer, length);
	jpeg_save_markers(&cinfo, JPEG_APP0 + 2, 0xFFFF);
	jpeg_read_header(&cinfo, TRUE);
	//get jpeg file info
	int src_width = cinfo.image_width;
	int src_height = cinfo.image_height;
	JOCTET *icc_profile = NULL;
	unsigned int icc_len = 0;
	int dst_length = 0;
	if (jpeg_read_icc_profile(&cinfo, &icc_profile, &icc_len)) {
		dst_length = icc_len;
		if (dst_length && icc_data) {
			memcpy(icc_data, icc_profile, dst_length);
		}
		free(icc_profile);
	}
	jpeg_destroy_decompress(&cinfo);
	return dst_length;
}


#ifdef _WIN32
WXIMAGE_API int WXImage_WriteFileU(const wchar_t* strOutput, uint8_t* buf, int length) {
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
			return -1;//参数错误
		}

		dib->m_stream = FreeImage_OpenMemory(NULL, 0);
		int ret = FreeImage_SaveToMemory(format, dib, dib->m_stream, 0); //编码到内存
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
	FIMEMORY* stream = FreeImage_OpenMemory(data, size);//构造为数据流对象
	if (stream) {
		src_format = FreeImage_GetFileTypeFromMemory(stream, 0);//获取图像类型
		if (src_format != FIF_UNKNOWN) {
			bitmap = FreeImage_LoadFromMemory(src_format, stream, 0);//解码图像
		}
		FreeImage_CloseMemory(stream);
	}
	if (bitmap) {
		bitmap->m_format = src_format;
		BITMAPINFO *bi = (BITMAPINFO*)FreeImage_GetInfo(bitmap);
		int height = bi->bmiHeader.biHeight;
		if (height > 0) { //这个时候数据直接用别的API处理会颠倒！！！比如OpenCV
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
			return -1;//参数错误
		}
	}
	return 0;
}
#endif

WXIMAGE_API int WXImage_Save(void* ptr, const char* szFileName, int type, int quality) {
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
			return -1;//参数错误
		}
		return FreeImage_Save(format, dib, szFileName, level);
	}
	return 0;
}


WXIMAGE_API uint8_t*  WXImage_GetIccData(void *ptr) {
	if (ptr) {
		FIBITMAP *dib = (FIBITMAP*)ptr;
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


//DPI 信息的获取和设置
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

//获取原始图像的编码格式
WXIMAGE_API int   WXImage_GetImageType(void* ptr) {
	if (ptr) {
		FIBITMAP* dib = (FIBITMAP*)ptr;
		int format = dib->m_format;
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