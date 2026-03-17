/**************************************************************
Vista 以上系统内录电脑声音
edit by TamXie
2019.08.12 增加设备异常后恢复播放后的录制功能
***************************************************************/
#include "WasapiDevice.h"
#include "ScreenCapture.h"


//WASAPI COM 操作错误
int WasapiDevice::OnError(HRESULT hr, const wchar_t* wszMsg) {
	Close();
	WXTcpLog(L"WasapiDevice[%ws]Failed DX_Error[%x]",
		wszMsg,hr);
	return m_bSystem ? _WX_ERROR_SOUND_SYSTEM_OPEN : _WX_ERROR_SOUND_MIC_OPEN;
}

void    WasapiDevice::ThreadPrepare() {
	if(m_bSystem && m_bDefault)
		::SetThreadPriority(::GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);//线程优先级
}

//8889000a 设备被占用
//0x88890004 设备被拔走了或者禁用之后
void    WasapiDevice::ThreadProcess() {
	::Sleep(5);

	if (m_pSoundCapture == nullptr) { //重新配置音频设备！！！
		::Sleep(500);
		BOOL initOK = FALSE;
		HRESULT hr = m_pEnum->GetDevice(m_strGuid.c_str(), &m_pDev); //重新获取设备
		if (SUCCEEDED(hr) && nullptr != m_pDev) {
			hr = m_pDev->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, (void**)&m_pCaptureAudioClient);//获取对象
			if (SUCCEEDED(hr) && nullptr != m_pCaptureAudioClient) {
				hr = m_pCaptureAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED,
					m_bSystem ? AUDCLNT_STREAMFLAGS_LOOPBACK : 0,
					0, 0, m_pwfx, 0);
				if (SUCCEEDED(hr)) {
					hr = m_pCaptureAudioClient->GetService(__uuidof(IAudioCaptureClient), (void**)&m_pSoundCapture);
					if (SUCCEEDED(hr) && nullptr != m_pSoundCapture) {
						hr = m_pCaptureAudioClient->Start(); //设备初始化
						if (SUCCEEDED(hr)) {
							initOK = TRUE;
							WXTcpLog(L"%ws [%ws][%ws]重新初始化成功!!", __FUNCTIONW__, m_strName.c_str(), m_strGuid.c_str());
						}
					}
				}
			}
		}
		if (!initOK) { //重新初始化失败！！！
			m_pSoundCapture = nullptr;
			m_pCaptureAudioClient = nullptr;
			m_pDev = nullptr;
		}
	}

	while (m_pSoundCapture && true) { //不断采集扬声器或者MIC的声音
		UINT32 nNextPacketSize = 0;
		HRESULT hr = m_pSoundCapture->GetNextPacketSize(&nNextPacketSize); //查询声卡上面有多少可用数据
		if (SUCCEEDED(hr)) {
			if (nNextPacketSize > 0) {
				BYTE *pData = nullptr;
				UINT32 nSampleRead = 0;
				DWORD dwFlags = 0;
				hr = m_pSoundCapture->GetBuffer(&pData, &nSampleRead, &dwFlags, nullptr, nullptr);
				if (SUCCEEDED(hr)) {
					if (nSampleRead) {
						m_inputFifo.Write(pData, nSampleRead * m_nBlockAlign);//采集的数据量
					}
					m_pSoundCapture->ReleaseBuffer(nSampleRead);
				}
			}
			else {
				break;//声卡已经没用可用的数据
			}
		}
		else {
			//设备被禁用、拔走、独占后，HR值都是0x88890004 
			//而且在取消禁用、插入、取消独占后无法恢复
			//需要重新配置
			m_pSoundCapture = nullptr;
			m_pCaptureAudioClient = nullptr;
			m_pDev = nullptr;
		}
	}


}

void WasapiDevice::ThreadCapture() {  //数据输出线程
	while (!m_bThreadStop) {
		::Sleep(10);
		int64_t ptsCurr = (::timeGetTime() - m_ptsStart) - m_ptsRead;//当前时间距离采集时间的间隔
		while (ptsCurr > 100) {  //采集100模式后开始输出，相当于做一个100ms的jitter_buffer

			if (m_bFloat32) {//F32数据处理
				m_inputFifo.Read2(m_bufferData.m_pBuf, m_iInSize);//取10ms数据包,有可能缓冲队列无数据
				m_AudioResampler.Push(m_bufferData.m_pBuf, m_iInSize);//输出给转格式
			}else { //S16数据处理
				m_inputFifo.Read2(m_bufferData.m_pBuf, m_iInSize);//取10ms数据包,有可能缓冲队列无数据
				m_AudioResampler.Push(m_bufferData.m_pBuf, m_iInSize);//输出给转格式
			}
			m_ptsRead += 10;//采集数据累加10
			ptsCurr -= 10;
		}
	}
}

WXTcpFifo * WasapiDevice::GetOutput() {  //录屏输出FIFO
	return &m_AudioResampler.m_outputFifo;
}

int   WasapiDevice::Open(int bSystem, const wchar_t* guid, ScreenCapture *capture, int nSampleRate, int nChannel) {
	m_bSystem = bSystem;
	m_strGuid = guid;

	if (m_bSystem) {
		const wchar_t* strGuid = WXTcpWasapiGetDefaultGuid(m_bSystem);
		if (wcsicmp(guid, strGuid) == 0) {
			m_bDefault = TRUE;
		}
	}

	HRESULT hr = ::CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&m_pEnum);
	if (FAILED(hr) || nullptr == m_pEnum) {
		return OnError(hr, L"IMMDevice Enumerator creation");
	}

	hr = m_pEnum->GetDevice(m_strGuid.c_str(), &m_pDev);

	if (FAILED(hr) || nullptr == m_pDev) {
		return OnError(hr, L"IMMDevice Device creation");
	}

	//获取设备名字
	CComPtr<IPropertyStore>pProps = nullptr;
	PROPVARIANT varName;
	hr = m_pDev->OpenPropertyStore(STGM_READ, &pProps);
	if (FAILED(hr) || nullptr == pProps ) {
		return OnError(hr, L"IMMDevice Device OpenPropertyStore");
	}
	PropVariantInit(&varName);
	hr = pProps->GetValue(PKEY_Device_FriendlyName, &varName);
	m_strName = varName.bstrVal;
	PropVariantClear(&varName);

	hr = m_pDev->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, (void**)&m_pCaptureAudioClient);
	if (FAILED(hr) || nullptr == m_pCaptureAudioClient) {
		return OnError(hr, L"m_pDev->Activate");
	}


	//2019.08.28
	//扬声器手动处理,先用PCM处理
	BOOL  bInitialize = FALSE;
	if (!m_bSystem) {
		hr = m_pCaptureAudioClient->GetMixFormat(&m_pwfx);//默认的支持格式
		m_bFloat32 = 0;
		m_pwfx->nBlockAlign    = m_pwfx->nChannels * sizeof(int16_t);
		m_pwfx->wBitsPerSample = 8 * sizeof(int16_t);
		m_pwfx->nAvgBytesPerSec = m_pwfx->nSamplesPerSec * m_pwfx->nBlockAlign;

		if (m_pwfx->wFormatTag == WAVE_FORMAT_EXTENSIBLE) {
			PWAVEFORMATEXTENSIBLE pEx = reinterpret_cast<PWAVEFORMATEXTENSIBLE>(m_pwfx);
			pEx->SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
			pEx->Samples.wValidBitsPerSample = 16;
		}else {
			m_pwfx->wFormatTag = WAVE_FORMAT_PCM;
		}

		hr = m_pCaptureAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED,
			m_bSystem ? AUDCLNT_STREAMFLAGS_LOOPBACK : 0,
			0, 0, m_pwfx, 0);
		if (SUCCEEDED(hr)) {
			bInitialize = TRUE;
		}else {
			if (m_pwfx) {
				CoTaskMemFree(m_pwfx);
				m_pwfx = nullptr;
			}
		}
	}

	if (!bInitialize) { //扬声器使用默认音频数据格式
		hr = m_pCaptureAudioClient->GetMixFormat(&m_pwfx);//默认的支持格式
		if (m_pwfx->wFormatTag == WAVE_FORMAT_PCM) {
			m_bFloat32 = 0;
		}if (m_pwfx->wFormatTag == WAVE_FORMAT_IEEE_FLOAT) {
			m_bFloat32 = 1;
		}else if (m_pwfx->wFormatTag == WAVE_FORMAT_EXTENSIBLE) {
			PWAVEFORMATEXTENSIBLE pEx = reinterpret_cast<PWAVEFORMATEXTENSIBLE>(m_pwfx);
			if (IsEqualGUID(KSDATAFORMAT_SUBTYPE_PCM, pEx->SubFormat)) {
				m_bFloat32 = 0; // PCM 16 格式
			}else if (IsEqualGUID(KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, pEx->SubFormat)) {
				m_bFloat32 = 1; //Float32 格式
			}else { //都不支持！！
				return OnError(hr, L"CheckTypeDefault ERROR!!");
			}
		}else {  //或者指定格式失败
			return OnError(hr, L"CheckTypeDefault ERROR!!!");
		}

		hr = m_pCaptureAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED,
			m_bSystem ? AUDCLNT_STREAMFLAGS_LOOPBACK : 0,
			0, 0, m_pwfx, 0);

		if (FAILED(hr)) {
			return OnError(hr, L"m_pCaptureAudioClient->Initializ 格式设置失败 ");
		}
	}

	//格式设置成功
	m_pSoundCapture = nullptr;
	hr = m_pCaptureAudioClient->GetService(__uuidof(IAudioCaptureClient), (void**)&m_pSoundCapture);
	if (FAILED(hr)) {
		return OnError(hr, L"m_pCaptureAudioClient->GetService 设备初始化失败");
	}

	//设备初始化成功
	m_nSampleRate = m_pwfx->nSamplesPerSec;
	m_nChannel = m_pwfx->nChannels;
	m_nBlockAlign = m_pwfx->nBlockAlign;

	hr =  m_pCaptureAudioClient->Start(); //设备初始化
	if(SUCCEEDED(hr)) {
		WXTcpLog(L"\r\n\tIMMDevice Open\r\n\tGUID=[%ws] \r\n\tname=[%ws]\r\n\t System=[%d]\r\n\tSamplerate=%d Channel=%d bFloat=%d",
			m_strGuid.c_str(),
			m_strName.c_str(),
			m_bSystem,
			m_nSampleRate,
			m_nChannel,
			m_bFloat32);
		m_dataBuffer.Init(NULL, m_nSampleRate);//补包缓冲区

		m_pCapture = capture;
		m_AudioResampler.Init(m_bFloat32, m_nSampleRate, m_nChannel,
			nSampleRate, nChannel);
		m_iInSize = m_nSampleRate * m_nChannel * 2 / 100;
		if (m_bFloat32)m_iInSize *= 2;
		m_bufferData.Init(nullptr, m_iInSize);
		m_bufferS16Data.Init(nullptr, m_iInSize);

		m_ptsStart = ::timeGetTime();//音频开始采集的时间


		ThreadStart();//原始数据采集线程
		m_threadCapture = new std::thread(&WasapiDevice::ThreadCapture, this);//编码数据输出线程
		return _WX_ERROR_SUCCESS;
	}else {
		return OnError(hr, L"m_pCaptureAudioClient->Start()");
	}
}

void  WasapiDevice::Close() {
	ThreadStop();//原始数据采集线程
	if (m_threadCapture) {//编码数据输出线程
		m_threadCapture->join();
		delete m_threadCapture;
		m_threadCapture = nullptr;
	}

	if (m_pCaptureAudioClient) {
		WXTcpLog(L"WasapiDeviceImpl [%ws] Stop At [%lld]", m_strName.c_str(),  ::timeGetTime());
		m_pCaptureAudioClient->Stop();
		m_pEnum = nullptr;
		m_pDev = nullptr;
		m_pCaptureAudioClient = nullptr;
		m_pSoundCapture = nullptr;
		if (m_pwfx) {
			CoTaskMemFree(m_pwfx);
			m_pwfx = nullptr;
		}
	}
	m_pCapture = nullptr;
}

