#ifndef _ScreenCaptureAudio_H_
#define _ScreenCaptureAudio_H_

#include "ScreenCapture.h"

//音频处理部分
class  ScreenCaptureAudio :public WXTcpThread {
public:

	WXScreenGrabParam* m_config = nullptr;
	ScreenCapture* m_pCapture = nullptr;
	FlvMaker* m_pMuxer = nullptr;

	WXTcpDataBuffer m_dataFrame;

	std::vector<WasapiDevice*>m_vecDevice;

	AudioMixer* m_pSystemAudioMixer = nullptr;//扬声器混音器,带音频处理
	AudioMixer* m_pMicAudioMixer = nullptr;//MIC混音器

	AudioMixer* m_pAudioMixer = nullptr;//最后的混音器
	int m_iOutFrameSize = 4096;

	void AddSystemSoundDevice(const wchar_t* wszGuid) {
		std::wstring strGuid = wszGuid;
		WasapiDevice* dev = WasapiDeviceGetInstance(TRUE, strGuid.c_str(),
			this->m_pCapture, m_config->nAudioSampleRate, m_config->nAudioChannel);
		if (dev != nullptr) {
			WXTcpLog(L"Add System Device OK[%ws][%ws]", dev->Name(), dev->Guid());
			m_vecDevice.push_back(dev);
			if (nullptr == m_pSystemAudioMixer) {
				m_pSystemAudioMixer = new AudioMixer(L"SystemAudioMixer",
					m_config->nAudioSampleRate, m_config->nAudioChannel);//扬声器音频混音处理器
				if (nullptr == m_pAudioMixer) {
					m_pAudioMixer = new AudioMixer(L"AllAudioMixer",
						m_config->nAudioSampleRate, m_config->nAudioChannel);//音频混音器
				}
				m_pAudioMixer->AddInput(m_pSystemAudioMixer->GetOutput());
			}
			m_pSystemAudioMixer->AddInput(dev->GetOutput());
		}
	}

	void AddMicSoundDevice(const wchar_t* wszGuid) {
		std::wstring strGuid = wszGuid;
		WasapiDevice* dev = WasapiDeviceGetInstance(FALSE, strGuid.c_str(),
			this->m_pCapture, m_config->nAudioSampleRate, m_config->nAudioChannel);
		if (dev != nullptr) {
			WXTcpLog(L"Add MIC Device OK[%ws][%ws]", dev->Name(), dev->Guid());
			m_vecDevice.push_back(dev);
			if (nullptr == m_pMicAudioMixer) {
				m_pMicAudioMixer = new AudioMixer(L"MicAudioMixer",
					m_config->nAudioSampleRate, m_config->nAudioChannel);//扬声器音频混音处理器
				if (nullptr == m_pAudioMixer) {
					m_pAudioMixer = new AudioMixer(L"AllAudioMixer",
						m_config->nAudioSampleRate, m_config->nAudioChannel);//音频混音器
				}
				m_pAudioMixer->AddInput(m_pMicAudioMixer->GetOutput());
			}
			m_pMicAudioMixer->AddInput(dev->GetOutput());
		}
	}
	int64_t m_ptsStart = 0;
public:

	int Config(ScreenCapture* pCapture) {
		m_pCapture = pCapture;
		m_config = &m_pCapture->m_param;
		m_iOutFrameSize = 4096;

		AudioDeviceResetDefault();//重新获得默认设备名字

		if (wcsicmp(m_config->m_micName, L"nullptr") != 0) {
			if (wcsicmp(m_config->m_micName, L"all") == 0) { //所有的扬声器设备
				const wchar_t* strGuid1 = WXTcpWasapiGetDefaultGuid(FALSE);
				if (wcsicmp(strGuid1, L"nullptr") != 0) {
					AddMicSoundDevice(strGuid1);
				}

				const wchar_t* strGuid2 = WXTcpWasapiGetDefaultCommGuid(FALSE);
				if (wcsicmp(strGuid2, strGuid1) != 0 && wcsicmp(strGuid2, L"nullptr") != 0) {
					AddMicSoundDevice(strGuid2);
				}
			}
			else if (wcsicmp(m_config->m_micName, L"default") == 0) { //所有的扬声器设备
				const wchar_t* strGuid = WXTcpWasapiGetDefaultGuid(FALSE);
				AddMicSoundDevice(strGuid);
			}
			else { //指定设备
				AddMicSoundDevice(m_config->m_micName);
			}
		}

		if (wcsicmp(m_config->m_systemName, L"nullptr") != 0) {
			if (wcsicmp(m_config->m_systemName, L"all") == 0) {//所有的麦克风设备
				const wchar_t* strGuid1 = WXTcpWasapiGetDefaultGuid(TRUE);
				if (wcsicmp(strGuid1, L"nullptr") != 0)
					AddSystemSoundDevice(strGuid1);

				const wchar_t* strGuid2 = WXTcpWasapiGetDefaultCommGuid(TRUE);
				if (wcsicmp(strGuid2, strGuid1) != 0 && wcsicmp(strGuid2, L"nullptr") != 0) {
					AddSystemSoundDevice(strGuid2);
				}
			}
			else if (wcsicmp(m_config->m_systemName, L"default") == 0) {//所有的麦克风设备
				const wchar_t* strGuid = WXTcpWasapiGetDefaultGuid(TRUE);
				AddSystemSoundDevice(strGuid);
			}
			else {
				AddSystemSoundDevice(m_config->m_systemName);
			}
		}

		if (m_pAudioMixer) {
			return _WX_ERROR_SUCCESS;
		}
		else {
			return _WX_ERROR_ERROR;
		}
	}


	void Start() { //启动
		if (m_bThreadStop) {
			m_ptsStart = ::timeGetTime();
			ThreadStart();
		}
	}

	void Stop() { //结束
		if (!m_bThreadStop) {
			ThreadStop();

			int64_t ptsDuration = ::timeGetTime() - m_ptsStart;
			WXTcpLog(L"%ws TimeDuration=%lld", __FUNCTIONW__, ptsDuration);

			SAFE_DELETE(m_pAudioMixer);
			SAFE_DELETE(m_pSystemAudioMixer);
			SAFE_DELETE(m_pMicAudioMixer);

			for (int i = 0; i < m_vecDevice.size(); i++) {
				WasapiDeviceReleaseInstance(m_vecDevice[i]);
			}
		}
	}

public:
	//音频线程函数
	virtual void  ThreadPrepare() {
		m_dataFrame.Init(nullptr, m_iOutFrameSize);
	}

	virtual void  ThreadProcess() {
		if (m_pSystemAudioMixer) {
			m_pSystemAudioMixer->Process();
		}
		if (m_pMicAudioMixer) {
			m_pMicAudioMixer->Process();
		}
		m_pAudioMixer->Process();
		WXTcpFifo* audioBuffer = m_pAudioMixer->GetOutput();
		while (audioBuffer->Size() >= m_iOutFrameSize) {
			audioBuffer->Read(m_dataFrame.m_pBuf, m_iOutFrameSize);
			if (!m_pCapture->m_bPause) {
				m_pMuxer->WriteAudioFrame(m_dataFrame.m_pBuf);
			}
		}
		::Sleep(10);//音频编码
	}

	virtual void  ThreadPost() {

		if (m_pSystemAudioMixer) {
			m_pSystemAudioMixer->Process();
		}

		if (m_pMicAudioMixer) {
			m_pMicAudioMixer->Process();
		}

		m_pAudioMixer->Process();
		WXTcpFifo* audioBuffer = m_pAudioMixer->GetOutput();

		while (audioBuffer->Size() >= m_iOutFrameSize) {
			audioBuffer->Read(m_dataFrame.m_pBuf, m_iOutFrameSize);
			if (!m_pCapture->m_bPause) {
				m_pMuxer->WriteAudioFrame(m_dataFrame.m_pBuf);
			}
		}
	}
};

#endif
