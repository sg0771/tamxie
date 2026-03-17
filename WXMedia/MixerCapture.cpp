/*
* 混合了DXGI/GDI的录屏类，在主显示器区域录制的时候优先使用DXGI采集
*/

#include "MixerCapture.h"
#include "ScreenGrab.hpp"
#include "ScreenCapture.h"

#define MAX_LOG_NUMBER 100 

int MixerCapture::OpenDevice() {
	m_strName = m_pParam->m_wszDevName;
	m_nTimeOutIndex = 0;
	m_pDev = nullptr;
	m_pContext = nullptr;
	m_pDesktopDevice = nullptr;
	m_pTexture = nullptr;

	// Driver types supported
	D3D_DRIVER_TYPE DriverTypes[3] = {
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT NumDriverTypes = 3;

	D3D_FEATURE_LEVEL FeatureLevels[5] = {
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_1
	};
	UINT NumFeatureLevels = 5;

	HRESULT hr = S_OK;

	// Create D3D11 device
	D3D_FEATURE_LEVEL FeatureLevel;
	for (UINT DriverTypeIndex = 0; DriverTypeIndex < NumDriverTypes; DriverTypeIndex++) {
		hr = D3D11CreateDevice(nullptr, DriverTypes[DriverTypeIndex], nullptr, 0, FeatureLevels, NumFeatureLevels,
			D3D11_SDK_VERSION, &m_pDev, &FeatureLevel, &m_pContext);
		if (SUCCEEDED(hr)) {
			break;
		}
	}
	if (m_pContext == nullptr) {
		WXTcpLog(L"D3D11CreateDevice DX Error[%x]", hr);
		return _WX_ERROR_ERROR;;
	}

	// Get DXGI device
	CComPtr<IDXGIDevice>DxgiDevice = nullptr;
	hr = m_pDev->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&DxgiDevice));
	if (SUCCEEDED(hr)) {
		CComPtr<IDXGIAdapter>DxgiAdapter = nullptr;
		hr = DxgiDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&DxgiAdapter));	// Get DXGI adapter
		if (SUCCEEDED(hr)) {
			// Get output
			CComPtr<IDXGIOutput>DxgiOutput = nullptr;
			UINT i = 0; //枚举所有的显示器，匹配名字
			while (SUCCEEDED(DxgiAdapter->EnumOutputs(i, &DxgiOutput))) {
				DXGI_OUTPUT_DESC desc;//可以获取名字
				DxgiOutput->GetDesc(&desc);
				if (wcsicmp(m_strName.c_str(), desc.DeviceName) == 0) { //找到同名显示器
					CComPtr<IDXGIOutput1>DxgiOutput1 = nullptr;
					hr = DxgiOutput->QueryInterface(__uuidof(DxgiOutput1), reinterpret_cast<void**>(&DxgiOutput1)); // QI for Output 1
					if (SUCCEEDED(hr)) {
						hr = DxgiOutput1->DuplicateOutput(m_pDev, &m_pDesktopDevice);//获取桌面采集对象
						if (SUCCEEDED(hr)) {
							DxgiOutput = nullptr;
							DxgiOutput1 = nullptr;
							WXTcpLog(L"%ws OK", __FUNCTIONW__);
							return _WX_ERROR_SUCCESS;
						}
					}
					DxgiOutput1 = nullptr;
					DxgiOutput = nullptr;//没有合适的设备
					break;//找到合适的设备
				}
				DxgiOutput = nullptr;//没有合适的设备
				i++;
			}
		}
		else {
			WXTcpLog(L"%ws GetParent->IDXGIAdapter Error", __FUNCTIONW__);
		}
		DxgiAdapter = nullptr;
	}
	else {
		WXTcpLog(L"%ws QueryInterface-IDXGIDevice Error", __FUNCTIONW__);
	}
	WXTcpLog(L"%ws Error", __FUNCTIONW__);
	DxgiDevice = nullptr;
	return _WX_ERROR_ERROR;
}

WXTcpVideoFrame* MixerCapture::GrabFrame() {
	WXTcpAutoLock al(m_mutex);
	{
		if (m_iPosX < m_iScreenLeft ||
			m_iPosY < m_iScreenTop ||
			m_iPosX + m_iWidth > m_iScreenLeft + m_iScreenWidth ||
			m_iPosY + m_iHeight > m_iScreenTop + m_iScreenHeight) {
			return GDIGrabFrame(); //跟随鼠标模式，使用GDI,超出主显示器图像区域
		}
		else {
			return DXGIGrabFrame();
		}
	}
	return nullptr;
}

//初始化GDI采集
int MixerCapture::Start(ScreenCapture* capture) {
	SetCapture(capture);

	//主显示器区域
	PlayInfo* infoDefault = WXTcpScreenGetDefaultInfo();
	if (infoDefault == nullptr) {
		WXTcpScreenInit();
		infoDefault = WXTcpScreenGetDefaultInfo();
		if (infoDefault == nullptr) {
			WXTcpLog(L"%ws Error", __FUNCTIONW__);
			return _WX_ERROR_ERROR;
		}
	}

	m_iScreenLeft = infoDefault->left;
	m_iScreenTop = infoDefault->top;
	m_iScreenRight = infoDefault->left + infoDefault->width;
	m_iScreenBottom = infoDefault->top + infoDefault->height;
	//获取主显示器大小

	m_iScreenWidth = infoDefault->width / 2 * 2;
	m_iScreenHeight = infoDefault->height / 2 * 2;// 主屏幕大小
	m_pScreenFrame.Init(m_iScreenWidth, m_iScreenHeight);	//屏幕数据
	WXTcpLog(L"%ws Create DXGI Screen=[%dx%d]", __FUNCTIONW__, m_iScreenWidth, m_iScreenHeight);


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

	m_strName = capture->m_param.m_wszDevName;
	m_hDC = ::GetDC(nullptr);
	m_hMemDC = ::CreateCompatibleDC(NULL);
	m_pBits = nullptr;
	m_hBitmap = ::CreateDIBSection(m_hMemDC, (const BITMAPINFO*)&m_InputVideoFrame.m_bih,
		DIB_RGB_COLORS, (void**)&m_pBits, NULL, 0);

	m_cursor.Capture();//获取鼠标信息

	m_bStart = TRUE;
	WXTcpLog(L"%ws OK", __FUNCTIONW__);
	return _WX_ERROR_SUCCESS;
}

//关闭设备
void MixerCapture::Stop() {
	if (m_bStart) {
		m_bStart = FALSE;
		SAFE_RELEASE_DC(nullptr, m_hDC)
			SAFE_DELETE_DC(m_hMemDC)
			SAFE_DELETE_OBJECT(m_hBitmap)
			m_pDev = nullptr;
		m_pContext = nullptr;
		m_pDesktopDevice = nullptr;
		m_pTexture = nullptr;
		WXTcpLog(L"%ws TimeOut=%lld m_nOpenForTimeOut=%lld", __FUNCTIONW__, m_nTimeOut, m_nOpenForTimeOut);
	}
}


WXTcpVideoFrame* MixerCapture::GDIGrabFrame() {

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

const wchar_t* MixerCapture::Type() {
	return L"Mixer";
}

void MixerCapture::Draw(int64_t ptsCapture) {
	if (m_bUseMouse && m_bMouseVisable) { //鼠标的相关操作
		//叠加鼠标图像 
		m_cursor.Draw(m_pScreenFrame, m_cursor.cursor_pos.x, m_cursor.cursor_pos.y, 100);
	}
}

WXTcpVideoFrame* MixerCapture::DXGIGrabFrame() {
	int64_t ptsVideo = ::timeGetTime();
	if (DxgiGrab()) {

		if (m_pScreenFrame.IsBlack()) {
			return nullptr;//纯黑帧！！
		}

		m_bMouseVisable = FALSE;//鼠标信息
		if (m_bUseMouse) {
			m_cursor.Capture();
			m_bMouseVisable = m_cursor.visible;//是否可见
		}

		// 在屏幕采集数据上画鼠标
		Draw(ptsVideo);

		if (!m_bUseMouse &&
			m_iPosX == 0 &&
			m_iPosY == 0 &&
			m_iRectW == m_iScreenWidth &&
			m_iRectH == m_iScreenHeight) {
			//全屏图像，直接输出
			memcpy(m_InputVideoFrame.m_pBuf, m_pScreenFrame.m_pBuf, m_iScreenWidth * m_iScreenHeight * 4);
		}
		else { //区域录制
			int PosX = m_iPosX;
			int PosY = m_iPosY;
			if (m_iRectW == m_iWidth && m_iRectH == m_iHeight) { //不缩放处理
				RgbaData::CopyRect(m_pScreenFrame, PosX, PosY, m_InputVideoFrame); //把图像拷贝到区域
			}
			else { //缩放处理
				if (m_RectFrame.m_iWidth != m_iRectW || m_RectFrame.m_iHeight) {
					m_RectFrame.Init(m_iRectW, m_iRectH);
				}
				RgbaData::CopyRect(m_pScreenFrame, PosX, PosY, m_RectFrame); //把图像拷贝到区域
				libyuv::ARGBScale(m_RectFrame.m_pBuf, m_RectFrame.m_iWidth * 4,
					m_RectFrame.m_iWidth, m_RectFrame.m_iHeight,
					m_InputVideoFrame.m_pBuf, m_InputVideoFrame.m_iWidth * 4,
					m_InputVideoFrame.m_iWidth, m_InputVideoFrame.m_iHeight,
					libyuv::FilterMode::kFilterBilinear);
			}
		}
		m_InputVideoFrame.m_pts = ptsVideo;
		return &m_InputVideoFrame;
	}
	return nullptr;
}

BOOL MixerCapture::DxgiGrab() {

	if (m_pDesktopDevice == nullptr)
		OpenDevice();//重新打开设备

	if (m_pDesktopDevice == nullptr) {
		//重新打开设备失败
		return FALSE;
	}

	CComPtr<IDXGIResource>DesktopResource = nullptr;
	DXGI_OUTDUPL_FRAME_INFO FrameInfo;
	HRESULT hr = m_pDesktopDevice->AcquireNextFrame(100, &FrameInfo, &DesktopResource);//获取桌面数据
	if (hr == DXGI_ERROR_ACCESS_LOST) { // 游戏启动过程会独占设备
		//WXTcpLog(L"DXGI_ERROR_ACCESS_LOST!!");
		::Sleep(1000);
		OpenDevice();//重新打开设备
		return DxgiGrab();
	}
	else if (hr == DXGI_ERROR_WAIT_TIMEOUT) { //获取超时
		//WXTcpLog(L"DXGI_ERROR_WAIT_TIMEOUT!!");
		//继续使用上一帧的数据
		m_nTimeOut++;
		m_nTimeOutIndex++;
		if (m_nTimeOutIndex < 10) {
			return !!m_pTexture;
		}
		else {
			m_nTimeOutIndex = 0;
			m_nOpenForTimeOut++;
			OpenDevice();//重新打开设备
			return DxgiGrab();
		}
	}
	else if (SUCCEEDED(hr)) { //正常采集
		m_nTimeOutIndex = 0;
		CComPtr<ID3D11Texture2D>AcquiredDesktopImage = nullptr;
		hr = DesktopResource->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&AcquiredDesktopImage));//获取桌面数据
		if (SUCCEEDED(hr)) {
			D3D11_TEXTURE2D_DESC frameDescriptor;
			AcquiredDesktopImage->GetDesc(&frameDescriptor); //注意一下采集的数据格式是否 DXGI_FORMAT_B8G8R8A8_UNORM
			if (frameDescriptor.Format != DXGI_FORMAT_B8G8R8A8_UNORM) {
				WXTcpLog(L"DXGI Error Format=%d", (int)frameDescriptor.Format);
				m_pDesktopDevice->ReleaseFrame();
				AcquiredDesktopImage = nullptr;
				DesktopResource = nullptr;
				return FALSE;
			}

			frameDescriptor.Usage = D3D11_USAGE_STAGING;
			frameDescriptor.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
			frameDescriptor.BindFlags = 0;
			frameDescriptor.MiscFlags = 0;
			frameDescriptor.MipLevels = 1;
			frameDescriptor.ArraySize = 1;
			frameDescriptor.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
			frameDescriptor.SampleDesc.Count = 1;
			//2019.01.08 修复因为UAC造成的停止视频录制问题
			if (m_pTexture == nullptr ||  //UAC操作之后为NULL
				m_tmpWidth != frameDescriptor.Width || //大小不匹配，比如某些游戏造成的问题
				m_tmpHeight != frameDescriptor.Height) {
				m_tmpWidth = frameDescriptor.Width;//当前抓取的图像大小
				m_tmpHeight = frameDescriptor.Height;
				WXTcpLog(L"DXGI NeSize=[%dx%d]", m_tmpWidth, m_tmpHeight);
				m_pTexture = nullptr;
				hr = m_pDev->CreateTexture2D(&frameDescriptor, nullptr, &m_pTexture);//创建内存表面
			}
			if (m_pTexture == nullptr) {  //创建显存失败！
				m_pDesktopDevice->ReleaseFrame();
				AcquiredDesktopImage = nullptr;
				DesktopResource = nullptr;
				return FALSE;
			}

			m_pContext->CopyResource(m_pTexture, AcquiredDesktopImage);// 桌面拷贝数据到显存
			m_pDesktopDevice->ReleaseFrame();

			AcquiredDesktopImage = nullptr;
			DesktopResource = nullptr;

			D3D11_MAPPED_SUBRESOURCE mappedRect = {};   //显存到内存
			hr = m_pContext->Map(m_pTexture, 0, D3D11_MAP_READ, 0, &mappedRect);
			if (SUCCEEDED(hr)) {
				if (m_tmpWidth != m_iScreenWidth || m_tmpHeight != m_iScreenHeight) { //分辨率被切换，一般是在游戏中
					libyuv::ARGBScale((uint8_t*)mappedRect.pData, mappedRect.RowPitch,
						m_tmpWidth, m_tmpHeight,
						m_pScreenFrame.m_pBuf, m_pScreenFrame.m_iWidth * 4,
						m_iScreenWidth, m_iScreenHeight,
						libyuv::kFilterLinear);
				}
				else {
					libyuv::ARGBCopy((uint8_t*)mappedRect.pData, mappedRect.RowPitch,
						m_pScreenFrame.m_pBuf, m_pScreenFrame.m_iWidth * 4,
						m_iScreenWidth, m_iScreenHeight);
				}
				m_pContext->Unmap(m_pTexture, 0);//不做操作
				return TRUE;//成功抓图
			}
		}
	}
	else {
		//WXTcpLog(L"%ws HR = %x",__FUNCTIONW__,hr);
		::Sleep(1000);
		OpenDevice();//重新打开设备
		return DxgiGrab();
	}
	return FALSE;
}
