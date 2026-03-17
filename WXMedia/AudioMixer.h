/*
音频混音器
输入输出都是指定格式PCM16音频数据
*/
#ifndef _AUDIO_MIXER_H_
#define _AUDIO_MIXER_H_

#include <ScreenGrab.hpp>
class AudioMixer {
	int64_t m_iWriteSize = 0;
	std::vector<WXTcpFifo*>m_vecInput;
	WXTcpFifo m_pOutSoundBuffer;//输出缓存
	int m_nSampleRate = 0;
	int m_nChannel = 0;
	int m_iOutFrameSize = 0;//10ms 数据量
	WXTcpDataBuffer m_tmpBufBase;
	WXTcpDataBuffer m_tmpBufAdd;
	std::wstring m_strName = L"AudioMixer";
	bool MixData();
public:
	AudioMixer(const wchar_t* wszName, int nSampleRate, int nChannel);
	virtual ~AudioMixer();
	void AddInput(WXTcpFifo* obj);
	WXTcpFifo* GetOutput();//输出
	void Process();
};

#endif

