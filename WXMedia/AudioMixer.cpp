/*
音频混音器
by TamXie
*/

#include  "AudioMixer.h"

static inline int16_t AddS16(int16_t a, int16_t b) {
	int sum = a + b;
	if (sum > 32767)return 32767;
	if (sum < -32767)return -32767;
	return sum;
}

static inline void AddAudio(int16_t* dst, int16_t* src, int count) { //可以用MMX优化
	for (int i = 0; i < count; i++) {
		dst[i] = AddS16(dst[i], src[i]);
	}
}

//2019.01.02 修改混音逻辑，只有当两路声音都有数据时才进行混音操作
//原来的逻辑中，如果扬声器有声音就混音，可能会导致MIC通道声音不连续，造成吱吱等杂音
bool AudioMixer::MixData() {
	int nCount = (int)m_vecInput.size();
	if (nCount == 0)
		return false;

	bool hasData = true;
	if (nCount > 1) { //多路输出
		for (int i = 0; i < nCount; i++) {
			if (m_vecInput[i]->Size() < m_iOutFrameSize) {
				hasData = false;
				break;
			}
		}
		if (hasData) { //输出
			memset(m_tmpBufBase.m_pBuf, 0, m_iOutFrameSize);
			for (int i = 0; i < nCount; i++) {//混音叠加
				m_vecInput[i]->Read(m_tmpBufAdd.m_pBuf, m_iOutFrameSize);
				AddAudio((int16_t*)m_tmpBufBase.m_pBuf, (int16_t*)m_tmpBufAdd.m_pBuf, m_iOutFrameSize / 2);
			}
		}
	}
	else { //单路数据
		if (m_vecInput[0]->Size() >= m_iOutFrameSize) {
			hasData = true;
			m_vecInput[0]->Read(m_tmpBufBase.m_pBuf, m_iOutFrameSize);
		}
		else {
			hasData = false;
		}
	}

	if (hasData) {
		m_iWriteSize += m_iOutFrameSize;
		m_pOutSoundBuffer.Write(m_tmpBufBase.m_pBuf, m_iOutFrameSize);//写数据
	}
	return hasData;
}

AudioMixer::AudioMixer(const wchar_t* wszName, int nSampleRate, int nChannel) {
	m_strName = wszName;
	m_iOutFrameSize = nSampleRate * 2 * nChannel / 100;//10ms 数据量有多少字节
	m_nSampleRate = nSampleRate;
	m_nChannel = nChannel;
	m_tmpBufBase.Init(nullptr, m_iOutFrameSize);
	m_tmpBufAdd.Init(nullptr, m_iOutFrameSize);
}

AudioMixer::~AudioMixer() {
	WXTcpLog(L"%ws  write Time=%0.2fs", m_strName.c_str(),
		(double)m_iWriteSize / m_nSampleRate / 2 / m_nChannel);
}

void AudioMixer::Process() {

	while (TRUE) { //取混音数据
		bool ret = MixData();
		if (ret == false)break;
	}
}

void AudioMixer::AddInput(WXTcpFifo* obj) {
	m_vecInput.push_back(obj);
}

WXTcpFifo* AudioMixer::GetOutput() {
	return  &m_pOutSoundBuffer;
}



