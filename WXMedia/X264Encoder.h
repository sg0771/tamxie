/*
X264 视频编码器
*/

#ifndef _AVC_ENCODER_H_
#define _AVC_ENCODER_H_

#include <ScreenGrab.hpp>

extern "C" {
#include "x264.h"
};

class X264Encoder {
public:
	WXTcpLocker m_mutex;
	x264_t* m_h = nullptr;
	x264_param_t m_param;

	WXTcpVideoFrame m_srcFrame;//缩放后的RGBA
	int m_iSrcWidth = 0;
	int m_iSrcHeight = 0;

	x264_picture_t m_pic;
	int m_iWidth = 0;
	int m_iHeight = 0;

	uint8_t m_extradata[200];
	int m_extradata_size = 0;

	int    m_nOutSize = 0;
	uint8_t* m_pOut = nullptr;

	BOOL m_bFLV = FALSE;
public:
	BOOL OpenH264(int ForceHeight, int nWidth, int nHeight, int nFps, int iQP, bool bFLV);
	BOOL Encode(WXTcpVideoFrame* frame, unsigned char*& pOut, int& outlen);
	void Close();
};

#endif