/*
* 基于GDI的桌面视频采集类
* by Tam.Xie 2017.08.01
* 首先枚举显示器设备，获取每个显示器在虚拟屏幕上的位置RECT(x,y,w,h)
* 把主屏幕HDC对应区域内容拷贝到内存DC，然后选入到HBITMAP，从HBITMAP获取RGBA数据
*/

#include "WindowsGdiCapture.h"

#pragma comment(lib,"Msimg32.lib")

#define MAX_LOG_NUMBER 100

void WindowsGdiCapture::ThreadProcess() {
	int64_t pts1 = ::timeGetTime();
	WXTcpVideoFrame* vidoe_frame = GDIGrabFrame();
	if (vidoe_frame) { //获取成功

		WXTcpAutoLock al(m_lockQueue);
		int idx = 0;
		for (int i = 0; i < m_nPool; i++) {
			if (!m_arrData[i].m_bUsed) {
				idx = i;
				m_arrData[i].m_bUsed = TRUE;
				break;
			}
		}

		memcpy(m_arrData[idx].m_pBuf, vidoe_frame->m_pBuf, m_iWidth * 4 * m_iHeight);//拷贝数据保存到队列
		m_arrData[idx].m_pts = vidoe_frame->m_pts;
		m_queueData.push(idx);//保存到队列

		int64_t pts2 = ::timeGetTime() - pts1;
		WXTcpSleepMs(m_iTime + 4 - pts2);
	}
	else {
		::Sleep(1); //失败
	}

}


//初始化GDI采集
int WindowsGdiCapture::Start(ScreenCapture* capture) {
	SetCapture(capture);

	{
		WXTcpLog(L"%ws Open Desktop Device [%ws]", __FUNCTIONW__, m_pParam->m_wszDevName);
		PlayInfo* info = WXTcpScreenGetInfoByName(m_pParam->m_wszDevName);
		if (info == nullptr) {
			info = WXTcpScreenGetDefaultInfo();
		}
		m_iPosX = info->left;
		m_iPosY = info->top;
		m_iRectW = info->width / 2 * 2;
		m_iRectH = info->height / 2 * 2;
	}

	m_iWidth = m_iRectW;
	m_iHeight = m_iRectH;
	m_InputVideoFrame.Init(m_iWidth, m_iHeight);//采集图像

	m_OutputVideoFrame.Init(m_iWidth, m_iHeight);//输出图像
	for (int i = 0; i < m_nPool; i++) {  //初始化
		m_arrData[i].Init(m_iWidth, m_iHeight);
	}

	m_strName = capture->m_param.m_wszDevName;
	m_hDC = ::GetDC(nullptr);
	m_hMemDC = ::CreateCompatibleDC(NULL);
	m_pBits = nullptr;
	m_hBitmap = ::CreateDIBSection(m_hMemDC, (const BITMAPINFO*)&m_InputVideoFrame.m_bih,
		DIB_RGB_COLORS, (void**)&m_pBits, NULL, 0);
	m_bStart = TRUE;
	WXTcpLog(L"%ws OK", __FUNCTIONW__);
	ThreadStart();
	return _WX_ERROR_SUCCESS;
}

//关闭设备
void WindowsGdiCapture::Stop() {
	if (m_bStart) {
		ThreadStop();
		m_bStart = FALSE;
		SAFE_RELEASE_DC(nullptr, m_hDC)
			SAFE_DELETE_DC(m_hMemDC)
			SAFE_DELETE_OBJECT(m_hBitmap)
	}
}

WXTcpVideoFrame* WindowsGdiCapture::GrabFrame() {
	WXTcpAutoLock al(m_lockQueue);
	if (!m_queueData.empty()) {
		m_nCapture++;//实际编码帧
		int idx = m_queueData.front();
		m_queueData.pop();

		memcpy(m_OutputVideoFrame.m_pBuf, m_arrData[idx].m_pBuf, m_iWidth * 4 * m_iHeight);

		m_OutputVideoFrame.m_pts = m_arrData[idx].m_pts;
		m_arrData[idx].m_bUsed = FALSE;//标记
		return &m_OutputVideoFrame;//外部不能直接操作 m_arrData 的数据
	}
	return nullptr;
}

WXTcpVideoFrame* WindowsGdiCapture::GDIGrabFrame() {
	WXTcpAutoLock al(m_mutex);
	int64_t pts = ::timeGetTime();

	m_bMouseVisable = FALSE;
	//使用鼠标或者跟随鼠标
	{ //获取鼠标位置和信息

		// 获取当前光标记起位置
		::GetCursorPos(&m_ptCursor);//基于主屏幕的鼠标位置
		CURSORINFO cursorInfo;
		cursorInfo.cbSize = sizeof(CURSORINFO);
		if (::GetCursorInfo(&cursorInfo)) {
			// 获取光标的图像数据
			if (::GetIconInfo(cursorInfo.hCursor, &m_iconInfo)) {
				if (m_iconInfo.hbmMask != nullptr) {
					DeleteObject(m_iconInfo.hbmMask);
				}
				if (m_iconInfo.hbmColor != nullptr) {
					DeleteObject(m_iconInfo.hbmColor);
				}
				m_bMouseVisable = TRUE; //有鼠标图像
				m_hCursor = cursorInfo.hCursor;
				m_ptCursor.x -= ((int)m_iconInfo.xHotspot);
				m_ptCursor.y -= ((int)m_iconInfo.yHotspot);
			}
		}
	}

	if (nullptr == m_hDC) {
		m_hDC = ::GetDC(nullptr);
		m_hMemDC = ::CreateCompatibleDC(NULL);
		m_pBits = nullptr;
		m_hBitmap = ::CreateDIBSection(m_hMemDC, (const BITMAPINFO*)&m_InputVideoFrame.m_bih,
			DIB_RGB_COLORS, (void**)&m_pBits, NULL, 0);
	}

	HBITMAP hOldBitmap = (HBITMAP)::SelectObject(m_hMemDC, m_hBitmap);

	BOOL bScale = FALSE;
	BOOL bCopyDC = FALSE;

	if (m_iRectW != m_iWidth || m_iRectH != m_iHeight) {
		bScale = TRUE;
		SetStretchBltMode(m_hMemDC, HALFTONE);//硬件缩放
		bCopyDC = ::StretchBlt(m_hMemDC, 0, 0, m_iWidth, m_iHeight,
			m_hDC,
			m_iPosX,
			m_iPosY,
			m_iRectW, m_iRectH,
			m_uCaptureBlt);//有拖影
	}
	else {
		bCopyDC = ::BitBlt(m_hMemDC, 0, 0, m_iWidth, m_iHeight,
			m_hDC,
			m_iPosX,
			m_iPosY,
			m_uCaptureBlt);//有拖影
	}

	if (!bCopyDC) { //从主桌面获取数据失败
		m_nDropBitBlt++;
		m_hBitmap = (HBITMAP)::SelectObject(m_hMemDC, hOldBitmap);
		SAFE_RELEASE_DC(nullptr, m_hDC)
			SAFE_DELETE_OBJECT(m_hBitmap)
			SAFE_DELETE_DC(m_hMemDC)
			return nullptr;
	}

	if (m_bMouseVisable) { //绘制鼠标
		{
			//鼠标在当前屏幕的位置，虚拟坐标位置
			//暂时不考虑图像缩放后鼠标的缩放问题！
			m_ptCursor.x -= m_iPosX;
			m_ptCursor.y -= m_iPosY;
			if (bScale) {
				m_ptCursor.x = m_ptCursor.x * m_iWidth / m_iRectW;
				m_ptCursor.y = m_ptCursor.y * m_iHeight / m_iRectH;
			}
			DrawEx(m_ptCursor.x, m_ptCursor.y);
		}
	}
	else {
		DrawEx();
	}
	//获取 m_hMemDC RGBA 数据
	m_hBitmap = (HBITMAP)::SelectObject(m_hMemDC, hOldBitmap);
	memcpy(m_InputVideoFrame.m_pBuf, m_pBits, m_iWidth * m_iHeight * 4);
	m_InputVideoFrame.m_pts = pts;
	return &m_InputVideoFrame;
}

const wchar_t* WindowsGdiCapture::Type() {
	return L"GDI";
}


