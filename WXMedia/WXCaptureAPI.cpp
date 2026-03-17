#include "ScreenGrab.h"
#include "ScreenCapture.h"

//设置默认录制参数
SCREENGRAB_CAPI  void WXScreenGrabDefaultConfig(WXScreenGrabParam* param) {
	if (param) {
		param->m_nPort = 0;
		memset(param->m_wszName, 0, MAX_PATH * sizeof(wchar_t));
		memset(param->m_systemName, 0, MAX_PATH * sizeof(wchar_t));
		wcscpy(param->m_systemName, _T("nullptr"));	//不启用

		memset(param->m_micName, 0, MAX_PATH * sizeof(wchar_t));
		wcscpy(param->m_micName, _T("nullptr"));//不启用

		param->nAudioBitarte = 128000;
		param->nAudioChannel = 2;
		param->nAudioSampleRate = 48000;

		memset(param->m_wszDevName, 0, MAX_PATH * sizeof(wchar_t));
		param->m_iFps = 25;
		param->m_iForceHeight = 0;
		param->m_iBitrate = 0;
	}
}

SCREENGRAB_CAPI void* WXScreenGrabStart(WXScreenGrabParam* param, int* error) {
	*error = 0;
	ScreenCapture* new_capture = new ScreenCapture;
	bool bOpen = new_capture->Create(param, *error);//先Create 试一下参数是否正常
	if (!bOpen) {
		SAFE_DELETE(new_capture);
		return nullptr;
	}
	new_capture->Start();//音视频通道同时Start
	return (void*)new_capture;
}

//关闭录屏录音
SCREENGRAB_CAPI void  WXScreenGrabStop(void* ptr) {
	if (ptr) {
		ScreenCapture* cap = (ScreenCapture*)ptr;
		cap->Stop();
	}
}

//暂停录屏录音
SCREENGRAB_CAPI void  WXScreenGrabPause(void* ptr) {
	if (ptr) {
		ScreenCapture* cap = (ScreenCapture*)ptr;
		cap->Pause();
	}
}

//恢复录屏录音
SCREENGRAB_CAPI void  WXScreenGrabResume(void* ptr) {
	if (ptr) {
		ScreenCapture* cap = (ScreenCapture*)ptr;
		cap->Resume();
	}
}
