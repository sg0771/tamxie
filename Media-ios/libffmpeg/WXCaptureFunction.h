/*
封装了WXCapture 的C 接口
*/
#ifndef _WX_CAPTURE_FUNCTION_H_
#define _WX_CAPTURE_FUNCTION_H_

#include "FfmpegIncludes.h"

struct AVFrame;
class  WXLog;
//桌面采集
//配置默认参数
WXFFMPEG_CAPI void WXCaptureDefaultParam(WXCaptureParam* param);

//启动录屏录音
WXFFMPEG_CAPI void* WXCaptureStart(WXCaptureParam* param, int *error);

//当前时间戳
WXFFMPEG_CAPI int64_t WXCaptureGetTime(void* ptr);
WXFFMPEG_CAPI int64_t WXCaptureGetVideoTime(void* ptr);
WXFFMPEG_CAPI int64_t WXCaptureGetAudioTime(void* ptr);


WXFFMPEG_CAPI int64_t WXCaptureGetVideoSize(void* ptr);
WXFFMPEG_CAPI int64_t WXCaptureGetVideoFrame(void* ptr);
WXFFMPEG_CAPI int64_t WXCaptureGetAudioSize(void* ptr);
WXFFMPEG_CAPI int64_t WXCaptureGetAudioFrame(void* ptr);

WXFFMPEG_CAPI int64_t WXCaptureGetAVDelay(void* ptr);
WXFFMPEG_CAPI int     WXCaptureGetSystemLength(void* ptr);
WXFFMPEG_CAPI int     WXCaptureGetMicLength(void* ptr);

//获取音频设备音量
WXFFMPEG_CAPI int WXCaptureGetSystemLevel(void* ptr);
WXFFMPEG_CAPI int WXCaptureGetMicLevel(void* ptr);

//设置采集音量
WXFFMPEG_CAPI void WXCaptureSetSystemLevel(void* ptr, int level);
WXFFMPEG_CAPI void WXCaptureSetSystemSilence(void* ptr, int bSilence);

WXFFMPEG_CAPI void WXCaptureSetMicLevel(void* ptr, int level);
WXFFMPEG_CAPI void WXCaptureSetMicSilence(void* ptr, int bSilence);

WXFFMPEG_CAPI void  WXCapturePause(void* ptr);
WXFFMPEG_CAPI void  WXCaptureResume(void* ptr);

WXFFMPEG_CAPI void  WXCaptureGetPicture1(void* ptr, WXCTSTR wsz, int quality);
WXFFMPEG_CAPI void  WXCaptureGetPicture2(void* ptr, WXCTSTR wsz, int quality);

//修改摄像头属性，亮度之类参数
WXFFMPEG_CAPI void  WXCaptureCameraSetting(void* ptr, HWND hwnd);
WXFFMPEG_CAPI void  WXCaptureChangeRect(void* ptr, int x, int y);//改变区域

//关闭录屏录音
WXFFMPEG_CAPI void WXCaptureStop(void* ptr);

//------------------- windows 设备枚举 ----------------------
WXFFMPEG_CAPI void WXDeviceInit(WXCTSTR wsz);//初始化
WXFFMPEG_CAPI void WXDeviceDeinit();//退出


 //WASAPI 设备属性
WXFFMPEG_CAPI void WXWasapiInit();
WXFFMPEG_CAPI void WXWasapiDeinit();
WXFFMPEG_CAPI int  WXWasapiGetRenderCount();
WXFFMPEG_CAPI SoundDeviceInfo* WXWasapiGetRenderInfo(int index);
WXFFMPEG_CAPI int WXWasapiGetCaptureCount();
WXFFMPEG_CAPI SoundDeviceInfo* WXWasapiGetCaptureInfo(int index);

//显示器设备，由于Win10 多显示器的不同分辨率问题，暂时处理两个设备
WXFFMPEG_CAPI void WXScreenInit();
WXFFMPEG_CAPI void WXScreenDeinit();
WXFFMPEG_CAPI int  WXScreenGetCount();
WXFFMPEG_CAPI MonitorInfo* WXScreenGetInfo(int index);

//一般用于预览图像。。。
WXFFMPEG_CAPI void* WXScreenOpenWithHwnd(WXCTSTR devName, HWND hwnd, int Fixed);
WXFFMPEG_CAPI void* WXScreenOpenWithSink(WXCTSTR devName, VideoCallBack cb);
WXFFMPEG_CAPI void  WXScreenClose(void*ptr);

//摄像头设备
WXFFMPEG_CAPI void WXCameraInit();
WXFFMPEG_CAPI void WXCameraDeinit();
WXFFMPEG_CAPI int  WXCameraGetCount();
WXFFMPEG_CAPI CameraInfo* WXCameraGetInfo(int index);

//打开指定摄像头
//一般用于预览图像。。。
//width height iFps 可以从 info 得到
WXFFMPEG_CAPI void* WXCameraOpenWithHwnd(WXCTSTR devGUID, int width, int height, int iFps, HWND hwnd, int Fixed);
WXFFMPEG_CAPI void* WXCameraOpenWithSink(WXCTSTR devGUID, int width, int height, int iFps, VideoCallBack cb);
WXFFMPEG_CAPI void  WXCameraSetting(void*ptr, HWND hwnd);
WXFFMPEG_CAPI void  WXCameraClose(void*ptr);

// win32 函数
//填充窗体颜色
WXFFMPEG_CAPI void WXWin32FillRGB(HWND hwnd, uint32_t rgb);
//预览某张图片
WXFFMPEG_CAPI void WXWin32Preview(HWND hwnd, WXCTSTR filanema);
//获取当前系统的版本号
WXFFMPEG_CAPI int  WXGetSystemVersion();
WXFFMPEG_CAPI int  WXSupportHarewareCodec();
WXFFMPEG_CAPI WXCTSTR WXGetH264Codec();//本机支持的H264硬编码器名字，不支持返回"libx264"

//isSystem = 1 为扬声器
//isSystem = 0 为麦克风
WXFFMPEG_CAPI  void  AudioDeviceOpen(WXCTSTR wszGuid, int isSystem);
WXFFMPEG_CAPI  int   AudioDeviceGetVolume(int isSystem);//可以在开启设备，或者开启录屏时获得音量值
WXFFMPEG_CAPI  void  AudioDeviceClose(int isSystem);

//设置截取波形长度
WXFFMPEG_CAPI void WXSetWaveLength(int n);
//获取波形数据
WXFFMPEG_CAPI int WXGetWaveData(int *pData);



class  WXFrame;
struct SwrContext;
struct AVCodecContext;
struct AVFrame;
class  DataFrame;

WXFFMPEG_CAPI void WXSaveHdcToJpeg(HDC hdc,int posx, int posy, int width, int height, int quatily, WXCTSTR wszName);

WXFFMPEG_CAPI void WXFrameCopyToDataFtame(WXFrame *wxframe, DataFrame*  df);
WXFFMPEG_CAPI SwrContext* WXMediaUtilsAllocSwrCtx(int inSampleRate, int inCh, enum AVSampleFormat inSampleFmt,
	int outSamplerate, int outCh, enum AVSampleFormat outSampleFmt);

WXFFMPEG_CAPI AVFrame *WXMediaUtilsAllocVideoFrame(enum AVPixelFormat pix_fmt, int width, int height);

WXFFMPEG_CAPI int WXMediaUtilsSaveAsJpeg(AVFrame *frame, WXCTSTR szName, int quality);

WXFFMPEG_CAPI int WXMediaUtilsSaveRGB32AsJpeg(uint8_t *buf, int width, int height, WXCTSTR szName, int quality);

WXFFMPEG_CAPI AVFrame * WXMediaUtilsAllocAudioFrame(int sample_rate, int channel, int nb_samples);
WXFFMPEG_CAPI AVFrame *WXMediaUtilsAllocAudioFrame2(enum AVSampleFormat sample_fmt, uint64_t channel_layout, int sample_rate, int nb_samples);


WXFFMPEG_CAPI WXFrame* WXMediaUtilsCreateTextWaterMarkByWinFont(WXCTSTR wszText, HFONT hfont, uint32_t Color, int x, int y);
WXFFMPEG_CAPI WXFrame* WXMediaUtilsCreateTextWaterMarkByFontName(WXCTSTR wszText, WXCTSTR wszFontName, int iFotnSize, uint32_t Color, int x, int y);
WXFFMPEG_CAPI WXFrame* WXMediaUtilsCreateImageWaterMarkByFile(WXCTSTR wszFileName, int iAlpha, int x, int y);//从图片生成YV12的水印	

WXFFMPEG_CAPI void WXMediaUtilsWXI420Copy(const uint8_t* src_y, int src_stride_y,
	const uint8_t* src_u, int src_stride_u,
	const uint8_t* src_v, int src_stride_v,
	uint8_t* dst_y, int dst_stride_y,
	uint8_t* dst_u, int dst_stride_u,
	uint8_t* dst_v, int dst_stride_v,
	int width, int height);


WXFFMPEG_CAPI int WXMediaUtilsWXARGBToI420(const uint8_t* src_argb, int src_stride_argb,
	WXFrame *wxframe, int width, int height, int mode);

WXFFMPEG_CAPI BITMAPINFO* WXMediaUtilsCreateBi(int width, int height, int bits);
WXFFMPEG_CAPI void        WXMediaUtilsDeleteBi(BITMAPINFO* bi);


//Airplay 解码都是 YUV420P
WXFFMPEG_CAPI void      WXAirplayPush(AVFrame *frame);//Airplay ，当然先把设备设置好 推送数据进行编码处理

//获取多媒体文件音频抽样值
WXFFMPEG_CAPI int  WXGetAudioVolumeData(WXCTSTR filename, int *pData, int Num, int bFullWave);

WXFFMPEG_CAPI const char * WXGetDXErrorString(HRESULT hr);
WXFFMPEG_CAPI const char * WXGetDXErrorDescription(HRESULT hr);

#endif