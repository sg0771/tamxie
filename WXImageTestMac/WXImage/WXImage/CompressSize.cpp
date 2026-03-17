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

static int s_nMinQuality     = 20;
static int s_nDefaultQuality = 75;
static int s_nMaxQuality     = 100;
WXIMAGE_API int SetQuality(int nMinQuality, int nDefaultQuality, int nMaxQuality) {
	if (nMinQuality < 0 || nMinQuality > 100 || nMinQuality > nDefaultQuality || nMinQuality > nMaxQuality) {
		return -1;
	}
	else if (nDefaultQuality < 0 || nDefaultQuality > 100 || nDefaultQuality > nMaxQuality) {
		return -2;
	}
	else if (nMaxQuality < 0 || nMaxQuality > 100) {
		return -3;
	}
	s_nMinQuality = std::max(0, std::min(100, nMinQuality));
	s_nDefaultQuality = std::max(0, std::min(100, nDefaultQuality));
	s_nMaxQuality = std::max(0, std::min(100, nMaxQuality));
	return 0;
}

//-----------核心功能------------
//功能:将输入图像压缩成接近指定大小的Jpeg/Webp文件流
static int __CompressSize(void* bitmap,
	int image_type,  //输出文件压缩类型，参加WXIMAGE_TYPE_等值
	int target_size, //制定输出大小，单位KByte
	int dst_width, //输出宽度，使用0表示保持不变
	int dst_height,  //输出高度，使用0表示保持不变
	DataBuffer* handle ) // 压缩数据缓存
{
	//开始编码
	int   _nWidth      = WXImage_GetWidth(bitmap); //图像宽度
	int   _nHeight     = WXImage_GetHeight(bitmap);//图像高度
	uint8_t* _pData    = WXImage_GetBits(bitmap);//解码图像数据
	int      _nPitch   = WXImage_GetPitch(bitmap);//解码图像数据每行字节数量
	int      _nTypeRGB  = WXImage_GetChannel(bitmap);//每个像素的数据位数
	uint8_t* _pIccData  = WXImage_GetIccData(bitmap);//ICC 数据
	int _nIccSize        = WXImage_GetIccSize(bitmap);//ICC 数据
	int _nDotsPerMeterX = WXImage_GetDotsPerMeterX(bitmap);
	int _nDotsPerMeterY = WXImage_GetDotsPerMeterY(bitmap);
	BITMAPINFO* src_bi = (BITMAPINFO*)WXImage_GetInfo(bitmap);//RGB8 

	int _nDstW = dst_width;
	int _nDstH = dst_height;

	if (dst_width == 0 && dst_height == 0) {
		_nDstW = _nWidth;
		_nDstH = _nHeight;
	}else if (dst_width == 0 && dst_height != 0) {
		_nDstW = ((int)((double)dst_height * (double)_nWidth / (double)_nHeight + 0.5) + 1) / 2 * 2;
	}
	else if (dst_width != 0 && dst_height == 0) {
		_nDstH = ((int)((double)dst_width * (double)_nHeight / (double)_nWidth + 0.5) + 1) / 2 * 2;
	}

	std::shared_ptr< FIBITMAP> _ScaleDib;//有可能缩放处理

	uint8_t* _pDst = _pData;
	int _nDstPitch = _nPitch;

	// 缩放处理
	if (_nDstW != _nWidth || _nDstH != _nHeight)  {
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
			libyuv::ScalePlane(_pData, _nPitch,_nWidth,_nHeight,
				_pDst, _nDstPitch,_nDstW,_nDstH,libyuv::kFilterLinear);
			//颜色表拷贝
			BITMAPINFO* dst_bi = FreeImage_GetInfo(_ScaleDib.get());
			memcpy(dst_bi->bmiColors, src_bi->bmiColors, 256 * TYPE_BGRA);//

		}else if (_nTypeRGB == TYPE_BGR) {
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

		}else if (_nTypeRGB == TYPE_BGRA) {
			libyuv::ARGBScale(_pData, _nPitch, _nWidth, _nHeight,
				_pDst, _nDstPitch, _nDstW, _nDstH, libyuv::kFilterLinear);
		}
	}

	int  _nTargetQuality = s_nDefaultQuality;//默认qulaity75
	int  _nTargetSize = target_size * 1024;

	//没有指定大小，使用默认的Quality
	if (_nTargetSize != 0) {
		int maxSize = Image_Encode(handle, WXIMAGE_TYPE_JPEG, s_nMaxQuality,
			_nTypeRGB, _pDst, _nDstW, _nDstH, _nDstPitch, _nDotsPerMeterX, _nDotsPerMeterY,
			nullptr, 0);
		if (maxSize < _nTargetSize ) {//最大quality编码大小比预设值还小
			_nTargetQuality = s_nMaxQuality; 
		}else{
			int minSize = Image_Encode(handle, WXIMAGE_TYPE_JPEG, s_nMinQuality,
				_nTypeRGB, _pDst, _nDstW, _nDstH, _nDstPitch, _nDotsPerMeterX, _nDotsPerMeterY,
				nullptr, 0);
			if (minSize > _nTargetSize) {//最小quality编码大小比预设值还大
				_nTargetQuality = s_nMinQuality;
			}else { //二分法查找qualit
				int  _image_size = 0;
				//查找合适的quality
				_image_size = Image_Encode(handle, WXIMAGE_TYPE_JPEG, s_nDefaultQuality,
					_nTypeRGB, _pDst, _nDstW, _nDstH, _nDstPitch, _nDotsPerMeterX, _nDotsPerMeterY,
					nullptr, 0);//默认大小
				std::map<int, int>_mapSize;
				_mapSize[s_nDefaultQuality] = _image_size;//保存指定quality后编码长度
				//二分法处理
				int  _nMaxQuality = s_nMaxQuality;
				int  _nMinQuality = s_nMinQuality;;
				int  _nPrevQuility = s_nDefaultQuality;
				int  _mMaxIndex = 6;//最多6次处理, 极端情况，连续6次都比预设值大，返回错误
				int  _index = 0;//迭代次数

				while (true){
					int _newQuality = 0;
					if (_image_size > _nTargetSize) { //编码输出比预设值大，降低Quality
						_newQuality = (_nPrevQuility + _nMinQuality) / 2;//更新区间
						_nMaxQuality = _nPrevQuility;//更新二分法区间
						if (_newQuality == _nPrevQuility) {
							_nTargetQuality = _nPrevQuility;
							break;
						}
					}else if (_image_size < _nTargetSize) { //编码输出比预设值小，增加Quality
						_newQuality = (_nPrevQuility + _nMaxQuality) / 2;
						_nMinQuality = _nPrevQuility; //更新区间
						if (_newQuality == _nPrevQuility) {
							_nTargetQuality = _nPrevQuility;
							break;
						}
					}else {
						//刚好相等预设值
						_nTargetSize = _nPrevQuility;//指定quality
						break;
					}

					_image_size = Image_Encode(handle, image_type, _newQuality,
						_nTypeRGB, _pDst, _nDstW, _nDstH, _nDstPitch,
						_nDotsPerMeterX, _nDotsPerMeterY,
						nullptr, 0);//测试大小

					_mapSize[_newQuality] = _image_size;//保存指定quality后编码长度
					if (_index++ >= _mMaxIndex) {
						//最后一次迭代
						if (_mapSize[_newQuality] > _nTargetSize && _mapSize[_nPrevQuility] > _nTargetSize) {
							_nTargetQuality = s_nMinQuality;//连续两次编码长度都比预设值大
							//应该不会存在这个问题
						}
						else  if (_mapSize[_newQuality] < _nTargetSize && _mapSize[_nPrevQuility] < _nTargetSize) {
							_nTargetQuality = _nPrevQuility;
						}
						else  if (_mapSize[_newQuality] > _nTargetSize && _mapSize[_nPrevQuility] < _nTargetSize) {
							_nTargetQuality = _nPrevQuility;
						}
						else  if (_mapSize[_newQuality] < _nTargetSize && _mapSize[_nPrevQuility] > _nTargetSize) {
							_nTargetQuality = _newQuality;
						}
						//最多迭代六次
						break;
					}
					_nPrevQuility = _newQuality;
				}
			}
		}
	}

	if (_nTargetQuality > 0) {
		//实际编码输出
		int _image_size = Image_Encode(handle, image_type, _nTargetQuality,
			_nTypeRGB, _pDst, _nDstW, _nDstH, _nDstPitch,
			_nDotsPerMeterX, _nDotsPerMeterY,
			_pIccData, _nIccSize);//用找到的quality+icc 来编码

		if (_image_size > 0) { //直接编码成功
			if (_nTargetSize > 0 && _image_size > _nTargetSize) {
				return ERR_SIZE + image_type; //编码成功，但是实际大小比预设值大
			}
			return image_type;
		}else if (_pIccData && _nIccSize > 0) { //编码失败，重新禁用icc再编码
			_image_size = Image_Encode(handle, image_type, _nTargetQuality,
				_nTypeRGB, _pDst, _nDstW, _nDstH, _nDstPitch,
				_nDotsPerMeterX, _nDotsPerMeterY,
				nullptr, 0);//最后编码
			if (_image_size > 0) {
				if (_nTargetSize > 0 && _image_size > _nTargetSize) {
					return ERR_SIZE + image_type; //编码成功，但是实际大小比预设值大
				}
				return image_type;
			}
		}
	}
	return WXIMAGE_STATUS_ERROR_OUTPUT_ENCODE;//编码错误
}

//-----------核心功能------------
//功能，将输入图像内存数据压缩成接近指定大小的Jpeg/Webp数据流
//参数:
//[buf]: 图像数据
//[buf_size]:图像数据长度
//[handle]: 输出数据句柄，就是HandlerCreate返回值
//[target_type]: 输出文件压缩类型，参加WXIMAGE_TYPE_等值
//[target_size]: 指定输出文件大小，单位KByte，如果为0，使用默认quality=75编码
//[dst_width]: 指定输出文件分辨率宽度，0 为原始宽度
//[dst_height]: 指定输出文件分辨率高度，0 为原始高度
//返回值: 小于0表示压缩失败，参看WXIMAGE_STATUS_ERROR_ 等值
//大于0 表示压缩成功， 返回值应该是  ABCD 四位数或者两位数
// CD 表示底层使用的quality 值
// B=1 表示预设的文件大小比最小的quality压缩出来的数据小
// A=1 表示输出文件使用原文件的icc信息重编码错误，已经忽略(会导致输出文件浏览时和原文件偏色)
WXIMAGE_API int CompressSize_BufferToBuffer(uint8_t* buf, int buf_size, void* handle,
	int target_type, int target_size, int dst_width, int dst_height) {
	//检查参数
	if (buf == nullptr || buf_size < 0) {
		WXLogA("%s inputData is empty\r\n", __FUNCTION__);
		return WXIMAGE_STATUS_ERROR_INPUT_NULL; //参数错误
	}
	else if (handle == nullptr) {
		WXLogA("%s output is empty\r\n", __FUNCTION__);
		return WXIMAGE_STATUS_ERROR_OUTPUT_NULL;//参数错误
	}
	else if (target_type != WXIMAGE_TYPE_ORIGINAL &&
		target_type != WXIMAGE_TYPE_JPEG && 
		target_type != WXIMAGE_TYPE_WEBP) {
		//指定大小压缩只支持jpg和webp两种格式
		WXLogA("%s image_type[%d] is error\r\n", __FUNCTION__, target_type);
		return WXIMAGE_STATUS_ERROR_DST_IMAGETYPE;//参数错误

	}
	else if (target_size < 0) {
		WXLogA("%s target_size is error\r\n", __FUNCTION__);
		return WXIMAGE_STATUS_ERROR_TARGET_SIZE;//参数错误
	}
	else if (dst_width < 0) {
		WXLogA("%s dst_width is error\r\n", __FUNCTION__);
		return WXIMAGE_STATUS_ERROR_DST_WIDTH;//参数错误
	}
	else if (dst_height < 0) {
		WXLogA("%s dst_height is error\r\n", __FUNCTION__);
		return WXIMAGE_STATUS_ERROR_DST_HEIGHT;//参数错误
	}
	//从内存buffer解码图像
	std::shared_ptr<void>pBitmap = std::shared_ptr<void>(
		WXImage_Load(buf, buf_size),
		[](void* p) {  if (p) { WXImage_Unload(p); p = nullptr; } });
	if (pBitmap.get() != nullptr) { //解码成功


		//指定大小压缩
		int _target_type = target_type;//输出类型
		if (target_type == WXIMAGE_TYPE_ORIGINAL) {
			//获取原文件的编码信息
			int imageTypeFormData = WXImage_GetImageType(pBitmap.get());
			if (imageTypeFormData == WXIMAGE_TYPE_JPEG ||
				imageTypeFormData == WXIMAGE_TYPE_WEBP) {
				_target_type = imageTypeFormData;
			}else {
				//编码格式都不是 JPEG、WEBP
				int typeRGB = WXImage_GetChannel(pBitmap.get());
				if (typeRGB == 4) { //带透明信息使用WEBP格式
					_target_type = WXIMAGE_TYPE_WEBP;
				}else {
					_target_type = WXIMAGE_TYPE_JPEG;//不带透明信息使用JPEG编码
				}
			}
		}
		WXLogA("%s _target_type = %d\r\n", __FUNCTION__, _target_type);
		int ret = __CompressSize(pBitmap.get(), _target_type,
			target_size, dst_width, dst_height, (DataBuffer*)handle); //指定大小压缩
		return ret;//返回编码大小或者编码错误信息
	}
	WXLogA("%s unknown error\r\n", __FUNCTION__);
	return WXIMAGE_STATUS_ERROR_INPUT_DECODE;
}

//-------------------------------------------------------------------------------------

//功能，将输入图像压缩成接近指定大小的Jpeg/Webp文件
//参数:
//[strInput]: 输入文件路径
//[strOutput]: 输出文件路径
//[target_type]: 输出文件压缩类型，参加WXIMAGE_TYPE_等值
//[target_size]: 指定输出文件大小，单位KByte，如果为0，使用默认quality=75编码
//[dst_width]: 指定输出文件分辨率宽度，0 为原始宽度
//[dst_height]: 指定输出文件分辨率高度，0 为原始高度
//返回值: 小于0表示压缩失败，参看WXIMAGE_STATUS_ERROR_ 等值
//大于0 表示压缩成功， 返回值应该是  ABCD 四位数或者两位数
// CD 表示底层使用的quality 值
// B=1 表示预设的文件大小比最小的quality压缩出来的数据小
// A=1 表示输出文件使用原文件的icc信息重编码错误，已经忽略(会导致输出文件浏览时和原文件偏色)
WXIMAGE_API int CompressSize_FileToFile(const char* strInput, const char* strOutput,
	int target_type, int target_size, int dst_width, int dst_height) {

	if (strInput == nullptr || strlen(strInput) == 0) {
		WXLogA("%s input is empty\r\n", __FUNCTION__);
		return WXIMAGE_STATUS_ERROR_INPUT_NULL; //参数错误
	}
	else if (strOutput == nullptr || strlen(strOutput) == 0) {
		WXLogA("%s output is empty\r\n", __FUNCTION__);
		return WXIMAGE_STATUS_ERROR_OUTPUT_NULL;//参数错误
	}
	else if (target_type != WXIMAGE_TYPE_ORIGINAL
		&& target_type != WXIMAGE_TYPE_JPEG
		&& target_type != WXIMAGE_TYPE_WEBP) {
		//指定大小压缩只支持jpg和webp两种格式
		WXLogA("%s image_type[%d] is error\r\n", __FUNCTION__, target_type);
		return WXIMAGE_STATUS_ERROR_DST_IMAGETYPE;//参数错误
	}
	else if (target_size < 0) {
		WXLogA("%s target_size is error\r\n", __FUNCTION__);
		return WXIMAGE_STATUS_ERROR_TARGET_SIZE;//参数错误
	}
	else if (dst_width < 0) {
		WXLogA("%s dst_width is error\r\n", __FUNCTION__);
		return WXIMAGE_STATUS_ERROR_DST_WIDTH;//参数错误
	}
	else if (dst_height < 0) {
		WXLogA("%s dst_height is error\r\n", __FUNCTION__);
		return WXIMAGE_STATUS_ERROR_DST_HEIGHT;//参数错误
	}
	int src_size = 0;
	DataBuffer data_handle;
	std::shared_ptr<uint8_t>src_buffer = _ReadFile(strInput, &src_size);
	if (src_buffer) {

		//指定大小压缩
		int _target_type = target_type;//输出类型
		if (target_type == WXIMAGE_TYPE_ORIGINAL) { //从文件名获取编码格式
			int imageTypeFormName = GetImageTypeFromName(strInput);
			if (imageTypeFormName == WXIMAGE_TYPE_JPEG || imageTypeFormName == WXIMAGE_TYPE_WEBP) {
				_target_type = imageTypeFormName;
			}
			else {
				_target_type = WXIMAGE_TYPE_ORIGINAL;
			}
			//WXIMAGE_TYPE_ORIGINAL 就交给解码处理格式
		}

		int ret = CompressSize_BufferToBuffer(src_buffer.get(), src_size, &data_handle, 
			_target_type,target_size,dst_width,dst_height);
		if (ret > 0) {
			//编码成功
			FilePtr fout = new_FilePtr(strOutput, FALSE);
			if (fout.get()) { //创建输出文件成功，写入数据
				fwrite(data_handle.GetBuffer(), data_handle.GetSize(), 1, fout.get());//输出到文件
				return ret;
			}else {
				return WXIMAGE_STATUS_ERROR_OUTPUT_CREATE;//输出文件不能创建
			}
		}
		return ret;
	}
	WXLogA("%s input[%s] can open\r\n", __FUNCTION__, strInput);
	return WXIMAGE_STATUS_ERROR_INPUT_READ;//输入文件不可读
}


//功能，将输入图像内存数据压缩成接近指定大小的Jpeg/Webp文件
//参数:
//[buf]: 图像数据
//[buf_size]:图像数据长度
//[strOutput]: 输出文件路径
//[target_type]: 输出文件压缩类型，参加WXIMAGE_TYPE_等值
//[target_size]: 指定输出文件大小，单位KByte，如果为0，使用默认quality=75编码
//[dst_width]: 指定输出文件分辨率宽度，0 为原始宽度
//[dst_height]: 指定输出文件分辨率高度，0 为原始高度
//返回值: 小于0表示压缩失败，参看WXIMAGE_STATUS_ERROR_ 等值
//大于0 表示压缩成功， 返回值应该是  ABCD 四位数或者两位数
// CD 表示底层使用的quality 值
// B=1 表示预设的文件大小比最小的quality压缩出来的数据小
// A=1 表示输出文件使用原文件的icc信息重编码错误，已经忽略(会导致输出文件浏览时和原文件偏色)
WXIMAGE_API int CompressSize_BufferToFile(uint8_t* buf, int buf_size, const char* strOutput,
	int target_type, int target_size, int dst_width, int dst_height) {

	if (buf == nullptr || buf_size <= 0) {
		WXLogA("%s input is empty\r\n", __FUNCTION__);
		return WXIMAGE_STATUS_ERROR_INPUT_NULL; //参数错误
	}
	else if (strOutput == nullptr || strlen(strOutput) == 0) {
		WXLogA("%s output is empty\r\n", __FUNCTION__);
		return WXIMAGE_STATUS_ERROR_OUTPUT_NULL;//参数错误
	}
	else if (target_type != WXIMAGE_TYPE_ORIGINAL &&
		target_type != WXIMAGE_TYPE_JPEG && 
		target_type != WXIMAGE_TYPE_WEBP) {
		//指定大小压缩只支持jpg和webp两种格式
		WXLogA("%s image_type[%d] is error\r\n", __FUNCTION__, target_type);
		return WXIMAGE_STATUS_ERROR_DST_IMAGETYPE;//参数错误
	}
	else if (target_size < 0) {
		WXLogA("%s target_size is error\r\n", __FUNCTION__);
		return WXIMAGE_STATUS_ERROR_TARGET_SIZE;//参数错误
	}
	else if (dst_width < 0) {
		WXLogA("%s dst_width is error\r\n", __FUNCTION__);
		return WXIMAGE_STATUS_ERROR_DST_WIDTH;//参数错误
	}
	else if (dst_height < 0) {
		WXLogA("%s dst_height is error\r\n", __FUNCTION__);
		return WXIMAGE_STATUS_ERROR_DST_HEIGHT;//参数错误
	}

	DataBuffer data_handle;
	int ret = CompressSize_BufferToBuffer(buf, buf_size, &data_handle,
		target_type, target_size, dst_width, dst_height);
	if (ret > 0) {
		//编码成功
		FilePtr fout = new_FilePtr(strOutput, FALSE);
		if (fout.get()) { //创建输出文件成功，写入数据
			fwrite(data_handle.GetBuffer(), data_handle.GetSize(), 1, fout.get());//输出到文件
			return ret;//输出成功
		}
		else {
			return WXIMAGE_STATUS_ERROR_OUTPUT_CREATE;//输出文件不能创建
		}
	}
	return ret;
}


//功能，将输入图像压缩成接近指定大小的Jpeg/Webp文件
//参数:
//[strInput]: 输入文件路径
//[handle]: 输出数据对象，HandleCreate的返回值
//[target_type]: 输出文件压缩类型，参加WXIMAGE_TYPE_等值
//[target_size]: 指定输出文件大小，单位KByte，如果为0，使用默认quality=75编码
//[dst_width]: 指定输出文件分辨率宽度，0 为原始宽度
//[dst_height]: 指定输出文件分辨率高度，0 为原始高度
//返回值: 小于0表示压缩失败，参看WXIMAGE_STATUS_ERROR_ 等值
//大于0 表示压缩成功， 返回值应该是  ABCD 四位数或者两位数
// CD 表示底层使用的quality 值
// B=1 表示预设的文件大小比最小的quality压缩出来的数据小
// A=1 表示输出文件使用原文件的icc信息重编码错误，已经忽略(会导致输出文件浏览时和原文件偏色)
WXIMAGE_API int CompressSize_FileToBuffer(const char* strInput, void* handle,
	int target_type, int target_size, int dst_width, int dst_height) {

	if (strInput == nullptr || strlen(strInput) == 0) {
		WXLogA("%s input is empty\r\n", __FUNCTION__);
		return WXIMAGE_STATUS_ERROR_INPUT_NULL; //参数错误
	}
	else if (handle == nullptr) {
		WXLogA("%s output is empty\r\n", __FUNCTION__);
		return WXIMAGE_STATUS_ERROR_OUTPUT_NULL;//参数错误
	}
	else if (target_type != WXIMAGE_TYPE_ORIGINAL &&
		target_type != WXIMAGE_TYPE_JPEG && 
		target_type != WXIMAGE_TYPE_WEBP) {
		//指定大小压缩只支持jpg和webp两种格式
		WXLogA("%s image_type[%d] is error\r\n", __FUNCTION__, target_type);
		return WXIMAGE_STATUS_ERROR_DST_IMAGETYPE;//参数错误
	}
	else if (target_size < 0) {
		WXLogA("%s target_size is error\r\n", __FUNCTION__);
		return WXIMAGE_STATUS_ERROR_TARGET_SIZE;//参数错误
	}
	else if (dst_width < 0) {
		WXLogA("%s dst_width is error\r\n", __FUNCTION__);
		return WXIMAGE_STATUS_ERROR_DST_WIDTH;//参数错误
	}
	else if (dst_height < 0) {
		WXLogA("%s dst_height is error\r\n", __FUNCTION__);
		return WXIMAGE_STATUS_ERROR_DST_HEIGHT;//参数错误
	}
	int src_size = 0;
	std::shared_ptr<uint8_t>src_buffer = _ReadFile(strInput, &src_size);
	if (src_buffer) {
		//指定大小压缩
		int _target_type = target_type;//输出类型
		if (target_type == WXIMAGE_TYPE_ORIGINAL) { //从文件名获取编码格式
			int imageTypeFormName = GetImageTypeFromName(strInput);
			if (imageTypeFormName == WXIMAGE_TYPE_JPEG || imageTypeFormName == WXIMAGE_TYPE_WEBP) {
				_target_type = imageTypeFormName;
			}
			else {
				_target_type = WXIMAGE_TYPE_ORIGINAL;
			}
			//WXIMAGE_TYPE_ORIGINAL 就交给解码处理格式
		}
		int ret = CompressSize_BufferToBuffer(src_buffer.get(), src_size, handle,
			_target_type, target_size, dst_width, dst_height);
		return ret;
	}
	WXLogA("%s input[%s] can open\r\n", __FUNCTION__, strInput);
	return WXIMAGE_STATUS_ERROR_INPUT_READ;//输入文件不可读
}



#ifdef _WIN32

//功能，将输入图像压缩成接近指定大小的Jpeg/Webp文件
//参数:
//[strInput]: 输入文件路径
//[strOutput]: 输出文件路径
//[target_type]: 输出文件压缩类型，参加WXIMAGE_TYPE_等值
//[target_size]: 指定输出文件大小，单位KByte，如果为0，使用默认quality=75编码
//[dst_width]: 指定输出文件分辨率宽度，0 为原始宽度
//[dst_height]: 指定输出文件分辨率高度，0 为原始高度
//返回值: 小于0表示压缩失败，参看WXIMAGE_STATUS_ERROR_ 等值
//大于0 表示压缩成功， 返回值应该是  ABCD 四位数或者两位数
// CD 表示底层使用的quality 值
// B=1 表示预设的文件大小比最小的quality压缩出来的数据小
// A=1 表示输出文件使用原文件的icc信息重编码错误，已经忽略(会导致输出文件浏览时和原文件偏色)
WXIMAGE_API int CompressSizeU_FileToFile(const wchar_t* strInput, const wchar_t* strOutput,
	int target_type, int target_size, int dst_width, int dst_height) {

	if (strInput == nullptr || wcslen(strInput) == 0) {
		WXLogA("%s input is empty\r\n", __FUNCTION__);
		return WXIMAGE_STATUS_ERROR_INPUT_NULL; //参数错误
	}
	else if (strOutput == nullptr || wcslen(strOutput) == 0) {
		WXLogA("%s output is empty\r\n", __FUNCTION__);
		return WXIMAGE_STATUS_ERROR_OUTPUT_NULL;//参数错误
	}
	else if (target_type != WXIMAGE_TYPE_ORIGINAL &&
		 target_type != WXIMAGE_TYPE_JPEG &&
		target_type != WXIMAGE_TYPE_WEBP) {
		//指定大小压缩只支持jpg和webp两种格式
		WXLogA("%s image_type[%d] is error\r\n", __FUNCTION__, target_type);
		return WXIMAGE_STATUS_ERROR_DST_IMAGETYPE;//参数错误
	}
	else if (target_size < 0) {
		WXLogA("%s target_size is error\r\n", __FUNCTION__);
		return WXIMAGE_STATUS_ERROR_TARGET_SIZE;//参数错误
	}
	else if (dst_width < 0) {
		WXLogA("%s dst_width is error\r\n", __FUNCTION__);
		return WXIMAGE_STATUS_ERROR_DST_WIDTH;//参数错误
	}
	else if (dst_height < 0) {
		WXLogA("%s dst_height is error\r\n", __FUNCTION__);
		return WXIMAGE_STATUS_ERROR_DST_HEIGHT;//参数错误
	}
	int src_size = 0;
	DataBuffer data_handle;
	std::shared_ptr<uint8_t>src_buffer = _ReadFile(strInput, &src_size);
	if (src_buffer) {

		//指定大小压缩
		int _target_type = target_type;//输出类型
		if (target_type == WXIMAGE_TYPE_ORIGINAL) { //从文件名获取编码格式
			int imageTypeFormName = GetImageTypeFromName(strInput);
			if (imageTypeFormName == WXIMAGE_TYPE_JPEG || imageTypeFormName == WXIMAGE_TYPE_WEBP) {
				_target_type = imageTypeFormName;
			}
			//WXIMAGE_TYPE_ORIGINAL 就交给解码处理格式
		}

		int ret = CompressSize_BufferToBuffer(src_buffer.get(), src_size, &data_handle,
			_target_type, target_size, dst_width, dst_height);
		if (ret > 0) {
			//编码成功
			FilePtr fout = new_FilePtr(strOutput, FALSE);
			if (fout.get()) { //创建输出文件成功，写入数据
				fwrite(data_handle.GetBuffer(), data_handle.GetSize(), 1, fout.get());//输出到文件
				return ret;
			}
			else {
				return WXIMAGE_STATUS_ERROR_OUTPUT_CREATE;//输出文件不能创建
			}
		}
		return ret;
	}
	WXLogW(L"%ws input[%ws] can open", __FUNCTIONW__, strInput);
	return WXIMAGE_STATUS_ERROR_INPUT_READ;//输入文件不可读
}


//功能，将输入图像内存数据压缩成接近指定大小的Jpeg/Webp文件
//参数:
//[buf]: 图像数据
//[buf_size]:图像数据长度
//[strOutput]: 输出文件路径
//[target_type]: 输出文件压缩类型，参加WXIMAGE_TYPE_等值
//[target_size]: 指定输出文件大小，单位KByte，如果为0，使用默认quality=75编码
//[dst_width]: 指定输出文件分辨率宽度，0 为原始宽度
//[dst_height]: 指定输出文件分辨率高度，0 为原始高度
//返回值: 小于0表示压缩失败，参看WXIMAGE_STATUS_ERROR_ 等值
//大于0 表示压缩成功， 返回值应该是  ABCD 四位数或者两位数
// CD 表示底层使用的quality 值
// B=1 表示预设的文件大小比最小的quality压缩出来的数据小
// A=1 表示输出文件使用原文件的icc信息重编码错误，已经忽略(会导致输出文件浏览时和原文件偏色)
WXIMAGE_API int CompressSizeU_BufferToFile(uint8_t* buf, int buf_size, const wchar_t* strOutput,
	int target_type, int target_size, int dst_width, int dst_height) {

	if (buf == nullptr || buf_size <= 0) {
		WXLogA("%s input is empty\r\n", __FUNCTION__);
		return WXIMAGE_STATUS_ERROR_INPUT_NULL; //参数错误
	}
	else if (strOutput == nullptr || wcslen(strOutput) == 0) {
		WXLogA("%s output is empty\r\n", __FUNCTION__);
		return WXIMAGE_STATUS_ERROR_OUTPUT_NULL;//参数错误
	}
	else if (target_type != WXIMAGE_TYPE_ORIGINAL &&
		target_type != WXIMAGE_TYPE_JPEG &&
		target_type != WXIMAGE_TYPE_WEBP) {
		//指定大小压缩只支持jpg和webp两种格式
		WXLogA("%s image_type[%d] is error\r\n", __FUNCTION__, target_type);
		return WXIMAGE_STATUS_ERROR_DST_IMAGETYPE;//参数错误
	}
	else if (target_size < 0) {
		WXLogA("%s target_size is error\r\n", __FUNCTION__);
		return WXIMAGE_STATUS_ERROR_TARGET_SIZE;//参数错误
	}
	else if (dst_width < 0) {
		WXLogA("%s dst_width is error\r\n", __FUNCTION__);
		return WXIMAGE_STATUS_ERROR_DST_WIDTH;//参数错误
	}
	else if (dst_height < 0) {
		WXLogA("%s dst_height is error\r\n", __FUNCTION__);
		return WXIMAGE_STATUS_ERROR_DST_HEIGHT;//参数错误
	}

	DataBuffer data_handle;
	int ret = CompressSize_BufferToBuffer(buf, buf_size, &data_handle,
		target_type, target_size, dst_width, dst_height);
	if (ret > 0) {
		//编码成功
		FilePtr fout = new_FilePtr(strOutput, FALSE);
		if (fout.get()) { //创建输出文件成功，写入数据
			fwrite(data_handle.GetBuffer(), data_handle.GetSize(), 1, fout.get());//输出到文件
			return ret;//输出成功
		}
		else {
			return WXIMAGE_STATUS_ERROR_OUTPUT_CREATE;//输出文件不能创建
		}
	}
	return ret;
}


//功能，将输入图像压缩成接近指定大小的Jpeg/Webp文件
//参数:
//[strInput]: 输入文件路径
//[handle]: 输出数据对象，HandleCreate的返回值
//[target_type]: 输出文件压缩类型，参加WXIMAGE_TYPE_等值
//[target_size]: 指定输出文件大小，单位KByte，如果为0，使用默认quality=75编码
//[dst_width]: 指定输出文件分辨率宽度，0 为原始宽度
//[dst_height]: 指定输出文件分辨率高度，0 为原始高度
//返回值: 小于0表示压缩失败，参看WXIMAGE_STATUS_ERROR_ 等值
//大于0 表示压缩成功， 返回值应该是  ABCD 四位数或者两位数
// CD 表示底层使用的quality 值
// B=1 表示预设的文件大小比最小的quality压缩出来的数据小
// A=1 表示输出文件使用原文件的icc信息重编码错误，已经忽略(会导致输出文件浏览时和原文件偏色)
WXIMAGE_API int CompressSizeU_FileToBuffer(const wchar_t* strInput, void* handle,
	int target_type, int target_size, int dst_width, int dst_height) {

	if (strInput == nullptr || wcslen(strInput) == 0) {
		WXLogA("%s input is empty\r\n", __FUNCTION__);
		return WXIMAGE_STATUS_ERROR_INPUT_NULL; //参数错误
	}
	else if (handle == nullptr) {
		WXLogA("%s output is empty\r\n", __FUNCTION__);
		return WXIMAGE_STATUS_ERROR_OUTPUT_NULL;//参数错误
	}
	else if (target_type != WXIMAGE_TYPE_ORIGINAL &&
		target_type != WXIMAGE_TYPE_JPEG &&
		target_type != WXIMAGE_TYPE_WEBP) {
		//指定大小压缩只支持jpg和webp两种格式
		WXLogA("%s image_type[%d] is error\r\n", __FUNCTION__, target_type);
		return WXIMAGE_STATUS_ERROR_DST_IMAGETYPE;//参数错误
	}
	else if (target_size < 0) {
		WXLogA("%s target_size is error\r\n", __FUNCTION__);
		return WXIMAGE_STATUS_ERROR_TARGET_SIZE;//参数错误
	}
	else if (dst_width < 0) {
		WXLogA("%s dst_width is error\r\n", __FUNCTION__);
		return WXIMAGE_STATUS_ERROR_DST_WIDTH;//参数错误
	}
	else if (dst_height < 0) {
		WXLogA("%s dst_height is error\r\n", __FUNCTION__);
		return WXIMAGE_STATUS_ERROR_DST_HEIGHT;//参数错误
	}
	int src_size = 0;
	std::shared_ptr<uint8_t>src_buffer = _ReadFile(strInput, &src_size);
	if (src_buffer) {
		//指定大小压缩
		int _target_type = target_type;//输出类型
		if (target_type == WXIMAGE_TYPE_ORIGINAL) { //从文件名获取编码格式
			int imageTypeFormName = GetImageTypeFromName(strInput);
			if (imageTypeFormName == WXIMAGE_TYPE_JPEG || imageTypeFormName == WXIMAGE_TYPE_WEBP) {
				_target_type = imageTypeFormName;
			}
			//WXIMAGE_TYPE_ORIGINAL 就交给解码处理格式
		}
		int ret = CompressSize_BufferToBuffer(src_buffer.get(), src_size, handle,
			_target_type, target_size, dst_width, dst_height);
		return ret;
	}
	WXLogW(L"%ws input[%ws] can open", __FUNCTIONW__, strInput);
	return WXIMAGE_STATUS_ERROR_INPUT_READ;//输入文件不可读
}


#endif