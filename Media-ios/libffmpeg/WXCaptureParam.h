/*
C 风格接口基本参数
*/
#ifndef _WXCAPTURE_PARAM_H_
#define _WXCAPTURE_PARAM_H_


#ifdef _WIN32
#include <Windows.h>
#define WXCHAR wchar_t
#else
#define WXCHAR char
#endif

#include <stdint.h>

#ifdef __cplusplus
extern "C"{
#endif

//错误码
#define WX_ERROR_SUCCESS                    0      //常规成功
#define WX_ERROR_ERROR                     (1 << 0)  //常规失败
#define WX_ERROR_OPEN_FILE                 (1 << 1)  //严重错误,创建文件失败

#define WX_WARNING_NO_VIDEO_DISPLAY_DATA   (1 << 2)  //没有抓屏数据
#define WX_WARNING_NO_VIDEO_CAREMA_DATA    (1 << 3)  //没有摄像头数据
#define WX_WARNING_NO_VIDEO_DATA           (1 << 4)  //没有录制视频数据
#define WX_WARNING_NO_IMAGE_WATERMARK_DATA (1 << 5)  //没有图像水印
#define WX_WARNING_NO_TEXT_WATERMARK_DATA  (1 << 6)  //没有文字水印

#define WX_WARNING_NO_SOUND_SYSTEM_DATA    (1 << 7) // 没有扬声器音频数据
#define WX_WARNING_NO_SOUND_MIC_DATA       (1 << 8) // 没有麦克风音频数据
#define WX_WARNING_NO_SOUND_DATA           (1 << 9) // 没有录制音频数据

#define WX_WARNING_VIDEO_RECT              (1 << 10)  //视频区域设置错误，抓取图像可能异常
#define WX_ERROR_VIDEO_NO_DEVICE           (1 << 11)  //指定设备名不存在
#define WX_ERROR_VIDEO_NO_PARAM            (1 << 12)  //设备不支持该格式参数 width/height/fps
#define WX_ERROR_VIDEO_DEVICE_OPEN         (1 << 13)  //视频设备打开失败,COM 操作失败

#define WX_ERROR_SOUND_SYSTEM_OPEN         (1 << 20) // WASAPI 播放设备录制打开失败
#define WX_ERROR_SOUND_NO_SYSTEM_DEVICE    (1 << 21) // 没有WASAPI 播放设备
#define WX_ERROR_SOUND_MIC_OPEN            (1 << 22) // WASAPI麦克风录制打开失败
#define WX_ERROR_SOUND_NO_MIC_DEVICE       (1 << 23) // 扬没有WASAPI 麦克风

typedef void(*VideoCallBack)(uint8_t* buf, int width, int height);//视频数据回调


//cbID
#define WX_EVENT_ID_CLOSE_FILE   0   //录制结束，后面参数是生成文件名
#define WX_EVENT_ID_SCRRENSHOT1  1  //截图1成功，后面参数是生成文件名
#define WX_EVENT_ID_SCRRENSHOT2  2  //截图1成功，后面参数是生成文件名

typedef void(*wxCallBack)(void* cbSink, UINT cbID, void* cbData);

//录屏模式
typedef enum _WXCaptureMode{
	MODE_FAST = 0,   //快捷模式, 不考虑固定时间戳，视频按照到达时间戳来编码  
	MODE_NORMAL = 1, //均衡模式
	MODE_BEST = 2,   //质量最好，插帧丢帧使生成文件的fps逼近设置值
}WXCaptureMode;

//音频设备参数
typedef struct _AudioDeviceParam {
	int has_audio;// = true;  //Version 1.0.0.5
	WXCHAR m_systemName[MAX_PATH];  //扬声器的名字，从SoundInfo 获得
	WXCHAR m_micName[MAX_PATH];   //麦克风的名字
	WXCHAR codec[16];//"aac" "mp3" 等编码格式
	//MIC 设置

	int bAGC;//设置声音增强
	int bNS;//设置降噪
}AudioDeviceParam;

//视频设备参数
typedef struct _VideoDeviceParam {
	int     m_bUse;// = 0;
	int     m_iFps;// = 25;
	int     m_bUseHW;//H264 硬编码，包括 QSV  NVENC  VideoToolBox
	WXCHAR   m_wszCodec[16];// "h264" "mpeg4" 等编码格式
	int     m_iBitrate;// = 1000*1000;//默认视频码率
	HWND  m_hwndPreview;// = nullptr; 预览窗口
	VideoCallBack onVideoData;// = nullptr  数据回调

	// 显示器使用枚举到的名字， 
	// 摄像头使用枚举到的GUID值，多个相同的摄像头名字可能一样，但是GUID值不一样 
	// Airplay等数据源 当成一种特殊的摄像头，名字使用 "WX_AIRPLAY"  "AIRPLAY"
	WXCHAR m_wszDevName[MAX_PATH];
	
	//摄像头参数
	int   m_bCamera;// = 0;  是否摄像头
	int   m_iCameraWidth;// = 640;
	int   m_iCameraHeight;// = 480;

	//桌面采集参数
	int   m_bDXGI;// Win10 DXGI桌面采集，目前只支持主屏幕， 鼠标绘制和区域采集的具体功能还没加上去
	int   m_bRect;// = 0,使用区域截图
	RECT  m_rcScreen;// = {0,0,100,100};
	int   m_bFollowMouse;// = 0 使用鼠标位置来截图，以鼠标为中心, 需要先设置区域的RECT值
	int   m_bCaptureBlt;//=0 某个API参数 CAPTUREBLT =0
	int   m_bForceHDC;// GDI采集 有些机器需要每次采集桌面都CreateDC

	int   m_iForceFps;//通过插值使得生成文件的帧率逼近预设值  //add by Tam 2018.05.26
	int   m_iAntiAliasing;//抗锯齿处理， H264 将使用 YUV444 编码
}VideoDeviceParam;

//文字水印参数
typedef struct _TextWaterMarkParam {
	int      m_bUsed;// = false;
	int      m_iPosX;// = 0; //文字水印位置X
	int      m_iPosY;// = 0; //文字水印位置Y
	int      m_iDelay;//== 0 延时使用水印,单位为秒

	WXCHAR  m_wszText[MAX_PATH];// = _T("录屏王测试工程"); //文字
	uint32_t m_iColor;// = RGB(255, 0, 0); //文字颜色 RGB(r,g,b)
	uint32_t m_BkColor;// = RGB(255, 0, 0); //背景颜色 RGB(r,g,b)// Add by Tam, 2018-04-02

	WXCHAR  m_wszFontName[MAX_PATH];// = _T("宋体"); //或者指定名字和大小
	int      m_iFontSize;// = 100;//字体大小

	HFONT   m_hfont;// = NULL, 

}TextWaterMarkParam;

//图像水印参数
typedef struct _ImageWaterMarkParam {
	int     m_bUsed;// = false; //是否使用图像水印
	int     m_iPosX;// = 200; //指定图像位置 x
	int     m_iPosY;// = 200;  //指定图像位置 y
	int     m_iDelay;//== 0 延时使用水印,单位为秒

	//int     m_iAlpha;// = 80;//透明度,0为不透明，100 就是透明
	float     m_fAlpha;//0.0-1.0越小越透明

	WXCHAR m_wszFileName[MAX_PATH];//水印图像文件名
}ImageWaterMarkParam;

//鼠标事件参数
typedef struct _MouseParam {
	int m_iUsed;//为0 时 不录制鼠标
	//鼠标热点
	int m_bMouseHotdot;// = false;
	int m_iHotdotRadius;// = 10;
	uint32_t m_colorMouse;// = RGB(255, 255, 0);
	float     m_fAlphaHotdot;//0.0-1.0越小越透明

	//鼠标点击动画
	int m_bMouseAnimation;// = false;
	int m_iAnimationRadius;// = 20;
	uint32_t m_colorLeft;// = RGB(255, 0, 0);
	uint32_t m_colorRight;// = RGB(0, 0, 255);

	float     m_fAlphaAnimation;//0.0-1.0越小越透明

	//float     m_fAlpha;//0.0-1.0越小越透明
}MouseParam;

typedef struct _LogParam{
	bool m_bUseLog;
	WXCHAR m_logFileName[MAX_PATH];
}LogParam;

typedef struct WXCaptureParam {
	WXCHAR m_wszFileName[MAX_PATH];

	WXCaptureMode m_mode;//录制模式
	AudioDeviceParam m_audio;
	VideoDeviceParam m_video;
	TextWaterMarkParam m_text;
	ImageWaterMarkParam m_image;

	MouseParam m_mouse;

	void *m_sink;//回调对象，可能是C++对象
	wxCallBack m_cb;//结束回调，函数
}WXCaptureParam;

//Dsound 设备名字
typedef struct _DSoundDeviceInfo {
	char szDesc[MAX_PATH];
	char szDrvName[MAX_PATH];
	GUID guid;
	int  bIsGuidNull;
}DSoundDeviceInfo;

//wassapi 设备
typedef struct _SoundDeviceInfo {
	WXCHAR m_strName[MAX_PATH];//设备名字
	WXCHAR m_strGUID[MAX_PATH];//端口名字
	int  isDefalut;//默认设备
	int  currLevel;//当前音量值
}SoundDeviceInfo;

//显示器设备属性
typedef struct _MonitorInfo {
	WXCHAR wszName[MAX_PATH];//名字
	int isPrimary; //主屏幕
	int left;
	int top;
	int width;
	int height;
}MonitorInfo;

typedef struct _CameraDataFormat {
	int width;
	int height;
	int fps;
	int mt;//采集格式
	int index;//pmt
	int64_t AvgTimePerFrame;
}CameraDataFormat;

typedef struct _CameraInfo {
	WXCHAR m_strName[MAX_PATH];//设备名字
	WXCHAR m_strGUID[MAX_PATH];//端口名字
	int size_fmt;
	CameraDataFormat m_arrFmt[MAX_PATH];//视频格式
}CameraInfo;

#ifdef  __cplusplus
};
#endif

#endif
