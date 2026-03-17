#ifndef GIPS_H
#define GIPS_H

#include "webrtc_aec.h"
#include "webrtc_agc.h"
#include "webrtc_ns.h"

#ifndef NULL
#define  NULL 0
#endif

class KK_SoundProcessing 
{
private:
	int m_iSampleRate;
	int m_iSampleCount;

	void *m_aecInst;
	void *m_agcInst;

	int m_iLevel;
public:
	KK_SoundProcessing()
	{
		m_iSampleRate = 8000;
		m_iSampleCount = 800;

		m_aecInst = NULL;
		m_agcInst = NULL;

		m_iLevel = 200;//AGC
	}
	~KK_SoundProcessing()
	{
		Close();
	}
public:
	void Open(int iSampleRate = 8000, int iSampleCount = 800);
	void Close();
	void Do(short *Capture, short *Play, int delay = 100);
};

#endif
