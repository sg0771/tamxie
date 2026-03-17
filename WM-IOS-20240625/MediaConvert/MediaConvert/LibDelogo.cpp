// LibDelogo.c : 定义 DLL 应用程序的导出函数。
//


#include "LibDelogoAPI.h"


#if USE_LIBYUV
#include "Libyuv.h"
#endif

#if USE_OPENCV
#include <opencv2/core/core_c.h>
#include <opencv2/core/types_c.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc_c.h>
//#pragma comment(lib, "Opencv450.lib")
#endif

#ifndef _WIN32
#include <strings.h>
#endif

#define FFMAX(a,b) ((a) > (b) ? (a) : (b))
#define ROUNDED_DIV(a,b) (((a)>=0 ? (a) + ((b)>>1) : (a) - ((b)>>1))/(b))
#define FFMIN(a,b) ((a) > (b) ? (b) : (a))
//#define AV_CEIL_RSHIFT(a,b)  -((-(a)) >> (b))
#define AV_CEIL_RSHIFT(a,b) (((a) + (1<<(b)) - 1) >> (b))

/**
* Apply a simple delogo algorithm to the image in src and put the
* result in dst.
*
* The algorithm is only applied to the region specified by the logo
* parameters.
*
* @param w      width of the input image
* @param h      height of the input image
* @param logo_x x coordinate of the top left corner of the logo region
* @param logo_y y coordinate of the top left corner of the logo region
* @param logo_w width of the logo
* @param logo_h height of the logo
* @param band   the size of the band around the processed area
* @param show   show a rectangle around the processed area, useful for
*               parameters tweaking
* @param direct if non-zero perform in-place processing
*/

static void image_copy_plane(uint8_t       *dst, ptrdiff_t dst_linesize,
	const uint8_t *src, ptrdiff_t src_linesize,
	ptrdiff_t bytewidth, int height)
{
	if (!dst || !src)
		return;
	//av_assert0(abs(src_linesize) >= bytewidth);
	//av_assert0(abs(dst_linesize) >= bytewidth);
	for (; height > 0; height--) {
		memcpy(dst, src, bytewidth);
		dst += dst_linesize;
		src += src_linesize;
	}
}

static int apply_delogo(uint8_t *dst, int dst_linesize,
	uint8_t *src, int src_linesize,
	int w, int h,
	int logo_x, int logo_y, int logo_w, int logo_h,
	int band, int show)
{
	if (w > dst_linesize || w > src_linesize)
		return 0;
	int x, y;
	uint64_t interp, weightl, weightr, weightt, weightb, weight;
	uint8_t *xdst, *xsrc;

	uint8_t *topleft, *botleft, *topright;
	unsigned int left_sample, right_sample;
	int xclipl, xclipr, yclipt, yclipb;
	int logo_x1, logo_x2, logo_y1, logo_y2;

	xclipl = FFMAX(-logo_x, 0);
	xclipr = FFMAX(logo_x + logo_w - w, 0);
	yclipt = FFMAX(-logo_y, 0);
	yclipb = FFMAX(logo_y + logo_h - h, 0);

	logo_x1 = logo_x + xclipl;
	logo_x2 = logo_x + logo_w - xclipr - 1;
	logo_y1 = logo_y + yclipt;
	logo_y2 = logo_y + logo_h - yclipb - 1;

	topleft = src + logo_y1 * src_linesize + logo_x1;
	topright = src + logo_y1 * src_linesize + logo_x2;
	botleft = src + logo_y2 * src_linesize + logo_x1;

	/*if (dst != src)
		image_copy_plane(dst, dst_linesize, src, src_linesize, w, h);*/

	dst += (logo_y1 + 1) * dst_linesize;
	src += (logo_y1 + 1) * src_linesize;

	for (y = logo_y1 + 1; y < logo_y2; y++) {
		left_sample = topleft[src_linesize*(y - logo_y1)] +
			topleft[src_linesize*(y - logo_y1 - 1)] +
			topleft[src_linesize*(y - logo_y1 + 1)];
		right_sample = topright[src_linesize*(y - logo_y1)] +
			topright[src_linesize*(y - logo_y1 - 1)] +
			topright[src_linesize*(y - logo_y1 + 1)];

		for (x = logo_x1 + 1,
			xdst = dst + logo_x1 + 1,
			xsrc = src + logo_x1 + 1; x < logo_x2; x++, xdst++, xsrc++) {

			if (show && (y == logo_y1 + 1 || y == logo_y2 - 1 ||
				x == logo_x1 + 1 || x == logo_x2 - 1)) {
				*xdst = 0;
				continue;
			}

			/* Weighted interpolation based on relative distances, taking SAR into account */
			weightl = (uint64_t)(logo_x2 - x) * (y - logo_y1) * (logo_y2 - y) ;
			weightr = (uint64_t)(x - logo_x1)               * (y - logo_y1) * (logo_y2 - y) ;
			weightt = (uint64_t)(x - logo_x1) * (logo_x2 - x)               * (logo_y2 - y);
			weightb = (uint64_t)(x - logo_x1) * (logo_x2 - x) * (y - logo_y1);

			interp =
				left_sample * weightl
				+
				right_sample * weightr
				+
				(topleft[x - logo_x1] +
					topleft[x - logo_x1 - 1] +
					topleft[x - logo_x1 + 1]) * weightt
				+
				(botleft[x - logo_x1] +
					botleft[x - logo_x1 - 1] +
					botleft[x - logo_x1 + 1]) * weightb;
			weight = (weightl + weightr + weightt + weightb) * 3U;
			interp = ROUNDED_DIV(interp, weight);

			if (y >= logo_y + band && y < logo_y + logo_h - band &&
				x >= logo_x + band && x < logo_x + logo_w - band) {
				*xdst = (uint8_t)interp;
			}
			else {
				int dist = 0;

				if (x < logo_x + band)
					dist = FFMAX(dist, logo_x - x + band);
				else if (x >= logo_x + logo_w - band)
					dist = FFMAX(dist, x - (logo_x + logo_w - 1 - band));

				if (y < logo_y + band)
					dist = FFMAX(dist, logo_y - y + band);
				else if (y >= logo_y + logo_h - band)
					dist = FFMAX(dist, y - (logo_y + logo_h - 1 - band));

				*xdst = (uint8_t)((*xsrc*dist + interp*(band - dist)) / band);
			}
		}

		dst += dst_linesize;
		src += src_linesize;
	}
	return 1;
}


static int apply_delogo_rgb(uint8_t *dst, int dst_linesize,
	uint8_t *src, int src_linesize,
	int w, int h,
	int logo_x, int logo_y, int logo_w, int logo_h,
	int band, int show, int channels)
{
	if (channels<3 || w*channels > dst_linesize || w*channels > src_linesize)
		return 0;
	int x, y, ch;
	uint64_t tmp,interp, weightl, weightr, weightt, weightb, weight;
	uint8_t *xdst, *xsrc;

	uint8_t *topleft, *botleft, *topright;
	unsigned int left_sample[4], right_sample[4];
	int xclipl, xclipr, yclipt, yclipb;
	int logo_x1, logo_x2, logo_y1, logo_y2;
	int xchannel, logxchannel;

	xclipl = FFMAX(-logo_x, 0);
	xclipr = FFMAX(logo_x + logo_w - w, 0);
	yclipt = FFMAX(-logo_y, 0);
	yclipb = FFMAX(logo_y + logo_h - h, 0);

	logo_x1 = logo_x + xclipl;
	logo_x2 = logo_x + logo_w - xclipr - 1;
	logo_y1 = logo_y + yclipt;
	logo_y2 = logo_y + logo_h - yclipb - 1;

	logxchannel = logo_x1*channels;

	topleft = src + logo_y1 * src_linesize + logxchannel;
	topright = src + logo_y1 * src_linesize + logo_x2*channels;
	botleft = src + logo_y2 * src_linesize + logxchannel;

	/*if (dst != src)
		image_copy_plane(dst, dst_linesize, src, src_linesize, w*channels, h);*/

	dst += (logo_y1 + 1) * dst_linesize;
	src += (logo_y1 + 1) * src_linesize;

	for (y = logo_y1 + 1; y < logo_y2; y++) {
		for (ch = 0; ch < channels; ch++) {

			left_sample[ch] = topleft[src_linesize*(y - logo_y1) + ch] +
				topleft[src_linesize*(y - logo_y1 - 1) + ch] +
				topleft[src_linesize*(y - logo_y1 + 1) + ch];
			right_sample[ch] = topright[src_linesize*(y - logo_y1) + ch] +
				topright[src_linesize*(y - logo_y1 - 1) + ch] +
				topright[src_linesize*(y - logo_y1 + 1) + ch];
		}

		for (x = logo_x1 + 1,
			xdst = dst + logxchannel + channels,
			xsrc = src + logxchannel + channels; x < logo_x2; x++, xdst += channels, xsrc += channels) {

			if (show && (y == logo_y1 + 1 || y == logo_y2 - 1 ||
				x == logo_x1 + 1 || x == logo_x2 - 1)) {
				for (ch = 0; ch < channels; ch++) {
					*(xdst + ch) = 0;
				}
				continue;
			}

			/* Weighted interpolation based on relative distances, taking SAR into account */
			weightl = (uint64_t)(logo_x2 - x) * (y - logo_y1) * (logo_y2 - y);
			weightr = (uint64_t)(x - logo_x1)               * (y - logo_y1) * (logo_y2 - y);
			weightt = (uint64_t)(x - logo_x1) * (logo_x2 - x)               * (logo_y2 - y);
			weightb = (uint64_t)(x - logo_x1) * (logo_x2 - x) * (y - logo_y1);
			xchannel = (x - logo_x1)*channels;
			for (ch = 0; ch < channels; ch++) {
				interp =
					left_sample[ch] * weightl
					+
					right_sample[ch] * weightr
					+
					(topleft[xchannel + ch] +
						topleft[xchannel - channels + ch] +
						topleft[xchannel + channels + ch]) * weightt
					+
					(botleft[xchannel + ch] +
						botleft[xchannel - channels + ch] +
						botleft[xchannel + channels + ch]) * weightb;
				weight = (weightl + weightr + weightt + weightb) * 3U;
				tmp = interp;
				//interp = (interp == 0 ? 0 : (uint64_t)ROUNDED_DIV(interp, weight));
				interp = (uint64_t)ROUNDED_DIV(interp, weight);

				if (y >= logo_y + band && y < logo_y + logo_h - band &&
					x >= logo_x + band && x < logo_x + logo_w - band) {
					*(xdst + ch) = (uint8_t)interp;
				}
				else {
					int dist = 0;

					if (x < logo_x + band)
						dist = FFMAX(dist, logo_x - x + band);
					else if (x >= logo_x + logo_w - band)
						dist = FFMAX(dist, x - (logo_x + logo_w - 1 - band));

					if (y < logo_y + band)
						dist = FFMAX(dist, logo_y - y + band);
					else if (y >= logo_y + logo_h - band)
						dist = FFMAX(dist, y - (logo_y + logo_h - 1 - band));

					*(xdst + ch) = (uint8_t)((*(xsrc + ch)*dist + interp*(band - dist)) / band);

				}
			}
		}

		dst += dst_linesize;
		src += src_linesize;
	}
	return 1;
}


static int delogos_rgb(uint8_t *dst, uint8_t *src, int width, int height,
	int logo_rectangles[NUM_MAX_RECTANGLE][NUM_DATA_RECTANGLE], int nb_rects, int channels) 
{
	
	if (!dst || !src || !logo_rectangles || width <= 0 || height <= 0 || nb_rects <= 0)
		return 0;

	int ret = 0;
	int logo_x = 0, logo_y = 0, logo_w = 0, logo_h = 0, band = 4;
	int bcopy = 0;
	int linesize = width * channels;

	for (int rect_index = 0; rect_index < nb_rects; rect_index++) {
		logo_x = logo_rectangles[rect_index][0];
		logo_y = logo_rectangles[rect_index][1];
		logo_w = logo_rectangles[rect_index][2];
		logo_h = logo_rectangles[rect_index][3];

		logo_w += band * 2;
		logo_h += band * 2;
		logo_x -= band;
		logo_y -= band;

		if (!bcopy && (dst != src)) {
			image_copy_plane(dst, linesize, src, linesize, linesize, height);
			src = dst;
		}

		ret = apply_delogo_rgb(dst, linesize,
			src, linesize,
			width, height, logo_x, logo_y, logo_w, logo_h,
			band, 0, channels);

		bcopy = 1;
	}
	return ret;
}

static void get_pix_fmt_desc(enum WXPixelFMT pix_fmt, int* log2_chroma_w, int* log2_chroma_h, int* nb_components) {
	switch (pix_fmt)
	{
	case WX_PIX_FMT_YUV420:
		*nb_components = 3;
		*log2_chroma_w = 1;
		*log2_chroma_h = 1;
		break;
	case WX_PIX_FMT_GRAY8:
	case WX_PIX_FMT_RGB:
	case WX_PIX_FMT_RGBA:
		*nb_components = 1;
		*log2_chroma_w = 0;
		*log2_chroma_h = 0;
		break;
	default:
		*log2_chroma_w = -1;
		*log2_chroma_h = -1;
		*nb_components = -1;
		break;
	}
}

WXDELOGO_CAPI int WXDelogo(uint8_t** dst, int* dst_linesize,
	uint8_t** src, int* src_linesize,
	int w, int h, enum WXPixelFMT pix_fmt,
	int logo_x, int logo_y, int logo_w, int logo_h,
	int band, int show) 
{

	if (pix_fmt <= WX_PIX_FMT_NONE || pix_fmt >= WX_PIX_FMT_NB)
		return -1;
	logo_w += band * 2;
	logo_h += band * 2;
	logo_x -= band;
	logo_y -= band;

	int log2_chroma_w = 0, log2_chroma_h = 0, nb_components = 0;
	get_pix_fmt_desc(pix_fmt, &log2_chroma_w, &log2_chroma_h, &nb_components);
	int hsub0 = log2_chroma_w;
	int vsub0 = log2_chroma_h;
	int ret = 1;

	if (pix_fmt == WX_PIX_FMT_RGB || pix_fmt == WX_PIX_FMT_RGBA) {
		if (dst[0] != src[0])
			image_copy_plane(dst[0], dst_linesize[0], src[0], src_linesize[0], w*(pix_fmt == WX_PIX_FMT_RGB ? 3 : 4), h);
		ret = apply_delogo_rgb(dst[0], dst_linesize[0],
			src[0], src_linesize[0],
			w, h, logo_x, logo_y, logo_w, logo_h,
			band, show, pix_fmt == WX_PIX_FMT_RGB ? 3 : 4);
	}
	else {

		for (int plane = 0; plane < nb_components; plane++) {
			int hsub = plane == 1 || plane == 2 ? hsub0 : 0;
			int vsub = plane == 1 || plane == 2 ? vsub0 : 0;

			if (!dst[plane] || !src[plane]) {
				ret = -1;
				break;
			}

			if (dst != src)
				image_copy_plane(dst[plane], dst_linesize[plane], src[plane], src_linesize[plane], AV_CEIL_RSHIFT(w, hsub), AV_CEIL_RSHIFT(h, vsub));

			ret = apply_delogo(dst[plane], dst_linesize[plane],
				src[plane], src_linesize[plane],
				AV_CEIL_RSHIFT(w, hsub),
				AV_CEIL_RSHIFT(h, vsub),
				logo_x >> hsub, logo_y >> vsub,
				/* Up and left borders were rounded down, inject lost bits
				* into width and height to avoid error accumulation */
				AV_CEIL_RSHIFT(logo_w + (logo_x & ((1 << hsub) - 1)), hsub),
				AV_CEIL_RSHIFT(logo_h + (logo_y & ((1 << vsub) - 1)), vsub),
				band >> FFMIN(hsub, vsub),
				show);
			if (ret <= 0)
				break;
			//}
		}
	}
	return ret;
}

WXDELOGO_CAPI int WXDelogos(uint8_t** dst, int* dst_linesize,
	uint8_t** src, int* src_linesize,
	int w, int h, enum WXPixelFMT pix_fmt,
	int logo_rectangles[NUM_MAX_RECTANGLE][NUM_DATA_RECTANGLE], int nb_rects,
	int band, int show) {

	if (pix_fmt <= WX_PIX_FMT_NONE || pix_fmt >= WX_PIX_FMT_NB)
		return -1;

	int ret = 0;
	int logo_x = 0, logo_y = 0, logo_w = 0, logo_h = 0;

	int log2_chroma_w = 0, log2_chroma_h = 0, nb_components = 0;
	get_pix_fmt_desc(pix_fmt, &log2_chroma_w, &log2_chroma_h, &nb_components);
	int hsub0 = log2_chroma_w;
	int vsub0 = log2_chroma_h;
	int plane = 0, hsub = 0, vsub = 0;
	int bcopy = 0;

	for (int rect_index = 0; rect_index < nb_rects; rect_index++) {
		logo_x = logo_rectangles[rect_index][0];
		logo_y = logo_rectangles[rect_index][1];
		logo_w = logo_rectangles[rect_index][2];
		logo_h = logo_rectangles[rect_index][3];

		logo_w += band * 2;
		logo_h += band * 2;
		logo_x -= band;
		logo_y -= band;

		if (pix_fmt == WX_PIX_FMT_RGB || pix_fmt == WX_PIX_FMT_RGBA) {
			if (!bcopy && (dst[0] != src[0]))
				image_copy_plane(dst[0], dst_linesize[0], src[0], src_linesize[0], w*(pix_fmt == WX_PIX_FMT_RGB ? 3 : 4), h);

			ret = apply_delogo_rgb(dst[0], dst_linesize[0],
				src[0], src_linesize[0],
				w, h, logo_x, logo_y, logo_w, logo_h,
				band, show, pix_fmt == WX_PIX_FMT_RGB ? 3 : 4);
		}
		else {
			for (plane = 0; plane < nb_components; plane++) {
				hsub = plane == 1 || plane == 2 ? hsub0 : 0;
				vsub = plane == 1 || plane == 2 ? vsub0 : 0;
				if (!bcopy) {
					if (!dst[plane] || !src[plane]) {
						ret = -1;
						break;
					}

					if (dst[plane] != src[plane])
						image_copy_plane(dst[plane], dst_linesize[plane], src[plane], src_linesize[plane], AV_CEIL_RSHIFT(w, hsub), AV_CEIL_RSHIFT(h, vsub));
				}
				ret = apply_delogo(dst[plane], dst_linesize[plane],
					src[plane], src_linesize[plane],
					AV_CEIL_RSHIFT(w, hsub),
					AV_CEIL_RSHIFT(h, vsub),
					logo_x >> hsub, logo_y >> vsub,
					AV_CEIL_RSHIFT(logo_w + (logo_x & ((1 << hsub) - 1)), hsub),
					AV_CEIL_RSHIFT(logo_h + (logo_y & ((1 << vsub) - 1)), vsub),
					band >> FFMIN(hsub, vsub),
					show);
				if (ret <= 0)
					break;
				//}
			}
		}
		bcopy = 1;
	}

	return ret;
}

WXDELOGO_CAPI int WXDelogosRGB(uint8_t *dst, uint8_t *src, int width, int height,
	int logo_rectangles[NUM_MAX_RECTANGLE][NUM_DATA_RECTANGLE], int nb_rects) 
{

	return delogos_rgb(dst, src, width, height, logo_rectangles, nb_rects, 3);
}

WXDELOGO_CAPI int WXDelogosRGBA(uint8_t *dst, uint8_t *src, int width, int height,
	int logo_rectangles[NUM_MAX_RECTANGLE][NUM_DATA_RECTANGLE], int nb_rects) 
{
	return delogos_rgb(dst, src, width, height, logo_rectangles, nb_rects, 4);
}

#if USE_LIBYUV

WXDELOGO_CAPI int WXDelogosScaleYUV(uint8_t** srcData, int* srcLinesize, int in_width, int in_height,
	uint8_t** dstData, int* dstLinesize , int out_width, int out_height,
	int logo_rectangles[NUM_MAX_RECTANGLE][NUM_DATA_RECTANGLE], int nb_rects) {

	int ret = 0;

	if (in_width != out_width || in_height != out_height) { //缩放到Dst
		ret = libyuv::I420Scale(srcData[0], srcLinesize[0],
			srcData[1], srcLinesize[1],
			srcData[2], srcLinesize[2],
			in_width, in_height,
			dstData[0], dstLinesize[0],
			dstData[1], dstLinesize[1],
			dstData[2], dstLinesize[2],
			out_width, out_height, libyuv::kFilterLinear);
		if (ret < 0)
			return 0;
	}
	else { //拷贝到Dst
		if (srcData != dstData) {
			libyuv::I420Copy(srcData[0], srcLinesize[0],
				srcData[1], srcLinesize[1],
				srcData[2], srcLinesize[2],
				dstData[0], dstLinesize[0],
				dstData[1], dstLinesize[1],
				dstData[2], dstLinesize[2],
				in_width, in_height);
		}
	}

	int logo_rectangles_tmp[NUM_MAX_RECTANGLE][NUM_DATA_RECTANGLE] = { { 0,0,0,0 } };
	if (nb_rects > NUM_MAX_RECTANGLE)
		nb_rects = NUM_MAX_RECTANGLE;
	float ratio = (float)out_width / in_width;
	for (int i = 0; i < nb_rects; i++) {
		logo_rectangles_tmp[i][0] = (int)(logo_rectangles[i][0] * ratio);
		logo_rectangles_tmp[i][1] = (int)(logo_rectangles[i][1] * ratio);
		logo_rectangles_tmp[i][2] = (int)(logo_rectangles[i][2] * ratio);
		logo_rectangles_tmp[i][3] = (int)(logo_rectangles[i][3] * ratio);
	}

	return WXDelogos(dstData, dstLinesize, dstData, dstLinesize, out_width, out_height, WX_PIX_FMT_YUV420, logo_rectangles_tmp, nb_rects, 4, 0);

}

WXDELOGO_CAPI int WXDelogosScaleRGBA(uint8_t *src, int in_width, int in_height,
	uint8_t *dst, int out_width, int out_height,
	int logo_rectangles[NUM_MAX_RECTANGLE][NUM_DATA_RECTANGLE], int nb_rects) {

	int ret = 0;

	if (in_width != out_width || in_height != out_height) {

		ret=libyuv::ARGBScale(src, in_width * 4, in_width, in_height, dst, out_width * 4, out_width, out_height, libyuv::kFilterLinear);
		if (ret < 0)
			return 0;
	}
	else {
		if (src != dst) {
			memcpy(dst, src, in_width*in_height * 4);
		}
	}

	int logo_rectangles_tmp[NUM_MAX_RECTANGLE][NUM_DATA_RECTANGLE] = { { 0,0,0,0 } };
	if (nb_rects > NUM_MAX_RECTANGLE)
		nb_rects = NUM_MAX_RECTANGLE;
	float ratio = (float)out_width / in_width;
	for (int i = 0; i < nb_rects; i++) {
		logo_rectangles_tmp[i][0] = (int)(logo_rectangles[i][0] * ratio);
		logo_rectangles_tmp[i][1] = (int)(logo_rectangles[i][1] * ratio);
		logo_rectangles_tmp[i][2] = (int)(logo_rectangles[i][2] * ratio);
		logo_rectangles_tmp[i][3] = (int)(logo_rectangles[i][3] * ratio);
	}

	return WXDelogosRGBA(dst, dst, out_width, out_height, logo_rectangles, nb_rects);

}

#endif

#if USE_OPENCV

//从mask中获取不规则水印框外接矩形
static int get_logos_rects(uint8_t* mask, int w, int h, cv::Rect* logo_rectangles, int step)
{
	if (!mask || !logo_rectangles || w <= 0 || h <= 0 || step < w)
		return 0;

	IplImage *pImg = cvCreateImageHeader(cvSize(w, h), IPL_DEPTH_8U, 1);
	if (!pImg)
		return 0;
	cvSetData(pImg, mask, step);

	CvMemStorage *pStorage = cvCreateMemStorage(0);
	if (!pStorage) {
		cvReleaseImageHeader(&pImg);
		pImg = NULL;
		return 0;
	}
	//cvThreshold(pImg, pImg, 128, 255, CV_THRESH_BINARY);
	CvSeq *pContour = NULL;
	cvFindContours(pImg, pStorage, &pContour, sizeof(CvContour), 2, 1, cvPoint(0, 0));
	if (!pContour) {
		cvClearMemStorage(pStorage);
		pStorage = NULL;
		cvReleaseImageHeader(&pImg);
		pImg = NULL;
		return 0;
	}
	int nb_rects = 0;

	for (; pContour != NULL&&nb_rects < NUM_MAX_RECTANGLE; pContour = pContour->h_next) {
		CvRect rect = cvBoundingRect(pContour, 0);
		logo_rectangles[nb_rects++] = cvRect(rect.x, rect.y, rect.width, rect.height);
	}

	cvReleaseMemStorage(&pStorage);
	pStorage = NULL;
	cvReleaseImageHeader(&pImg);
	pImg = NULL;

	return nb_rects;
}

//不确定是否有问题
static int apply_delogo_mask(
	uint8_t *dst, int dst_linesize,
	uint8_t *src, int src_linesize,
	uint8_t *mask, int mask_linesize,
	int w, int h,
	int logo_x, int logo_y, int logo_w, int logo_h,
	int src_logo_x, int src_logo_y,
	int band, int divide)
{
	if (w > dst_linesize || w > src_linesize || w > mask_linesize)
		return 0;
	int x, y;
	uint64_t interp, weightl, weightr, weightt, weightb, weight;
	uint8_t *xdst, *xsrc, *xmask;

	uint8_t *topleft, *botleft, *topright;
	unsigned int left_sample, right_sample;
	int xclipl, xclipr, yclipt, yclipb;
	int logo_x1, logo_x2, logo_y1, logo_y2;

	xclipl = FFMAX(-logo_x, 0);
	xclipr = FFMAX(logo_x + logo_w - w, 0);
	yclipt = FFMAX(-logo_y, 0);
	yclipb = FFMAX(logo_y + logo_h - h, 0);

	logo_x1 = logo_x + xclipl;
	logo_x2 = logo_x + logo_w - xclipr - 1;
	logo_y1 = logo_y + yclipt;
	logo_y2 = logo_y + logo_h - yclipb - 1;

	topleft = src + logo_y1 * src_linesize + logo_x1;
	topright = src + logo_y1 * src_linesize + logo_x2;
	botleft = src + logo_y2 * src_linesize + logo_x1;

	/*if (dst != src)
	image_copy_plane(dst, dst_linesize, src, src_linesize, w, h);*/

	dst += (logo_y1 + 1) * dst_linesize;
	src += (logo_y1 + 1) * src_linesize;
	mask += (src_logo_y + 1)* mask_linesize;
	for (y = logo_y1 + 1; y < logo_y2; y++) {
		left_sample = topleft[src_linesize*(y - logo_y1)] +
			topleft[src_linesize*(y - logo_y1 - 1)] +
			topleft[src_linesize*(y - logo_y1 + 1)];
		right_sample = topright[src_linesize*(y - logo_y1)] +
			topright[src_linesize*(y - logo_y1 - 1)] +
			topright[src_linesize*(y - logo_y1 + 1)];
		for (x = logo_x1 + 1,
			xdst = dst + logo_x1 + 1,
			xsrc = src + logo_x1 + 1,
			xmask = mask+src_logo_x+1
			; x < logo_x2; x++, xdst++, xsrc++, divide == 0 ? xmask++ : (xmask += 2)) {
			if (*xmask == 0)
			{
				continue;
			}

			/* Weighted interpolation based on relative distances, taking SAR into account */
			weightl = (uint64_t)(logo_x2 - x) * (y - logo_y1) * (logo_y2 - y);
			weightr = (uint64_t)(x - logo_x1)               * (y - logo_y1) * (logo_y2 - y);
			weightt = (uint64_t)(x - logo_x1) * (logo_x2 - x)               * (logo_y2 - y);
			weightb = (uint64_t)(x - logo_x1) * (logo_x2 - x) * (y - logo_y1);

			interp =
				left_sample * weightl
				+
				right_sample * weightr
				+
				(topleft[x - logo_x1] +
					topleft[x - logo_x1 - 1] +
					topleft[x - logo_x1 + 1]) * weightt
				+
				(botleft[x - logo_x1] +
					botleft[x - logo_x1 - 1] +
					botleft[x - logo_x1 + 1]) * weightb;
			weight = (weightl + weightr + weightt + weightb) * 3U;
			interp = ROUNDED_DIV(interp, weight);

			if (y >= logo_y + band && y < logo_y + logo_h - band &&
				x >= logo_x + band && x < logo_x + logo_w - band) {
				*xdst = (uint8_t)interp;
			}
			else {
				int dist = 0;

				if (x < logo_x + band)
					dist = FFMAX(dist, logo_x - x + band);
				else if (x >= logo_x + logo_w - band)
					dist = FFMAX(dist, x - (logo_x + logo_w - 1 - band));

				if (y < logo_y + band)
					dist = FFMAX(dist, logo_y - y + band);
				else if (y >= logo_y + logo_h - band)
					dist = FFMAX(dist, y - (logo_y + logo_h - 1 - band));

				*xdst = (uint8_t)((*xsrc*dist + interp*(band - dist)) / band);
			}
		}

		dst += dst_linesize;
		src += src_linesize;
		mask += (divide == 0 ? mask_linesize : mask_linesize * 2);
	}
	return 1;
}

	static int apply_delogo_rgb_mask(uint8_t *dst, int dst_linesize,
		uint8_t *src, int src_linesize,
		uint8_t *mask, int mask_linesize,
		int w, int h,
		int logo_x, int logo_y, int logo_w, int logo_h,
		int band, int channels)
	{
		if (channels<3 || w*channels > dst_linesize || w*channels > src_linesize)
			return 0;
		int x=0, y=0, ch=0;
		uint64_t tmp, interp, weightl, weightr, weightt, weightb, weight;
		uint8_t *xdst, *xsrc, *xmask;

		uint8_t *topleft, *botleft, *topright;
		unsigned int left_sample[4], right_sample[4];
		int xclipl, xclipr, yclipt, yclipb;
		int logo_x1, logo_x2, logo_y1, logo_y2;
		int xchannel, logxchannel;

		xclipl = FFMAX(-logo_x, 0);
		xclipr = FFMAX(logo_x + logo_w - w, 0);
		yclipt = FFMAX(-logo_y, 0);
		yclipb = FFMAX(logo_y + logo_h - h, 0);

		logo_x1 = logo_x + xclipl;
		logo_x2 = logo_x + logo_w - xclipr - 1;
		logo_y1 = logo_y + yclipt;
		logo_y2 = logo_y + logo_h - yclipb - 1;

		logxchannel = logo_x1*channels;

		topleft = src + logo_y1 * src_linesize + logxchannel;
		topright = src + logo_y1 * src_linesize + logo_x2*channels;
		botleft = src + logo_y2 * src_linesize + logxchannel;

		/*if (dst != src)
		image_copy_plane(dst, dst_linesize, src, src_linesize, w*channels, h);*/

		dst += (logo_y1 + 1) * dst_linesize;
		src += (logo_y1 + 1) * src_linesize;
		mask += (logo_y1 + 1) *mask_linesize;

		for (y = logo_y1 + 1; y < logo_y2; y++) {
			for (ch = 0; ch < channels; ch++) {

				left_sample[ch] = topleft[src_linesize*(y - logo_y1) + ch] +
					topleft[src_linesize*(y - logo_y1 - 1) + ch] +
					topleft[src_linesize*(y - logo_y1 + 1) + ch];
				right_sample[ch] = topright[src_linesize*(y - logo_y1) + ch] +
					topright[src_linesize*(y - logo_y1 - 1) + ch] +
					topright[src_linesize*(y - logo_y1 + 1) + ch];
			}

			for (x = logo_x1 + 1,
				xdst = dst + logxchannel + channels,
				xsrc = src + logxchannel + channels,
				xmask = mask + logo_x1 + 1; x < logo_x2; x++, xdst += channels, xsrc += channels, xmask++) {
				if (*xmask == 0)
				{
					continue;
				}
				/* Weighted interpolation based on relative distances, taking SAR into account */
				weightl = (uint64_t)(logo_x2 - x) * (y - logo_y1) * (logo_y2 - y);
				weightr = (uint64_t)(x - logo_x1)               * (y - logo_y1) * (logo_y2 - y);
				weightt = (uint64_t)(x - logo_x1) * (logo_x2 - x)               * (logo_y2 - y);
				weightb = (uint64_t)(x - logo_x1) * (logo_x2 - x) * (y - logo_y1);
				xchannel = (x - logo_x1)*channels;
				for (ch = 0; ch < channels; ch++) {
					interp =
						left_sample[ch] * weightl
						+
						right_sample[ch] * weightr
						+
						(topleft[xchannel + ch] +
							topleft[xchannel - channels + ch] +
							topleft[xchannel + channels + ch]) * weightt
						+
						(botleft[xchannel + ch] +
							botleft[xchannel - channels + ch] +
							botleft[xchannel + channels + ch]) * weightb;
					weight = (weightl + weightr + weightt + weightb) * 3U;
					tmp = interp;
					//interp = (interp == 0 ? 0 : (uint64_t)ROUNDED_DIV(interp, weight));
					interp = (uint64_t)ROUNDED_DIV(interp, weight);

					if (y >= logo_y + band && y < logo_y + logo_h - band &&
						x >= logo_x + band && x < logo_x + logo_w - band) {
						*(xdst + ch) = (uint8_t)interp;
					}
					else {
						int dist = 0;

						if (x < logo_x + band)
							dist = FFMAX(dist, logo_x - x + band);
						else if (x >= logo_x + logo_w - band)
							dist = FFMAX(dist, x - (logo_x + logo_w - 1 - band));

						if (y < logo_y + band)
							dist = FFMAX(dist, logo_y - y + band);
						else if (y >= logo_y + logo_h - band)
							dist = FFMAX(dist, y - (logo_y + logo_h - 1 - band));

						*(xdst + ch) = (uint8_t)((*(xsrc + ch)*dist + interp*(band - dist)) / band);

					}
				}
			}

			dst += dst_linesize;
			src += src_linesize;
			mask += mask_linesize;
		}
		return 1;
	}

	WXDELOGO_CAPI int WXDelogosMask(uint8_t** dst, int* dst_linesize,
		uint8_t** src, int* src_linesize,
		uint8_t* mask,
		int w, int h, enum WXPixelFMT pix_fmt,
		int band) {
		if (pix_fmt <= WX_PIX_FMT_NONE || pix_fmt >= WX_PIX_FMT_NB)
			return -1;

		int nb_rects = 0, step=w;
		cv::Rect logo_rectangles[NUM_MAX_RECTANGLE];
		nb_rects = get_logos_rects(mask, w, h, logo_rectangles, step);

		if (nb_rects <= 0)
			return -1;

		int ret = 0;
		int logo_x = 0, logo_y = 0, logo_w = 0, logo_h = 0;

		int log2_chroma_w = 0, log2_chroma_h = 0, nb_components = 0;
		get_pix_fmt_desc(pix_fmt, &log2_chroma_w, &log2_chroma_h, &nb_components);
		int hsub0 = log2_chroma_w;
		int vsub0 = log2_chroma_h;
		int plane = 0, hsub = 0, vsub = 0;
		int bcopy = 0;

		for (int rect_index = 0; rect_index < nb_rects; rect_index++) {
			logo_x = logo_rectangles[rect_index].x;
			logo_y = logo_rectangles[rect_index].y;
			logo_w = logo_rectangles[rect_index].width;
			logo_h = logo_rectangles[rect_index].height;

			logo_w += band * 2;
			logo_h += band * 2;
			logo_x -= band;
			logo_y -= band;

			if (pix_fmt == WX_PIX_FMT_RGB || pix_fmt == WX_PIX_FMT_RGBA) {
				if (!bcopy && (dst[0] != src[0]))
					image_copy_plane(dst[0], dst_linesize[0], src[0], src_linesize[0], w*(pix_fmt == WX_PIX_FMT_RGB ? 3 : 4), h);

				ret = apply_delogo_rgb_mask(dst[0], dst_linesize[0],
					src[0], src_linesize[0],
					mask, step,
					w, h, logo_x, logo_y, logo_w, logo_h,
					band, pix_fmt == WX_PIX_FMT_RGB ? 3 : 4);
			}
			else {
				for (plane = 0; plane < nb_components; plane++) {
					hsub = plane == 1 || plane == 2 ? hsub0 : 0;
					vsub = plane == 1 || plane == 2 ? vsub0 : 0;
					if (!bcopy) {
						if (!dst[plane] || !src[plane]) {
							ret = -1;
							break;
						}

						if (dst[plane] != src[plane])
							image_copy_plane(dst[plane], dst_linesize[plane], src[plane], src_linesize[plane], AV_CEIL_RSHIFT(w, hsub), AV_CEIL_RSHIFT(h, vsub));
					}
					ret = apply_delogo_mask(dst[plane], dst_linesize[plane],
						src[plane], src_linesize[plane],
						mask, step,
						AV_CEIL_RSHIFT(w, hsub),
						AV_CEIL_RSHIFT(h, vsub),
						logo_x >> hsub, logo_y >> vsub,
						AV_CEIL_RSHIFT(logo_w + (logo_x & ((1 << hsub) - 1)), hsub),
						AV_CEIL_RSHIFT(logo_h + (logo_y & ((1 << vsub) - 1)), vsub),
						logo_x,logo_y,
						band >> FFMIN(hsub, vsub), plane == 0 ? 0 : 1);
					if (ret <= 0)
						break;
					//}
				}
			}
			bcopy = 1;
		}

		return ret;
	}

	WXDELOGO_CAPI int WXDelogosRGBMask(uint8_t *dst, uint8_t *src, uint8_t* mask, int mask_stride, int width, int height)
	{
		int nb_rects = 0, step = mask_stride;
		cv::Rect logo_rectangles[NUM_MAX_RECTANGLE];
		nb_rects = get_logos_rects(mask, width, height, logo_rectangles, step);
		if (nb_rects <= 0)
			return -1;

		int ret = 0;
		int logo_x = 0, logo_y = 0, logo_w = 0, logo_h = 0;
		int bcopy = 0, band = 4;

		for (int rect_index = 0; rect_index < nb_rects; rect_index++) {
			logo_x = logo_rectangles[rect_index].x;
			logo_y = logo_rectangles[rect_index].y;
			logo_w = logo_rectangles[rect_index].width;
			logo_h = logo_rectangles[rect_index].height;

			logo_w += band * 2;
			logo_h += band * 2;
			logo_x -= band;
			logo_y -= band;
			int line_size = width * 3;
			if (!bcopy && (dst != src))
				image_copy_plane(dst, line_size, src, line_size, line_size, height);

			ret = apply_delogo_rgb_mask(dst, line_size,
				src, line_size,
				mask, step,
				width, height, logo_x, logo_y, logo_w, logo_h,
				band, 3);

			bcopy = 1;
		}
		return ret;
	}

	WXDELOGO_CAPI int WXDelogosRGBAMask(uint8_t *dst, uint8_t *src, uint8_t* mask, int mask_stride, int width, int height)
	{
		int nb_rects = 0, step = mask_stride;
		cv::Rect logo_rectangles[NUM_MAX_RECTANGLE];
		nb_rects=get_logos_rects(mask, width, height, logo_rectangles, step);

		if (nb_rects <= 0 || step < width)
			return -1;

		int ret = 0;
		int logo_x = 0, logo_y = 0, logo_w = 0, logo_h = 0;
		int bcopy = 0, band = 4;

		for (int rect_index = 0; rect_index < nb_rects; rect_index++) {
			logo_x = logo_rectangles[rect_index].x;
			logo_y = logo_rectangles[rect_index].y;
			logo_w = logo_rectangles[rect_index].width;
			logo_h = logo_rectangles[rect_index].height;

			logo_w += band * 2;
			logo_h += band * 2;
			logo_x -= band;
			logo_y -= band;
			int line_size = width * 4;
			if (!bcopy && (dst != src))
				image_copy_plane(dst, line_size, src, line_size, line_size, height);

			ret = apply_delogo_rgb_mask(dst, line_size,
				src, line_size,
				mask, step,
				width, height, logo_x, logo_y, logo_w, logo_h,
				band, 4);

			bcopy = 1;
		}
		return ret;
	}

	WXDELOGO_CAPI int WXDelogosYUVMask2(uint8_t** dstData, int* dstLinesize,
		uint8_t** srcData, int* srcLinesize, uint8_t* mask, int width, int height) {
		return WXDelogosMask(dstData, dstLinesize, srcData, srcLinesize, mask, width, height, WX_PIX_FMT_YUV420, 4);
	}

	WXDELOGO_CAPI int WXDelogosYUVMask(uint8_t *dst, uint8_t *src, uint8_t* mask, int width, int height) {
		uint8_t* yuv_data[3];
		int yuv_linesize[3];
		int64_t datasize = width*height;
		yuv_data[0] = dst;
		yuv_linesize[0] = width;
		yuv_data[1] = dst + datasize;
		yuv_linesize[1] = width / 2;
		yuv_data[2] = dst + datasize * 5 / 4;
		yuv_linesize[2] = width / 2;

		if (src != dst) {
			memcpy(dst, src, width*height * 3 / 2);
		}

		return WXDelogosMask(yuv_data, yuv_linesize, yuv_data, yuv_linesize, mask, width, height, WX_PIX_FMT_YUV420, 4);
	}

#endif


#ifdef _WIN32

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}


#endif
