/*
PC 录屏录音
*/

#ifndef __WX_CAPTURE_H_
#define __WX_CAPTURE_H_

#include <ScreenGrab.hpp>

#include "FLVMaker.h"

class ScreenCaptureAudio;
class ScreenCaptureVideo;

class  ScreenCapture {
public:

	WXScreenGrabParam m_param; //扩展参数
	ScreenCaptureAudio* m_pAudio = nullptr;//音频线程
	ScreenCaptureVideo* m_pVideo = nullptr;//视频线程
	std::atomic<bool>  m_bStart = false;//开始标记
	std::atomic<bool>  m_bPause = false;//暂停标记
	FlvMaker* m_pMuxer = nullptr;//混流器
	std::wstring m_strFileName; // 输出文件名

	BOOL m_bDXGI = FALSE;
public:
	ScreenCapture();
	virtual ~ScreenCapture();

public: //控制API
	bool    Create(WXScreenGrabParam* param, int& error); //启动,返回true才能Start
	void    Start();
	void    Stop();
	void    Pause();
	void    Resume();
};

#endif

