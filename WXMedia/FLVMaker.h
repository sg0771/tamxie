/*
FLV
*/
#ifndef _FlvMaker_H_
#define _FlvMaker_H_

#include <ScreenGrab.hpp>

#include "X264Encoder.h"
#define H264Encoder X264Encoder

EXTERN_C void* FaacEncoderOpen(int sample_rate /*= 44100*/, int channels /*= 2*/, int bitrate /*= 96*/);
EXTERN_C int  FaacEncoderEncode(void* ptr, uint8_t* pBuf, uint8_t** pOut, int* iOutSize);
EXTERN_C void FaacEncoderClose(void* ptr);

EXTERN_C void* TSMuxerCreate();
EXTERN_C void TSMuxerDestroy(void* ptr);
EXTERN_C void TSMuxerHandleVideo(void* ptr, uint8_t* vidoebuf, uint32_t bufsize, int64_t tic, uint8_t** pOut, int* nOutSize);
EXTERN_C void TSMuxerHandleAudio(void* ptr, uint8_t* aacbuf, uint32_t bufsize, int64_t tic, uint8_t** pOut, int* nOutSize);


class FlvMaker{
	WXTcpLocker m_mutex;
	std::ofstream m_pfOut;
	H264Encoder m_h264;
	void* m_aac = nullptr;
	int m_iWidth = 0;
	int m_iHeight = 0;
	int m_iForceHeight = 0;// 强制720P 1080P 和 原始分辨率输出
	int64_t m_nAudio = 0;
	void* m_pTS = nullptr;
public:
	void WriteData(const void* data, int data_size) {
		WXTcpAutoLock al(m_mutex);
		m_pfOut.write((const char*)data, data_size);
	}

	void Close() {
		WXTcpAutoLock al(m_mutex);
		if (m_pfOut.is_open()) {
			m_h264.Close();
			m_pfOut.close();
			if (m_aac) {
				FaacEncoderClose(m_aac);
				m_aac = nullptr;
			}
		}
		if (m_pTS) {
			TSMuxerDestroy(m_pTS);
			m_pTS = nullptr;
		}
	}

	int  Open(const wchar_t* wszName, int nPort, int width, int height, int forceHeight) { //创建FLV文件头
		WXTcpAutoLock al(m_mutex);

		m_pTS = TSMuxerCreate();
		m_iWidth = width;
		m_iHeight = height;
		m_iForceHeight = forceHeight;

		m_pfOut.open(wszName, std::ios::binary);
		if (m_pfOut.is_open()) {

			if (m_iWidth && m_iHeight) {
				m_h264.OpenH264(m_iForceHeight, m_iWidth, m_iHeight, 25, 23, TRUE);
			}

			m_aac = FaacEncoderOpen(48000,2,96);
			return _WX_ERROR_SUCCESS;
		}
		return _WX_ERROR_ERROR;
	}

	void WriteAudioFrame(uint8_t* buf) {
		uint8_t* pOutAAC = nullptr;
		int iOutSizeAAC = 0;
		int ret = FaacEncoderEncode(m_aac, buf, &pOutAAC, &iOutSizeAAC);
		if (iOutSizeAAC > 0) {
			int ptsAudio = m_nAudio * 1024 * 1000 / 48000 * 90;
			m_nAudio++;
			uint8_t* pOut = nullptr;
			int iOutSize = 0;
			TSMuxerHandleAudio(m_pTS, pOutAAC, iOutSizeAAC, ptsAudio, &pOut, &iOutSize);
			if (iOutSize) {
				WriteData(pOut, iOutSize);
			}
		}
	}

	void WriteVideoFrame(WXTcpVideoFrame* frame) {
		uint8_t* pOutH264 = nullptr;
		int iOutSizeH264 = 0;
		if (m_h264.Encode(frame, pOutH264, iOutSizeH264)) {
			int64_t ptsVideo = frame->m_pts * 90;
			uint8_t* pOut = nullptr;
			int iOutSize = 0;
			TSMuxerHandleVideo(m_pTS, pOutH264, iOutSizeH264, ptsVideo, &pOut, &iOutSize);
			if (iOutSize) {
				WriteData(pOut, iOutSize);
			}
		}
	}
};

#endif
