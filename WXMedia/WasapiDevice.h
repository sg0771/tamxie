/**************************************************************
基于WASAPI的音频设备采集播放处理

使用静音数据流虽然可以使得扬声器声音比较连续，但是在某些时候会导致系统音量变小

2019.07.27 改为使用双线程来处理音频数据
***************************************************************/

#ifndef __WX_WASAPI_DEVICE_H_
#define __WX_WASAPI_DEVICE_H_

#include <ScreenGrab.hpp>
#include "AudioResampler.h"


class ScreenCapture;
class WasapiDevice:  public WXTcpThread {
	BOOL m_bDefault = FALSE;//是否默认设备
	BOOL m_bSystem = FALSE;  //是否扬声器
	WXTcpDataBuffer m_dataBuffer;
	int64_t m_ptsStart = 0;//音频开始采集的时间
	std::wstring m_strName;//设备名字
	AudioResampler m_AudioResampler;

	std::wstring m_strGuid;
	CComPtr<IMMDeviceEnumerator>m_pEnum = nullptr;
	CComPtr<IMMDevice>m_pDev = nullptr;
	
	//声音采集
	CComPtr<IAudioClient>m_pCaptureAudioClient = nullptr;
	CComPtr<IAudioCaptureClient>m_pSoundCapture = nullptr;

	//输出4声道时的处理
	BOOL  m_bFloat32 = FALSE;//输出格式是否FLOAT32数据
	int  m_nSampleRate = 0;
	int  m_nChannel = 0;
	int  m_nBlockAlign = 0;
	int64_t m_ptsRead = 0;//已经读取数据对应的时间长度，10ms一个包累加
	WAVEFORMATEX *m_pwfx = nullptr; //需要释放
	int      OnError(HRESULT hr,const wchar_t* wszMsg);

	ScreenCapture *m_pCapture = nullptr;//输出管道
	std::thread *m_threadCapture = nullptr;
	void ThreadCapture();
	WXTcpFifo m_inputFifo;
	WXTcpDataBuffer m_bufferData;
	WXTcpDataBuffer m_bufferS16Data;
	int m_iInSize = 3840;//
public:
	virtual  void     ThreadPrepare();
	virtual  void     ThreadProcess();
public:
	int      Open(int bSystem, const wchar_t* guid, ScreenCapture *capture, int nSampleRate, int nChannel);
	void     Close();
	WXTcpFifo * GetOutput();
	const wchar_t*  Name() { return m_strName.c_str(); }
	const wchar_t*  Guid() { return m_strGuid.c_str(); }
};

//获取一个设备指针
WasapiDevice *WasapiDeviceGetInstance(int bSystem, const wchar_t* strID, ScreenCapture *capture = nullptr, int nSampleRate = 44100, int nChannel = 2);

//释放设备指针，如果这个设备是当前正在监听的设备，则不删除，否则需要删除设备
void          WasapiDeviceReleaseInstance(WasapiDevice *dev);

#endif 