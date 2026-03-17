#ifndef _WX_FFMPEG_DELOGO_CONVERT_H_
#define _WX_FFMPEG_DELOGO_CONVERT_H_

#include "WXFFmpegConvertAPI.h"
#include "LibDelogoAPI.h"

class FFmpegDelogoConvert
{
public:
	FFmpegDelogoConvert();
	~FFmpegDelogoConvert();

private:
	static WXCTSTR image_fmts[];//支持的图像格式
	static int nb_image_fmts;

	WXCTSTR in_file;//输入文件路径
	WXCTSTR out_file;//输出文件路径
	void* m_convert;//转换器
	int in_width,in_height;//源媒体文件宽高
	int oriention;
	int bimage;//图片标志
	int (*logo_rects)[4];//水印框
	int nb_rects;//水印个数
	void* m_mediainfo;//媒体信息
	int64_t start_time, end_time;//起止时间  ms
	int64_t video_duration;//媒体文件时长  ms

public: 
	//开始转换，异步
	void StartConvert();
	//停止转换
	void StopConvert();
	//初始化转换资源
	int InitSource(WXCTSTR in_file, WXCTSTR out_file, int(*logo_rectangles)[4], int nb_rects);
	//设置转换的起止时间，毫秒
	void SetVideoConvertTime(int64_t _start_time, int64_t duration);
	//转换进度  0---1000
	int GetConvertProgress();
	
	//转换时对图像帧进行去水印
	void DoDelogo(int width, int height, uint8_t** pData, int* linesize, int64_t pts);
private:
	//获取媒体信息
	int getmediainfo();
};

#endif

