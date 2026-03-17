/*
视频数据采集类
有DXGI/GDI/Window/Virutal类型
*/
#include "WindowsDxgiCapture.h"
#include "WindowsGdiCapture.h"
#include "MixerCapture.h"

VideoSource* VideoSource::Create(const wchar_t* wszType) {
	if (wcsicmp(wszType, _T("GDI")) == 0) {
		WindowsGdiCapture* p = new WindowsGdiCapture;
		return (VideoSource*)p;
	}
	else if (wcsicmp(wszType, _T("DXGI")) == 0) {
		WindowsDxgiCapture* p = new WindowsDxgiCapture;
		return (VideoSource*)p;
	}
	else if (wcsicmp(wszType, _T("Mixer")) == 0) {
		MixerCapture* p = new MixerCapture;
		return (VideoSource*)p;
	}
	return nullptr;
}

void VideoSource::Destroy(VideoSource* ptr) {
	if (ptr) {
		if (wcsicmp(ptr->Type(), _T("GDI")) == 0) {
			WindowsGdiCapture* p = (WindowsGdiCapture*)ptr;
			delete p;
		}
		else if (wcsicmp(ptr->Type(), _T("DXGI")) == 0) {
			WindowsDxgiCapture* p = (WindowsDxgiCapture*)ptr;
			delete p;
		}
		else if (wcsicmp(ptr->Type(), _T("Mixer")) == 0) {
			MixerCapture* p = (MixerCapture*)ptr;
			delete p;
		}
	}
}