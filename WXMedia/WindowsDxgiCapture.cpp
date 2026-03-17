/*
基于DXGI桌面视频采集类，现在只支持主显示器
by Tam.Xie 2017.08.01
* Start接口中  index=0表示默认显示器,rect为nullptr 表示全屏抓取
* 2018.12.19 修复AcquireNextFrame的bug， DesktopResource对象用完应该Release
* 多屏幕时可能会导致录制黑屏，建议使用GDI
*/
#include "ScreenGrab.hpp"
#include "WindowsDxgiCapture.h"
#include "ScreenCapture.h"

#define MAX_LOG_NUMBER 100 

int      WindowsDxgiCapture::OpenDevice() {

	if (wcsicmp(m_pParam->m_wszDevName, L"default") == 0) {
		PlayInfo* info = WXTcpScreenGetDefaultInfo();
		m_strName = info->wszName;
	}
	else {
		m_strName = m_pParam->m_wszDevName;
	}

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

	D3D_FEATURE_LEVEL FeatureLevels[4] = {
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_1
	};
	UINT NumFeatureLevels = 4;

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

void     WindowsDxgiCapture::Stop() {
	if (m_bStart) {
		m_bStart = FALSE;
		m_pDev = nullptr;
		m_pContext = nullptr;
		m_pDesktopDevice = nullptr;
		m_pTexture = nullptr;
		WXTcpLog(L"%ws TimeOut=%lld m_nOpenForTimeOut=%lld", __FUNCTIONW__, m_nTimeOut, m_nOpenForTimeOut);
	}
}


void WindowsDxgiCapture::Draw(int64_t ptsCapture) {

	if (m_bUseMouse && m_bMouseVisable) { //鼠标的相关操作
		//叠加鼠标图像 
		m_cursor.Draw(m_pScreenFrame, m_cursor.cursor_pos.x, m_cursor.cursor_pos.y, 100);
	}
}

//通过HDC来操作数据很慢。。
WXTcpVideoFrame* WindowsDxgiCapture::GrabFrame() {
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
			memcpy(m_InputVideoFrame.m_pBuf, m_pScreenFrame.m_pBuf, m_iScreenWidth & m_iScreenHeight * 4);

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

BOOL WindowsDxgiCapture::DxgiGrab() {

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

int WindowsDxgiCapture::Start(ScreenCapture* capture) {
	if (WXTcpGetSystemVersion() <= 7)
		return _WX_ERROR_VIDEO_NO_DEVICE;//支持Win8 及以上的系统

	SetCapture(capture);
	int ret = OpenDevice();
	ret == _WX_ERROR_SUCCESS ? WXTcpLog(L"DXGI Open OK") : WXTcpLog(L"DXGI Open Failed");
	if (ret == _WX_ERROR_SUCCESS) {

		//获取主显示器大小
		PlayInfo* info = WXTcpScreenGetDefaultInfo();
		m_iScreenWidth = info->width / 2 * 2;
		m_iScreenHeight = info->height / 2 * 2;// 主屏幕大小
		m_pScreenFrame.Init(m_iScreenWidth, m_iScreenHeight);	//屏幕数据
		WXTcpLog(L"%ws Create DXGI Screen=[%dx%d]", __FUNCTIONW__, m_iScreenWidth, m_iScreenHeight);

		m_cursor.Capture();//获取鼠标信息


		{ //全屏幕
			m_iWidth = m_iScreenWidth;
			m_iHeight = m_iScreenHeight;
			m_iRectW = m_iWidth;
			m_iRectH = m_iHeight;
			m_iPosX = 0;
			m_iPosY = 0;
		}

		m_InputVideoFrame.Init(m_iWidth, m_iHeight);//采集图像

		m_bStart = TRUE;
		return _WX_ERROR_SUCCESS;
	}
	return ret;
}

const wchar_t* WindowsDxgiCapture::Type() {
	return L"DXGI";
}
