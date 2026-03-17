/*
Media Player
*/
#include "IWXVideo.h"
#include "FfmpegIncludes.h"

WXFFMPEG_CAPI void*    WXFfplayCreate(WXCTSTR wszType, WXCTSTR wszInput, int speed, int64_t seek) {
	IWXPlay *play = nullptr;
//#ifdef _WIN32
//	if (WXStrcmp(wszType, _T("LAV")) == 0)
//		play = IWXPlay_Create_LAV();
//	else
//#endif
		play = IWXPlay_Create_FFPLAY();

	play->SetFileName(wszInput);
	play->SetInitSpeed(speed);
	play->SetStartTime(seek);
	int bRet = play->OpenFile();
	if (!bRet) {
		WXFfplayDestroy(play);
		return NULL;
	}
	return (void*)play;
}


WXFFMPEG_CAPI double WXFfplayGetSpeed(void* p) {
	IWXPlay *play = (IWXPlay *)p;
	return play ? play->GetSpeed() : 0;
}


WXFFMPEG_CAPI void WXFfplayDestroy(void* ptr) {
	IWXPlay *play = (IWXPlay *)ptr;
	if (play) {
//#ifdef _WIN32
//		if (WXStrcmp(play->GetType(), _T("LAV")) == 0) {
//			IWXPlay_Destroy_LAV(play);
//			play = nullptr;
//		}	else 
//#endif
		{
			IWXPlay_Destroy_FFPLAY(play);
			play = nullptr;
		}		
	}
}

WXFFMPEG_CAPI void     WXFfplaySetView(void* p, HWND hwnd) {
	IWXPlay *play = (IWXPlay *)p;
	if (play)play->SetView(hwnd);
}

WXFFMPEG_CAPI void     WXFfplaySetVideoCB(void* p, WXFfmpegOnVideoData cb) {
	IWXPlay *play = (IWXPlay *)p;
	if (play) play->SetVideoCB(cb);
}

WXFFMPEG_CAPI void     WXFfplaySetVolume(void* p, int volume) {
	IWXPlay *play = (IWXPlay *)p;
	if (play)play->SetVolume(volume);
}

WXFFMPEG_CAPI void     WXFfplaySetEventOwner(void* p, void *owner) {
	IWXPlay *play = (IWXPlay *)p;
	if (play)play->SetEventOwner(owner);
}

WXFFMPEG_CAPI void     WXFfplaySetEventCb(void* p, WXFfmpegOnEvent cb) {
	IWXPlay *play = (IWXPlay *)p;
	if (play)play->SetEventCB(cb);
}


WXFFMPEG_CAPI void     WXFfplaySetEventID(void* p, WXCTSTR szID) {
	IWXPlay *play = (IWXPlay *)p;
	if (play)return play->SetEventID(szID);
}

WXFFMPEG_CAPI WXCTSTR  WXFfplayGetEventID(void* p) {
	IWXPlay *play = (IWXPlay *)p;
	if (play)return play->GetEventID();
	return NULL;
}

WXFFMPEG_CAPI int  WXFfplayStart(void* p) {
	IWXPlay *play = (IWXPlay *)p;
	if (play)return play->Start();
	return 0;
}

WXFFMPEG_CAPI void     WXFfplayStop(void* p) {
	IWXPlay *play = (IWXPlay *)p;
	if (play) play->Stop();
}


WXFFMPEG_CAPI void  WXFfplayShotPicture(void* p, WXCTSTR  wszName, int quality) {
	IWXPlay *play = (IWXPlay *)p;
	if (play) {
		play->ShotPicture(wszName, quality);
	}
}

WXFFMPEG_CAPI void     WXFfplayPause(void* p) {
	IWXPlay *play = (IWXPlay *)p;
	if (play)play->Pause();
}

WXFFMPEG_CAPI void     WXFfplayResume(void* p) {
	IWXPlay *play = (IWXPlay *)p;
	if (play)play->Resume();
}


WXFFMPEG_CAPI void     WXFfplayRefresh(void* p) {
	IWXPlay *play = (IWXPlay *)p;
	if (play)play->Refresh();
}

WXFFMPEG_CAPI int64_t  WXFfplayGetCurrTime(void* p) {
	IWXPlay *play = (IWXPlay *)p;
	if (play)return play->GetCurrTime();
	return 0;
}

WXFFMPEG_CAPI int64_t  WXFfplayGetTotalTime(void* p) {
	IWXPlay *play = (IWXPlay *)p;
	if (play)return play->GetTotalTime();
	return 0;
}

WXFFMPEG_CAPI int  WXFfplayGetVolume(void* p) {
	IWXPlay *play = (IWXPlay *)p;
	if (play)return play->GetVolume();
	return 0;
}

WXFFMPEG_CAPI void  WXFfplaySeek(void* p, int64_t pts) {
	IWXPlay *play = (IWXPlay *)p;
	if (play)play->SetSeek(pts);
}

WXFFMPEG_CAPI void  WXFfplaySpeed(void* p, int speed) {
	IWXPlay *play = (IWXPlay *)p;
	if (play)play->SetSpeed(speed);
}


WXFFMPEG_CAPI int  WXFfplayGetState(void* p) {
	IWXPlay *play = (IWXPlay *)p;
	if (play)return play->GetState();
	return 0;
}

WXFFMPEG_CAPI void     WXFfplaySetReset(void* p) {
	IWXPlay *play = (IWXPlay *)p;
	if (play)play->Reset();
}


WXFFMPEG_CAPI void     WXFfplaySetSubtitle(void* p, WXCTSTR  wsz) {
	IWXPlay *play = (IWXPlay *)p;
	if (play)play->SetSubtitle(wsz);
}

WXFFMPEG_CAPI void     WXFfplaySetSubtitleFont(void * p, WXCTSTR  wszFontName, int FontSize, int FontColor) {
	IWXPlay *play = (IWXPlay *)p;
	if (play)play->SetSubtitleFont(wszFontName, FontSize, FontColor);
}

WXFFMPEG_CAPI void     WXFfplaySetSubtitleAlpha(void * p, int alpha) {
	IWXPlay *play = (IWXPlay *)p;
	if (play)play->SetSubtitleAlpha(alpha);
}

WXFFMPEG_CAPI void     WXFfplaySetSubtitlePostion(void * p, int postion) {
	IWXPlay *play = (IWXPlay *)p;
	if (play)play->SetSubtitlePostion(postion);
}

WXFFMPEG_CAPI void     WXFfplaySetSubtitleAlignment(void * p, int alignment) {
	IWXPlay *play = (IWXPlay *)p;
	if (play)play->SetSubtitleAlignment(alignment);
}

WXFFMPEG_CAPI void     WXFfplayCrop(void* p, int x, int y, int w, int h) {
	IWXPlay *play = (IWXPlay *)p;
	if (play)play->SetCrop(x, y, w, h);
}

WXFFMPEG_CAPI void     WXFfplayVFlip(void* p, int b) {
	IWXPlay *play = (IWXPlay *)p;
	if (play)play->SetVFlip(b);
}

WXFFMPEG_CAPI void     WXFfplayHFlip(void* p, int b) {
	IWXPlay *play = (IWXPlay *)p;
	if (play)play->SetHFlip(b);
}

WXFFMPEG_CAPI void     WXFfplayRotate(void* p, int rotate) {
	IWXPlay *play = (IWXPlay *)p;
	if (play)play->SetRoate(rotate);
}

WXFFMPEG_CAPI void     WXFfplayPictureQuality(void* p, int Brightness, int Contrast, int Saturation) {
	IWXPlay *play = (IWXPlay *)p;
	if (play)play->SetPictureQuality(Brightness, Contrast, Saturation);
}


WXFFMPEG_CAPI void     WXFfplayBrightness(void* p, int Brightness) {
	IWXPlay *play = (IWXPlay *)p;
	if (play)play->SetBrightness(Brightness);
}

WXFFMPEG_CAPI void     WXFfplayContrast(void* p, int Contrast) {
	IWXPlay *play = (IWXPlay *)p;
	if (play)play->SetContrast(Contrast);
}

WXFFMPEG_CAPI void     WXFfplaySaturation(void* p, int Saturation) {
	IWXPlay *play = (IWXPlay *)p;
	if (play)play->SetSaturation(Saturation);
}
