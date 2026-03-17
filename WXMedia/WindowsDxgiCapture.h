/*
	桌面视频采集类
	by Tam.Xie 2017.08.01
	* Start接口中  index=0表示默认显示器,rect为nullptr 表示全屏抓取
	基于 D3D11 DXGI 桌面采集接口
*/
#ifndef _WINDOWS_DXGI_CAPTURE_H_
#define _WINDOWS_DXGI_CAPTURE_H_

#include <ScreenGrab.hpp>
#include "VideoSource.h"
#include <d3d11.h>
#include "CursorCapture.h"

class WindowsDxgiCapture :public VideoSource {
	int64_t  m_nTimeOut = 0;//超时次数
	int64_t  m_nTimeOutIndex = 0;//连续超时次数
	int64_t  m_nOpenForTimeOut = 0;//因为超时导致的设备重启

	//采集时的分辨率，全屏游戏的时候可能会改变当前显示器的分辨率
	int m_tmpWidth = 0;
	int m_tmpHeight = 0;

	//初始化状态时的分辨率
	HDC m_hScreenMemDC = nullptr;
	int m_iScreenWidth = 0;
	int m_iScreenHeight = 0;
	WXTcpVideoFrame m_pScreenFrame;//全屏数据，无鼠标
	WXTcpVideoFrame m_RectFrame;//全屏数据，无鼠标

	CComPtr<ID3D11Device>m_pDev = nullptr;//D3D11 设备
	CComPtr<ID3D11DeviceContext>m_pContext = nullptr;//D3D11 上下文
	CComPtr<IDXGIOutputDuplication>m_pDesktopDevice = nullptr;//D3D11 桌面设备
	CComPtr<ID3D11Texture2D>m_pTexture = nullptr;//内存表面

	CursorCapture m_cursor;//鼠标信息
	WXTcpVideoFrame m_cursorHotdotFrame;//鼠标热点
	WXTcpVideoFrame m_cursorLeftFrame;  //鼠标左键
	WXTcpVideoFrame m_cursorRightFrame; //鼠标右键


	int  OpenDevice();
	BOOL DxgiGrab();
	void Draw(int64_t ptsCapture);//绘制鼠标、文字、图像等
public:
	virtual int      Start(ScreenCapture* capture);//初始化是否成功
	virtual void     Stop();
	virtual WXTcpVideoFrame* GrabFrame();//内部线程获取数据
	virtual const wchar_t* Type();
};

#endif //_WINDOWS_DXGI_CAPTURE_H_

