#ifndef _LIB_DELOGGO_API_H_
#define _LIB_DELOGGO_API_H_


#include <stdint.h>

#define USE_LIBYUV 1 //缩放

#ifdef _WIN32
#define USE_OPENCV 1 //不规则水印框  mask
#else
#define USE_OPENCV 0 //不规则水印框  mask
#endif

#ifdef _WIN32

#include <Windows.h>
#include <tchar.h>
#pragma warning(disable : 4068)

#ifdef LIBDELOGO_EXPORTS
#define DLLDELOGO_API  __declspec(dllexport)
#else
#define DLLDELOGO_API  __declspec(dllimport)
#endif

#else //__APPLE__
#define DLLDELOGO_API
#ifdef __cplusplus
#define EXTERN_C extern "C"
#else
#define EXTERN_C
#endif
#endif

#define WXDELOGO_CAPI EXTERN_C DLLDELOGO_API

#ifdef __cplusplus
extern "C" {
#endif
#define NUM_DATA_POINTERS 8
#define NUM_DATA_RECTANGLE 4
#define NUM_MAX_RECTANGLE 30

	//用于底层测试  不用管
#define OUTINFO_1_PARAM(fmt, var) { CHAR sOut[256]; CHAR sfmt[100]; sprintf_s(sfmt, "%s%s", "INFO--", fmt); sprintf_s(sOut, (sfmt), var); OutputDebugStringA(sOut); }
#define OUTINFO_2_PARAM(fmt,var1,var2) {CHAR sOut[256];CHAR sfmt[100];sprintf_s(sfmt,"%s%s","INFO--",fmt);sprintf_s(sOut,(sfmt),var1,var2);OutputDebugStringA(sOut);} 

	// Supported Pixel format.
	enum WXPixelFMT {
		WX_PIX_FMT_NONE = 0,
		WX_PIX_FMT_YUV420,   ///< planar YUV 4:2:0, 12bpp, YUV420P,YV12
		WX_PIX_FMT_GRAY8,     ///<        Y        ,  8bpp
		WX_PIX_FMT_RGB,      
		WX_PIX_FMT_RGBA,
		WX_PIX_FMT_NB
	};

	//单个水印去除，基于avframe数据，但是并不依赖相关库,dst与src地址相同，不用拷贝，不同需要拷贝，必须同分辨率
	//dst：目标帧data[]
	//dst_linesize：目标帧linesize[]
	//src：源帧data[]
	//src_linesize：源帧linesize[]
	//w:图像宽
	//h:图像高
	//pix_fmt:图像颜色空间，比较有限
	//logo_x:水印x坐标
	//logo_y:水印y坐标
	//logo_w:水印宽
	//logo_h:水印高
	//band:水印外扩宽
	//logo_h:是否显示去水印框
	WXDELOGO_CAPI int WXDelogo(uint8_t** dstData, int* dst_linesize,
		uint8_t** srcData, int* src_linesize,
		int w, int h, enum WXPixelFMT pix_fmt,
		int logo_x, int logo_y, int logo_w, int logo_h,
		int band, int show);

	//多个水印去除，基于avframe数据，但是并不依赖相关库
	//dst：目标帧data[]
	//dst_linesize：目标帧linesize[]
	//src：源帧data[]
	//src_linesize：源帧linesize[]
	//w:图像宽
	//h:图像高
	//pix_fmt:图像颜色空间，比较有限
	//logo_rectangles:水印数组
	//nb_rects:水印个数
	//band:水印外扩宽
	//show:是否显示去水印框
	WXDELOGO_CAPI int WXDelogos(uint8_t** dstData, int* dst_linesize,
		uint8_t** srcData, int* src_linesize,
		int w, int h, enum WXPixelFMT pix_fmt,
		int logo_rectangles[NUM_MAX_RECTANGLE][NUM_DATA_RECTANGLE], int nb_rects,
		int band, int show);

	//rgb数据去除多水印，输入输出宽高一致
	//dst：目标buf
	//src：源buf
	//width:图像宽
	//height:图像高
	//logo_rectangles:水印数组
	//nb_rects:水印个数
	WXDELOGO_CAPI int WXDelogosRGB(uint8_t *dst, uint8_t *src, int width, int height,
		int logo_rectangles[NUM_MAX_RECTANGLE][NUM_DATA_RECTANGLE], int nb_rects);

	//rgba数据去除多水印，输入输出宽高一致
	//dst：目标buf
	//src：源buf
	//width:图像宽
	//height:图像高
	//logo_rectangles:水印数组 在原图中的水印框  内部会进行缩放
	//nb_rects:水印个数
	WXDELOGO_CAPI int WXDelogosRGBA(uint8_t *dst, uint8_t *src, int width, int height,
		int logo_rectangles[NUM_MAX_RECTANGLE][NUM_DATA_RECTANGLE], int nb_rects);

#if USE_LIBYUV 
	//yuv数据去除多水印，输入输出分辨率可以不一致
	//srcData：源数据，同AVFrame的data
	//srcLinesize:同AVFrame的linesize
	//in_width:源图像宽
	//in_height:源图像高
	//dst：目标数据，同AVFrame的data
	//dstLinesize：同AVFrame的linesize
	//out_width:目标图像宽
	//out_height:目标图像高
	//logo_rectangles:水印数组
	//nb_rects:水印个数
	WXDELOGO_CAPI int WXDelogosScaleYUV(uint8_t **srcData, int*srcLinesize, int in_width, int in_height,
		uint8_t **dstData, int* dstLinesize, int out_width, int out_height,
		int logo_rectangles[NUM_MAX_RECTANGLE][NUM_DATA_RECTANGLE], int nb_rects);

	//rgba数据去除多水印，输入输出分辨率可以不一致
	//src：源buf
	//in_width:源图像宽
	//in_height:源图像高
	//dst：目标buf
	//out_width:目标图像宽
	//out_height:目标图像高
	//logo_rectangles:水印数组 在原图中的水印框  内部会进行缩放
	//nb_rects:水印个数
	WXDELOGO_CAPI int WXDelogosScaleRGBA(uint8_t *src, int in_width, int in_height,
		uint8_t *dst, int out_width, int out_height,
		int logo_rectangles[NUM_MAX_RECTANGLE][NUM_DATA_RECTANGLE], int nb_rects);

#endif

#if USE_OPENCV
	//基于avframe数据,dst与src地址相同，不用拷贝，不同需要拷贝，必须同分辨率
	//dst：目标帧data[]
	//dst_linesize：目标帧linesize[]
	//src：源帧data[]
	//src_linesize：源帧linesize[]
	//mask：不规则遮罩，像素值：255表示需要处理，0不处理
	//w:图像宽
	//h:图像高
	//pix_fmt:图像颜色空间，比较有限
	//band:水印外扩宽
	WXDELOGO_CAPI int WXDelogosMask(uint8_t** dstData, int* dst_linesize,
		uint8_t** srcData, int* src_linesize,
		uint8_t* mask,
		int w, int h, enum WXPixelFMT pix_fmt,
		int band);

	//rgb数据去除多水印，输入输出宽高一致
	//dst：目标buf
	//src：源buf
	//width:图像宽
	//height:图像高
	//logo_rectangles:水印数组
	//nb_rects:水印个数
	WXDELOGO_CAPI int WXDelogosRGBMask(uint8_t *dst, uint8_t *src, uint8_t* mask,int mask_stride, int width, int height);

	//rgba数据去除多水印，输入输出宽高一致
	//dst：目标buf
	//src：源buf
	//width:图像宽
	//height:图像高
	//logo_rectangles:水印数组 在原图中的水印框  内部会进行缩放
	//nb_rects:水印个数
	WXDELOGO_CAPI int WXDelogosRGBAMask(uint8_t *dst, uint8_t *src, uint8_t* mask, int mask_stride, int width, int height);

	//yuv数据去除多水印，输入输出宽高一致
	//紧凑的YUV数据
	WXDELOGO_CAPI int WXDelogosYUVMask(uint8_t *dst, uint8_t *src, uint8_t* mask, int width, int height);

	//yuv数据去除多水印，输入输出宽高一致
	//AVFrame  YUV420数据
	WXDELOGO_CAPI int WXDelogosYUVMask2(uint8_t** dstData, int *dstLinesize, 
		uint8_t** srcData,int* srcLinesize, uint8_t* mask, int width, int height);
#endif

#ifdef __cplusplus
};
#endif

#endif /* _LIB_DELOGGO_API_H_ */
