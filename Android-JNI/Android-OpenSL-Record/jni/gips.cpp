#include "gips.h"

void KK_SoundProcessing::Open(int iSampleRate /* = 8000 */, int iSampleCount /* = 800 */)
{
	m_iSampleRate = iSampleRate;
	m_iSampleCount = iSampleCount;
	WebRtcAec_Create(&m_aecInst);
	WebRtcAec_Init(m_aecInst, m_iSampleRate, m_iSampleRate);
	WebRtcAgc_Create(&m_agcInst);
	WebRtcAgc_Init(m_agcInst,0 , 255, 1, m_iSampleCount);
	//WebRtcAgc_config_t agcConfig = {3, 9, 1};//7 0 1
	WebRtcAgc_config_t agcConfig = {7,0,1};//7 0 1
	WebRtcAgc_set_config(m_agcInst, agcConfig);
}

void KK_SoundProcessing::Close()
{
	if(m_aecInst)
	{
		WebRtcAec_Free(m_aecInst);
		m_aecInst = NULL;
		WebRtcAgc_Free(m_agcInst);
		m_agcInst = NULL;
	}
}

//??????????
//????????
//??????
void KK_SoundProcessing::Do(short *Capture, short *Play, int delay/* = 100*/)
{
	for (int i = 0; i < m_iSampleCount / 80; i++)
	{
		short *pcm = Capture + i * 80;
		short *ck  = Play + i * 80;
		WebRtcAec_BufferFarend(m_aecInst, ck, 80);//AEC²Î¿¼ÉùÒô
		WebRtcAec_Process(m_aecInst,pcm, NULL,pcm, NULL, 80,delay,0);
#if 1
		WebRtcAgc_AddFarend(m_agcInst, ck, 80);//AGC ²Î¿¼ÉùÒô
		int outLevel = 0;
		unsigned char saturation_warning = 0; 
		WebRtcAgc_Process(m_agcInst, pcm, NULL, 80, pcm, NULL, 
			m_iLevel, &outLevel, 0, &saturation_warning);
		m_iLevel = outLevel;
#endif
	}
}

