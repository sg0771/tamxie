//
//  WXMediaAPI.h
//

#ifndef _WX_MEDIA_API_H_
#define _WX_MEDIA_API_H_

#include <stdint.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>


#ifdef _WIN32

#include <Windows.h>
#include <tchar.h>
#pragma warning(disable : 4068)

#ifdef _WX_EXPORT
#define DLL_API  __declspec(dllexport)
#else
#define DLL_API  __declspec(dllimport)
#endif

#define WXCTSTR const wchar_t*
#define WXTSTR  wchar_t*

#else //__APPLE__
#define MAX_PATH  1024
#define HWND void*
#define DLL_API
#define WXCTSTR const char*
#define WXTSTR  char*
#ifdef __cplusplus
#define EXTERN_C extern "C"
#else
#define EXTERN_C
#endif
#endif

#define WXFFMPEG_CAPI EXTERN_C DLL_API

#ifdef __cplusplus
extern "C" {
#endif

//视频操作回调函数
typedef void(*WXFfmpegOnEvent)(void *ctx, WXCTSTR szID, uint32_t iEvent, WXCTSTR szMsg);

//视频数据回调函数
typedef void(*WXFfmpegOnVideoData)(uint8_t* buf, int width, int height);

typedef struct WXCtx WXCtx;

enum {
    FFMPEG_ERROR_OK = 0,
    FFMPEG_ERROR_NOFILE = 1,
    FFMPEG_ERROR_EMPTYFILE = 2,
    FFMPEG_ERROR_INIT = 3,
    FFMPEG_ERROR_READFILE = 4,
    FFMPEG_ERROR_PARSE = 5,
    FFMPEG_ERROR_BREADK = 6,
    FFMPEG_ERROR_NO_MEIDADATA = 7,
    FFMPEG_ERROR_PROCESS = 8,
    FFMPEG_ERROR_NO_OUTPUT_FILE = 9,
    FFMPEG_ERROR_TRANSCODE = 10,
    FFMPEG_ERROR_DECODE_ERROR_STAT = 11,
    FFMPEG_ERROR_ASSERT_AVOPTIONS = 12,
    FFMPEG_ERROR_ABORT_CODEC_EXPERIMENTAL = 13,
    FFMPEG_ERROR_DO_AUDIO_OUT = 14,
    FFMPEG_ERROR_DO_SUBTITLE_OUT = 15,
    FFMPEG_ERROR_DO_VIDEO_OUT = 16,
    FFMPEG_ERROR_DO_VIDEO_STAT = 17,
    FFMPEG_ERROR_READ_FILTERS = 18,
    FFMPEG_ERROR_FLUSH_ENCODERS = 19,
    FFMPEG_ERROR_ON_FILTERS = 20,
    FFMPEG_ERROR_ON_OPTS = 21,
    FFMPEG_ERROR_LIBAVUTIL = 22,
    FFMPEG_ERROR_EXIT_ON_ERROR = 23,

    FFPLAY_ERROR_OK_START = 30,    //ffplay 启动
    FFPLAY_ERROR_OK_STOP   = 31,    //ffplay 结束
    FFPLAY_ERROR_OK_GET_PICTURE = 32,//ffplay 获取图片成功

    FFPLAY_ERROR_OK_GET_EOF = 33,    //ffplay 读取文件完毕
    FFPLAY_ERROR_OK_VIDEO_STOP = 34,    //ffplay 视频数据播放完毕
    FFPLAY_ERROR_OK_AUDIO_STOP = 35,    //ffplay 音频数据播放完毕
	FFPLAY_ERROR_OK_FINISH = 36, //Add by Vicky
};

//播放速度
#define AV_SPEED_SLOW 50  //0.5 speed
#define	AV_SPEED_NORMAL  100  //1.0 speed
#define	AV_SPEED_SPEED  150  //1.5 speed
#define	AV_SPEED_FAST  200 //2.0 speed
	
//播放器状态
#define FFPLAY_STATE_UNAVAILABLE 0 //不可用， 刚创建的对象或者已经销毁的对象
#define FFPLAY_STATE_WAITING     1 //已经创建或者进行了Stop操作，等待Start或者Destroy
#define FFPLAY_STATE_PLAYING     2 //正在播放状态
#define FFPLAY_STATE_PAUSE       3// 已经开始播放，但是处于暂停状态
#define FFPLAY_STATE_PLAYING_END 4 //数据播放完毕

WXFFMPEG_CAPI int64_t  WXGetTimeMs();     //时间戳
WXFFMPEG_CAPI void     WXSleepMs(int ms); //Sleep x ms
WXFFMPEG_CAPI void     WXSetLogFile(WXCTSTR wszFileName); //设置Log文件
WXFFMPEG_CAPI WXTSTR   WXStrcpy(WXTSTR str1, WXCTSTR str2);//字符串拷贝
WXFFMPEG_CAPI int      WXStrcmp(WXCTSTR str1, WXCTSTR str2);//字符串比较
WXFFMPEG_CAPI int      WXStrlen(WXCTSTR str); //字符串长度
WXFFMPEG_CAPI WXCTSTR  WXStrdup(WXCTSTR str); //字符串拷贝构造，需要用free来销毁内存

WXFFMPEG_CAPI void     WXFfmpegInit(void);//ffmpeg 初始化
WXFFMPEG_CAPI void     WXFfmpegDeinit(void);//ffmpeg 退出
WXFFMPEG_CAPI WXCTSTR  WXFfmpegGetError(int code);//获取错误代码
WXFFMPEG_CAPI void     WXFfmpegSetLogFile(WXCTSTR szFileName);//设置Log文件名
WXFFMPEG_CAPI void     WXLogWriteNew(const char *format, ...);//全局log函数

//获取多媒体文件信息
WXFFMPEG_CAPI void*    WXMediaInfoCreate(WXCTSTR szFileName, int *error);
WXFFMPEG_CAPI void     WXMediaInfoDestroy(void *p);

WXFFMPEG_CAPI int      WXMediaInfoGetPicture(void *p,  WXCTSTR szFileName);
WXFFMPEG_CAPI int      WXMediaInfoGetAudioChannelNumber(void *p);//Type=0
WXFFMPEG_CAPI int      WXMediaInfoGetVideoChannelNumber(void *p);//Type=1
WXFFMPEG_CAPI int      WXMediaInfoGetAttachChannelNumber(void *p);//Type=2 //add

WXFFMPEG_CAPI int64_t  WXMediaInfoGetFileSize(void *p);
WXFFMPEG_CAPI int64_t  WXMediaInfoGetFileDuration(void *p);
WXFFMPEG_CAPI WXCTSTR  WXMediaInfoGetFormat(void *p);
WXFFMPEG_CAPI int      WXMediaInfoGetChannelNumber(void *p);
WXFFMPEG_CAPI int64_t  WXMediaInfoGetVideoBitrate(void *p);

//获取视频分辨率，相当于PAR
WXFFMPEG_CAPI int      WXMediaInfoGetVideoWidth(void *p);
WXFFMPEG_CAPI int      WXMediaInfoGetVideoHeight(void *p);

//获取视频显示比例 DAR
WXFFMPEG_CAPI int      WXMediaInfoGetVideoDisplayRatioWidth(void *p);
WXFFMPEG_CAPI int      WXMediaInfoGetVideoDisplayRatioHeight(void *p);

//获得 Channle 的 类型
WXFFMPEG_CAPI int      WXMediaInfoChannelGetType(void *p, int index);//0 Audio 1 Video 2 Attach //change
WXFFMPEG_CAPI WXCTSTR  WXMediaInfoChannelCodec(void *p, int index);
WXFFMPEG_CAPI int      WXMediaInfoChannelAudioBitrate(void *p, int index);
WXFFMPEG_CAPI int      WXMediaInfoChannelAudioSampleRate(void *p, int index);
WXFFMPEG_CAPI int      WXMediaInfoChannelAudioChanels(void *p, int index);

//attach MJPG , 获得MP3 专辑封面的 JPG内存数据
//PNG 或者 JPG 数据格式
WXFFMPEG_CAPI int      WXMediaInfoChannelGetAttachSize(void *p, int index); //add
WXFFMPEG_CAPI uint8_t* WXMediaInfoChannelGetAttachData(void *p, int index); //add
WXFFMPEG_CAPI int      WXMediaInfoGetAudioPitcutre(void *p, WXCTSTR strName);



//  视频转换 API
WXFFMPEG_CAPI void*    WXFfmpegParamCreate(void);
WXFFMPEG_CAPI void     WXFfmpegParamDestroy(void * p);
WXFFMPEG_CAPI void     WXFfmpegParamSetEventOwner(void * p, void *ownerEvent);
WXFFMPEG_CAPI void     WXFfmpegParamSetEventCb(void * p, WXFfmpegOnEvent cbEvent);
WXFFMPEG_CAPI void     WXFfmpegParamSetEventID(void * p, WXCTSTR szEvent);

// 水印图片设置
// 不可以重复Add相同的文件名
//最后的4个参数是基于左上角的坐标系
WXFFMPEG_CAPI void     WXFfmpegParamAddWMImage(void * p, WXCTSTR szImage, int x, int y, int w, int h);

//设置旋转角度
WXFFMPEG_CAPI void     WXFfmpegParamSetRotate(void * p, int rotate);

//设置垂直旋转
WXFFMPEG_CAPI void     WXFfmpegParamSetVFlip(void * p, int b);

//设置水平旋转
WXFFMPEG_CAPI void     WXFfmpegParamSetHFlip(void * p, int b);

//设置裁剪区域
WXFFMPEG_CAPI void     WXFfmpegParamSetCrop(void * p, int x, int y, int w, int h);

//输入参数50-200, 文件转码后的播放速度 0.5-2.0
WXFFMPEG_CAPI void     WXFfmpegParamSetSpeed(void * p, int speed); 

//亮度(-100,100) 默认0,对比度 (-100,100)  默认 50,饱和度(0,300) 默认100
WXFFMPEG_CAPI void     WXFfmpegParamSetPictureQuality(void * p, int brightness, int contrast, int saturation);

//音量 0-1000  默认256
WXFFMPEG_CAPI void     WXFfmpegParamSetVolume(void * p, int volume);

//用于依次输入合并文件
WXFFMPEG_CAPI void     WXFfmpegParamAddInput(void * p, WXCTSTR szInput);

//字幕设置
WXFFMPEG_CAPI void     WXFfmpegParamSetSubtitle(void * p, WXCTSTR sz);
WXFFMPEG_CAPI void     WXFfmpegParamSetSubtitleFont(void * p, WXCTSTR sz, int FontSize, int FontColor);
WXFFMPEG_CAPI void     WXFfmpegParamSetSubtitleAlpha(void * p, int alpha);
WXFFMPEG_CAPI void     WXFfmpegParamSetSubtitlePostion(void * p, int postion);
WXFFMPEG_CAPI void     WXFfmpegParamSetSubtitleAlignment(void * p, int alignment);// 0 bottom, 1, center, 2 top
    
//视频转换参数

//设置转换时间，单位ms
WXFFMPEG_CAPI void     WXFfmpegParamSetConvertTime(void * p, int64_t ptsStart, int64_t ptsDuration);


//设置视频编码器
WXFFMPEG_CAPI void     WXFfmpegParamSetVideoCodecStr(void * p, WXCTSTR sz);

//设置底层H264编码器编码模式 0 Faset 1 Normal 2 Best
WXFFMPEG_CAPI void     WXFfmpegParamSetVideoMode(void * p, int mode);

//设置转换后的帧率
WXFFMPEG_CAPI void     WXFfmpegParamSetVideoFps(void * p, double fps);

//设置转换后的视频分辨率
WXFFMPEG_CAPI void     WXFfmpegParamSetVideoSize(void * p, int width, int height);

//设置转换后的显示比例
WXFFMPEG_CAPI void WXFfmpegParamSetVideoDar(void * p, int dar_width, int dar_height);


//设置视频码率
WXFFMPEG_CAPI void     WXFfmpegParamSetVideoBitrate(void * p, int bitrate);


//设置音频编码器
WXFFMPEG_CAPI void     WXFfmpegParamSetAudioCodecStr(void * p, WXCTSTR sz);

//设置音频码率
WXFFMPEG_CAPI void     WXFfmpegParamSetAudioBitrate(void * p, int bitrate);

//设置音频采样频率
WXFFMPEG_CAPI void     WXFfmpegParamSetAudioSampleRate(void * p, int sample_rate);

//设置音频声道数
WXFFMPEG_CAPI void     WXFfmpegParamSetAudioChannel(void * p, int channel);


//async = 1 ,底层另外开一个线程来执行，异步等待执行结果

// ptsStart 开始时间 ms
// ptsDuration 长度 ms ， 两者为0表示全部处理
// Fast， 1 表示不重新编码(更换格式之类，建议先录制ts、flv(H264+AAC)之类流式文件避免录制失败难以恢复，再转码为MP4), 0 表示重新编码
WXFFMPEG_CAPI int  WXFfmpegCutFile(void * p, WXCTSTR strInput, WXCTSTR strOutput, 
	int64_t ptsStart, int64_t ptsDuration, int Fast, int async);


WXFFMPEG_CAPI int      WXFfmpegConvertVideo(void * p, WXCTSTR strInput, WXCTSTR strOutput, int async);

WXFFMPEG_CAPI int      WXFfmpegConvertAudio(void * p, WXCTSTR strInput, WXCTSTR strOutput, int async);

//多个同样编码格式的文件文件按输入依次合并为一个文件，多次AddInput
WXFFMPEG_CAPI int      WXFfmpegMergerFile(void * p, WXCTSTR strOutput, WXCTSTR strTemp, int async);


//音频和视频文件合并为一个文件
WXFFMPEG_CAPI int      WXFfmpegMixerAV(void * p, WXCTSTR strVideo, WXCTSTR strAudio,  WXCTSTR strMixer, int async);

//替换视频文件中的音频数据
WXFFMPEG_CAPI int      WXFfmpegReplaceAudio(void * p, WXCTSTR strVideo, WXCTSTR strAudio, WXCTSTR strOutput, int copy, int async);

//视频文件截图
WXFFMPEG_CAPI int      WXFfmpegShotPicture(void * p, WXCTSTR strInput, int64_t ts, WXCTSTR strOutput);

//视频转换状态，异步操作时使用它来得到转换的结果
WXFFMPEG_CAPI int64_t  WXFfmpegGetCurrTime(void * p);//获取当前已处理的总时长，单位ms
WXFFMPEG_CAPI int64_t  WXFfmpegGetTotalTime(void * p);//获取需要处理的总时长，单位ms
WXFFMPEG_CAPI int      WXFfmpegGetState(void * p);//获取状态
WXFFMPEG_CAPI void     WXFfmpegInterrupt(void * p);//中断操作

// 视频播放器
//Windows 的 szType分为 LAV FFPLAY两种
//mac的 szType可以为NULL
WXFFMPEG_CAPI void*    WXFfplayCreate(WXCTSTR szType, WXCTSTR szInput, int speed, int64_t seek);//默认值1.0 0
WXFFMPEG_CAPI void     WXFfplayDestroy(void* p);
WXFFMPEG_CAPI void     WXFfplaySetView(void* p, HWND hwnd);
WXFFMPEG_CAPI void     WXFfplaySetVideoCB(void* p, WXFfmpegOnVideoData cb);
WXFFMPEG_CAPI double   WXFfplayGetSpeed(void* p);
WXFFMPEG_CAPI void     WXFfplaySetEventOwner(void* p, void *owner);
WXFFMPEG_CAPI void     WXFfplaySetEventCb(void* p, WXFfmpegOnEvent cb);
WXFFMPEG_CAPI void     WXFfplaySetEventID(void* p, WXCTSTR szID);
WXFFMPEG_CAPI WXCTSTR  WXFfplayGetEventID(void* p);
WXFFMPEG_CAPI int      WXFfplayStart(void* p);
WXFFMPEG_CAPI void     WXFfplayStop(void* p);
WXFFMPEG_CAPI void     WXFfplayPause(void* p);
WXFFMPEG_CAPI void     WXFfplayResume(void* p);
WXFFMPEG_CAPI void     WXFfplayRefresh(void* p);
WXFFMPEG_CAPI void     WXFfplayShotPicture(void* p, WXCTSTR szName, int quality);
WXFFMPEG_CAPI int64_t  WXFfplayGetCurrTime(void* p);
WXFFMPEG_CAPI int64_t  WXFfplayGetTotalTime(void* p);
WXFFMPEG_CAPI int	   WXFfplayGetVolume(void* p);
WXFFMPEG_CAPI int      WXFfplayGetState(void* p);
WXFFMPEG_CAPI void     WXFfplaySetReset(void* p);
WXFFMPEG_CAPI void     WXFfplaySetSubtitle(void* p, WXCTSTR sz);
WXFFMPEG_CAPI void     WXFfplaySetSubtitleFont(void * p, WXCTSTR szFontName, int FontSize, int FontColor);
WXFFMPEG_CAPI void     WXFfplaySetSubtitleAlpha(void * p, int alpha);
WXFFMPEG_CAPI void     WXFfplaySetSubtitlePostion(void * p, int postion) ;//0-270....
WXFFMPEG_CAPI void     WXFfplaySetSubtitleAlignment(void * p, int alignment);// 0 bottom, 1, center, 2 top
WXFFMPEG_CAPI void     WXFfplaySetVolume(void* p, int volume);
WXFFMPEG_CAPI void     WXFfplaySeek(void* p, int64_t pts);
WXFFMPEG_CAPI void     WXFfplaySpeed(void* p, int speed);
WXFFMPEG_CAPI void     WXFfplayCrop(void* p, int x, int y, int w, int h);
WXFFMPEG_CAPI void     WXFfplayVFlip(void* p, int b); //1为垂直翻转
WXFFMPEG_CAPI void     WXFfplayHFlip(void* p, int b); //1为水平翻转
WXFFMPEG_CAPI void     WXFfplayRotate(void* p, int rotate); //旋转角度 0-360,默认值0
WXFFMPEG_CAPI void     WXFfplayBrightness(void* p, int Brightness); //亮度 -100 100，默认值0
WXFFMPEG_CAPI void     WXFfplayContrast(void* p, int Contrast); //对比度 -100 100，默认值50
WXFFMPEG_CAPI void     WXFfplaySaturation(void* p, int Saturation); //饱和度 0-300，默认值100
WXFFMPEG_CAPI void     WXFfplayPictureQuality(void* p, int Brightness, int Contrast, int Saturation);

#ifdef __cplusplus
};
#endif

#endif /* _WX_MEDIA_API_H_ */
