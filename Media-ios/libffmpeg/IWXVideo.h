/*
Windows  视频处理基类
*/
#ifndef _IWX_VIDEO_H_
#define _IWX_VIDEO_H_

#include "FfmpegIncludes.h"

class IWXPlay {
public:
	virtual WXCTSTR GetType() = 0;
	virtual void    SetFileName(WXCTSTR wszFileName) = 0;
	virtual void    SetStartTime(int64_t seek) = 0;
	virtual void    SetInitSpeed(int speed) = 0;	
	virtual bool    OpenFile() = 0;
	virtual void    Destroy() = 0;
	virtual bool    Start() = 0;
	virtual bool    Pause() = 0;
	virtual bool    Resume() = 0;
	virtual bool    Stop() = 0;
	virtual void    Refresh() = 0;

	virtual bool    ShotPicture(WXCTSTR szName, int quality) = 0;
	virtual int     GetState() = 0;
	virtual int64_t GetTotalTime() = 0;
	virtual int64_t GetCurrTime() = 0;

	virtual bool    SetView(HWND hwnd) = 0;
	virtual bool    SetVolume(int volume) = 0;
	virtual int     GetVolume() = 0; 
	virtual void    SetVideoCB(WXFfmpegOnVideoData cb) = 0;
	virtual void    SetEventOwner(void *owner) = 0;
	virtual void    SetEventCB(WXFfmpegOnEvent cb) = 0;
	virtual void    SetEventID(WXCTSTR sz) = 0;
	virtual WXCTSTR GetEventID() = 0;
	virtual bool    SetSeek(int64_t pos) = 0;
	virtual void    SetSpeed(int iSpeed) = 0; 
	virtual double  GetSpeed() = 0;

	virtual void    Reset() = 0;
	virtual void    SetSubtitle(WXCTSTR sz) = 0;
	virtual void    SetSubtitleFont(WXCTSTR szFontName, int FontSize, int FontColor) = 0;
	virtual void    SetSubtitleAlpha(int alpha) = 0;
	virtual void    SetSubtitlePostion(int postion) = 0;
	virtual void    SetSubtitleAlignment(int Alignment) = 0;

	virtual void    SetCrop(int x, int y, int w, int h) = 0;
	virtual void    SetVFlip(int b) = 0;
	virtual void    SetHFlip(int b) = 0;
	virtual void    SetRoate(int rotate) = 0;
	virtual void    SetPictureQuality(int Brightness, int Contrast, int Saturation) = 0;

	virtual void    SetBrightness(int Brightness) = 0;
	virtual void    SetContrast(int Contrast) = 0;
	virtual void    SetSaturation(int Saturation) = 0;
};

class IWXVideoRender {
public:
	virtual void    SetView(void *view) = 0;
	virtual void    SetSize(int width, int height) = 0;
	virtual int     isOpen() = 0;
	virtual int     Open() = 0;
	virtual void    Display(AVFrame *frame) = 0; //AVFrame
	virtual void    Close() = 0;
	virtual int     GetWidth() = 0;
	virtual int     GetHeight() = 0;
	virtual WXCTSTR GetType() = 0;
	virtual void    SetBgColor(double red, double greed, double blue, double alpha) = 0;
};

class IH264Decoder {
public:
	virtual int  Open(uint8_t *extradata, int extrasize) = 0;
	virtual void Close() = 0;
	virtual int  GetWidth() = 0;
	virtual int  GetHeight() = 0;
	virtual AVFrame* DecodeFrame(uint8_t *buf, int size) = 0;
};

WXFFMPEG_CAPI IWXVideoRender *IWXVideoRenderCreate();//默认显示模式
WXFFMPEG_CAPI IWXVideoRender *IWXVideoRenderCreateByName(WXCTSTR strName, int async); 
WXFFMPEG_CAPI void IWXVideoRenderDestroy(IWXVideoRender*p);

#ifdef _WIN32
WXFFMPEG_CAPI IWXPlay *IWXPlay_Create_LAV();
WXFFMPEG_CAPI void IWXPlay_Destroy_LAV(IWXPlay *);
#endif

WXFFMPEG_CAPI IWXPlay *IWXPlay_Create_FFPLAY();
WXFFMPEG_CAPI void IWXPlay_Destroy_FFPLAY(IWXPlay *);
WXFFMPEG_CAPI IH264Decoder *WXH264DecoderCreate(uint8_t *extradata, int extrasize);
WXFFMPEG_CAPI void WXH264DecoderDestroy(IH264Decoder *p);

#ifdef WIN32

class WXCapture;
struct WXCaptureParam;
class IWXVideoDev {
public:
	static IWXVideoDev *Create(WXCTSTR wszType);
	static void Destroy(IWXVideoDev * p);
public:
	virtual int     Open(const WXCaptureParam *param, WXCapture *capture) = 0;
	virtual void    Close() = 0;
	virtual void    Start(bool useSystemTime) = 0;
	virtual void    Stop() = 0;
	virtual void    DesktopChangeRect(int x, int y) = 0;
	virtual void    CameraSetting(HWND hwnd) = 0;
	virtual int     GetWidth() = 0;
	virtual int     GetHeight() = 0;
	virtual void    GetPicture(WXCTSTR wszName, int quality) = 0; //获取没加水印的采集图像
	virtual WXCTSTR GetType() = 0;
	virtual int     GetFormat() = 0;
};

#endif

#endif
