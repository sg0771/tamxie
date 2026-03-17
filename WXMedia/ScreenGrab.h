/*
ScreenGrab.h
*/
#ifndef __WX_MEDIA_H_
#define __WX_MEDIA_H_

#include <stdint.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

#include  <stdint.h>
#include  <stdlib.h>
#include  <setjmp.h>  
#include  <ctype.h>
#include  <string.h>
#include  <math.h>
#include  <errno.h>
#include  <limits.h>
#include  <time.h>
#include  <stdio.h>
#include  <errno.h>
#include  <wchar.h>
#include  <stdint.h>
#include  <string.h>
#include  <stdarg.h> 
#include  <stdlib.h>
#include  <stdio.h>
#include  <string.h>
#include  <time.h>
#include  <sys/stat.h>
#include  <assert.h>
#include <Windows.h>
#include <tchar.h>
#include <malloc.h>

#include <psapi.h>
#include <DbgHelp.h>  //Dump

#include  <fstream>
#include  <queue>
#include  <mutex>
#include  <string>
#include  <thread>
#include  <atomic>
#include  <map>
#include  <vector>

#ifdef _WXCTP_EXPORT
#define DLL_API  //__declspec(dllexport)
#else
#define DLL_API  //__declspec(dllimport)
#endif

#define  SAFE_RELEASE_DC(hwnd,hdc)  if(hdc){::ReleaseDC(hwnd,hdc);hdc=nullptr;}
#define  SAFE_DELETE_DC(hdc)  if(hdc){::DeleteDC(hdc);hdc=nullptr;}
#define  SAFE_DELETE_OBJECT(obj)  if(obj){::DeleteObject(obj);obj=nullptr;}
#define  SAFE_RELEASE(p)  if(p){(p)->Release();p=nullptr;}

#define SCREENGRAB_CAPI EXTERN_C DLL_API

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)  if(p){delete p;p=nullptr;}
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(P)  {if (P) {delete[] P; P = NULL;}}
#endif

#ifndef MAX
#define MAX(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a,b)            (((a) < (b)) ? (a) : (b))
#endif

//错误码
#define _WX_ERROR_SUCCESS                    0      //常规成功
#define _WX_ERROR_ERROR                     (1 << 0)  //常规失败
#define _WX_ERROR_OPEN_FILE                 (1 << 1)  //严重错误,创建文件失败

#define _WX_WARNING_NO_VIDEO_DISPLAY_DATA   (1 << 2)  //没有抓屏数据
#define _WX_WARNING_NO_VIDEO_CAREMA_DATA    (1 << 3)  //没有摄像头数据
#define _WX_WARNING_NO_VIDEO_DATA           (1 << 4)  //没有录制视频数据
#define _WX_WARNING_NO_IMAGE_WATERMARK_DATA (1 << 5)  //没有图像水印
#define _WX_WARNING_NO_TEXT_WATERMARK_DATA  (1 << 6)  //没有文字水印

#define _WX_WARNING_NO_SOUND_SYSTEM_DATA    (1 << 7) // 没有扬声器音频数据
#define _WX_WARNING_NO_SOUND_MIC_DATA       (1 << 8) // 没有麦克风音频数据
#define _WX_WARNING_NO_SOUND_DATA           (1 << 9) // 没有录制音频数据

#define _WX_WARNING_VIDEO_RECT              (1 << 10)  //视频区域设置错误，抓取图像可能异常
#define _WX_ERROR_VIDEO_NO_DEVICE           (1 << 11)  //指定设备名不存在
#define _WX_ERROR_VIDEO_NO_PARAM            (1 << 12)  //设备不支持该格式参数 width/height/fps
#define _WX_ERROR_VIDEO_DEVICE_OPEN         (1 << 13)  //视频设备打开失败,COM 操作失败

#define _WX_ERROR_SOUND_SYSTEM_OPEN         (1 << 20) // WASAPI 播放设备录制打开失败
#define _WX_ERROR_SOUND_NO_SYSTEM_DEVICE    (1 << 21) // 没有WASAPI 播放设备
#define _WX_ERROR_SOUND_MIC_OPEN            (1 << 22) // WASAPI麦克风录制打开失败
#define _WX_ERROR_SOUND_NO_MIC_DEVICE       (1 << 23) // 扬没有WASAPI 麦克风

//2019-01-14
//扬声器事件
#define _WX_EVENT_WASAPI_SYSTEM_INIT_ERROR   20  //WASAPI设备初始化失败,可能需要更新驱动
#define _WX_EVENT_WASAPI_SYSTEM_INIT_ERROR_FOR_EXCLUSIVE   21  //WASAPI设备因为外部应用独占而初始化失败
#define _WX_EVENT_WASAPI_SYSTEM_STOP_EXCLUSIVE    22  //WASAPI设备被外部应用独占而停止录制
#define _WX_EVENT_WASAPI_SYSTEM_STOP_MOVED    23  //WASAPI设备被拔除而停止录制

//MIC事件
#define _WX_EVENT_WASAPI_MIC_INIT_ERROR   30  //WASAPI设备初始化失败,可能需要更新驱动
#define _WX_EVENT_WASAPI_MIC_INIT_ERROR_FOR_EXCLUSIVE   31  //WASAPI设备因为外部应用独占而初始化失败
#define _WX_EVENT_WASAPI_MIC_STOP_EXCLUSIVE    32  //WASAPI设备被外部应用独占而停止录制
#define _WX_EVENT_WASAPI_MIC_STOP_MOVED    33  //WASAPI设备被拔除而停止录制


SCREENGRAB_CAPI const wchar_t*      WXTcpGetPath(); //获取当前程序所在的目录

SCREENGRAB_CAPI void         WXTcpUtilsInit(const wchar_t* logfile);
SCREENGRAB_CAPI int          WXTcpGetSystemVersion();

SCREENGRAB_CAPI int          WXTcpSupportDXGI();//设备是否支持 DXGI采集1 为可以，0 不可以
SCREENGRAB_CAPI void         WXTcpSetLogFile(const wchar_t* wszFileName); //设置日志文件
SCREENGRAB_CAPI void		   WXTcpLog(const wchar_t *format, ...);//用于某些UNICODE字符的日志
SCREENGRAB_CAPI void         WXTcpSleepMs(int ms);

//显示器设备属性
typedef struct _PlayInfo {
	wchar_t wszName[MAX_PATH];//名字
	int isPrimary; //主屏幕
	int left;
	int top;
	int width;
	int height;
}PlayInfo;

SCREENGRAB_CAPI void WXTcpScreenInit();   //初始化显示器管理
SCREENGRAB_CAPI void WXTcpScreenDeinit(); //
SCREENGRAB_CAPI int  WXTcpScreenGetCount(); //返回显示器个数
SCREENGRAB_CAPI PlayInfo* WXTcpScreenGetInfo(int index); //返回对应显示器属性
SCREENGRAB_CAPI PlayInfo* WXTcpScreenGetInfoByName(const wchar_t* wszDevice);//获取指定名字显示器属性
SCREENGRAB_CAPI PlayInfo* WXTcpScreenGetDefaultInfo();//获取默认显示器属性

SCREENGRAB_CAPI int WXTcpScreenGetRotate(const wchar_t* wszDevice); //返回对应显示器旋转角度0 90 180 270

//wassapi 设备
typedef struct _AudioDevInfo {
	wchar_t m_strName[MAX_PATH];//设备名字
	wchar_t m_strGuid[MAX_PATH];//端口名字
	int  isDefalut;//默认设备
	int  isDefalutComm;//默认通信设备
}AudioDevInfo;

//WASAPI 设备属性
SCREENGRAB_CAPI void WXTcpWasapiInit();
SCREENGRAB_CAPI void WXTcpWasapiDeinit();
SCREENGRAB_CAPI int  WXTcpWasapiGetRenderCount();
SCREENGRAB_CAPI AudioDevInfo* WXTcpWasapiGetRenderInfo(int index);
SCREENGRAB_CAPI int WXTcpWasapiGetCaptureCount();
SCREENGRAB_CAPI AudioDevInfo* WXTcpWasapiGetCaptureInfo(int index);

//bSystem = 1 为扬声器
//bSystem = 0 为麦克风
//获取设备音量
SCREENGRAB_CAPI  void  AudioDeviceResetDefault();//重新遍历设备
SCREENGRAB_CAPI  const wchar_t*  WXTcpWasapiGetDefaultGuid(int bSystem);
SCREENGRAB_CAPI  const wchar_t*  WXTcpWasapiGetDefaultName(int bSystem);
SCREENGRAB_CAPI  const wchar_t*  WXTcpWasapiGetDefaultCommGuid(int bSystem);
SCREENGRAB_CAPI  const wchar_t*  WXTcpWasapiGetDefaultCommName(int bSystem);


typedef struct WXScreenGrabParam {
	//网络
	int m_nPort; //0 不使用网络
	wchar_t m_wszName[MAX_PATH]; //网络IP或者文件名

	//音频
	wchar_t m_systemName[MAX_PATH];  //扬声器的名字，从SoundInfo 获得
	wchar_t m_micName[MAX_PATH];   //麦克风的名字
	int nAudioSampleRate;//采样频率，默认48000
	int nAudioChannel;//声道，默认为2
	int nAudioBitarte;//音频码率

	//视频
	wchar_t   m_wszDevName[MAX_PATH];
	int      m_iForceHeight;
	int      m_iFps;
	int      m_iBitrate;
}WXScreenGrabParam;

SCREENGRAB_CAPI void WXScreenGrabInit(const wchar_t* wsz);//初始化，路径应该为全局路径
SCREENGRAB_CAPI void  WXScreenGrabDefaultConfig(WXScreenGrabParam* param);
SCREENGRAB_CAPI void* WXScreenGrabStart(WXScreenGrabParam* param, int *error);//使用新的录制结构体的录制参数
SCREENGRAB_CAPI void  WXScreenGrabStop(void* ptr);
SCREENGRAB_CAPI void  WXScreenGrabPause(void* ptr);
SCREENGRAB_CAPI void  WXScreenGrabResume(void* ptr);

/*
基于libevent2 的TCP服务端+接收端
要先启动服务端，接收端才能连接发数据
*/
typedef void(*OnRecvData)(void *ctx, const void *data, int len);
SCREENGRAB_CAPI void* WXTcpServerStart(int nPort, void *sink, OnRecvData cb);
SCREENGRAB_CAPI void  WXTcpServerStop(void* server);

SCREENGRAB_CAPI void *WXTcpClientConnect(const wchar_t *wszIP, int nPort);
SCREENGRAB_CAPI void WXTcpClientSendData(void* client, const void *data, int data_size);
SCREENGRAB_CAPI void WXTcpClientDisconnect(void* client);

#endif