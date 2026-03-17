//
//  WXFFmpegConvertAPI.h
//

#ifndef _WX_FFMPEG_CONVERT_API_H_
#define _WX_FFMPEG_CONVERT_API_H_

#include <stdint.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>


#ifdef _WIN32

#include <Windows.h>
#include <tchar.h>
#pragma warning(disable : 4068)

#define WXCTSTR const wchar_t*
#define WXTSTR  wchar_t*

#else //__APPLE__
#define MAX_PATH  1024
#define HWND void*
#define WXCTSTR const char*
#define WXTSTR  char*
#ifdef __cplusplus
#define EXTERN_C extern "C"
#else
#define EXTERN_C
#endif
#endif

#include "LibDelogoAPI.h"

#ifdef __cplusplus
extern "C" {
#endif

//视频操作回调函数
typedef void(*WXFfmpegOnEvent)(void *ctx, WXCTSTR szID, uint32_t iEvent, WXCTSTR szMsg);

//转换时解码数据回调
typedef void(*onFfmpegVideoData)(void *ctx, int width, int height, uint8_t** pData, int* linesize, int64_t pts);//视频编码过程中的视频数据回调

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

//Log 等级
#define  WX_LOG_FATAL     0
#define  WX_LOG_ERROR     1
#define  WX_LOG_WARNING   2
#define  WX_LOG_INFO      3
#define  WX_LOG_VERBOSE   4

//播放速度
#define AV_SPEED_SLOW 50  //0.5 speed
#define	AV_SPEED_NORMAL  100  //1.0 speed
#define	AV_SPEED_SPEED  150  //1.5 speed
#define	AV_SPEED_FAST  200 //2.0 speed
	
//播放器状态
#define FFPLAY_STATE_UNAVAILABLE 0 //不可用， 刚创建的对象或者已经销毁的对象
#define FFPLAY_STATE_WAITING     1 //已经创建或者进行了Stop操作，等待Start或者Dewszoy
#define FFPLAY_STATE_PLAYING     2 //正在播放状态
#define FFPLAY_STATE_PAUSE       3// 已经开始播放，但是处于暂停状态
#define FFPLAY_STATE_PLAYING_END 4 //数据播放完毕


#define SOUND_TYPE_DEFAULT 0  //默认格式
#define SOUND_TYPE_PCM16   1  //PCM16 格式
#define SOUND_TYPE_FLOAT32 2  //FLOAT格式

WXDELOGO_CAPI WXTSTR   WXStrcpy(WXTSTR wsz1, WXCTSTR wsz2);//字符串拷贝
WXDELOGO_CAPI int      WXStrcmp(WXCTSTR wsz1, WXCTSTR wsz2);//字符串比较
WXDELOGO_CAPI int      WXStrlen(WXCTSTR wsz); //字符串长度
WXDELOGO_CAPI WXCTSTR  WXStrdup(WXCTSTR wsz); //字符串拷贝构造，需要用free来销毁内存

WXDELOGO_CAPI void     WXFfmpegInit(void);//ffmpeg 初始化
WXDELOGO_CAPI void     WXFfmpegDeinit(void);//ffmpeg 退出
WXDELOGO_CAPI WXCTSTR  WXFfmpegGetError(int code);//获取错误代码
WXDELOGO_CAPI void     WXFfmpegSetLogFile(WXCTSTR szFileName);//设置Log文件名

//  视频转换 API
WXDELOGO_CAPI void*    WXFfmpegParamCreate(void);
WXDELOGO_CAPI void     WXFfmpegParamDestroy(void * p);
WXDELOGO_CAPI void     WXFfmpegParamSetEventOwner(void * p, void *ownerEvent);
WXDELOGO_CAPI void     WXFfmpegParamSetEventCb(void * p, WXFfmpegOnEvent cbEvent);
WXDELOGO_CAPI void     WXFfmpegParamSetEventID(void * p, WXCTSTR szEvent);

// 水印图片设置
// 不可以重复Add相同的文件名
//最后的4个参数是基于左上角的坐标系
WXDELOGO_CAPI void     WXFfmpegParamAddWMImage(void * p, WXCTSTR szImage, int x, int y, int w, int h);

//设置旋转角度
WXDELOGO_CAPI void     WXFfmpegParamSetRotate(void * p, int rotate);

//设置垂直旋转
WXDELOGO_CAPI void     WXFfmpegParamSetVFlip(void * p, int b);

//设置水平旋转
WXDELOGO_CAPI void     WXFfmpegParamSetHFlip(void * p, int b);

//设置裁剪区域
WXDELOGO_CAPI void     WXFfmpegParamSetCrop(void * p, int x, int y, int w, int h);

//输入参数50-200, 文件转码后的播放速度 0.5-2.0
WXDELOGO_CAPI void     WXFfmpegParamSetSpeed(void * p, int speed); 

//亮度(-100,100) 默认0,对比度 (-100,100)  默认 50,饱和度(0,300) 默认100
WXDELOGO_CAPI void     WXFfmpegParamSetPictureQuality(void * p, int brightness, int contrast, int saturation);

//音量 0-1000  默认256
WXDELOGO_CAPI void     WXFfmpegParamSetVolume(void * p, int volume);

//用于依次输入合并文件
WXDELOGO_CAPI void     WXFfmpegParamAddInput(void * p, WXCTSTR szInput);

//字幕设置
WXDELOGO_CAPI void     WXFfmpegParamSetSubtitle(void * p, WXCTSTR sz);
WXDELOGO_CAPI void     WXFfmpegParamSetSubtitleFont(void * p, WXCTSTR sz, int FontSize, int FontColor);
WXDELOGO_CAPI void     WXFfmpegParamSetSubtitleAlpha(void * p, int alpha);
WXDELOGO_CAPI void     WXFfmpegParamSetSubtitlePostion(void * p, int postion);
WXDELOGO_CAPI void     WXFfmpegParamSetSubtitleAlignment(void * p, int alignment);// 0 bottom, 1, center, 2 top
    
//视频转换参数

//设置转换时间，单位ms
WXDELOGO_CAPI void     WXFfmpegParamSetConvertTime(void * p, int64_t ptsStart, int64_t ptsDuration);


//设置转换过程中编码前的回调函数
WXDELOGO_CAPI void     WXFfmpegParamSetVideoCB(void * p, onFfmpegVideoData cb);

//设置转换后的视频格式 yuv420p 。。
WXDELOGO_CAPI void     WXFfmpegParamSetVideoFmtStr(void * p, WXCTSTR sz);

//设置视频编码器
WXDELOGO_CAPI void     WXFfmpegParamSetVideoCodecStr(void * p, WXCTSTR sz);

//设置底层H264编码器编码模式 0 Faset 1 Normal 2 Best
WXDELOGO_CAPI void     WXFfmpegParamSetVideoMode(void * p, int mode);

//设置转换后的帧率
WXDELOGO_CAPI void     WXFfmpegParamSetVideoFps(void * p, double fps);

//设置转换后的视频分辨率
WXDELOGO_CAPI void     WXFfmpegParamSetVideoSize(void * p, int width, int height);


//设置转换后的显示比例
WXDELOGO_CAPI void WXFfmpegParamSetVideoDar(void * p, int dar_width, int dar_height);


//设置视频码率
WXDELOGO_CAPI void     WXFfmpegParamSetVideoBitrate(void * p, int bitrate);


//设置音频编码器
WXDELOGO_CAPI void     WXFfmpegParamSetAudioCodecStr(void * p, WXCTSTR sz);

//设置音频码率
WXDELOGO_CAPI void     WXFfmpegParamSetAudioBitrate(void * p, int bitrate);

//设置音频采样频率
WXDELOGO_CAPI void     WXFfmpegParamSetAudioSampleRate(void * p, int sample_rate);

//设置音频声道数
WXDELOGO_CAPI void     WXFfmpegParamSetAudioChannel(void * p, int channel);


//async = 1 ,底层另外开一个线程来执行，异步等待执行结果
// ptsStart 开始时间 ms
// ptsDuration 长度 ms ， 两者为0表示全部处理
// Fast， 1 表示不重新编码(更换格式之类，建议先录制ts、flv(H264+AAC)之类流式文件避免录制失败难以恢复，再转码为MP4), 0 表示重新编码
WXDELOGO_CAPI int  WXFfmpegCutFile(void * p, WXCTSTR wszInput, WXCTSTR wszOutput, 
	int64_t ptsStart, int64_t ptsDuration, int Fast, int async);


WXDELOGO_CAPI int      WXFfmpegConvertVideo(void * p, WXCTSTR wszInput, WXCTSTR wszOutput, int async);



WXDELOGO_CAPI int      WXFfmpegConvertAudio(void * p, WXCTSTR wszInput, WXCTSTR wszOutput, int async);

//多个同样编码格式的文件文件按输入依次合并为一个文件，多次AddInput
WXDELOGO_CAPI int      WXFfmpegMergerFile(void * p, WXCTSTR wszOutput, WXCTSTR wszTemp, int fast, int async);


//音频和视频文件合并为一个文件
WXDELOGO_CAPI int      WXFfmpegMixerAV(void * p, WXCTSTR wszVideo, WXCTSTR wszAudio,  WXCTSTR wszMixer, int async);

//替换视频文件中的音频数据
WXDELOGO_CAPI int      WXFfmpegReplaceAudio(void * p, WXCTSTR wszVideo, WXCTSTR wszAudio, WXCTSTR wszOutput, int copy, int async);

//视频文件截图
WXDELOGO_CAPI int      WXFfmpegShotPicture(void * p, WXCTSTR wszInput, int64_t ts, WXCTSTR wszOutput);

//视频转换状态，异步操作时使用它来得到转换的结果
WXDELOGO_CAPI int64_t  WXFfmpegGetCurrTime(void * p);//获取当前已处理的总时长，单位ms
WXDELOGO_CAPI int64_t  WXFfmpegGetTotalTime(void * p);//获取需要处理的总时长，单位ms
WXDELOGO_CAPI int      WXFfmpegGetState(void * p);//获取状态
WXDELOGO_CAPI void     WXFfmpegInterrupt(void * p);//中断操作

//获取多媒体文件信息
WXDELOGO_CAPI void*    WXMediaInfoCreate(WXCTSTR szFileName, int *error);
WXDELOGO_CAPI void     WXMediaInfoDestroy(void *p);

//WXDELOGO_CAPI int      WXMediaInfoGetPicture(void *p, WXCTSTR szFileName);
WXDELOGO_CAPI int      WXMediaInfoGetAudioChannelNumber(void *p);//Type=0
WXDELOGO_CAPI int      WXMediaInfoGetVideoChannelNumber(void *p);//Type=1
WXDELOGO_CAPI int      WXMediaInfoGetAttachChannelNumber(void *p);//Type=2 //add

WXDELOGO_CAPI int64_t  WXMediaInfoGetFileSize(void *p);
WXDELOGO_CAPI int64_t  WXMediaInfoGetFileDuration(void *p);
WXDELOGO_CAPI WXCTSTR  WXMediaInfoGetFormat(void *p);
WXDELOGO_CAPI int      WXMediaInfoGetChannelNumber(void *p);
WXDELOGO_CAPI int64_t  WXMediaInfoGetVideoBitrate(void *p);
WXDELOGO_CAPI int      WXMediaInfoGetVideoOrientation(void *p);


//获取视频分辨率，相当于PAR
WXDELOGO_CAPI int      WXMediaInfoGetVideoWidth(void *p);
WXDELOGO_CAPI int      WXMediaInfoGetVideoHeight(void *p);

//获取视频显示比例 SAR
WXDELOGO_CAPI int      WXMediaInfoGetVideoSarWidth(void *p);
WXDELOGO_CAPI int      WXMediaInfoGetVideoSarHeight(void *p);

//获取视频显示比例 DAR
WXDELOGO_CAPI int      WXMediaInfoGetVideoDisplayRatioWidth(void *p);
WXDELOGO_CAPI int      WXMediaInfoGetVideoDisplayRatioHeight(void *p);

//获得 Channle 的 类型
WXDELOGO_CAPI int      WXMediaInfoChannelGetType(void *p, int index);//0 Audio 1 Video 2 Attach //change
WXDELOGO_CAPI WXCTSTR  WXMediaInfoChannelCodec(void *p, int index);
WXDELOGO_CAPI int      WXMediaInfoChannelAudioBitrate(void *p, int index);
WXDELOGO_CAPI int      WXMediaInfoChannelAudioSampleRate(void *p, int index);
WXDELOGO_CAPI int      WXMediaInfoChannelAudioChanels(void *p, int index);

#ifdef __cplusplus
};
#endif

#endif /* _WX_FFMPEG_CONVERT_API_H_ */
