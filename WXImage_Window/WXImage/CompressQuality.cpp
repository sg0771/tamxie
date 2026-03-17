#include <stdio.h>
#include <math.h>
#include <setjmp.h>
#include <memory>
#include <stdint.h>



#include "WXImage.h"
#include "WXImageBase.h"  /* typedefs, common macros, public prototypes */
#include "FreeImage/FreeImage.h"

extern int RGBtoPNG8(DataBuffer* obj, int quality,
	uint8_t* rgb_buffer, int channel, int width, int height, int pitch,
	int res_x, int res_y);

//核心功能
//功能: 指定参数图像压缩，内存->内存
//参数:
//[buf]: 图像Buffer
//[buf_size]:  图像Buffer长度
//[handle]: 输出Buffer
//[target_type]: 输出文件压缩类型，参加WXIMAGE_TYPE_等值
//[target_level]: 压缩参数，JPEG/WEBP 为quality(值越大输出文件越大)， PNG 为底层压缩参数(值越大输出文件越小)，范围0-100
//[dst_width]: 指定输出文件分辨率宽度，0 为原始宽度
//[dst_height]: 指定输出文件分辨率高度，0 为原始高度
//返回值: 参看WXIMAGE_STATUS_ERROR_ 等值
static int _BufferToBuffer(uint8_t* buf, int buf_size,
	void* handle,
	int image_type, int target_level, int dst_width, int dst_height) {

    WXLogA("%s Begin AAA\r\n", __FUNCTION__);

	if (buf == nullptr || buf_size <= 0) {
		WXLogA("%s input is empty\r\n", __FUNCTION__);
		return WXIMAGE_STATUS_ERROR_INPUT_NULL; //参数错误
	}
	else if (handle == nullptr) {
		WXLogA("%s output is empty\r\n", __FUNCTION__);
		return WXIMAGE_STATUS_ERROR_OUTPUT_NULL;//参数错误
	}
	else if (image_type < 0 || image_type > WXIMAGE_TYPE_MAX) {
		WXLogA("%s target_type[%d] is error\r\n", __FUNCTION__, image_type);
		return WXIMAGE_STATUS_ERROR_QUALITY;//参数错误
	}
	else if (target_level < 0 || target_level > 100) {
		WXLogA("%s target_level[%d] is error\r\n", __FUNCTION__, target_level);
		return WXIMAGE_STATUS_ERROR_TARGET_SIZE;//参数错误
	}
	else if (dst_width < 0) {
		WXLogA("%s dst_width[%d] is error\r\n", __FUNCTION__, dst_width);
		return WXIMAGE_STATUS_ERROR_DST_WIDTH;//参数错误
	}
	else if (dst_height < 0) {
		WXLogA("%s dst_height[%d] is error\r\n", __FUNCTION__, dst_width);
		return WXIMAGE_STATUS_ERROR_DST_HEIGHT;//参数错误
	}

	WXLogA("%s Begin BBB \r\n", __FUNCTION__);

	//从内存buffer解码图像
	std::shared_ptr<void>bitmap = std::shared_ptr<void>(
		WXImage_Load(buf, buf_size),
		[](void* p) {  if (p) { WXImage_Unload(p); p = nullptr; } });

	WXLogA("%s Begin WXImage_Load [%d]\r\n", __FUNCTION__, buf_size );

	if (bitmap.get() == nullptr) {
		WXLogA("%s WXIMAGE_STATUS_ERROR_INPUT_DECODE\r\n", __FUNCTION__);
		return WXIMAGE_STATUS_ERROR_INPUT_DECODE; //解码错误
	}
	else {
		WXLogA("%s Decode OK \r\n", __FUNCTION__);
		//开始编码
		int   _nWidth = WXImage_GetWidth(bitmap.get()); //图像宽度
		int   _nHeight = WXImage_GetHeight(bitmap.get());//图像高度
		uint8_t* _pData = WXImage_GetBits(bitmap.get());//解码图像数据
		int      _nPitch = WXImage_GetPitch(bitmap.get());//解码图像数据每行字节数量
		int      _nTypeRGB = WXImage_GetChannel(bitmap.get());//每个像素的数据位数
		uint8_t* _pIccData = WXImage_GetIccData(bitmap.get());//ICC 数据
		int _nIccSize = WXImage_GetIccSize(bitmap.get());//ICC 数据
		int _nDotsPerMeterX = WXImage_GetDotsPerMeterX(bitmap.get());
		int _nDotsPerMeterY = WXImage_GetDotsPerMeterY(bitmap.get());
		BITMAPINFO* src_bi = (BITMAPINFO*)WXImage_GetInfo(bitmap.get());//RGB8 

		int _nDstW = dst_width;
		int _nDstH = dst_height;

		if (dst_width == 0 && dst_height == 0) {
			_nDstW = _nWidth;
			_nDstH = _nHeight;
		}
		else if (dst_width == 0 && dst_height != 0) {
			_nDstW = ((int)((double)dst_height * (double)_nWidth / (double)_nHeight + 0.5) + 1) / 2 * 2;
		}
		else if (dst_width != 0 && dst_height == 0) {
			_nDstH = ((int)((double)dst_width * (double)_nHeight / (double)_nWidth + 0.5) + 1) / 2 * 2;
		}

		std::shared_ptr< FIBITMAP> _ScaleDib;//有可能缩放处理

		uint8_t* _pDst = _pData;
		int _nDstPitch = _nPitch;

		// 缩放处理
		if (_nDstW != _nWidth || _nDstH != _nHeight) {
			//新图像
			_ScaleDib = std::shared_ptr< FIBITMAP>(
				FreeImage_Allocate(_nDstW, _nDstH, _nTypeRGB * 8), [](FIBITMAP* dib) {
					if (dib) {
						FreeImage_Unload(dib);
						dib = nullptr;
					}
				});
			_pDst = FreeImage_GetBits(_ScaleDib.get());
			_nDstPitch = FreeImage_GetPitch(_ScaleDib.get());

			if (_nTypeRGB == TYPE_GRAY) { //Gray Scale
				libyuv::ScalePlane(_pData, _nPitch, _nWidth, _nHeight,
					_pDst, _nDstPitch, _nDstW, _nDstH, libyuv::kFilterLinear);
				//颜色表拷贝
				BITMAPINFO* dst_bi = FreeImage_GetInfo(_ScaleDib.get());
				memcpy(dst_bi->bmiColors, src_bi->bmiColors, 256 * TYPE_BGRA);//

			}
			else if (_nTypeRGB == TYPE_BGR) {
				//指定分辨率输出，需要先进行数据缩放处理
				U8Ptr rgb32_ptr = new_u8_ptr(_nWidth * _nHeight * TYPE_BGRA);
				libyuv::RGB24ToARGB(_pData, _nPitch, rgb32_ptr.get(), _nWidth * TYPE_BGRA, _nWidth, _nHeight);

				//所有数据先转换到ARGB图像，然后缩放，再转换原来的格式
				U8Ptr argb_scale_ptr = new_u8_ptr(_nDstW * _nDstH * TYPE_BGRA);
				libyuv::ARGBScale(rgb32_ptr.get(), _nWidth * TYPE_BGRA, _nWidth, _nHeight,
					argb_scale_ptr.get(), _nDstW * TYPE_BGRA, _nDstW, _nDstH,
					libyuv::FilterMode::kFilterBilinear);
				libyuv::ARGBToRGB24(argb_scale_ptr.get(), _nDstW * TYPE_BGRA,
					_pDst, _nDstPitch,
					_nDstW, _nDstH);

			}
			else if (_nTypeRGB == TYPE_BGRA) {
				libyuv::ARGBScale(_pData, _nPitch, _nWidth, _nHeight,
					_pDst, _nDstPitch, _nDstW, _nDstH, libyuv::kFilterLinear);
			}
		}
		int _image_type = image_type;
		if (_image_type == WXIMAGE_TYPE_ORIGINAL) {
			//获取原文件的编码信息
			int imageTypeFormData = WXImage_GetImageType(bitmap.get());
			if (imageTypeFormData != WXIMAGE_TYPE_UNKNOWN) {
				_image_type = imageTypeFormData; //JPG PNG 或者 WEBP格式
			}else {
				if (_nTypeRGB == 4) { //带透明信息使用WEBP格式
					_image_type = WXIMAGE_TYPE_PNG;
				}else {
					_image_type = WXIMAGE_TYPE_JPEG;//不带透明信息使用JPEG编码
				}
			}
		}

		WXLogA("%s Image_Encode!!! \r\n", __FUNCTION__);
		int _image_size = 0;
		if (_image_type == WXIMAGE_TYPE_JPEG || 
			_image_type == WXIMAGE_TYPE_PNG ||
			_image_type == WXIMAGE_TYPE_WEBP ||
			_image_type == WXIMAGE_TYPE_MOZJPEG) {
			//图像编码
			_image_size = Image_Encode2(handle, _image_type, target_level,
				_nTypeRGB, _pDst, _nDstW, _nDstH, _nDstPitch,
				_nDotsPerMeterX, _nDotsPerMeterY,
				_pIccData, _nIccSize, bitmap.get());
			if (_image_size <= 0 && _pIccData && _nIccSize > 0) {
				//可能不支持该ICC
				_image_size = Image_Encode2(handle, _image_type, target_level,
					_nTypeRGB, _pDst, _nDstW, _nDstH, _nDstPitch,
					_nDotsPerMeterX, _nDotsPerMeterY,
					nullptr, 0, bitmap.get());
			}
		}
		else if (_image_type == WXIMAGE_TYPE_PNG8) {
			/*_image_size = CompressPNG8_BufferToBuffer(uint8_t * buf, int buf_size,
				handle, int quality, int dst_width, int dst_height)*/
			if (_nTypeRGB == 3 || _nTypeRGB == 4) {
				_image_size = RGBtoPNG8((DataBuffer*)handle, target_level,
					_pDst, _nTypeRGB,  _nDstW, _nDstH, _nDstPitch,
					_nDotsPerMeterX, _nDotsPerMeterY);
			}

		}
		if (_image_size > 0) { //编码成功，取出数据并写入文件
			if (_image_size > buf_size) {
				return ERR_SIZE + _image_type; //编码数据比原文件大，返回值加10
			}
			return _image_type;
		}
		else {
			WXLogA("%s output encode error\r\n", __FUNCTION__);
			return WXIMAGE_STATUS_ERROR_OUTPUT_ENCODE;
		}
	}

	WXLogA("%s unknown error\r\n", __FUNCTION__);
	return WXIMAGE_STATUS_ERROR_UNKNOWN;
}


//功能: 指定参数图像压缩，内存->内存
//参数:
//[buf]: 图像Buffer
//[buf_size]:  图像Buffer长度
//[handle]: 输出Buffer
//[target_type]: 输出文件压缩类型，参加WXIMAGE_TYPE_等值
//[target_level]: 压缩参数，JPEG/WEBP 为quality(值越大输出文件越大)， PNG 为底层压缩参数(值越大输出文件越小)，范围0-100
//[dst_width]: 指定输出文件分辨率宽度，0 为原始宽度
//[dst_height]: 指定输出文件分辨率高度，0 为原始高度
//返回值: 参看WXIMAGE_STATUS_ERROR_ 等值
WXIMAGE_API int CompressQuality_BufferToBuffer(uint8_t* buf, int buf_size,
	void* handle,
	int image_type, int target_level, int dst_width, int dst_height) {
	if (IsTimeout(0,100,200)) {
		return WXIMAGE_STATUS_TIMEOUT;
	}
	return _BufferToBuffer(buf, buf_size, handle,
		image_type, target_level, dst_width, dst_height);
}

//功能: 指定参数图像压缩，文件->文件
//参数:
//[strInput]: 输入文件路径
//[strOutput]: 输出文件路径
//[target_type]: 输出文件压缩类型，参加WXIMAGE_TYPE_等值
//[target_level]: 压缩参数，JPEG/WEBP 为quality(值越大输出文件越大)， PNG 为底层压缩参数(值越大输出文件越小)，范围0-100
//[dst_width]: 指定输出文件分辨率宽度，0 为原始宽度
//[dst_height]: 指定输出文件分辨率高度，0 为原始高度
//返回值: 参看WXIMAGE_STATUS_ERROR_ 等值
WXIMAGE_API int CompressQuality_FileToFile(const char* strInput, const char* strOutput,
	int target_type, int target_level, int dst_width, int dst_height) {

         WXLogA("%s Test\r\n", __FUNCTION__);
		 if (IsTimeout(0,100,200)) {
			 return WXIMAGE_STATUS_TIMEOUT;
		 }
	if (strInput == nullptr) {
		WXLogA("%s input is empty\r\n", __FUNCTION__);
		return WXIMAGE_STATUS_ERROR_INPUT_NULL; //参数错误
	}
	else if (strOutput == nullptr) {
		WXLogA("%s output is empty\r\n", __FUNCTION__);
		return WXIMAGE_STATUS_ERROR_OUTPUT_NULL;//参数错误
	}
	else if (target_type < 0 || target_type > WXIMAGE_TYPE_MAX) {
		WXLogA("%s target_type[%d] is error\r\n", __FUNCTION__, target_type);
		return WXIMAGE_STATUS_ERROR_QUALITY;//参数错误
	}
	else if (target_level < 0 || target_level > 100) {
		WXLogA("%s target_level[%d] is error\r\n", __FUNCTION__, target_level);
		return WXIMAGE_STATUS_ERROR_TARGET_SIZE;//参数错误
	}
	else if (dst_width < 0) {
		WXLogA("%s dst_width[%d] is error\r\n", __FUNCTION__, dst_width);
		return WXIMAGE_STATUS_ERROR_DST_WIDTH;//参数错误
	}
	else if (dst_height < 0) {
		WXLogA("%s dst_height[%d] is error\r\n", __FUNCTION__, dst_width);
		return WXIMAGE_STATUS_ERROR_DST_HEIGHT;//参数错误
	}
	int src_size = 0;
	DataBuffer data_handle;
	std::shared_ptr<uint8_t>src_buffer = _ReadFile(strInput, &src_size);
	if (src_buffer) { //读取输入文件正常

		WXLogA("%s ReadFile = %d\r\n", __FUNCTION__, src_size);

		int _target_type = target_type;
		if (target_type == WXIMAGE_TYPE_ORIGINAL) {
			//从文件名获取编码格式
			int imageTypeFormName = GetImageTypeFromName(strInput);
			if (imageTypeFormName != WXIMAGE_TYPE_UNKNOWN) { //根据文件名来确定
				_target_type = imageTypeFormName;
			}
		}

		WXLogA("%s _target_type = %d\r\n", __FUNCTION__, _target_type);
		//内存到内存的编码
		int ret = _BufferToBuffer(src_buffer.get(), src_size,
			&data_handle,_target_type,target_level, dst_width, dst_height);

		if (ret >= 0) {
			FilePtr fout = new_FilePtr(strOutput, FALSE);
			if (fout == nullptr) {
				//输出文件创建失败！！
				WXLogA("%s output[%s] create error\r\n", __FUNCTION__, strOutput);
				return WXIMAGE_STATUS_ERROR_OUTPUT_CREATE;
			}
			fwrite(data_handle.GetBuffer(), data_handle.GetSize(), 1, fout.get());
			return ret;
		}
	}else {
		WXLogA("%s input[%s] is not a image\r\n", __FUNCTION__, strInput);
		return WXIMAGE_STATUS_ERROR_INPUT_READ;//输入文件不可读
	}
	WXLogA("%s unknown error\r\n", __FUNCTION__);
	return WXIMAGE_STATUS_ERROR_UNKNOWN;
}


//功能: 指定参数图像压缩，文件->内存
//参数:
//[strInput]: 输入文件路径
//[handle]: 输出句柄
//[target_type]: 输出文件压缩类型，参加WXIMAGE_TYPE_等值
//[target_level]: 压缩参数，JPEG/WEBP 为quality(值越大输出文件越大)， PNG 为底层压缩参数(值越大输出文件越小)，范围0-100
//[dst_width]: 指定输出文件分辨率宽度，0 为原始宽度
//[dst_height]: 指定输出文件分辨率高度，0 为原始高度
//返回值: 参看WXIMAGE_STATUS_ERROR_ 等值
WXIMAGE_API int CompressQuality_FileToBuffer(const char* strInput, void* handle,
	int target_type, int target_level, int dst_width, int dst_height) {
	if (IsTimeout(0,100,200)) {
		return WXIMAGE_STATUS_TIMEOUT;
	}
	if (strInput == nullptr) {
		WXLogA("%s input is empty\r\n", __FUNCTION__);
		return WXIMAGE_STATUS_ERROR_INPUT_NULL; //参数错误
	}
	else if (handle == nullptr) {
		WXLogA("%s output is empty\r\n", __FUNCTION__);
		return WXIMAGE_STATUS_ERROR_OUTPUT_NULL;//参数错误
	}
	else if (target_type < 0 || target_type > WXIMAGE_TYPE_MAX) {
		WXLogA("%s target_type[%d] is error\r\n", __FUNCTION__, target_type);
		return WXIMAGE_STATUS_ERROR_QUALITY;//参数错误
	}
	else if (target_level < 0 || target_level > 100) {
		WXLogA("%s target_level[%d] is error\r\n", __FUNCTION__, target_level);
		return WXIMAGE_STATUS_ERROR_TARGET_SIZE;//参数错误
	}
	else if (dst_width < 0) {
		WXLogA("%s dst_width[%d] is error\r\n", __FUNCTION__, dst_width);
		return WXIMAGE_STATUS_ERROR_DST_WIDTH;//参数错误
	}
	else if (dst_height < 0) {
		WXLogA("%s dst_height[%d] is error\r\n", __FUNCTION__, dst_width);
		return WXIMAGE_STATUS_ERROR_DST_HEIGHT;//参数错误
	}
	int src_size = 0;
	std::shared_ptr<uint8_t>src_buffer = _ReadFile(strInput, &src_size);
	if (src_buffer) { //读取输入文件正常
		int _target_type = target_type;
		if (target_type == WXIMAGE_TYPE_ORIGINAL) {
			//从文件名获取编码格式
			int imageTypeFormName = GetImageTypeFromName(strInput);
			if (imageTypeFormName != WXIMAGE_TYPE_UNKNOWN) { //根据文件名来确定
				_target_type = imageTypeFormName;
			}
		}
		//内存到内存的编码
		int ret = _BufferToBuffer(src_buffer.get(), src_size,
			handle, _target_type, target_level, dst_width, dst_height);
		return ret;
	}
	else {
		WXLogA("%s input[%s] is not a image\r\n", __FUNCTION__, strInput);
		return WXIMAGE_STATUS_ERROR_INPUT_READ;//输入文件不可读
	}
	WXLogA("%s unknown error\r\n", __FUNCTION__);
	return WXIMAGE_STATUS_ERROR_UNKNOWN;
}


//功能: 指定参数图像压缩，内存->文件
//参数:
//[buf]: 图像Buffer
//[buf_size]:  图像Buffer长度
//[strOutput]: 输出文件路径
//[target_type]: 输出文件压缩类型，参加WXIMAGE_TYPE_等值
//[target_level]: 压缩参数，JPEG/WEBP 为quality(值越大输出文件越大)， PNG 为底层压缩参数(值越大输出文件越小)，范围0-100
//[dst_width]: 指定输出文件分辨率宽度，0 为原始宽度
//[dst_height]: 指定输出文件分辨率高度，0 为原始高度
//返回值: 参看WXIMAGE_STATUS_ERROR_ 等值
WXIMAGE_API int CompressQuality_BufferToFile(uint8_t* buf, int buf_size, const char* strOutput,
	int target_type, int target_level, int dst_width, int dst_height) {
	if (IsTimeout(0,100,200)) {
		return WXIMAGE_STATUS_TIMEOUT;
	}
	if (buf == nullptr || buf_size < 0) {
		WXLogA("%s input is empty\r\n", __FUNCTION__);
		return WXIMAGE_STATUS_ERROR_INPUT_NULL; //参数错误
	}
	else if (strOutput == nullptr) {
		WXLogA("%s output is empty\r\n", __FUNCTION__);
		return WXIMAGE_STATUS_ERROR_OUTPUT_NULL;//参数错误
	}
	else if (target_type < 0 || target_type > WXIMAGE_TYPE_MAX) {
		WXLogA("%s target_type[%d] is error\r\n", __FUNCTION__, target_type);
		return WXIMAGE_STATUS_ERROR_QUALITY;//参数错误
	}
	else if (target_level < 0 || target_level > 100) {
		WXLogA("%s target_level[%d] is error\r\n", __FUNCTION__, target_level);
		return WXIMAGE_STATUS_ERROR_TARGET_SIZE;//参数错误
	}
	else if (dst_width < 0) {
		WXLogA("%s dst_width[%d] is error\r\n", __FUNCTION__, dst_width);
		return WXIMAGE_STATUS_ERROR_DST_WIDTH;//参数错误
	}
	else if (dst_height < 0) {
		WXLogA("%s dst_height[%d] is error\r\n", __FUNCTION__, dst_width);
		return WXIMAGE_STATUS_ERROR_DST_HEIGHT;//参数错误
	}

	DataBuffer data_handle;
	int _target_type = target_type;
	//内存到内存的编码
	int ret = _BufferToBuffer(buf, buf_size,
		&data_handle, _target_type, target_level, dst_width, dst_height);

	if (ret >= 0) {
		FilePtr fout = new_FilePtr(strOutput, FALSE);
		if (fout == nullptr) {
			//输出文件创建失败！！
			WXLogA("%s output[%s] create error\r\n", __FUNCTION__, strOutput);
			return WXIMAGE_STATUS_ERROR_OUTPUT_CREATE;
		}
		fwrite(data_handle.GetBuffer(), data_handle.GetSize(), 1, fout.get());
		return ret;
	}
	WXLogA("%s unknown error\r\n", __FUNCTION__);
	return WXIMAGE_STATUS_ERROR_UNKNOWN;
}


#ifdef _MSC_VER
//功能: 指定参数图像压缩，文件->文件
//参数:
//[strInput]: 输入文件路径
//[strOutput]: 输出文件路径
//[target_type]: 输出文件压缩类型，参加WXIMAGE_TYPE_等值
//[target_level]: 压缩参数，JPEG/WEBP 为quality(值越大输出文件越大)， PNG 为底层压缩参数(值越大输出文件越小)，范围0-100
//[dst_width]: 指定输出文件分辨率宽度，0 为原始宽度
//[dst_height]: 指定输出文件分辨率高度，0 为原始高度
//返回值: 参看WXIMAGE_STATUS_ERROR_ 等值
WXIMAGE_API int CompressQualityU_FileToFile(const wchar_t* strInput, const wchar_t* strOutput,
	int target_type, int target_level, int dst_width, int dst_height) {
	if (IsTimeout(0,100,200)) {
		return WXIMAGE_STATUS_TIMEOUT;
	}
	if (strInput == nullptr) {
		WXLogA("%s input is empty\r\n", __FUNCTION__);
		return WXIMAGE_STATUS_ERROR_INPUT_NULL; //参数错误
	}
	else if (strOutput == nullptr) {
		WXLogA("%s output is empty\r\n", __FUNCTION__);
		return WXIMAGE_STATUS_ERROR_OUTPUT_NULL;//参数错误
	}
	else if (target_type < 0 || target_type > WXIMAGE_TYPE_MAX) {
		WXLogA("%s target_type[%d] is error\r\n", __FUNCTION__, target_type);
		return WXIMAGE_STATUS_ERROR_QUALITY;//参数错误
	}
	else if (target_level < 0 || target_level > 100) {
		WXLogA("%s target_level[%d] is error\r\n", __FUNCTION__, target_level);
		return WXIMAGE_STATUS_ERROR_TARGET_SIZE;//参数错误
	}
	else if (dst_width < 0) {
		WXLogA("%s dst_width[%d] is error\r\n", __FUNCTION__, dst_width);
		return WXIMAGE_STATUS_ERROR_DST_WIDTH;//参数错误
	}
	else if (dst_height < 0) {
		WXLogA("%s dst_height[%d] is error\r\n", __FUNCTION__, dst_width);
		return WXIMAGE_STATUS_ERROR_DST_HEIGHT;//参数错误
	}
	int src_size = 0;
	DataBuffer data_handle;
	std::shared_ptr<uint8_t>src_buffer = _ReadFile(strInput, &src_size);
	if (src_buffer) { //读取输入文件正常
		int _target_type = target_type;
		if (target_type == WXIMAGE_TYPE_ORIGINAL) {
			//从文件名获取编码格式
			int imageTypeFormName = GetImageTypeFromName(strInput);
			if (imageTypeFormName != WXIMAGE_TYPE_UNKNOWN) { //根据文件名来确定
				_target_type = imageTypeFormName;
			}
		}
		//内存到内存的编码
		int ret = _BufferToBuffer(src_buffer.get(), src_size,
			&data_handle, _target_type, target_level, dst_width, dst_height);

		if (ret >= 0) {
			FilePtr fout = new_FilePtr(strOutput, FALSE);
			if (fout == nullptr) {
				//输出文件创建失败！！
				WXLogW(L"%ws output[%ws] create error", __FUNCTIONW__, strOutput);
				return WXIMAGE_STATUS_ERROR_OUTPUT_CREATE;
			}
			fwrite(data_handle.GetBuffer(), data_handle.GetSize(), 1, fout.get());
			return ret;
		}
		
		return ret;
	}
	else {
		WXLogW(L"%ws input[%ws] is not a image", __FUNCTIONW__, strInput);
		return WXIMAGE_STATUS_ERROR_INPUT_READ;//输入文件不可读
	}
	WXLogA("%s unknown error\r\n", __FUNCTION__);
	return WXIMAGE_STATUS_ERROR_UNKNOWN;
}

//功能: 指定参数图像压缩，文件->内存
//参数:
//[strInput]: 输入文件路径
//[handle]: 输出句柄
//[target_type]: 输出文件压缩类型，参加WXIMAGE_TYPE_等值
//[target_level]: 压缩参数，JPEG/WEBP 为quality(值越大输出文件越大)， PNG 为底层压缩参数(值越大输出文件越小)，范围0-100
//[dst_width]: 指定输出文件分辨率宽度，0 为原始宽度
//[dst_height]: 指定输出文件分辨率高度，0 为原始高度
//返回值: 参看WXIMAGE_STATUS_ERROR_ 等值
WXIMAGE_API int CompressQualityU_FileToBuffer(const wchar_t* strInput, void* handle,
	int target_type, int target_level, int dst_width, int dst_height) {
	if (IsTimeout(0,100,200)) {
		return WXIMAGE_STATUS_TIMEOUT;
	}
	if (strInput == nullptr) {
		WXLogA("%s input is empty\r\n", __FUNCTION__);
		return WXIMAGE_STATUS_ERROR_INPUT_NULL; //参数错误
	}
	else if (handle == nullptr) {
		WXLogA("%s output is empty\r\n", __FUNCTION__);
		return WXIMAGE_STATUS_ERROR_OUTPUT_NULL;//参数错误
	}
	else if (target_type < 0 || target_type > WXIMAGE_TYPE_MAX) {
		WXLogA("%s target_type[%d] is error\r\n", __FUNCTION__, target_type);
		return WXIMAGE_STATUS_ERROR_QUALITY;//参数错误
	}
	else if (target_level < 0 || target_level > 100) {
		WXLogA("%s target_level[%d] is error\r\n", __FUNCTION__, target_level);
		return WXIMAGE_STATUS_ERROR_TARGET_SIZE;//参数错误
	}
	else if (dst_width < 0) {
		WXLogA("%s dst_width[%d] is error\r\n", __FUNCTION__, dst_width);
		return WXIMAGE_STATUS_ERROR_DST_WIDTH;//参数错误
	}
	else if (dst_height < 0) {
		WXLogA("%s dst_height[%d] is error\r\n", __FUNCTION__, dst_width);
		return WXIMAGE_STATUS_ERROR_DST_HEIGHT;//参数错误
	}
	int src_size = 0;
	std::shared_ptr<uint8_t>src_buffer = _ReadFile(strInput, &src_size);
	if (src_buffer) { //读取输入文件正常
		int _target_type = target_type;
		if (target_type == WXIMAGE_TYPE_ORIGINAL) {
			//从文件名获取编码格式
			int imageTypeFormName = GetImageTypeFromName(strInput);
			if (imageTypeFormName != WXIMAGE_TYPE_UNKNOWN) { //根据文件名来确定
				_target_type = imageTypeFormName;
			}
		}
		//内存到内存的编码
		int ret = _BufferToBuffer(src_buffer.get(), src_size,
			handle, _target_type, target_level, dst_width, dst_height);
		return ret;
	}
	else {
		WXLogW(L"%ws input[%ws] is not a image", __FUNCTIONW__, strInput);
		return WXIMAGE_STATUS_ERROR_INPUT_READ;//输入文件不可读
	}
	WXLogA("%s unknown error\r\n", __FUNCTION__);
	return WXIMAGE_STATUS_ERROR_UNKNOWN;
}


//功能: 指定参数图像压缩，内存->文件
//参数:
//[buf]: 图像Buffer
//[buf_size]:  图像Buffer长度
//[strOutput]: 输出文件路径
//[target_type]: 输出文件压缩类型，参加WXIMAGE_TYPE_等值
//[target_level]: 压缩参数，JPEG/WEBP 为quality(值越大输出文件越大)， PNG 为底层压缩参数(值越大输出文件越小)，范围0-100
//[dst_width]: 指定输出文件分辨率宽度，0 为原始宽度
//[dst_height]: 指定输出文件分辨率高度，0 为原始高度
//返回值: 参看WXIMAGE_STATUS_ERROR_ 等值
WXIMAGE_API int CompressQualityU_BufferToFile(uint8_t* buf, int buf_size, const wchar_t* strOutput,
	int target_type, int target_level, int dst_width, int dst_height) {
	if (IsTimeout(0,100,200)) {
		return WXIMAGE_STATUS_TIMEOUT;
	}
	if (buf == nullptr || buf_size < 0) {
		WXLogA("%s input is empty\r\n", __FUNCTION__);
		return WXIMAGE_STATUS_ERROR_INPUT_NULL; //参数错误
	}
	else if (strOutput == nullptr) {
		WXLogA("%s output is empty\r\n", __FUNCTION__);
		return WXIMAGE_STATUS_ERROR_OUTPUT_NULL;//参数错误
	}
	else if (target_type < 0 || target_type > WXIMAGE_TYPE_MAX) {
		WXLogA("%s target_type[%d] is error\r\n", __FUNCTION__, target_type);
		return WXIMAGE_STATUS_ERROR_QUALITY;//参数错误
	}
	else if (target_level < 0 || target_level > 100) {
		WXLogA("%s target_level[%d] is error\r\n", __FUNCTION__, target_level);
		return WXIMAGE_STATUS_ERROR_TARGET_SIZE;//参数错误
	}
	else if (dst_width < 0) {
		WXLogA("%s dst_width[%d] is error\r\n", __FUNCTION__, dst_width);
		return WXIMAGE_STATUS_ERROR_DST_WIDTH;//参数错误
	}
	else if (dst_height < 0) {
		WXLogA("%s dst_height[%d] is error\r\n", __FUNCTION__, dst_width);
		return WXIMAGE_STATUS_ERROR_DST_HEIGHT;//参数错误
	}

	DataBuffer data_handle;
	int _target_type = target_type;
	//内存到内存的编码
	int ret = _BufferToBuffer(buf, buf_size,
		&data_handle, _target_type, target_level, dst_width, dst_height);

	if (ret >= 0) {
		FilePtr fout = new_FilePtr(strOutput, FALSE);
		if (fout == nullptr) {
			//输出文件创建失败！！
			WXLogW(L"%ws output[%ws] create error", __FUNCTIONW__, strOutput);
			return WXIMAGE_STATUS_ERROR_OUTPUT_CREATE;
		}
		fwrite(data_handle.GetBuffer(), data_handle.GetSize(), 1, fout.get());
		return ret;
	}
	WXLogA("%s unknown error\r\n", __FUNCTION__);
	return WXIMAGE_STATUS_ERROR_UNKNOWN;
}

#endif