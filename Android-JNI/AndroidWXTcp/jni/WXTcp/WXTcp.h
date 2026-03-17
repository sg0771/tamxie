#ifndef  WXTCP_H
#define WXTCP_H

#include <jni.h>

#include <stdio.h>
#include <stdlib.h>  
#include <string.h>
#include <unistd.h>
#include <android/log.h>  
#include <sys/socket.h>  
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <linux/tcp.h>
#include <mutex>
#include <thread>
#include <queue>
#include <map>
#include "WXBase.h"

#define   LOG_TAG  "TamTcp"
#define   LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)  



//音视频数据包结构
//4个字节， 第一个字节表示数据类型
//2-4 字节表示数据长度，网络顺序
//主要类型
#define TYPE_VIDEO_H264 0x00
#define TYPE_VIDEO_H265 0x01
#define TYPE_VIDEO_VP8  0x02  //声网直播？

//视频参数
#define TYPE_VIDEO_WIDTH   0x10  
#define TYPE_VIDEO_HEIGHT  0x11  
#define TYPE_VIDEO_FPS     0x12  


//音频参数
#define TYPE_AUDIO_SAMPLERATE   0x70
#define TYPE_AUDIO_CHANNEL      0x71

#define TYPE_AUDIO_PCM   0x80
#define TYPE_AUDIO_AAC   0x81
#define TYPE_AUDIO_OPUS  0x82 //声网音频格式


#include <opus.h>

//多路TCP接收处理

class WXOpusDecoder {
	WXLocker m_mutex;
	OpusDecoder *m_pDecoder = nullptr;
	WXDataBuffer m_buf;
	int m_nFrameSize = 0;
	int m_nDataSize = 0;
	int m_nSampleRate = 0;
	int m_nChannel = 0;
public:
	int Open(int nSampleRate, int nChannel) {
		WXAutoLock al(m_mutex);
		if (nullptr != m_pDecoder || m_nSampleRate != nSampleRate || m_nChannel != nChannel) {
			if (m_pDecoder) {
				opus_decoder_destroy(m_pDecoder);
				m_pDecoder = nullptr;
			}
			int error = 0;
			m_pDecoder = opus_decoder_create(nSampleRate, nChannel, &error);
			int value = 0;
			int ret = opus_decoder_ctl(m_pDecoder, OPUS_GET_GAIN(&value));
			if (ret >= 0) {
				m_nDataSize = nSampleRate * nChannel * 2 / 100;//10ms 数据量
				m_nFrameSize = nSampleRate / 100;
				m_buf.Init(nullptr, m_nDataSize);
				return 1;
			}
			else {
				opus_decoder_destroy(m_pDecoder);
				m_pDecoder = nullptr;
			}
		}
		return 0;
	}

	void Close() {
		WXAutoLock al(m_mutex);
		if (m_pDecoder) {
			opus_decoder_destroy(m_pDecoder);
			m_pDecoder = nullptr;
		}
	}


	int  DecodeFrame(uint8_t *encbuf, int encsize, uint8_t** decpcm, int* nOutSize) {
		WXAutoLock al(m_mutex);
		*decpcm = nullptr;
		*nOutSize = 0;
		if (m_pDecoder) {
			int ret = opus_decode(m_pDecoder, encbuf, encsize, (int16_t*)m_buf.m_pBuf, m_nFrameSize, 0);
			if (ret <= 0) {
				return 0;
			}
			*decpcm = (uint8_t*)m_buf.m_pBuf;
			*nOutSize = m_buf.m_iBufSize;
			return 1;
		}
		return 0;
	}
};

#endif