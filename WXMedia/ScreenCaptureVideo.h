/*
录屏时的视频
*/
#ifndef _ScreenCaptureVideo_H_
#define _ScreenCaptureVideo_H_

#include "ScreenCapture.h"
#include "VideoSource.h"
#include "FlvMaker.h"

static int s_bDisableDXGI = FALSE;
SCREENGRAB_CAPI void WXTcpDisableDXGI(int b) {  //招商银行的触摸屏使用DXGI采集会黑屏，要禁用DXGI录屏功能，只能用GDI
	s_bDisableDXGI = b;
}

//视频处理部分
class  ScreenCaptureVideo :public WXTcpThread {
public:
	int64_t  m_ptsLast = 0;
	FlvMaker* m_pMuxer = nullptr;
	ScreenCapture* m_pCapture = nullptr;
	VideoSource* m_pVideoSource = nullptr;

	int      m_iWidth = 0;
	int      m_iHeight = 0;

	int64_t  m_ptsLastVideo = 0;//时间戳（包含暂停时间中的）
	int64_t  m_ptsDelay = 0;//时间戳
	BOOL     m_bStart = FALSE;

	int m_iTime = 0;//帧间时间间隔
	int64_t m_ptsStart = 0;

	BOOL m_bFirst = FALSE;
	void WriteVideoFrame(WXTcpVideoFrame* frame) {  //每一帧的时间戳都是绝对时间！！！
		frame->m_pts -= m_ptsStart;//处理为相对时间

		if (frame->m_pts <= m_ptsLastVideo)
			return;//时间戳不动了

		//暂停处理
		if (m_pCapture->m_bPause) {
			m_ptsDelay += (frame->m_pts - m_ptsLastVideo);//视频暂停时间
			m_ptsLastVideo = frame->m_pts;
			return;//暂停时间不写文件
		}
		else {
			m_ptsLastVideo = frame->m_pts;
		}
		int64_t ptsVideo = frame->m_pts - m_ptsDelay;//计算输出时间戳

		if (!m_bFirst) { //补0s时的图像
			m_bFirst = TRUE;
			frame->m_pts = 0; //实际的视频时间戳
			m_pMuxer->WriteVideoFrame(frame);//直接输出
		}

		frame->m_pts = ptsVideo; //实际的视频时间戳
		m_ptsLast = ::timeGetTime();//最后一次视频实际编码时间
		m_pMuxer->WriteVideoFrame(frame);//直接输出
	}
public:
	void ThreadProcess() {  //编码输出
		int64_t ptsA = ::timeGetTime();
		WXTcpVideoFrame* video_frame = m_pVideoSource->GrabFrame();
		if (video_frame) {
			WriteVideoFrame(video_frame);
			int64_t ptsB = ::timeGetTime() - ptsA; //编码耗时
			WXTcpSleepMs(m_iTime - ptsB + 4);
		}
		else {
			::Sleep(1);
		}
	}
public:
	int  Config(ScreenCapture* pCapture) {
		m_pCapture = pCapture;
		WXScreenGrabParam* video_param = &pCapture->m_param;
		m_iTime = 1000 / video_param->m_iFps;
		if (!s_bDisableDXGI) {
			if (!WXTcpSupportDXGI()) { //Win7 System!!", __FUNCTIONW__);
				m_pVideoSource = VideoSource::Create(_T("GDI"));
			}
			else {
				int nNumDisplay = WXTcpScreenGetCount();
				if (nNumDisplay == 1) { //单显示器
					int nRotate = WXTcpScreenGetRotate(video_param->m_wszDevName);//旋转角度， 带旋转的禁止使用DXGI采集  2019.04.11  By Tam.Xie 
					if (nRotate == 0) { //单显示器，不转
						WXTcpLog(L"%ws Only one Screen, Using DXGI", __FUNCTIONW__);
						m_pVideoSource = VideoSource::Create(_T("DXGI")); //Win8 支持DXGI
					}
				}
				//else {
				//	PlayInfo *info = WXTcpScreenGetDefaultInfo();//默认显示器
				//	int nRotate = WXTcpScreenGetRotate(info->wszName);//旋转角度
				//	if (nRotate == 0) { //多显示器，但是主显示器不旋转
				//		WXTcpLog(L"%ws Using GDI/DXGI Mixer mode!!", __FUNCTIONW__);
				//		m_pVideoSource = VideoSource::Create(_T("Mixer")); //Win8 支持DXGI
				//	}
				//}
			}
		}

		if (nullptr == m_pVideoSource) {
			WXTcpLog(L"%ws new GDI Capture", __FUNCTIONW__);
			m_pVideoSource = VideoSource::Create(_T("GDI"));
		}

		if (m_pVideoSource) {
			int ret = m_pVideoSource->Start(this->m_pCapture);
			if (ret != _WX_ERROR_SUCCESS) {
				WXTcpLog(L"Open Video Capture Error!!!!");
				VideoSource::Destroy(m_pVideoSource);
				m_pVideoSource = nullptr;
				return ret;
			}
			else {
				m_iWidth = m_pVideoSource->GetWidth();
				m_iHeight = m_pVideoSource->GetHeight();
				WXTcpLog(L"Set Video Size = %dx%d", m_iWidth, m_iHeight);
			}
			return _WX_ERROR_SUCCESS;
		}
		return _WX_ERROR_ERROR;
	}

	void Start() { //启动
		if (m_pVideoSource) {
			m_ptsStart = ::timeGetTime();
			ThreadStart();
		}
	}

	void Stop() { //结束
		if (m_pVideoSource) {
			ThreadStop();

			m_pVideoSource->Stop();
			VideoSource::Destroy(m_pVideoSource);
			m_pVideoSource = nullptr;

			if (m_ptsDelay) //暂停时间
				WXTcpLog(L"%ws TimeDelay=%lld", __FUNCTION__, m_ptsDelay);

			int64_t ptsDuration = ::timeGetTime() - m_ptsStart;
			WXTcpLog(L"%ws TimeDuration=%lld", __FUNCTIONW__, ptsDuration);
		}
	}
};

#endif
