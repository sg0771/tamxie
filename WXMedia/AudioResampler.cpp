/*
音频格式转换类
*/

#include "AudioResampler.h"
#include <WXResampleapi.h>


void AudioResampler::Init(int bFloat32, int inSampleRate, int inChannel, int outSampleRate, int outChannel) {
	m_bFloat32 = bFloat32;
	m_iInSampleRate = inSampleRate;
	m_iInChannel = inChannel;
	m_iInSampleCount = m_iInSampleRate / 100; //10ms

	m_iOutSampleRate = outSampleRate;
	m_iOutChannel = outChannel;
	m_iOutSampleCount = m_iOutSampleRate / 100;
	m_iOutSize = m_iOutSampleCount * m_iOutChannel * 2;

	m_pDstFrame.Init(nullptr, m_iOutSize);//输出缓存 

	if (m_iInSampleRate != m_iOutSampleRate ||
		m_iInChannel != m_iOutChannel ||
		m_bFloat32) {
		m_pAudioConvert = WXResampleCreate(FALSE, m_iOutChannel, outSampleRate, m_bFloat32, m_iInChannel, inSampleRate);
	}
}

AudioResampler::~AudioResampler() {
	if (m_pAudioConvert) { WXResampleDestroy(&m_pAudioConvert); m_pAudioConvert = nullptr; }
}

void AudioResampler::Push(uint8_t* buf, int size) {
	uint8_t* pOutData[4] = { m_pDstFrame.m_pBuf };
	uint8_t* pInData[4] = { buf };
	if (m_pAudioConvert) {
		WXResampleConvert(m_pAudioConvert, pOutData, m_iOutSampleCount, (const uint8_t**)pInData, m_iInSampleCount);//直接转换为输出格式 ！！！
	}
	else {
		memcpy(m_pDstFrame.m_pBuf, buf, m_iOutSize);
	}

	if (m_outputFifo.Size() <= m_iOutSize * 1000) { //输出到缓存队列
		m_outputFifo.Write(m_pDstFrame.m_pBuf, m_iOutSize);//如果缓存队列的数据长度超过10s，就不保存，避免内存溢出 2019.01.02
	}
}
