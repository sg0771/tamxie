/*
视频数据源
*/
#ifndef _IWXTcpVIDEO_DEV_H
#define _IWXTcpVIDEO_DEV_H

#include "ScreenGrab.hpp"
#include "ScreenCapture.h"

//双缓冲
#define   MAX_POOL 100

class ScreenCapture;
class  VideoSource {

public:
	static VideoSource *Create(const wchar_t* wszType);
	static void Destroy(VideoSource * p);
public:
	virtual int      Start(ScreenCapture *capture) = 0;//启动
	virtual void     Stop() = 0;     //结束
	virtual WXTcpVideoFrame* GrabFrame() = 0; //获取数据
	virtual const wchar_t*  Type() = 0;  //类型

public:


	WXTcpVideoFrame m_tempFrame;//RGBA
public:
	virtual ~VideoSource() {
		WXTcpLog(L"%ws %lld", __FUNCTIONW__, m_nInsert);
		SAFE_RELEASE_DC(m_hWnd, m_hDC)
		SAFE_DELETE_OBJECT(m_hBitmap)
		SAFE_DELETE_DC(m_hMemDC)
		SAFE_DELETE(m_pBrushMouse);
	}
public:
	//鼠标
	BOOL m_bUseMouse = FALSE;//使用鼠标
	BOOL m_bMouseVisable = FALSE;//鼠标是否可见
	POINT    m_ptCursor;
	HCURSOR  m_hCursor = nullptr;
	ICONINFO m_iconInfo;
	int m_iAlphaHotdot = 1;//热点透明度
	int m_iAlphaAnimation = 1;//动画透明度
	Gdiplus::SolidBrush *m_pBrushMouse = nullptr;
	BOOL m_bClickLeft = FALSE;
	BOOL m_bClickRight = FALSE;
	int m_lastX = -1000;
	int m_lastY = -1000;
	int64_t m_ptsMouseLeftAction = -1; //鼠标左键点击时间
	int64_t m_ptsMouseRightAction = -1; //鼠标右键点击时间

	//GDI录制
	uint32_t m_uCaptureBlt = SRCCOPY;
	HDC m_hDC = nullptr;
	HDC m_hMemDC = nullptr;// 屏幕和内存设备描述表
	HBITMAP m_hBitmap = nullptr;

	//录屏对象
	void SetCapture(ScreenCapture *capture) { 
		m_pCapture = capture;
		if (m_pCapture) {
			m_pParam = &m_pCapture->m_param;
			m_uCaptureBlt =  SRCCOPY;//CAPTUREBLT 鼠标有闪烁

			//默认25fps
			if (m_pParam->m_iFps == 0)
				m_iTime = 40;//最高100fps
			else
				m_iTime = (int)(1000.0 / m_pParam->m_iFps + 0.5);
			
			m_ptsLast = -m_iTime;
			m_bUseMouse = TRUE; // 是否使用鼠标信息
		}
	}

	//绘制鼠标、文字、图像等东西
	void DrawEx(int posx = 0, int posy = 0) {
		if (m_hMemDC == nullptr)return;
		if (m_bMouseVisable == FALSE)
			return;
		if (m_bMouseVisable) { //鼠标绘制
			int DrawX = posx;
			int DrawY = posy;
			//最后绘制鼠标！！
			::DrawIconEx(m_hMemDC, DrawX, DrawY, m_hCursor, 0, 0, 0, nullptr, DI_NORMAL | DI_COMPAT);//鼠标居中
		}
	}

public:
	int64_t m_nCapture = 0;
	int64_t m_nInsert = 0;
	int64_t m_ptsLast = 0;//上一帧的采集时间
	int64_t m_nDrop = 0;//采集丢包
	std::wstring m_strName;
	HWND m_hWnd = nullptr;
	int m_iOrgW = 0;
	int m_iOrgH = 0;
public:
	int64_t m_nDropRegion = 0;//区域录制失败
	int64_t m_nDropBitBlt = 0;//BitBlt失败
	int64_t m_nDropGetDIBits = 0;//GetDIBits失败


public:
	inline int GetWidth() {
		return m_iWidth;
	}
	inline int GetHeight() {
		return m_iHeight;
	}
public:
	VideoSource() {}
public:
	WXTcpVideoFrame  m_InputVideoFrame;//输入的图像
	volatile BOOL m_bStart = FALSE;

	WXTcpLocker m_mutex;
	int64_t m_iPosX = 0;
	int64_t m_iPosY = 0;//带RECT区域时候的起始位置
	int64_t m_iRectW = 0;
	int64_t m_iRectH = 0;

	int m_iWidth = 0;
	int m_iHeight = 0;
	int m_iTime = 40;//40ms
	WXScreenGrabParam *m_pParam = nullptr;
	ScreenCapture *m_pCapture = nullptr;
};

#endif