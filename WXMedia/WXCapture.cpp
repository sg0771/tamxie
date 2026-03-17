/*
音视频采集后封装为多媒体文件格式
*/

#include <ScreenGrab.hpp>
#include "ScreenCapture.h"
#include "AudioMixer.h"
#include "WasapiDevice.h"
#include "VideoSource.h"
#include "ScreenCaptureAudio.h"
#include "ScreenCaptureVideo.h"
#include "FLVMaker.h"



bool  ScreenCapture::Create(WXScreenGrabParam* paramExt, int& error) {
	WXTcpLog(L"%ws Video Param ", __FUNCTIONW__);
	error = 0;
	if (paramExt) {
		memcpy(&m_param, paramExt, sizeof(WXScreenGrabParam));
	}

	m_strFileName = m_param.m_wszName;//输出文件名

	{
		m_pVideo = new ScreenCaptureVideo();
		int retVideo = m_pVideo->Config(this); //添加视频设备
		error |= retVideo;
		if (retVideo != _WX_ERROR_SUCCESS) {
			WXTcpLog(L"Open Video Device Error!!!");
			SAFE_DELETE(m_pVideo);
		}
	}
	//输出文件支持音频，比如GIF就没有音频
	{
		m_pAudio = new ScreenCaptureAudio();
		int retAudio = m_pAudio->Config(this);
		if (retAudio != _WX_ERROR_SUCCESS) {
			SAFE_DELETE(m_pAudio);
			error |= _WX_WARNING_NO_SOUND_DATA; //没有视频
			WXTcpLog(L"No Video Data");
		}
	}

	if (m_pAudio == nullptr && m_pVideo == nullptr) {
		WXTcpLog(L"No Media data");
		return false;
	}

	m_pMuxer = new FlvMaker();

	int ret = m_pMuxer->Open(
		m_param.m_wszName,
		m_param.m_nPort,
		m_pVideo ? m_pVideo->m_iWidth : 0,
		m_pVideo ? m_pVideo->m_iHeight : 0,
		m_param.m_iForceHeight); //创建Muxer编解码器,创建文件失败会直接退出
	if (ret != _WX_ERROR_SUCCESS) {
		error |= ret;
		Stop();
		WXTcpLog(L"Create File Failed = %ws, exit", m_strFileName.c_str());
		return false; //严重错误，crash。。
	}
	return true;
}

ScreenCapture::ScreenCapture() {

}

ScreenCapture::~ScreenCapture() {}

void ScreenCapture::Start() {
	if (!m_bStart && (m_pAudio || m_pVideo)) {
		if (m_pVideo) {
			m_pVideo->m_pMuxer = m_pMuxer;
			m_pVideo->Start();//混音线程
		}
		if (m_pAudio) {
			m_pAudio->m_pMuxer = m_pMuxer;
			m_pAudio->Start();//混音线程
		}
		m_bStart = true;
	}
}

void ScreenCapture::Stop() {
	if (m_bStart) {

		m_bStart = false;
		m_bPause = false;

		if (m_pAudio) {
			m_pAudio->Stop();
			delete m_pAudio;
			m_pAudio = nullptr;
		}

		if (m_pVideo) {
			m_pVideo->Stop();
			delete m_pVideo;
			m_pVideo = nullptr;
		}
		if (m_pMuxer) {
			m_pMuxer->Close();
			delete m_pMuxer;
			m_pMuxer = nullptr;
		}

	}
}

void ScreenCapture::Pause() {
	WXTcpLog(L" ScreenCapture pause ");
	if (!m_bPause) {
		m_bPause = true;
	}
}

void ScreenCapture::Resume() {
	WXTcpLog(L" ScreenCapture Resume ");
	if (m_bPause) {
		m_bPause = false;
	}
}

