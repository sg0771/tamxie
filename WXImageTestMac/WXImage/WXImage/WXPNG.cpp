#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <stddef.h>
#include <setjmp.h>
#include "./libpng/libpng.h"
#include "WXImageBase.h"
#include "WXImage.h"

//-----------------------------------------------------------------------------------
static void cbPngReader(png_structp png_ptr, png_bytep data, png_size_t length) {
	DataBuffer * isource = (DataBuffer*)png_get_io_ptr(png_ptr);
	int Pos = isource->GetSize();
	memcpy(data, isource->GetBuffer() + isource->GetSize(), length);
	isource->SetPos(Pos + length);
}

int png_read_icc(uint8_t *icc_data, const uint8_t* png_buffer, int length) {
	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr) {
		return -1;
	}
	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) {
		return -2;
	}
	if (setjmp(png_jmpbuf(png_ptr))) {
		return -3;
	}

	DataBuffer PngSource;
	PngSource.Init((uint8_t*)png_buffer,length);

	png_set_read_fn(png_ptr, &PngSource, cbPngReader);//解码数据读取回调
	png_read_info(png_ptr, info_ptr);

	png_bytep icc_profile = NULL;
	png_charp unused1 = NULL;
	int unused2 = 0;
	png_uint_32 icc_len = 0;
	int has_profile = png_get_iCCP(png_ptr, info_ptr, &unused1, &unused2, &icc_profile, &icc_len);

	int dst_length = 0;
	if (has_profile) {
		dst_length = icc_len;
		if (icc_data && dst_length) {
			memcpy(icc_data, icc_profile, dst_length);
		}
	}
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	return dst_length;
}

 WXIMAGE_API int   RGBtoPNG(void* handle, int level,
	 int rgb_type, const uint8_t* rgb_buffer, int width, int height, int stride,
	 const uint8_t* icc_data /*= NULL*/, int icc_size/* = 0*/)//ICC数据
 {
	 if (handle == NULL) {
		 WXLogA( "%s handle=NULL  failed \n", __FUNCTION__);
		 return -1;
	 }
	 else if (level < 0 || level > 100) {
		 WXLogA( "%s level=%d  failed \n", __FUNCTION__, level);
		 return -2;
	 }
	 else if (rgb_type < TYPE_GRAY || rgb_type > TYPE_BGRA) {
		 WXLogA( "%s rgb_type=%d  failed \n", __FUNCTION__, rgb_type);
		 return -3;
	 }
	 else if (rgb_type == 2) {
		 WXLogA( "%s rgb_type=%d  failed \n", __FUNCTION__, rgb_type);
		 return -4;
	 }
	 else if (rgb_buffer == NULL) {
		 WXLogA( "%s rgb_buffer=NULL  failed\n", __FUNCTION__);
		 return -5;
	 }
	 else if (width <= 0 || height <= 0) {
		 WXLogA( "Size Error %s Size=[%d,%d] failed\n", __FUNCTION__, width, height);
		 return -6;
	 }
	 else if (stride < width * rgb_type) {
		 WXLogA( " Encode Erro %s stride=%d width=%d type=%d failed\n", __FUNCTION__,
			 stride, width, rgb_type);
		 return -7;
	 }
	 else if ((icc_data != 0 && icc_size <= 0) || (icc_data == 0 && icc_size > 0)) {
		 WXLogA( "Icc profile error %s icc_data=%p icc_size=%d  failed \n",
			 __FUNCTION__, icc_data, icc_size);
		 return -8;
	 }


	 DataBuffer* obj = (DataBuffer*)handle;
	 int new_size = width * height * 4;
	 obj->ReSize(new_size);//重新申請内存

	 png_structp m_png_ptr = NULL;
	 png_infop   m_info_ptr = NULL;

	 // Silence silly gcc
	 png_byte bit_depth = 8;
	 png_byte interlace_type = PNG_INTERLACE_NONE;


	 png_byte color_type = PNG_COLOR_TYPE_RGBA;
	 if (rgb_type == TYPE_BGR) {
		 //不含Alpha，只需要PNG24
		 color_type = PNG_COLOR_TYPE_RGB;
	 }
	 else if (rgb_type == TYPE_GRAY) {
		 //灰度图
		 color_type = PNG_COLOR_TYPE_GRAY;
	 }

	 uint8_t* m_data = (uint8_t*)malloc(width * height * rgb_type);//输入缓冲
	 uint8_t** m_rowPointers = (uint8_t**)malloc(height * sizeof(uint8_t*));
	 for (int i = 0; i < height; i++) {
		 m_rowPointers[i] = m_data + i * width * rgb_type;
	 }

	 if (rgb_type == TYPE_BGR) {
		 for (int i = 0; i < height; i++) {
			 for (int j = 0; j < width; j++) {
				 int pos_src = i * stride + j * rgb_type;
				 int pos_dst = i * width * rgb_type + j * rgb_type;
				 m_data[0 + pos_dst] = rgb_buffer[2 + pos_src];
				 m_data[1 + pos_dst] = rgb_buffer[1 + pos_src];
				 m_data[2 + pos_dst] = rgb_buffer[0 + pos_src];
			 }
		 }
	 }
	 else if (rgb_type == TYPE_BGRA) {
		 for (int i = 0; i < height; i++) {
			 for (int j = 0; j < width; j++) {
				 int pos_src = i * stride + j * rgb_type;
				 int pos_dst = i * width * rgb_type + j * rgb_type;
				 m_data[0 + pos_dst] = rgb_buffer[2 + pos_src];
				 m_data[1 + pos_dst] = rgb_buffer[1 + pos_src];
				 m_data[2 + pos_dst] = rgb_buffer[0 + pos_src];
				 m_data[3 + pos_dst] = rgb_buffer[3 + pos_src];
			 }
		 }
	 }
	 else if (rgb_type == TYPE_GRAY) {
		 for (int i = 0; i < height; i++) {
			 memcpy(m_data + i * width, rgb_buffer + i * stride, width);
		 }
	 }

	 /* initialize stuff */
	 m_png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	 if (!m_png_ptr) {
		 //error(-1, "png_create_write_struct failed");
		 return 0;
	 }

	 m_info_ptr = png_create_info_struct(m_png_ptr);
	 if (setjmp(png_jmpbuf(m_png_ptr))) {
		 //error(-1, "png_jmpbuf failed");
		 return 0;
	 }

	 /* write header */

	 struct png_write_state write_state;
	 write_state.buffer = obj->GetBuffer();
	 write_state.pos = 0;
	 png_set_write_fn(m_png_ptr, &write_state, user_write_data, user_flush_data);//编码回调

	 // Set up the type of PNG image and the compression level
	 png_set_compression_level(m_png_ptr, level /*Z_BEST_COMPRESSION*/);
	 //写入PNG文件头信息
	 png_set_IHDR(m_png_ptr, m_info_ptr, width, height,
		 bit_depth,
		 color_type,
		 interlace_type,
		 PNG_COMPRESSION_TYPE_DEFAULT,
		 PNG_FILTER_TYPE_DEFAULT);

	 if (icc_data != NULL && icc_size > 0)
		 png_set_iCCP(m_png_ptr, m_info_ptr, "icc", PNG_COMPRESSION_TYPE_BASE, (png_const_bytep)icc_data, icc_size);
	 else
		 png_set_sRGB(m_png_ptr, m_info_ptr, PNG_sRGB_INTENT_RELATIVE);

	 png_write_info(m_png_ptr, m_info_ptr);
	 png_write_image(m_png_ptr, m_rowPointers);//写入数据，在回调里面接收处理结果

	 /* end write */
	 png_write_end(m_png_ptr, m_info_ptr);

	 /* cleanup heap allocation */
	 png_destroy_write_struct(&m_png_ptr, &m_info_ptr);
	 free(m_data);
	 free(m_rowPointers);
	 obj->SetPos(write_state.pos);
	 return obj->GetSize();
 }

 //获取Image buffer 中的 icc profile
 WXIMAGE_API int ReadICC(uint8_t* icc_data, const uint8_t* buffer, int length) {
	 int ret = 0;
	 if (buffer[0] == 0xFF && buffer[1] == 0xD8 &&
		 buffer[length - 2] == 0xFF && buffer[length - 1] == 0xD9) {
		 ret = jpeg_read_icc(icc_data, buffer, length);
	 }
	 else  if (buffer[1] == 'P' && buffer[2] == 'N' && buffer[3] == 'G') {
		 ret = png_read_icc(icc_data, buffer, length);
	 }
	 return ret;
 }