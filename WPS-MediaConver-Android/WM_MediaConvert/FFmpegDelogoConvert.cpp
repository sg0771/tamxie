#include "stdio.h"

#include "WXBase.h"
#include "FFmpegDelogoConvert.h"

#ifdef __ANDROID__
#include <jni.h>
#include <android/log.h>  
#define   LOG_TAG  "wxmedia"
#define   LOGE(...)   __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)  
#else
#define   LOGE(...)
#endif

WXCTSTR FFmpegDelogoConvert::image_fmts[]= { _T("apng"),_T("bmp_pipe"),_T("dds_pipe"),
											 _T("gif"),_T("ico"),_T("image2"),_T("image2pipe"),
											 _T("jpeg_pipe"), _T("jpegls_pipe"),_T("png_pipe"),
											 _T("smjpeg"), _T("tiff_pipe") };
int FFmpegDelogoConvert::nb_image_fmts = 12;

FFmpegDelogoConvert::FFmpegDelogoConvert()
{
	m_convert = nullptr;
	in_width = 0;
	in_height = 0;
	bimage = 0;
	logo_rects = nullptr;
	nb_rects = 0;
	m_mediainfo = nullptr;
	start_time = 0;
	end_time = 0;
	video_duration = 0; 
	oriention = 1;
}

FFmpegDelogoConvert::~FFmpegDelogoConvert()
{
	if (logo_rects)
		delete []logo_rects;
	if (m_convert) {
		StopConvert();
	}
}

int FFmpegDelogoConvert::InitSource(WXCTSTR in_file, WXCTSTR out_file, int (*logo_rectangles)[4], int nb_rects) {
	if (in_file == nullptr || out_file == nullptr || logo_rectangles == nullptr || nb_rects <= 0){
		return 0;
	}
	this->m_in_file = in_file;
	this->m_out_file = out_file;
	this->logo_rects = new int[nb_rects][4];//(int**)malloc(nb_rects * 4 * sizeof(int));

	int index = 0;
	int x, y, w, h;
	for (int i = 0; i < nb_rects; i++) {
		x = logo_rectangles[i][0];
		y = logo_rectangles[i][1];
		w = logo_rectangles[i][2];
		h = logo_rectangles[i][3];
		if (w > 0 && h > 0) {
			logo_rects[index][0] = (x >= 0 ? x : 0);
			logo_rects[index][1] = (y >= 0 ? y : 0);
			logo_rects[index][2] = w;
			logo_rects[index][3] = h;
			index++;
		}
	}
	if (index == 0){
		  LOGE("Error:logo is invalid\n");
		return 0;
	}
	this->nb_rects = index;

	if (getmediainfo() <= 0){
		  LOGE("Error:mediainfo error\n");
		return 0;
	}
	return 1;
}

//去水印方法回调函数
EXTERN_C void convert_data(void *ctx, int width, int height, uint8_t** pData, int* linesize, int64_t pts) 
{
	if (ctx) {
		FFmpegDelogoConvert* delogo_convert = (FFmpegDelogoConvert*)ctx;
		delogo_convert->DoDelogo(width, height, pData, linesize, pts);
	}
	
}

//去水印
void FFmpegDelogoConvert::DoDelogo(int width, int height, uint8_t** pData, int* linesize, int64_t pts)
{
	WXDelogos(pData, linesize, pData, linesize, width, height, WX_PIX_FMT_YUV420, logo_rects, nb_rects, 4, 0);
}

void FFmpegDelogoConvert::StartConvert()
{
	if (m_convert)
		return;
	m_convert = WXFfmpegParamCreate();
	WXFfmpegParamSetEventOwner(m_convert, this);
	WXFfmpegParamSetVideoSize(m_convert, in_width, in_height);//设置输出文件
	WXFfmpegParamSetVideoCB(m_convert, convert_data);
	if (!bimage){
		if (end_time > 0 || start_time > 0){
			WXFfmpegParamSetConvertTime(m_convert, start_time, end_time);//转换时间
		}

		//WXFfmpegParamSetVideoCodecStr(m_convert, _T("libx264"));//x264
        WXFfmpegParamSetVideoCodecStr(m_convert, _T("h264"));//可能是openh264
    
    }
	else { //处理旋转信息
		switch (oriention)
		{
		case 1:
			break;
		case 2:
			WXFfmpegParamSetHFlip(m_convert, 1);
			break;
		case 3:
			WXFfmpegParamSetRotate(m_convert, 180);
			break;
		case 4:
			WXFfmpegParamSetHFlip(m_convert, 1);
			WXFfmpegParamSetRotate(m_convert, 180);
			break;
		case 5:
			WXFfmpegParamSetVFlip(m_convert, 1);
			WXFfmpegParamSetRotate(m_convert, 90);
			break;
		case 6:
			WXFfmpegParamSetRotate(m_convert, 90);
			break;
		case 7:
			WXFfmpegParamSetVFlip(m_convert, 1);
			WXFfmpegParamSetRotate(m_convert, 270);
			break;
		case 8:
			WXFfmpegParamSetRotate(m_convert, 270);
			break;
		default:
			break;
		}
	}
	WXFfmpegParamSetVideoFmtStr(m_convert, _T("yuv420p"));

	WXFfmpegConvertVideo(m_convert, m_in_file.c_str(), m_out_file.c_str(), 1);
}

void FFmpegDelogoConvert::StopConvert() 
{
	if (m_convert) {
		WXFfmpegInterrupt(m_convert);
	}
}

int FFmpegDelogoConvert::GetConvertProgress() 
{
	int pos = 0;
	if (m_convert) {
		int state = WXFfmpegGetState(m_convert);
		if (state == FFMPEG_ERROR_PROCESS) {
			int64_t ptsTotal = WXFfmpegGetTotalTime(m_convert);
			int64_t ptsCurr = WXFfmpegGetCurrTime(m_convert);
			if (ptsCurr > 0 || ptsTotal > 0) {
				pos = (int)(ptsCurr * 100 / ptsTotal);
			}
		}
		else {
			if (state == FFMPEG_ERROR_OK||state == FFMPEG_ERROR_BREADK) {
				pos = 100;
			}
			else {
				pos = -1;
			}
			WXFfmpegParamDestroy(m_convert);
			m_convert = nullptr;
		}
	}
	return pos;
}

int FFmpegDelogoConvert::getmediainfo() 
{
	
	LOGE("getmediainfo Begin\n");
	if (!m_mediainfo) {
		int error = 0;
		m_mediainfo = WXMediaInfoCreate(m_in_file.c_str(), &error);
		int video_ch = WXMediaInfoGetVideoChannelNumber(m_mediainfo);
		if (video_ch <= 0){
		      LOGE("Error:no video channel\n");
			return 0;
		}
	}

	if (!m_mediainfo){
		LOGE("Error:mediainfo construct error\n");
		return 0;
	}

	bimage = 0;
	int audio_ch = WXMediaInfoGetAudioChannelNumber(m_mediainfo);
	if (audio_ch <= 0){ 
		WXCTSTR fmt = WXMediaInfoGetFormat(m_mediainfo);
		LOGE("WXMediaInfoGetFormat %s\n audio_ch=%d", fmt,audio_ch);
		for (int i = 0; i < nb_image_fmts; i++) {
			if (WXStrcmp(fmt, image_fmts[i]) == 0) {
				bimage = 1;
				LOGE("Is Image  %s\n", fmt);
				break;
			}
		}
	}

	//if (in_width == 0 || in_height == 0) {
	in_width = WXMediaInfoGetVideoWidth(m_mediainfo);
	in_height = WXMediaInfoGetVideoHeight(m_mediainfo);
	if (in_width <= 0 || in_height <= 0){
		LOGE("Error:in_width or in_height error\n");
		return 0;
	}else{
		LOGE("Video Size=%dx%d\n",in_width,in_height);
	}
	int sar_w = WXMediaInfoGetVideoSarWidth(m_mediainfo);
	int sar_h = WXMediaInfoGetVideoSarHeight(m_mediainfo);
	in_width = in_width * sar_w / sar_h;//target width
	in_width  = in_width / 4 * 4;
	in_height = in_height / 4 * 4;

	LOGE("Video Dst Size=%dx%d\n",in_width,in_height);

	if (!bimage) { //获取时间戳
		LOGE("WXMediaInfoGetFileDuration Begin\n");
		video_duration = WXMediaInfoGetFileDuration(m_mediainfo);
		LOGE("WXMediaInfoGetFileDuration End\n");
	}
	else { //图像标记,获取旋转信息
		LOGE("WXMediaInfoGetVideoOrientation Begin\n");
		oriention = WXMediaInfoGetVideoOrientation(m_mediainfo);
		LOGE("WXMediaInfoGetVideoOrientation End\n");
		int temp = in_width;
		switch (oriention)
		{
		case 5:
		case 6:
		case 7:
		case 8:
			in_width = in_height;
			in_height = temp;
			break;
		default:
			break;
		}
	}
	LOGE("getmediainfo End\n");
	WXMediaInfoDestroy(m_mediainfo);
	m_mediainfo = nullptr;

	return 1;
}

void FFmpegDelogoConvert::SetVideoConvertTime(int64_t _start_time, int64_t duration)
{
	if (duration > 0 && video_duration > 0) {
		end_time = _start_time + duration;
		if (_start_time >= video_duration)
			return;
		start_time = _start_time;
		if (end_time > video_duration)
			end_time = video_duration;
	}
}




