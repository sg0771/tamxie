/*
PNG、JPG处理
*/
#ifndef _WXIMAGE_H_
#define _WXIMAGE_H_

#include <stdint.h>
#include <stddef.h>

#ifndef EXTERN_C
#ifdef __cplusplus
#define EXTERN_C    extern "C"
#else
#define EXTERN_C    extern
#endif
#endif

#ifdef _WIN32
#ifdef WXIMAGE_EXPORTS
#define WXIMAGE_API  EXTERN_C __declspec(dllexport)
#else
#define WXIMAGE_API  EXTERN_C __declspec(dllimport)
#endif
#else
#define    WXIMAGE_API EXTERN_C  __attribute__((visibility ("default"))) 
#endif

#ifdef _ANDROID
#define WXIMAGE_API
#endif 

/* RGB数据类型 rgb_type */
#define TYPE_GRAY 1
#define TYPE_BGR  3
#define TYPE_BGRA 4

/* 编码类型 */
//未知格式
#define WXIMAGE_TYPE_UNKNOWN  -1 

// 先按文件名后缀设置输出编码类型
// 如果不支持再按解码类型设置输出编码类型
// 如果输入格式和输出格式不匹配时
// 指定输出大小时带透明度用WEBP
// 指定输出参数时带同名都用PNG
// 不带透明度用JPEG压缩
#define WXIMAGE_TYPE_ORIGINAL  0 

//指定JPEG编码
#define WXIMAGE_TYPE_JPEG      1

//指定PNG编码
#define WXIMAGE_TYPE_PNG       2

//指定WEBP编码
#define WXIMAGE_TYPE_WEBP      3 

//指定PNG8编码
#define WXIMAGE_TYPE_PNG8      4 

//指定MOZJPEG编码
#define WXIMAGE_TYPE_MOZJPEG   5


#define WXIMAGE_TYPE_MAX       WXIMAGE_TYPE_MOZJPEG


//编码信息和返回值

//压缩成功

//压缩成功，返回JPEG类型
#define WXIMAGE_STATUS_JPEG      WXIMAGE_TYPE_JPEG

//压缩成功，返回JPEG类型
#define WXIMAGE_STATUS_PNG       WXIMAGE_TYPE_PNG

//压缩成功，返回WEBP类型
#define WXIMAGE_STATUS_WEBP      WXIMAGE_TYPE_WEBP

#define ERR_SIZE  10

//指定大小压缩JPEG，压缩长度比预设长度大
#define WXIMAGE_STATUS_JPEG_ERR_SIZE      (WXIMAGE_TYPE_JPEG+ERR_SIZE)

//指定大小压缩WEBP，压缩长度比预设长度大
#define WXIMAGE_STATUS_WEBP_ERR_SIZE      (WXIMAGE_TYPE_WEBP+ERR_SIZE)

//压缩成功
#define WXIMAGE_STATUS_SUCCESSED      0

//压缩错误
//输入文件为空
#define WXIMAGE_STATUS_ERROR_INPUT_NULL    -21
//输入文件不可读
#define WXIMAGE_STATUS_ERROR_INPUT_READ    -22
//输入文件解码错误
#define WXIMAGE_STATUS_ERROR_INPUT_DECODE  -23

//输出文件为空
#define WXIMAGE_STATUS_ERROR_OUTPUT_NULL   -24
//输出文件创建失败
#define WXIMAGE_STATUS_ERROR_OUTPUT_CREATE -25

//设置目标大小错误
#define WXIMAGE_STATUS_ERROR_TARGET_SIZE    -26

//设置格式错误
#define WXIMAGE_STATUS_ERROR_DST_IMAGETYPE   -27

//设置宽度错误
#define WXIMAGE_STATUS_ERROR_DST_WIDTH      -28

//设置高度错误
#define WXIMAGE_STATUS_ERROR_DST_HEIGHT     -29

//编码错误
#define WXIMAGE_STATUS_ERROR_OUTPUT_ENCODE  -30


//quality 设置错误
#define WXIMAGE_STATUS_ERROR_QUALITY -31


//设置DPI错误
#define WXIMAGE_STATUS_ERROR_DPI     -32

//设置ICC错误
#define WXIMAGE_STATUS_ERROR_ICC     -33

//设置Stride错误
#define WXIMAGE_STATUS_ERROR_DST_STRIDE      -34


//设置rgb_type错误
#define WXIMAGE_STATUS_ERROR_RGB_TYPE      -35

//设置rgb buffer错误
#define WXIMAGE_STATUS_ERROR_RGB_BUFFER      -36

//编码缓存区设置错误
#define WXIMAGE_STATUS_ERROR_HANDLE     -37


//有效期超时
#define WXIMAGE_STATUS_TIMEOUT     -40


//未知错误
#define WXIMAGE_STATUS_ERROR_UNKNOWN  -100


//功能: 库调用时的初始化
WXIMAGE_API void WXImage_InitLibrary();


//JPEG等图像解码时是否使用LCMS2库转成正常的不带IccProfile 数据的 RGB图像
WXIMAGE_API void UseIccParser(int b);

//功能: 设置是否使用日志打印，默认不打印
//参数:
//[bUse]: 1使用，0不使用
WXIMAGE_API void WXImage_SetLog(int bUse);
	
//功能: 构造数据对象
//返回值: 数据对象
WXIMAGE_API void* HandlerCreate();

//功能:获取数据对象处理数据长度
//说明:调用者在外部申请足够长度的内存，然后通过HandlerGetData拷贝数据
//参数:
//[handle]:数据对象
//返回值:数据对象有效数据长度
WXIMAGE_API int    HandlerGetSize(void* handle);

//功能:获取数据对象中的处理数据
//参数:
//[handle]:数据对象
//[buf]:外部申请用来拷贝数据对象内部数据的内存
//返回值:数据对象有效数据长度
WXIMAGE_API int    HandlerGetData(void* handle, uint8_t* buf);

//功能:销毁数据对象
//参数:
//[handle]:数据对象
//返回值: 无意义
WXIMAGE_API int    HandlerDestroy(void* handle);


//功能:设置按文件大小压缩图像的quality
//[nMinQuality]:默认是20
//[nDefaultQuality]:默认75
//[nMaxQuality]:默认是100
//返回值:大于等于表示设置成功, 小于0表示失败
WXIMAGE_API int SetQuality(int nMinQuality, int nDefaultQuality, int nMaxQuality);


//功能: 将图像文件数据压缩按指定编码参数进行压缩
//参数:
//[buf]: 图像Buffer
//[buf_size]:  图像Buffer长度
//[handle]: 输出Buffer
//[target_type]: 输出文件压缩类型，参加WXIMAGE_TYPE_等值
//[target_level]: 压缩参数，
// JPEG/WEBP 为quality(值越大输出文件越大)，
// PNG 为底层压缩参数(值越大输出文件越大，编码越快)，范围0-100
//[dst_width]: 指定输出文件分辨率宽度，0 为原始宽度
//[dst_height]: 指定输出文件分辨率高度，0 为原始高度
//返回值: 小于0表示失败,参看WXIMAGE_STATUS_ERROR_ 等值
//大于0表示，编码正常
//大于10表示编码正常，但是编码数据比原文件大
WXIMAGE_API int CompressQuality_BufferToBuffer(uint8_t* buf, int buf_size,
	void* handle,
	int target_type, int target_level, int dst_width, int dst_height);


//功能，将图像文件压缩按指定编码参数level的JPEG/WEBP/PNG文件
//参数:
//[strInput]: 输入文件路径
//[strOutput]: 输出文件路径
//[target_type]: 输出文件压缩类型，参加WXIMAGE_TYPE_等值
//[target_level]: 压缩参数，范围0-100
// JPEG/WEBP 为quality(值越大输出文件越大)，
// PNG 为底层压缩参数(值越大输出文件越大，编码越快)
//[dst_width]: 指定输出文件分辨率宽度，0 为原始宽度
//[dst_height]: 指定输出文件分辨率高度，0 为原始高度
//返回值: 小于0表示失败,参看WXIMAGE_STATUS_ERROR_ 等值
//大于0表示，编码正常
//大于10表示编码正常，但是编码数据比原文件大
WXIMAGE_API int CompressQuality_FileToFile(const char* strInput, const char* strOutput,
	int target_type, int target_level, int dst_width, int dst_height);

//功能: 指定参数图像压缩，文件->内存
//参数:
//[strInput]: 输入文件路径
//[handle]: 输出句柄
//[target_type]: 输出文件压缩类型，参加WXIMAGE_TYPE_等值
//[target_level]: 压缩参数，范围0-100
// JPEG/WEBP 为quality(值越大输出文件越大)，
// PNG 为底层压缩参数(值越大输出文件越大，编码越快)
//[dst_width]: 指定输出文件分辨率宽度，0 为原始宽度
//[dst_height]: 指定输出文件分辨率高度，0 为原始高度
//返回值: 小于0表示失败,参看WXIMAGE_STATUS_ERROR_ 等值
//大于0表示，编码正常
//大于10表示编码正常，但是编码数据比原文件大
WXIMAGE_API int CompressQuality_FileToBuffer(const char* strInput, void* handle,
	int target_type, int target_level, int dst_width, int dst_height);


//功能: 指定参数图像压缩，内存->文件
//参数:
//[buf]: 图像Buffer
//[buf_size]:  图像Buffer长度
//[strOutput]: 输出文件路径
//[target_type]: 输出文件压缩类型，参加WXIMAGE_TYPE_等值
//[target_level]: 压缩参数，范围0-100
// JPEG/WEBP 为quality(值越大输出文件越大)，
// PNG 为底层压缩参数(值越大输出文件越大，编码越快)
//[dst_width]: 指定输出文件分辨率宽度，0 为原始宽度
//[dst_height]: 指定输出文件分辨率高度，0 为原始高度
//返回值: 小于0表示失败,参看WXIMAGE_STATUS_ERROR_ 等值
//大于0表示，编码正常
//大于10表示编码正常，但是编码数据比原文件大
WXIMAGE_API int CompressQuality_BufferToFile(uint8_t* buf, int buf_size, const char* strOutput,
	int target_type, int target_level, int dst_width, int dst_height);

//-----------核心功能------------
//功能:将输入图像内存数据压缩成接近指定大小的Jpeg/Webp数据流
//参数:
//[buf]: 图像数据
//[buf_size]:图像数据长度
//[handle]: 输出数据句柄，就是HandlerCreate返回值
//[target_type]: 输出文件压缩类型，参加WXIMAGE_TYPE_等值
//[target_size]: 指定输出文件大小，单位KByte，如果为0，使用默认quality=75编码
//[dst_width]: 指定输出文件分辨率宽度，0 为原始宽度
//[dst_height]: 指定输出文件分辨率高度，0 为原始高度
//返回值: 小于0表示失败,参看WXIMAGE_STATUS_ERROR_ 等值
//大于0表示，编码正常
//大于10表示编码正常，但是编码数据比原文件大
WXIMAGE_API int CompressSize_BufferToBuffer(uint8_t* buf, int buf_size, void* handle,
	int target_type, int target_size, int dst_width, int dst_height);

//功能，将输入图像压缩成接近指定大小的Jpeg/Webp文件
//参数:
//[strInput]: 输入文件路径
//[strOutput]: 输出文件路径
//[target_type]: 输出文件压缩类型，参加WXIMAGE_TYPE_等值
//[target_size]: 指定输出文件大小，单位KByte，如果为0，使用默认quality=75编码
//[dst_width]: 指定输出文件分辨率宽度，0 为原始宽度
//[dst_height]: 指定输出文件分辨率高度，0 为原始高度
//返回值: 小于0表示失败,参看WXIMAGE_STATUS_ERROR_ 等值
//大于0表示，编码正常
//大于10表示编码正常，但是编码数据比原文件大
WXIMAGE_API int CompressSize_FileToFile(const char* strInput, const char* strOutput,
	int target_type, int target_size, int dst_width, int dst_height);


//功能，将输入图像内存数据压缩成接近指定大小的Jpeg/Webp文件
//参数:
//[buf]: 图像数据
//[buf_size]:图像数据长度
//[strOutput]: 输出文件路径
//[target_type]: 输出文件压缩类型，参加WXIMAGE_TYPE_等值
//[target_size]: 指定输出文件大小，单位KByte，如果为0，使用默认quality=75编码
//[dst_width]: 指定输出文件分辨率宽度，0 为原始宽度
//[dst_height]: 指定输出文件分辨率高度，0 为原始高度
//返回值: 小于0表示失败,参看WXIMAGE_STATUS_ERROR_ 等值
//大于0表示，编码正常
//大于10表示编码正常，但是编码数据比原文件大
WXIMAGE_API int CompressSize_BufferToFile(uint8_t* buf, int buf_size, const char* strOutput,
	int target_type, int target_size, int dst_width, int dst_height);


//功能，将输入图像压缩成接近指定大小的Jpeg/Webp文件
//参数:
//[strInput]: 输入文件路径
//[handle]: 输出数据对象，HandleCreate的返回值
//[target_type]: 输出文件压缩类型，参加WXIMAGE_TYPE_等值
//[target_size]: 指定输出文件大小，单位KByte，如果为0，使用默认quality=75编码
//[dst_width]: 指定输出文件分辨率宽度，0 为原始宽度
//[dst_height]: 指定输出文件分辨率高度，0 为原始高度
//返回值: 小于0表示失败,参看WXIMAGE_STATUS_ERROR_ 等值
//大于0表示，编码正常
//大于10表示编码正常，但是编码数据比原文件大
WXIMAGE_API int CompressSize_FileToBuffer(const char* strInput, void* handle,
	int target_type, int target_size, int dst_width, int dst_height);


#ifdef _WIN32
//功能，将图像文件压缩按指定编码参数level的JPEG/WEBP/PNG文件
//参数:
//[strInput]: 输入文件路径
//[strOutput]: 输出文件路径
//[target_type]: 输出文件压缩类型，参加WXIMAGE_TYPE_等值
//[target_level]: 压缩参数，范围0-100
// JPEG/WEBP 为quality(值越大输出文件越大)，
// PNG 为底层压缩参数(值越大输出文件越大，编码越快)
//[dst_width]: 指定输出文件分辨率宽度，0 为原始宽度
//[dst_height]: 指定输出文件分辨率高度，0 为原始高度
//返回值: 小于0表示失败,参看WXIMAGE_STATUS_ERROR_ 等值
//大于0表示，编码正常
//大于10表示编码正常，但是编码数据比原文件大
WXIMAGE_API int CompressQualityU_FileToFile(const wchar_t* strInput, const wchar_t* strOutput,
	int target_type, int target_level, int dst_width, int dst_height);

//功能: 指定参数图像压缩，文件->内存
//参数:
//[strInput]: 输入文件路径
//[handle]: 输出句柄
//[target_type]: 输出文件压缩类型，参加WXIMAGE_TYPE_等值
//[target_level]: 压缩参数，范围0-100
// JPEG/WEBP 为quality(值越大输出文件越大)，
// PNG 为底层压缩参数(值越大输出文件越大，编码越快)
//[dst_width]: 指定输出文件分辨率宽度，0 为原始宽度
//[dst_height]: 指定输出文件分辨率高度，0 为原始高度
//返回值: 小于0表示失败,参看WXIMAGE_STATUS_ERROR_ 等值
//大于0表示，编码正常
//大于10表示编码正常，但是编码数据比原文件大
WXIMAGE_API int CompressQualityU_FileToBuffer(const wchar_t* strInput, void* handle,
	int target_type, int target_level, int dst_width, int dst_height);

//功能: 指定参数图像压缩，内存->文件
//参数:
//[buf]: 图像Buffer
//[buf_size]:  图像Buffer长度
//[strOutput]: 输出文件路径
//[target_type]: 输出文件压缩类型，参加WXIMAGE_TYPE_等值
//[target_level]: 压缩参数，范围0-100
// JPEG/WEBP 为quality(值越大输出文件越大)，
// PNG 为底层压缩参数(值越大输出文件越大，编码越快)
//[dst_width]: 指定输出文件分辨率宽度，0 为原始宽度
//[dst_height]: 指定输出文件分辨率高度，0 为原始高度
//返回值: 小于0表示失败,参看WXIMAGE_STATUS_ERROR_ 等值
//大于0表示，编码正常
//大于10表示编码正常，但是编码数据比原文件大
WXIMAGE_API int CompressQualityU_BufferToFile(uint8_t* buf, int buf_size, const wchar_t* strOutput,
	int target_type, int target_level, int dst_width, int dst_height);

//功能，将输入图像压缩成接近指定大小的Jpeg/Webp文件
//参数:
//[strInput]: 输入文件路径
//[strOutput]: 输出文件路径
//[target_type]: 输出文件压缩类型，参加WXIMAGE_TYPE_等值
//[target_size]: 指定输出文件大小，单位KByte，如果为0，使用默认quality=75编码
//[dst_width]: 指定输出文件分辨率宽度，0 为原始宽度
//[dst_height]: 指定输出文件分辨率高度，0 为原始高度
//返回值: 小于0表示失败,参看WXIMAGE_STATUS_ERROR_ 等值
//大于0表示，编码正常
//大于10表示编码正常，但是编码数据比原文件大
WXIMAGE_API int CompressSizeU_FileToFile(const wchar_t* strInput, const wchar_t* strOutput,
	int target_type, int target_size, int dst_width, int dst_height);

//功能，将输入图像内存数据压缩成接近指定大小的Jpeg/Webp文件
//参数:
//[buf]: 图像数据
//[buf_size]:图像数据长度
//[strOutput]: 输出文件路径
//[target_type]: 输出文件压缩类型，参加WXIMAGE_TYPE_等值
//[target_size]: 指定输出文件大小，单位KByte，如果为0，使用默认quality=75编码
//[dst_width]: 指定输出文件分辨率宽度，0 为原始宽度
//[dst_height]: 指定输出文件分辨率高度，0 为原始高度
//返回值: 小于0表示失败,参看WXIMAGE_STATUS_ERROR_ 等值
//大于0表示，编码正常
//大于10表示编码正常，但是编码数据比原文件大
WXIMAGE_API int CompressSizeU_BufferToFile(uint8_t* buf, int buf_size, const wchar_t* strOutput,
	int image_type, int target_size, int dst_width, int dst_height);

//功能，将输入图像压缩成接近指定大小的Jpeg/Webp文件
//参数:
//[strInput]: 输入文件路径
//[handle]: 输出数据对象，HandleCreate的返回值
//[target_type]: 输出文件压缩类型，参加WXIMAGE_TYPE_等值
//[target_size]: 指定输出文件大小，单位KByte，如果为0，使用默认quality=75编码
//[dst_width]: 指定输出文件分辨率宽度，0 为原始宽度
//[dst_height]: 指定输出文件分辨率高度，0 为原始高度
//返回值: 小于0表示失败,参看WXIMAGE_STATUS_ERROR_ 等值
//大于0表示，编码正常
//大于10表示编码正常，但是编码数据比原文件大
WXIMAGE_API int CompressSizeU_FileToBuffer(const wchar_t* strInput, void* handle,
	int target_type, int target_size, int dst_width, int dst_height);

#endif

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
//[DotsPerMeterX]: DPI信息，72DPI对应 2835
//[DotsPerMeterY]: DPI信息，72DPI对应 2835
//[icc_data] : ICC Profile 数据
//[icc_size] : ICC Profile 数据长度
//[bitmap]: 原图像信息，可能含有EXIF等
WXIMAGE_API int   Image_Encode(void* handle, int target_type, int quality,
	int rgb_type, const uint8_t* rgb_buffer, int width, int height, int stride,
	int DotsPerMeterX, int DotsPerMeterY, const uint8_t* icc_data /*= NULL*/, int icc_size/* = 0*/);

WXIMAGE_API int   Image_Encode2(void* handle, int target_type, int quality,
	int rgb_type, const uint8_t* rgb_buffer, int width, int height, int stride,
	int DotsPerMeterX, int DotsPerMeterY, const uint8_t* icc_data /*= NULL*/, int icc_size/* = 0*/, void* bitmap/*= NULL*/);

//弃用
//功能: 将RGB数据压缩成PNG
//level设置压缩级别 0-100
//100 最快，但是文件比较大
//0 最慢，文件比较小
WXIMAGE_API int    RGBtoPNG(void* handle, int level,
	int rgb_type, const uint8_t* rgba_buffer, int width, int height, int stride,
	const uint8_t* icc_data /*= NULL*/, int icc_size/* = 0*/);

//弃用
//RGB数据压缩成JPG
//qualit:表示编码质量(0-100)
// 100最高，默认75
//数值越大生成的文件越大

WXIMAGE_API int   RGBtoJPG(void* handle, int quality,
	int rgb_type, const uint8_t* rgba_buffer, int width, int height, int stride,
	const uint8_t* icc_data /*= NULL*/, int icc_size/* = 0*/);

//功能: 获取图像内存数据中的IccProfile数据
//参数:
//[icc_data]:外部申请用来存放IccProfile数据的内存，当为NULL时返回ICC的数据长度
//[data]: 图像内存数据
//[size]: 图像内存数据长度
WXIMAGE_API int ReadICC(uint8_t* icc_data, const uint8_t* data, int size);

//基于FreeImage 的图像处理封装
//Add By Tam  2021.10.14
//图像处理，将数据写入文件

#ifdef _WIN32
//功能: 把内存数据写入文件
//参数:
//[wszName]:输出文件名
//[buf]: 内存数据
//[size]: 数据长度
//返回值: 1表示成功，0表示失败
WXIMAGE_API int        WXImage_WriteFileU(const wchar_t* wszName, uint8_t* buf, int length);

//功能: 解码图像文件
//参数:
//[wszName]:图像文件名
//返回值: 解码对象
WXIMAGE_API void*      WXImage_LoadFromFileU(const wchar_t* wszName);

//功能: 把解码对象保存为图像文件
//参数:
//[ptr]:解码对象
//[wszName]:输出文件名
//[nImgType]:编码类型
//[quality]: 编码系数
//返回值: 1表示成功，0表示失败
WXIMAGE_API int        WXImage_SaveU(void* ptr, const wchar_t* wszName, int nImgType, int quality);
#endif


//功能: 把内存数据写入文件
//参数:
//[szName]:输出文件名
//[buf]: 内存数据
//[size]: 数据长度
//返回值: 1表示成功，0表示失败
WXIMAGE_API int        WXImage_WriteFile(const char* szName, uint8_t* buf, int size);


//功能: 解码图像文件
//参数:
//[szName]:图像文件名
//返回值: 解码对象
WXIMAGE_API void*      WXImage_LoadFromFile(const char* szName);


//功能: 把解码对象保存为图像文件
//参数:
//[ptr]:解码对象
//[szName]:输出文件名
//[nImgType]:编码类型
//[quality]: 编码系数
//返回值: 1表示成功，0表示失败
WXIMAGE_API int        WXImage_Save(void* ptr, const char* szName, int nImgType, int quality);

//功能: 获取IccProfile数据
//参数:
//[ptr]:解码对象
//返回值: IccProfile数据
WXIMAGE_API int        WXImage_GetIccSize(void* ptr);


//功能: 获取IccProfile数据长度
//参数:
//[ptr]:解码对象
//返回值: IccProfile数据长度
WXIMAGE_API uint8_t*   WXImage_GetIccData(void* ptr);


//功能: 获取BITMAPINFO对象，便于GDI预览 
//参数:
//[ptr]:解码对象
//返回值: BITMAPINFO对象
WXIMAGE_API void*      WXImage_GetInfo(void* ptr);

//功能: 解码内存数据
//参数:
//[data]:图像数据
//[size]:图像数据长度
//返回值: 解码对象
WXIMAGE_API void*      WXImage_Load(uint8_t* data, int size);

//功能: 释放解码对象
//参数:
//[ptr]:解码对象
//返回值: 无意义
WXIMAGE_API int        WXImage_Unload(void* ptr);


//功能: 对解码对象进行编码操作，可以通过WXImage_GetEncData获取编码数据
//参数:
//[ptr]:解码对象
//[nImgType]: 表示编码类型
//[quality]: 在JPEG WEBP表示编码质量，默认75,
// 在PNG 中表示zlib 压缩系数 10 表示 0 ， 100 表示9
//也就是 (level +1)*10
//返回值: 表示编码长度
WXIMAGE_API int        WXImage_Encode(void* ptr, int nImgType, int quality);

//功能: WXImage_Encode 编码之后获取编码数据
//参数:
//[ptr]:解码对象
//返回值: 编码数据
WXIMAGE_API uint8_t*    WXImage_GetEncData(void* ptr);

//功能:获取解码图像的分辨率高度
//参数:
//[ptr]:解码对象
//返回值:分辨率宽度
WXIMAGE_API int         WXImage_GetWidth(void* ptr);

//功能:获取解码图像的分辨率高度
//参数:
//[ptr]:解码对象
//返回值:分辨率高度
WXIMAGE_API int         WXImage_GetHeight(void* ptr);

//功能:获取解码图像的RGB数据的channel
//参数:
//[ptr]:解码对象
//返回值:4为带透明的RGBA，3 为RGB，1 为Gray或者 RGB8
WXIMAGE_API int         WXImage_GetChannel(void* ptr);

//功能:获取解码图像的RGB数据的每行字节数
//参数:
//[ptr]:解码对象
//返回值:解码图像的RGB数据的每行字节数
WXIMAGE_API int         WXImage_GetPitch(void* ptr);


//功能: 获取图像解码数据(一般为RGB8、RGB24、RGB32数据)
//参数:
//[ptr]:解码对象
//返回值:图像解码RGB数据
WXIMAGE_API uint8_t*    WXImage_GetBits(void* ptr);


//功能: 获取原始图像的编码格式
//参数:
//[ptr]:解码对象
//返回值: WXIMAGE_TYPE_XXX 系列值
WXIMAGE_API int         WXImage_GetImageType(void* ptr);


//功能: DPI 信息的获取和设置
WXIMAGE_API int         WXImage_GetDotsPerMeterX(void* ptr);
WXIMAGE_API int         WXImage_GetDotsPerMeterY(void* ptr);
WXIMAGE_API void        WXImage_SetDotsPerMeterX(void* ptr, int res);
WXIMAGE_API void        WXImage_SetDotsPerMeterY(void* ptr, int res);

//功能:将rgb数据压缩成PNG8图像
//参数:
//[buf]:输入图像内存数据
//[buf_size]:输入图像内存数据长度
//[handle]:编码缓存区
//[target_level]:编码质量参数，0-100
//[dst_width]:指定输出宽度
//[dst_height]:指定输出高度，dst_width，dst_height同时为0时保持不变，只有一个为0按保持比例处理
//返回值: 小于0表示失败,参看WXIMAGE_STATUS_ERROR_ 等值
//大于0表示，编码正常
//大于10表示编码正常，但是编码数据比原文件大
WXIMAGE_API int CompressPNG8_BufferToBuffer(uint8_t* buf, int buf_size,
	void* handle, int quality, int dst_width, int dst_height);


//功能: rgb数据压缩成PNG8图像
//参数:
//[buf]:输入图像内存数据
//[buf_size]:输入图像内存数据长度
//[strOutput]:输出文件名
//[quality]:编码质量参数，0-100
//[dst_width]:指定输出宽度
//[dst_height]:指定输出高度，dst_width，dst_height同时为0时保持不变，只有一个为0按保持比例处理
//返回值: 大于0表示编码长度，小于零参见 WXIMAGE_SATUS_ERROR_
WXIMAGE_API int CompressPNG8_BufferToFile(uint8_t* buf, int buf_size,
	const char* strOutput, int quality, int dst_width, int dst_height);

//功能: rgb数据压缩成PNG8图像
//参数:
//[strInput]:输入图像名
//[handle]:编码缓存区
//[quality]:编码质量参数，0-100
//[dst_width]:指定输出宽度
//[dst_height]:指定输出高度，dst_width，dst_height同时为0时保持不变，只有一个为0按保持比例处理
//返回值: 小于0表示失败,参看WXIMAGE_STATUS_ERROR_ 等值
//大于0表示，编码正常
//大于10表示编码正常，但是编码数据比原文件大
WXIMAGE_API int CompressPNG8_FileToBuffer(const char* strInput,
	void* handle, int quality, int dst_width, int dst_height);


//功能: rgb数据压缩成PNG8图像
//参数:
//[strInput]:输入图像文件名
//[strOutput]:速出文件名
//[quality]:编码质量参数，0-100
//[dst_width]:指定输出宽度
//[dst_height]:指定输出高度，dst_width，dst_height同时为0时保持不变，只有一个为0按保持比例处理
//返回值: 小于0表示失败,参看WXIMAGE_STATUS_ERROR_ 等值
//大于0表示，编码正常
//大于10表示编码正常，但是编码数据比原文件大
WXIMAGE_API int CompressPNG8_FileToFile(const char* strInput,
	const char* strOutput, int quality, int dst_width, int dst_height);


#ifdef _WIN32

//PNG8 压缩功能
//功能: rgb数据压缩成PNG8图像
//参数:
//[buf]:输入图像内存数据
//[buf_size]:输入图像内存数据长度
//[strOutput]:输出文件名
//[quality]:编码质量参数，0-100
//[dst_width]:指定输出宽度
//[dst_height]:指定输出高度，dst_width，dst_height同时为0时保持不变，只有一个为0按保持比例处理
//返回值: 小于0表示失败,参看WXIMAGE_STATUS_ERROR_ 等值
//大于0表示，编码正常
//大于10表示编码正常，但是编码数据比原文件大
WXIMAGE_API int CompressPNG8U_BufferToFile(uint8_t* buf, int buf_size,
	const wchar_t* strOutput, int quality, int dst_width, int dst_height);

//PNG8 压缩功能
//功能: rgb数据压缩成PNG8图像
//参数:
//[strInput]:输入图像名
//[handle]:编码缓存区
//[quality]:编码质量参数，0-100
//[dst_width]:指定输出宽度
//[dst_height]:指定输出高度，dst_width，dst_height同时为0时保持不变，只有一个为0按保持比例处理
//返回值: 小于0表示失败,参看WXIMAGE_STATUS_ERROR_ 等值
//大于0表示，编码正常
//大于10表示编码正常，但是编码数据比原文件大
WXIMAGE_API int CompressPNG8U_FileToBuffer(const wchar_t* strInput,
	void* handle, int quality, int dst_width, int dst_height);

//PNG8 压缩功能
//功能: rgb数据压缩成PNG8图像
//参数:
//[strInput]:输入图像文件名
//[strOutput]:速出文件名
//[quality]:编码质量参数，0-100
//[dst_width]:指定输出宽度
//[dst_height]:指定输出高度，dst_width，dst_height同时为0时保持不变，只有一个为0按保持比例处理
//返回值: 小于0表示失败,参看WXIMAGE_STATUS_ERROR_ 等值
//大于0表示，编码正常
//大于10表示编码正常，但是编码数据比原文件大
WXIMAGE_API int CompressPNG8U_FileToFile(const wchar_t* strInput,
	const wchar_t* strOutput, int quality, int dst_width, int dst_height);
#endif


#ifdef _WIN32
//功能: 获取JPEG文件地Quality数值
//参数:
//[strInput]:输入图像文件名
//返回值: 编码Jpeg文件时地Quality值
WXIMAGE_API int GetJpegQualityU(const wchar_t* strInput);
#endif

//功能: 获取JPEG文件地Quality数值
//参数:
//[strInput]:输入图像文件名
//返回值: 编码Jpeg文件时地Quality值
WXIMAGE_API int GetJpegQuality(const char* strInput);

#endif
