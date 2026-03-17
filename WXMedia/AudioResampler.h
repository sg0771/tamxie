/*
音频格式转换类
*/

#ifndef _AUDIO_RESAMPLER_H_
#define _AUDIO_RESAMPLER_H_

#include <ScreenGrab.hpp>

class AudioResampler {
public:
    WXTcpFifo m_outputFifo;//输出缓存 输出为S16数据

    BOOL m_bFloat32 = TRUE;
    int m_iInSampleRate = 48000;//输入采样频率
    int m_iInChannel = 2;//输出声道
    int m_iInSampleCount = 480;//10ms数据量

    int m_iOutSampleRate = 48000;//输入采样频率
    int m_iOutChannel = 2;//输出声道
    int m_iOutSampleCount = 480;//10ms数据量
    int m_iOutSize = 1920;//

    WXTcpDataBuffer m_pDstFrame;

    void* m_pAudioConvert = nullptr;
public:
    AudioResampler() {}
    void Init(int bFloat32, int inSampleRate, int inChannel, int outSampleRate, int outChannel);
    virtual ~AudioResampler();
public:
    void Push(uint8_t* buf, int size);//输入数据
};


#endif
