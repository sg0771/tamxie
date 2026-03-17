/*
将图像文件或者图像数据按照指定quality和分辨率压缩成PNG8格式图像
*/
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <stddef.h>

#include "./libpng/libpng.h"
#include "WXImageBase.h"
#include "WXImage.h"
#include "pngquant/libimagequant.h" 
#include "FreeImage/FreeImage.h"


static png_bytepp rwpng_create_row_pointers(png_infop info_ptr, png_structp png_ptr, unsigned char* base, size_t height, png_size_t rowbytes)
{
	if (!rowbytes) {
		rowbytes = png_get_rowbytes(png_ptr, info_ptr);
	}

	png_bytepp row_pointers = (png_bytepp)malloc(height * sizeof(row_pointers[0]));
	if (!row_pointers) return NULL;
	for (size_t row = 0; row < height; row++) {
		row_pointers[row] = base + row * rowbytes;
	}
	return row_pointers;
}

static int read_chunk_callback(png_structp png_ptr, png_unknown_chunkp in_chunk)
{
	if (0 == memcmp("iCCP", in_chunk->name, 5) ||
		0 == memcmp("cHRM", in_chunk->name, 5) ||
		0 == memcmp("gAMA", in_chunk->name, 5)) {
		return 0; // not handled
	}

	if (in_chunk->location == 0) {
		return 1; // ignore chunks with invalid location
	}

	struct rwpng_chunk** head = (struct rwpng_chunk**)png_get_user_chunk_ptr(png_ptr);

	struct rwpng_chunk* chunk = (struct rwpng_chunk*)malloc(sizeof(struct rwpng_chunk));
	memcpy(chunk->name, in_chunk->name, 5);
	chunk->size = in_chunk->size;
	chunk->location = in_chunk->location;
	chunk->data = in_chunk->size ? (uint8_t*)malloc(in_chunk->size) : NULL;
	if (in_chunk->size) {
		memcpy(chunk->data, in_chunk->data, in_chunk->size);
	}

	chunk->next = *head;
	*head = chunk;

	return 1; // marks as "handled", libpng won't store it
}

static void rwpng_error_handler(png_structp png_ptr, png_const_charp msg)
{
	png8_image* mainprog_ptr = (png8_image*)png_get_error_ptr(png_ptr);
	if (mainprog_ptr == NULL) abort();

	longjmp(mainprog_ptr->jmpbuf, 1);
}

static void rwpng_free_chunks(struct rwpng_chunk* chunk) {
	if (!chunk) return;
	rwpng_free_chunks(chunk->next);
	free(chunk->data);
	free(chunk);
}
static pngquant_error rwpng_write_image_init(png8_image* mainprog_ptr, png_structpp png_ptr_p, png_infopp info_ptr_p)
{
	*png_ptr_p = png_create_write_struct(PNG_LIBPNG_VER_STRING, mainprog_ptr, rwpng_error_handler, NULL);
	if (!(*png_ptr_p)) {
		return LIBPNG_INIT_ERROR;   /* out of memory */
	}
	*info_ptr_p = png_create_info_struct(*png_ptr_p);
	if (!(*info_ptr_p)) {
		png_destroy_write_struct(png_ptr_p, NULL);
		return LIBPNG_INIT_ERROR;   /* out of memory */
	}
	if (setjmp(mainprog_ptr->jmpbuf)) {
		png_destroy_write_struct(png_ptr_p, info_ptr_p);
		return LIBPNG_INIT_ERROR;   /* libpng error (via longjmp()) */
	}

	png_set_compression_level(*png_ptr_p, Z_BEST_COMPRESSION);
	png_set_compression_mem_level(*png_ptr_p, 5); // judging by optipng results, smaller mem makes libpng compress slightly better

	return SUCCESS;
}

static void rwpng_write_end(png_infopp info_ptr_p, png_structpp png_ptr_p, png_bytepp row_pointers) {
	png_write_info(*png_ptr_p, *info_ptr_p);
	png_set_packing(*png_ptr_p);
	png_write_image(*png_ptr_p, row_pointers);
	png_write_end(*png_ptr_p, NULL);
	png_destroy_write_struct(png_ptr_p, info_ptr_p);
}

static void rwpng_set_gamma(png_infop info_ptr, png_structp png_ptr, double gamma, rwpng_color_transform color)
{
	if (color != RWPNG_GAMA_ONLY && color != RWPNG_NONE) {
		png_set_gAMA(png_ptr, info_ptr, gamma);
	}
	if (color == RWPNG_SRGB) {
		png_set_sRGB(png_ptr, info_ptr, 0); // 0 = Perceptual
	}
}

static int rwpng_write_image8(uint8_t* output_buffer, png8_image* mainprog_ptr)
{
	png_structp png_ptr;
	png_infop info_ptr;

	if (mainprog_ptr->num_palette > 256) return INVALID_ARGUMENT;

	pngquant_error retval = rwpng_write_image_init(mainprog_ptr, &png_ptr, &info_ptr);
	if (retval) return retval;

	struct png_write_state write_state;
	write_state.buffer = output_buffer;
	write_state.pos = 0;
	png_set_write_fn(png_ptr, &write_state, user_write_data, user_flush_data);

	// Palette images generally don't gain anything from filtering
	png_set_filter(png_ptr, PNG_FILTER_TYPE_BASE, PNG_FILTER_VALUE_NONE);

	rwpng_set_gamma(info_ptr, png_ptr, mainprog_ptr->gamma, mainprog_ptr->output_color);


	/* set the image parameters appropriately */
	int sample_depth;
#if PNG_LIBPNG_VER > 10400 /* old libpng corrupts files with low depth */
	if (mainprog_ptr->num_palette <= 2)
		sample_depth = 1;
	else if (mainprog_ptr->num_palette <= 4)
		sample_depth = 2;
	else if (mainprog_ptr->num_palette <= 16)
		sample_depth = 4;
	else
#endif
		sample_depth = 8;



	png_set_IHDR(png_ptr, info_ptr, mainprog_ptr->width, mainprog_ptr->height,
		sample_depth, PNG_COLOR_TYPE_PALETTE,
		0, PNG_COMPRESSION_TYPE_DEFAULT,
		PNG_FILTER_TYPE_BASE);

	png_color palette[256];
	png_byte trans[256];
	unsigned int num_trans = 0;
	for (unsigned int i = 0; i < mainprog_ptr->num_palette; i++) {
		palette[i].red = mainprog_ptr->m_palette[i].r,
			palette[i].green = mainprog_ptr->m_palette[i].g,
			palette[i].blue = mainprog_ptr->m_palette[i].b,
			trans[i] = mainprog_ptr->m_palette[i].a;
		if (mainprog_ptr->m_palette[i].a < 255) {
			num_trans = i + 1;
		}
	}

	png_set_PLTE(png_ptr, info_ptr, palette, mainprog_ptr->num_palette);

	if ((mainprog_ptr->res_x > 0) && (mainprog_ptr->res_y > 0)) {
		png_set_pHYs(png_ptr, info_ptr, mainprog_ptr->res_x, mainprog_ptr->res_y, PNG_RESOLUTION_METER);
	}

	if (num_trans > 0) {
		png_set_tRNS(png_ptr, info_ptr, trans, num_trans, NULL);
	}

	rwpng_write_end(&info_ptr, &png_ptr, mainprog_ptr->row_pointers);

	return write_state.pos;
}

//PNG buffer 应该是  w*h 大小，确保空间足够
//数据要 R-G-B-A 的排列，而不是 BMP中的 B-R-G-A
static int fRGBAtoPNG8(uint8_t* _dst_buffer, int quality,
	uint8_t* rgb32_buffer, int width, int height, int res_x, int res_y) {
	liq_attr* liq = liq_attr_create();
	int qmin = quality - 40;
	if (qmin < 0)qmin = 0;
	liq_set_quality(liq, qmin, quality);
	liq_image* input_image = liq_image_create_rgba(liq, rgb32_buffer, width, height, 0);
	liq_result* remap = NULL;

	png8_image output_image = {};
	pngquant_error retval = SUCCESS;

	liq_error remap_error = liq_image_quantize(input_image, liq, &remap);//量化结果

	if (LIQ_OK == remap_error) {

		// fixed gamma ~2.2 for the web. PNG can't store exact 1/2.2
		// NB: can't change gamma here, because output_color is allowed to be an sRGB tag
		liq_set_output_gamma(remap, 0.45455);
		liq_set_dithering_level(remap, 1.0f);


		//初始化输出内存
		retval = prepare_output_image(remap, input_image, RWPNG_SRGB, &output_image);
		if (SUCCESS == retval) {
			if (LIQ_OK != liq_write_remapped_image_rows(remap, input_image, output_image.row_pointers)) {
				retval = OUT_OF_MEMORY_ERROR;
			}

			set_palette(remap, &output_image);

			double palette_error = liq_get_quantization_error(remap);
			if (palette_error >= 0) {
				int quality_percent = liq_get_quantization_quality(remap);
			}
		}
		liq_result_destroy(remap);
	}
	else if (LIQ_QUALITY_TOO_LOW == remap_error) {
		retval = TOO_LOW_QUALITY;
	}
	else {
		retval = INVALID_ARGUMENT; // dunno
	}

	//处理成功
	//编码PNG8 数据


	int Pos = 0;
	if (SUCCESS == retval) { //libpng
		output_image.res_x = res_x;
		output_image.res_y = res_y;
		Pos = rwpng_write_image8(_dst_buffer, &output_image);
	}

	if (input_image)
		liq_image_destroy(input_image);
	rwpng_free_image8(&output_image);
	liq_attr_destroy(liq);
	return Pos;
}

//PNG8 压缩功能
//功能: rgb数据压缩成PNG8图像
//参数:
//[buf]:输入图像内存数据
//[buf_size]:输入图像内存数据长度
//[handle]:编码缓存区
//[quality]:编码质量参数，0-100
//[dst_width]:指定输出宽度
//[dst_height]:指定输出高度，dst_width，dst_height同时为0时保持不变，只有一个为0按保持比例处理
//返回值: 大于0表示成功，大于10表示编码数据比原文件大，小于零参见 WXIMAGE_SATUS_ERROR_
static int _BufferToBuffer(uint8_t* buf, int buf_size,
	void* handle, int quality, int dst_width, int dst_height) {
	if (buf == nullptr || buf_size <= 0) {
		WXLogA("%s input is empty\r\n", __FUNCTION__);
		return WXIMAGE_STATUS_ERROR_INPUT_NULL; //参数错误
	}
	else if (handle == nullptr) {
		WXLogA("%s output is empty\r\n", __FUNCTION__);
		return WXIMAGE_STATUS_ERROR_OUTPUT_NULL;//参数错误
	}
	else if (quality < 0 || quality > 100) {
		WXLogA("%s target_level[%d] is error\r\n", __FUNCTION__, quality);
		return WXIMAGE_STATUS_ERROR_QUALITY;//参数错误
	}
	else if (dst_width < 0) {
		WXLogA("%s dst_width[%d] is error\r\n", __FUNCTION__, dst_width);
		return WXIMAGE_STATUS_ERROR_DST_WIDTH;//参数错误
	}
	else if (dst_height < 0) {
		WXLogA("%s dst_height[%d] is error\r\n", __FUNCTION__, dst_width);
		return WXIMAGE_STATUS_ERROR_DST_HEIGHT;//参数错误
	}

	//先解码成RGB数据
	//从内存buffer解码图像
	std::shared_ptr<void>bitmap = std::shared_ptr<void>(
		WXImage_Load(buf, buf_size),
		[](void* p) {  if (p) { WXImage_Unload(p); p = nullptr; } });

	if (bitmap.get() == nullptr) {
		return WXIMAGE_STATUS_ERROR_INPUT_DECODE; //解码错误
	}
	else {
		//开始编码
		int   _src_width = WXImage_GetWidth(bitmap.get()); //图像宽度
		int   _src_height = WXImage_GetHeight(bitmap.get());//图像高度
		int _res_x = WXImage_GetDotsPerMeterX(bitmap.get());
		int _res_y = WXImage_GetDotsPerMeterY(bitmap.get());

		int _nDstW = dst_width;
		int _nDstH = dst_height;

		if (dst_width == 0 && dst_height == 0) {
			_nDstW = _src_width;
			_nDstH = _src_height;
		}
		else if (dst_width == 0 && dst_height != 0) {
			_nDstW = ((int)((double)dst_height * (double)_src_width / (double)_src_height + 0.5) + 1) / 2 * 2;
		}
		else if (dst_width != 0 && dst_height == 0) {
			_nDstH = ((int)((double)dst_width * (double)_src_height / (double)_src_width + 0.5) + 1) / 2 * 2;
		}

		//统一转换BGRA内存数据
		FIBITMAP* pSrc = (FIBITMAP*)bitmap.get();
		std::shared_ptr<FIBITMAP>_RgbaDib
			= std::shared_ptr<FIBITMAP>(FreeImage_ConvertTo32Bits(pSrc), [](FIBITMAP* dib) {
			if (dib) {
				FreeImage_Unload(dib);
				dib = nullptr;
			}
				});

		uint8_t* rgb32_buffer = nullptr;
		int _nPitch = 0;

		std::shared_ptr< FIBITMAP> _ScaleDib;//有可能缩放处理

		// 缩放处理
		if (_nDstW != _src_width || _nDstH != _src_height) {//新图像
			_ScaleDib = std::shared_ptr< FIBITMAP>(
				FreeImage_Allocate(_nDstW, _nDstH, 32), [](FIBITMAP* dib) {
					if (dib) {
						FreeImage_Unload(dib);
						dib = nullptr;
					}
				});

			uint8_t* _pSrc = FreeImage_GetBits(_RgbaDib.get());
			int _nSrcStride = FreeImage_GetPitch(_RgbaDib.get());
			rgb32_buffer = FreeImage_GetBits(_ScaleDib.get());
			_nPitch = FreeImage_GetPitch(_ScaleDib.get());
			libyuv::ARGBScale(_pSrc, _nSrcStride, _src_width, _src_height,
				rgb32_buffer, _nPitch, _nDstW, _nDstH, libyuv::kFilterLinear);
		}
		else {
			rgb32_buffer = FreeImage_GetBits(_RgbaDib.get());
			_nPitch = FreeImage_GetPitch(_RgbaDib.get());
		}

		//BGRA 内存转 RGBA 内存
		DataBuffer* obj = (DataBuffer*)handle;
		obj->ReSize(_nDstW * _nDstH * 4);

		for (int h = 0; h < _nDstH; h++) {
			for (int w = 0; w < _nDstW; w++) {
				int pos_src = h * _nPitch + w * 4;
				uint8_t tmp = rgb32_buffer[2 + pos_src];
				rgb32_buffer[2 + pos_src] = rgb32_buffer[0 + pos_src];
				rgb32_buffer[0 + pos_src] = tmp;
			}
		}
		int _image_size = fRGBAtoPNG8(obj->GetBuffer(), quality, rgb32_buffer, _nDstW, _nDstH, _res_x, _res_y);
		if (_image_size > 0) { //编码成功，取出数据并写入文件
			obj->SetPos(_image_size);
			if (_image_size > buf_size) {
				return ERR_SIZE + WXIMAGE_TYPE_PNG;//编码数据比原文件大，返回12
			}
			return WXIMAGE_TYPE_PNG; //返回编码长度
		}
		else {
			WXLogA("%s output encode error\r\n", __FUNCTION__);
			return WXIMAGE_STATUS_ERROR_OUTPUT_ENCODE;
		}
	}

	WXLogA("%s unknown error\r\n", __FUNCTION__);
	return WXIMAGE_STATUS_ERROR_UNKNOWN;
}

//PNG8 压缩功能
//功能: rgb数据压缩成PNG8图像
//参数:
//[buf]:输入图像内存数据
//[buf_size]:输入图像内存数据长度
//[handle]:编码缓存区
//[quality]:编码质量参数，0-100
//[dst_width]:指定输出宽度
//[dst_height]:指定输出高度，dst_width，dst_height同时为0时保持不变，只有一个为0按保持比例处理
//返回值: 大于0表示编码长度，小于零参见 WXIMAGE_SATUS_ERROR_
WXIMAGE_API int CompressPNG8_BufferToBuffer(uint8_t* buf, int buf_size,
	void* handle, int quality, int dst_width, int dst_height) {
	return _BufferToBuffer(buf,buf_size,handle,quality,dst_width,dst_height);
}


//PNG8 压缩功能
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
	const char* strOutput, int quality, int dst_width, int dst_height) {
	if (buf == nullptr || buf_size <= 0) {
		WXLogA("%s input is empty\r\n", __FUNCTION__);
		return WXIMAGE_STATUS_ERROR_INPUT_NULL; //参数错误
	}
	else if (strOutput == nullptr || strlen(strOutput) == 0) {
		WXLogA("%s output is empty\r\n", __FUNCTION__);
		return WXIMAGE_STATUS_ERROR_OUTPUT_NULL;//参数错误
	}
	else if (quality < 0 || quality > 100) {
		WXLogA("%s target_level[%d] is error\r\n", __FUNCTION__, quality);
		return WXIMAGE_STATUS_ERROR_QUALITY;//参数错误
	}
	else if (dst_width < 0) {
		WXLogA("%s dst_width[%d] is error\r\n", __FUNCTION__, dst_width);
		return WXIMAGE_STATUS_ERROR_DST_WIDTH;//参数错误
	}
	else if (dst_height < 0) {
		WXLogA("%s dst_height[%d] is error\r\n", __FUNCTION__, dst_width);
		return WXIMAGE_STATUS_ERROR_DST_HEIGHT;//参数错误
	}

	DataBuffer obj;
	int ret = _BufferToBuffer(buf, buf_size,
		&obj, quality, dst_width, dst_height);
	if (ret > 0) {
		FilePtr fout = new_FilePtr(strOutput, FALSE);
		if (fout.get()) {
			fwrite(obj.GetBuffer(), obj.GetSize(), 1, fout.get());
			return ret; //返回编码长度
		}
		else {
			return WXIMAGE_STATUS_ERROR_OUTPUT_CREATE;
		}
	}
	return ret;
}

//PNG8 压缩功能
//功能: rgb数据压缩成PNG8图像
//参数:
//[strInput]:输入图像名
//[handle]:编码缓存区
//[quality]:编码质量参数，0-100
//[dst_width]:指定输出宽度
//[dst_height]:指定输出高度，dst_width，dst_height同时为0时保持不变，只有一个为0按保持比例处理
//返回值: 大于0表示编码长度，小于零参见 WXIMAGE_SATUS_ERROR_
WXIMAGE_API int CompressPNG8_FileToBuffer(const char* strInput,
	void* handle, int quality, int dst_width, int dst_height) {
	if (strInput == nullptr || strlen(strInput) == 0) {
		WXLogA("%s input is empty\r\n", __FUNCTION__);
		return WXIMAGE_STATUS_ERROR_INPUT_NULL; //参数错误
	}
	else if (handle == nullptr) {
		WXLogA("%s output is empty\r\n", __FUNCTION__);
		return WXIMAGE_STATUS_ERROR_OUTPUT_NULL;//参数错误
	}
	else if (quality < 0 || quality > 100) {
		WXLogA("%s target_level[%d] is error\r\n", __FUNCTION__, quality);
		return WXIMAGE_STATUS_ERROR_QUALITY;//参数错误
	}
	else if (dst_width < 0) {
		WXLogA("%s dst_width[%d] is error\r\n", __FUNCTION__, dst_width);
		return WXIMAGE_STATUS_ERROR_DST_WIDTH;//参数错误
	}
	else if (dst_height < 0) {
		WXLogA("%s dst_height[%d] is error\r\n", __FUNCTION__, dst_width);
		return WXIMAGE_STATUS_ERROR_DST_HEIGHT;//参数错误
	}
	int buf_size = 0;
	std::shared_ptr<uint8_t>buf = _ReadFile(strInput, &buf_size);
	if (buf.get() == nullptr) {
		WXLogA("%s input[%s] is not a image\r\n", __FUNCTION__, strInput);
		return WXIMAGE_STATUS_ERROR_INPUT_READ;
	}
	int ret = _BufferToBuffer(buf.get(), buf_size,
		handle, quality, dst_width, dst_height);
	return ret;
}


//PNG8 压缩功能
//功能: rgb数据压缩成PNG8图像
//参数:
//[strInput]:输入图像文件名
//[strOutput]:速出文件名
//[quality]:编码质量参数，0-100
//[dst_width]:指定输出宽度
//[dst_height]:指定输出高度，dst_width，dst_height同时为0时保持不变，只有一个为0按保持比例处理
//返回值: 大于0表示编码长度，小于零参见 WXIMAGE_SATUS_ERROR_
WXIMAGE_API int CompressPNG8_FileToFile(const char* strInput,
	const char* strOutput, int quality, int dst_width, int dst_height) {
	if (strInput == nullptr || strlen(strInput) == 0) {
		WXLogA("%s input is empty\r\n", __FUNCTION__);
		return WXIMAGE_STATUS_ERROR_INPUT_NULL; //参数错误
	}
	else if (strOutput == nullptr || strlen(strOutput) == 0) {
		WXLogA("%s output is empty\r\n", __FUNCTION__);
		return WXIMAGE_STATUS_ERROR_OUTPUT_NULL;//参数错误
	}
	else if (quality < 0 || quality > 100) {
		WXLogA("%s target_level[%d] is error\r\n", __FUNCTION__, quality);
		return WXIMAGE_STATUS_ERROR_QUALITY;//参数错误
	}
	else if (dst_width < 0) {
		WXLogA("%s dst_width[%d] is error\r\n", __FUNCTION__, dst_width);
		return WXIMAGE_STATUS_ERROR_DST_WIDTH;//参数错误
	}
	else if (dst_height < 0) {
		WXLogA("%s dst_height[%d] is error\r\n", __FUNCTION__, dst_width);
		return WXIMAGE_STATUS_ERROR_DST_HEIGHT;//参数错误
	}

	int buf_size = 0;
	std::shared_ptr<uint8_t>buf = _ReadFile(strInput, &buf_size);
	if (buf.get() == nullptr) {
		WXLogA("%s input[%s] is not a image\r\n", __FUNCTION__, strInput);
		return WXIMAGE_STATUS_ERROR_INPUT_READ;
	}
	DataBuffer obj;
	int ret = _BufferToBuffer(buf.get(), buf_size,
		&obj, quality, dst_width, dst_height);
	if (ret > 0) {
		FilePtr fout = new_FilePtr(strOutput, FALSE);
		if (fout.get()) {
			fwrite(obj.GetBuffer(), obj.GetSize(), 1, fout.get());
			return ret; //返回编码长度
		}
		else {
			return WXIMAGE_STATUS_ERROR_OUTPUT_CREATE;
		}
	}
	return ret;
}


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
//返回值: 大于0表示编码长度，小于零参见 WXIMAGE_SATUS_ERROR_
WXIMAGE_API int CompressPNG8U_BufferToFile(uint8_t* buf, int buf_size,
	const wchar_t* strOutput, int quality, int dst_width, int dst_height) {
	if (buf == nullptr || buf_size <= 0) {
		WXLogA("%s input is empty\r\n", __FUNCTION__);
		return WXIMAGE_STATUS_ERROR_INPUT_NULL; //参数错误
	}
	else if (strOutput == nullptr || wcslen(strOutput) == 0) {
		WXLogA("%s output is empty\r\n", __FUNCTION__);
		return WXIMAGE_STATUS_ERROR_OUTPUT_NULL;//参数错误
	}
	else if (quality < 0 || quality > 100) {
		WXLogA("%s target_level[%d] is error\r\n", __FUNCTION__, quality);
		return WXIMAGE_STATUS_ERROR_QUALITY;//参数错误
	}
	else if (dst_width < 0) {
		WXLogA("%s dst_width[%d] is error\r\n", __FUNCTION__, dst_width);
		return WXIMAGE_STATUS_ERROR_DST_WIDTH;//参数错误
	}
	else if (dst_height < 0) {
		WXLogA("%s dst_height[%d] is error\r\n", __FUNCTION__, dst_width);
		return WXIMAGE_STATUS_ERROR_DST_HEIGHT;//参数错误
	}

	DataBuffer obj;
	int ret = _BufferToBuffer(buf, buf_size,
		&obj, quality, dst_width, dst_height);
	if (ret > 0) {
		FilePtr fout = new_FilePtr(strOutput, FALSE);
		if (fout.get()) {
			fwrite(obj.GetBuffer(), obj.GetSize(), 1, fout.get());
			return ret; //返回编码长度
		}
		else {
			return WXIMAGE_STATUS_ERROR_OUTPUT_CREATE;
		}
	}
	return ret;
}

//PNG8 压缩功能
//功能: rgb数据压缩成PNG8图像
//参数:
//[strInput]:输入图像名
//[handle]:编码缓存区
//[quality]:编码质量参数，0-100
//[dst_width]:指定输出宽度
//[dst_height]:指定输出高度，dst_width，dst_height同时为0时保持不变，只有一个为0按保持比例处理
//返回值: 大于0表示编码长度，小于零参见 WXIMAGE_SATUS_ERROR_
WXIMAGE_API int CompressPNG8U_FileToBuffer(const wchar_t* strInput,
	void* handle, int quality, int dst_width, int dst_height) {
	if (strInput == nullptr || wcslen(strInput) == 0) {
		WXLogA("%s input is empty\r\n", __FUNCTION__);
		return WXIMAGE_STATUS_ERROR_INPUT_NULL; //参数错误
	}
	else if (handle == nullptr) {
		WXLogA("%s output is empty\r\n", __FUNCTION__);
		return WXIMAGE_STATUS_ERROR_OUTPUT_NULL;//参数错误
	}
	else if (quality < 0 || quality > 100) {
		WXLogA("%s target_level[%d] is error\r\n", __FUNCTION__, quality);
		return WXIMAGE_STATUS_ERROR_QUALITY;//参数错误
	}
	else if (dst_width < 0) {
		WXLogA("%s dst_width[%d] is error\r\n", __FUNCTION__, dst_width);
		return WXIMAGE_STATUS_ERROR_DST_WIDTH;//参数错误
	}
	else if (dst_height < 0) {
		WXLogA("%s dst_height[%d] is error\r\n", __FUNCTION__, dst_width);
		return WXIMAGE_STATUS_ERROR_DST_HEIGHT;//参数错误
	}
	int buf_size = 0;
	std::shared_ptr<uint8_t>buf = _ReadFile(strInput, &buf_size);
	if (buf.get() == nullptr) {
		WXLogA("%s input[%s] is not a image\r\n", __FUNCTION__, strInput);
		return WXIMAGE_STATUS_ERROR_INPUT_READ;
	}
	int ret = _BufferToBuffer(buf.get(), buf_size,
		handle, quality, dst_width, dst_height);
	return ret;
}


//PNG8 压缩功能
//功能: rgb数据压缩成PNG8图像
//参数:
//[strInput]:输入图像文件名
//[strOutput]:速出文件名
//[quality]:编码质量参数，0-100
//[dst_width]:指定输出宽度
//[dst_height]:指定输出高度，dst_width，dst_height同时为0时保持不变，只有一个为0按保持比例处理
//返回值: 大于0表示编码长度，小于零参见 WXIMAGE_SATUS_ERROR_
WXIMAGE_API int CompressPNG8U_FileToFile(const wchar_t* strInput,
	const wchar_t* strOutput, int quality, int dst_width, int dst_height) {
	if (strInput == nullptr || wcslen(strInput) == 0) {
		WXLogA("%s input is empty\r\n", __FUNCTION__);
		return WXIMAGE_STATUS_ERROR_INPUT_NULL; //参数错误
	}
	else if (strOutput == nullptr || wcslen(strOutput) == 0) {
		WXLogA("%s output is empty\r\n", __FUNCTION__);
		return WXIMAGE_STATUS_ERROR_OUTPUT_NULL;//参数错误
	}
	else if (quality < 0 || quality > 100) {
		WXLogA("%s target_level[%d] is error\r\n", __FUNCTION__, quality);
		return WXIMAGE_STATUS_ERROR_QUALITY;//参数错误
	}
	else if (dst_width < 0) {
		WXLogA("%s dst_width[%d] is error\r\n", __FUNCTION__, dst_width);
		return WXIMAGE_STATUS_ERROR_DST_WIDTH;//参数错误
	}
	else if (dst_height < 0) {
		WXLogA("%s dst_height[%d] is error\r\n", __FUNCTION__, dst_width);
		return WXIMAGE_STATUS_ERROR_DST_HEIGHT;//参数错误
	}

	int buf_size = 0;
	std::shared_ptr<uint8_t>buf = _ReadFile(strInput, &buf_size);
	if (buf.get() == nullptr) {
		WXLogA("%s input[%s] is not a image\r\n", __FUNCTION__, strInput);
		return WXIMAGE_STATUS_ERROR_INPUT_READ;
	}
	DataBuffer obj;
	int ret = _BufferToBuffer(buf.get(), buf_size,
		&obj, quality, dst_width, dst_height);
	if (ret > 0) {
		FilePtr fout = new_FilePtr(strOutput, FALSE);
		if (fout.get()) {
			fwrite(obj.GetBuffer(), obj.GetSize(), 1, fout.get());
			return ret; //返回编码长度
		}
		else {
			return WXIMAGE_STATUS_ERROR_OUTPUT_CREATE;
		}
	}
	return ret;
}


#endif
