/*
内部编译使用的头文件
*/

#ifndef  _WXTcpMEDIA_CPP_H_
#define  _WXTcpMEDIA_CPP_H_

#include <WXBase.h>
#include <ScreenGrab.h>


//GDI+头文件
#include <gdiplus.h>

//DirectShow头文件
#include <strmif.h>
#include <atlbase.h>
#include <dvdmedia.h>
#include <amvideo.h>
#include <control.h>
#include <uuids.h>
#include <ks.h>
#include <ksmedia.h>

//DXGI头文件
#include <dxgi.h>
#include <dxgi1_2.h>

#include <mfidl.h>
#include <libyuv.h>

//WASAPI头文件
#include <Avrt.h>
#include <MMSystem.h>
#include <MMDeviceapi.h>
#include <AudioClient.h>
#include <Functiondiscoverykeys_devpkey.h>
#include <EndpointVolume.h> 
#include <initguid.h>

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)  if(p){p->Release();p=NULL;}
#endif

#ifndef HR_RETURN_NULL
#define HR_RETURN_NULL(hr)  {if(FAILED(hr))return NULL;}
#endif

#ifndef HR_RETURN_0
#define HR_RETURN_0(hr)  {if(FAILED(hr))return 0;}
#endif

#define FAILED_RETURN2(hr, p, str)    if(FAILED(hr) || NULL == p){ WXTcpLog(L"\t[%ws] %ws=%08x",__FUNCTIONW__, str, hr);return;}

#define FAILED_CONTINUE1(hr, str)     if(FAILED(hr)){ WXTcpLog(L"\t[%ws] %ws=%08x",__FUNCTIONW__, str, hr);return;}
#define FAILED_CONTINUE2(hr, p, str)  if(FAILED(hr) || NULL == p){WXTcpLog(L"\t[%ws] %ws=%08x",__FUNCTIONW__, str, hr);return;}

class WXTcpVideoFrame {
public:
	BOOL m_bUsed = FALSE;//队列标记位
	BITMAPINFOHEADER m_bih;

	uint8_t *m_pBuf = nullptr;

	int64_t m_pts = 0;
	int m_iWidth = 0;
	int m_iHeight = 0;

	WXTcpVideoFrame() {}


	virtual ~WXTcpVideoFrame() {
		SAFE_DELETE(m_pBuf);
	}

	//------------------------------------------
	void Init(int width, int height) {
		m_iWidth = width;
		m_iHeight = height;
		m_pBuf = new uint8_t[m_iWidth * 4 * m_iHeight];

		memset(&m_bih, 0, sizeof(BITMAPINFOHEADER));
		m_bih.biSize = sizeof(BITMAPINFOHEADER);
		m_bih.biBitCount = 32;
		m_bih.biWidth = width;
		m_bih.biHeight = -height;
		m_bih.biPlanes = 1;
		m_bih.biCompression = BI_RGB;
		m_bih.biSizeImage = width * height * 4;
	}

	BOOL IsBlack() {
		int64_t sum = m_iWidth * m_iHeight / 2;
		int64_t count = 0;
		for (int64_t i = 0; i < m_iHeight * m_iWidth * 2; i += 4) {
			if (m_pBuf[i] == 0) {
				count++;
			}
		}
		int64_t value = count * 100 / sum;
		if (value > 90) {
			WXTcpLog(L"%ws Is Black Frame", __FUNCTIONW__);
			return TRUE;
		}
		return FALSE;
	}
};

class RgbaData {
public:
	static void CopyRect(const WXTcpVideoFrame& srcFrame, const int PosX, const int PosY, WXTcpVideoFrame &dstFrame) {
		if (srcFrame.m_pBuf == nullptr || dstFrame.m_pBuf == nullptr) {
			return;
		}
		memset(dstFrame.m_pBuf, 0, dstFrame.m_iWidth * dstFrame.m_iHeight * 4);//清0

		int StartY = max(PosY, 0);
		int StopY = min(dstFrame.m_iHeight + PosY, srcFrame.m_iHeight);
		int Height = StopY - StartY;

		int StartX = max(PosX, 0);
		int StopX = min(dstFrame.m_iWidth + PosX, srcFrame.m_iWidth);
		int Width = StopX - StartX;

		libyuv::ARGBCopy(
			srcFrame.m_pBuf + StartY * srcFrame.m_iWidth * 4 + StartX * 4, srcFrame.m_iWidth * 4,
			dstFrame.m_pBuf + (StartY - PosY) * dstFrame.m_iWidth * 4 + (StartX - PosX) * 4, dstFrame.m_iWidth * 4,
			Width, Height);
	}

	//区域叠加，一般用于，鼠标叠加
	static void DrawMouse(WXTcpVideoFrame& MixFrame,
		const int PosX,
		const int PosY,
		const WXTcpVideoFrame &AlphaFrame) {
		if (MixFrame.m_pBuf == nullptr ||
			AlphaFrame.m_pBuf == nullptr) {
			return;
		}

		//计算显示区域
		int StartX = max(0, -PosX);
		int StopX = min(min(AlphaFrame.m_iWidth, AlphaFrame.m_iWidth + PosX), MixFrame.m_iWidth - PosX);
		int StartY = max(0, -PosY);
		int StopY = min(min(AlphaFrame.m_iHeight, AlphaFrame.m_iHeight + PosY), MixFrame.m_iHeight - PosY);
		for (int h = StartY; h < StopY; h++) {
			for (int w = StartX; w < StopX; w++) {
				int posSrc = (h + PosY) * MixFrame.m_iWidth * 4 + (w + PosX) * 4;//输入位置
				int posDst = h * AlphaFrame.m_iWidth * 4 + w * 4;
				if (AlphaFrame.m_pBuf[posDst + 3] != 0) {
					int Alpha = AlphaFrame.m_pBuf[posDst + 3];
					int Alpha2 = 255 - Alpha;

					MixFrame.m_pBuf[posSrc + 0] = (AlphaFrame.m_pBuf[posDst + 0] * Alpha +
						MixFrame.m_pBuf[posSrc + 0] * Alpha2) / 255;
					MixFrame.m_pBuf[posSrc + 1] = (AlphaFrame.m_pBuf[posDst + 1] * Alpha +
						MixFrame.m_pBuf[posSrc + 1] * Alpha2) / 255;
					MixFrame.m_pBuf[posSrc + 2] = (AlphaFrame.m_pBuf[posDst + 2] * Alpha +
						MixFrame.m_pBuf[posSrc + 2] * Alpha2) / 255;
					MixFrame.m_pBuf[posSrc + 3] = 255;
				}
			}
		}
	}
};


#endif
