#ifndef WX__MEDIACONVERT__H
#define WX__MEDIACONVERT__H
//头
#include <WXBase.h>
#include "FfmpegConvertClass.h"

using namespace std;
//宏
#define  MAX_FFMPEG_ARGC 50

//结构体
struct WM_Data {
	WXString m_strImage = _T(""); //WaterMark Image
	int m_iWMPosX = 0;
	int m_iWMPosY = 0;
	int m_iWMWidth = 0;
	int m_iWMHeight = 0;
};

//类
class MediaConvert
{
private :
	FfmpegConvert* m_pFfmpegConv = NULL;

public :
	MediaConvert();
	virtual ~MediaConvert();

private :
	int  m_argc = 0;
	char *m_argv[MAX_FFMPEG_ARGC];
	WXString m_arrStrArgv[MAX_FFMPEG_ARGC];

	//input files
	std::vector<WXString> m_arrInput;
	//
	std::vector<WM_Data> m_arrWatermark;
	//
	void *avffmpeg_owner = nullptr;// = NULL;

//私有变量
private :
	//旋转角度
	int m_iRotate = 0;

	int m_bVFilp = 0;
	int m_bHFilp = 0;

	int m_iCropX = 0;
	int m_iCropY = 0;
	int m_iCropW = 0;
	int m_iCropH = 0;

	//速度 0.5--2.0 //默认速度 1.0   100
	int  m_iSpeed = 100;
	//音量  0-1000 //默认音量256
	int  m_iVolume = 256;


	//亮度，对比度，饱和度
	int m_iBrightness = 0;
	int m_iContrast = 50;
	int m_iSaturation = 100;


	//字幕
	WXString m_strSubtitle = _T("");
	WXString  m_strSubtitleFontName = _T("");
	int       m_iSubtitleFontSize = 20;
	int       m_iSubtitleFontColor = 0xFFFFFF;
	int       m_iSubtitleFontAlpha = 0;
	//0--270
	int       m_iSubtitlePostion = 0;
	//2 Bottom 10 center 6 Top
	int       m_iAlignment = 2;


	WXFfmpegOnEvent m_cbEvent;
	WXCTSTR m_szEvent;

	//Convert
	int64_t   m_ptsStart = 0;  //-ss
	int64_t   m_ptsDuration = 0;  //-t


	//video
	onFfmpegVideoData m_cbVideo = nullptr;
	int  m_iVodeoCodecMode = 1;// 0 Faset 1 Normal 2 Best
	WXString m_strVideoFilter = _T(""); //水印
	double    m_fFps = 0.0;//
	int       m_iVideoBitrate = 0;  //-b:v
	WXString  m_strVideoFmt = _T("noset");// -pix_fmt yuv420p
	WXString  m_strVideoCodec = _T("noset");// -vcodec libx264
	int m_iWidth = 0;
	int m_iHeight = 0;
	int m_iSrcWidth = 0;
	int m_iSrcHeight = 0;
	int m_iDARWidth = 0;
	int m_iDARHeight = 0;

	//audio
	WXString  m_strAudioCodec = _T("noset"); // -acodec aac
	int       m_iAudioBitrate = 0; // -b:a
	int       m_iAudioSampleRate = 0; // -ar 8000
	int       m_iAudioChannel = 0;  // -ac 1
	WXString m_strAudioFilter = _T(""); //-af
										//水印filter
	

public :
	//设置参数
	void SetRoate(int rotate);


	void SetVFlip(int b);
	void SetHFlip(int b);


	void SetCrop(int x, int y, int w, int h);


	void SetSpeed(float speed);
	void SetVolume(int volume);

	//质量
	void SetPictureQuality(int brightness, int contrast, int saturation);

	//字幕
	void SetSubtitle(WXCTSTR wsz);
	void SetSubtitleFont(WXCTSTR wsz, int FontSize, int FontColor);
	void SetSubtitleAlpha(int alpha);
	void SetSubtitlePostion(int postion);
	void SetSubtitleAlignment(int alignment);


	void SetEventOwner(void *ownerEvent);
	void SetEventCb(WXFfmpegOnEvent cbEvent);
	void SetEventID(WXCTSTR  szEvent);


	void SetConvertTime(int64_t ptsStart, int64_t ptsDuration);

	//video
	void SetVideoCB(onFfmpegVideoData cb);
	void SetVideoFmtStr(WXCTSTR wsz);
	void SetVideoCodecStr(WXCTSTR wsz);
	void SetVideoCodecMode(int mode);
	void SetVideoFps(double fps);
	void SetVideoSize(int width, int height);
	void SetVideoDar(int dar_width, int dar_height);
	void SetVideoBitrate(int bitrate);

	//audio
	void SetAudioCodecStr(WXCTSTR wsz);
	void SetAudioBitrate(int bitrate);
	void SetAudioSampleRate(int sample_rate);
	void SetAudioChannel(int channel);

	void AddInput(WXCTSTR wszInput);
	int LogRet(int ret);
	int WX_GCD(int m, int n);
	int64_t GetCurrTime();
	int64_t GetTotalTime();
	int GetState();
	void Break();
	int Convert(int argc, char **argv);

	int ConvertVideoFast(WXCTSTR strInput, WXCTSTR strOutput);
	int MixerAV(WXCTSTR strVideo, WXCTSTR strAudio, WXCTSTR strMixer);
	int ReplaceAudio(WXCTSTR strVideo, WXCTSTR strAudio, WXCTSTR strMixer, int copy);
	void HandleWaterMark();
	void HandleSubtitle();
	void HandleFilters();
	int ConvertVideo(WXCTSTR strInput, WXCTSTR strOutput);
	int ConvertAudio(WXCTSTR strInput, WXCTSTR strOutput);
	int ShotPicture(WXCTSTR strInput, int64_t ts, WXCTSTR strOutput);
	void AddWMImage(WXCTSTR szImage, int x = 0, int y = 0, int w = 0, int h = 0);

public :
	int ConvertVideo2Gif(WXCTSTR wszInput, WXCTSTR wszOutput, int64_t t_start, int64_t t_duration);
	int GetAudioFromVideo(WXCTSTR wszInput, WXCTSTR wszOutput, WXCTSTR wszFormat, int nb_Samples);
	int MergerFile(WXCTSTR strOutput, WXCTSTR strTemp, int fast);
	int CutFile(WXCTSTR strInput, WXCTSTR strOutput, int64_t ptsStart, int64_t ptsDuration, int Fast);

};

#endif
