
//------------------------------------------------------------------------------------------------
/*
WASAPI 设备管理
*/
#include <ScreenGrab.hpp>
#include "WasapiDevice.h"
#include "ScreenCapture.h"

class WasapiDeviceManager {
public:
    WXTcpLocker m_mutex;

    //扬声器-默认设备
    std::wstring m_strDefaultSystemGuid = L"nullptr";
    std::wstring m_strDefaultSystemName = L"nullptr";

    //扬声器-默认通信设备
    std::wstring m_strDefaultCommSystemGuid = L"nullptr";
    std::wstring m_strDefaultCommSystemName = L"nullptr";

    //麦克风-默认设备
    std::wstring m_strDefaultMicGuid = L"nullptr";
    std::wstring m_strDefaultMicName = L"nullptr";

    //麦克风-默认通信设备
    std::wstring m_strDefaultCommMicGuid = L"nullptr";
    std::wstring m_strDefaultCommMicName = L"nullptr";

    std::vector<AudioDevInfo> m_vecSoundRenderInfo;
    std::vector<AudioDevInfo> m_vecSoundCaptureInfo;

    //正在监听的设备
    WasapiDevice* m_pListenSystemDevice = nullptr;//当前使用的扬声器设备
    WasapiDevice* m_pListenMicDevice = nullptr;//当前使用的MIC设备

    //获取默认设备的GUID值和名字
    void GetDefaultName(BOOL system) {
        if (system) {
            m_strDefaultSystemGuid = L"nullptr";
            m_strDefaultSystemName = L"nullptr";
        }
        else {
            m_strDefaultMicGuid = L"nullptr";
            m_strDefaultMicName = L"nullptr";
        }
        CComPtr<IMMDeviceEnumerator>pEnumerator = nullptr;//设备枚举器
        HRESULT hr = pEnumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator));
        if (SUCCEEDED(hr)) {
            CComPtr<IMMDevice>pEndpoint = nullptr;
            hr = pEnumerator->GetDefaultAudioEndpoint(system ? eRender : eCapture, eConsole, &pEndpoint);
            if (SUCCEEDED(hr)) {
                LPWSTR pwszID = nullptr;
                hr = pEndpoint->GetId(&pwszID);
                if (SUCCEEDED(hr)) {
                    CComPtr<IPropertyStore>pProps = nullptr;//属性管理器
                    hr = pEndpoint->OpenPropertyStore(STGM_READ, &pProps);
                    PROPVARIANT varName;//设备显示名字，有可能重复
                    PropVariantInit(&varName);
                    hr = pProps->GetValue(PKEY_Device_FriendlyName, &varName);
                    if (system) {
                        m_strDefaultSystemGuid = pwszID;
                        m_strDefaultSystemName = varName.bstrVal;
                        WXTcpLog(L"WASAPI System Default[%ws][%ws]",
                            m_strDefaultSystemName.c_str(), m_strDefaultSystemGuid.c_str());
                    }
                    else {
                        m_strDefaultMicGuid = pwszID;
                        m_strDefaultMicName = varName.bstrVal;
                        WXTcpLog(L"WASAPI Mic Default[%s][%s]",
                            m_strDefaultMicGuid.c_str(), m_strDefaultMicName.c_str());
                    }
                    CoTaskMemFree(pwszID);
                    PropVariantClear(&varName);
                }
            }
        }
    }

    //获取默认通信设备的GUID值和名字
    void GetDefaultCommName(BOOL system) {
        if (system) {
            m_strDefaultCommSystemGuid = L"nullptr";
            m_strDefaultCommSystemName = L"nullptr";
        }
        else {
            m_strDefaultCommMicGuid = L"nullptr";
            m_strDefaultCommMicName = L"nullptr";
        }
        CComPtr<IMMDeviceEnumerator>pEnumerator = nullptr;//设备枚举器
        HRESULT hr = pEnumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator));
        if (SUCCEEDED(hr)) {
            CComPtr<IMMDevice>pEndpoint = nullptr;
            hr = pEnumerator->GetDefaultAudioEndpoint(system ? eRender : eCapture, eCommunications, &pEndpoint);
            if (SUCCEEDED(hr)) {
                LPWSTR pwszID = nullptr;
                hr = pEndpoint->GetId(&pwszID);
                if (SUCCEEDED(hr)) {
                    CComPtr<IPropertyStore>pProps = nullptr;//属性管理器
                    hr = pEndpoint->OpenPropertyStore(STGM_READ, &pProps);
                    PROPVARIANT varName;//设备显示名字，有可能重复
                    PropVariantInit(&varName);
                    hr = pProps->GetValue(PKEY_Device_FriendlyName, &varName);
                    if (system) {
                        m_strDefaultCommSystemGuid = pwszID;
                        m_strDefaultCommSystemName = varName.bstrVal;
                        WXTcpLog(L"WASAPI System Default[%ws][%ws]",
                            m_strDefaultCommSystemName.c_str(), m_strDefaultCommSystemGuid.c_str());
                    }
                    else {
                        m_strDefaultCommMicGuid = pwszID;
                        m_strDefaultCommMicName = varName.bstrVal;
                        WXTcpLog(L"WASAPI Mic Default[%s][%s]",
                            m_strDefaultCommMicGuid.c_str(), m_strDefaultCommMicName.c_str());
                    }
                    CoTaskMemFree(pwszID);
                    PropVariantClear(&varName);
                }
            }
        }
    }

    //查询播放设备
    void ListSoundRenderDevice() {
        m_vecSoundRenderInfo.clear();
        GetDefaultName(TRUE);
        GetDefaultCommName(TRUE);

        CComPtr<IMMDeviceEnumerator>pEnumerator = nullptr;//设备枚举器
        HRESULT hr = pEnumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator));
        FAILED_RETURN2(hr, pEnumerator, L"pEnumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator))")

            CComPtr<IMMDeviceCollection>pCollection = nullptr;//设备节点管理
        hr = pEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &pCollection);
        FAILED_RETURN2(hr, pCollection, L"pEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &pCollection)")

            UINT  count = 0;
        hr = pCollection->GetCount(&count);//获取设备数量
        FAILED_RETURN2(hr, count, L"扬声器设备为0")

            for (ULONG i = 0; i < count; i++) {
                CComPtr<IMMDevice>pEndpoint = nullptr;
                hr = pCollection->Item(i, &pEndpoint);
                FAILED_CONTINUE2(hr, pEndpoint, L"pCollection->Item(i, &pEndpoint)");

                //有时候某些程序独占时会初始化失败!!!，需要取消独占后重启电脑
                CComPtr<IAudioClient>pClient = nullptr;
                hr = pEndpoint->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, (void**)&pClient);
                FAILED_CONTINUE2(hr, pClient, L"pEndpoint->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, (void**)&pClient)");

                WAVEFORMATEX* pwfx = nullptr;
                hr = pClient->GetMixFormat(&pwfx);//默认的支持格式
                FAILED_CONTINUE2(hr, pwfx, L"pClient->GetMixFormat(&pwfx)");
                CoTaskMemFree(pwfx);

                LPWSTR pwszID = nullptr;//设备GUID值，本机唯一
                hr = pEndpoint->GetId(&pwszID);
                FAILED_CONTINUE2(hr, pwszID, L"pEndpoint->GetId(&pwszID)");

                CComPtr<IPropertyStore>pProps = nullptr;//属性管理器
                hr = pEndpoint->OpenPropertyStore(STGM_READ, &pProps);
                FAILED_CONTINUE2(hr, pProps, L"pEndpoint->OpenPropertyStore(STGM_READ, &pProps)");

                PROPVARIANT varName;//设备显示名字，有可能重复
                PropVariantInit(&varName);
                hr = pProps->GetValue(PKEY_Device_FriendlyName, &varName);
                FAILED_CONTINUE1(hr, L"pEndpoint->OpenPropertyStore(STGM_READ, &pProps)");

                AudioDevInfo info;
                info.isDefalut = FALSE;
                info.isDefalutComm = FALSE;
                if (wcsicmp(m_strDefaultSystemGuid.c_str(), pwszID) == 0) {
                    info.isDefalut = TRUE;
                }

                if (wcsicmp(m_strDefaultCommSystemGuid.c_str(), pwszID) == 0) {
                    info.isDefalutComm = TRUE;
                }
                memset(info.m_strGuid, 0, sizeof(wchar_t) * MAX_PATH);;
                wcscpy(info.m_strGuid, pwszID);

                memset(info.m_strName, 0, sizeof(wchar_t) * MAX_PATH);;
                wcscpy(info.m_strName, varName.bstrVal);
                m_vecSoundRenderInfo.push_back(info);

                CoTaskMemFree(pwszID);
                PropVariantClear(&varName);
            }
    }

    void ListSoundCaptureDevice() {
        m_vecSoundCaptureInfo.clear();
        GetDefaultName(FALSE);
        GetDefaultCommName(FALSE);

        CComPtr<IMMDeviceEnumerator>pEnumerator = nullptr;

        HRESULT hr = pEnumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator));
        FAILED_RETURN2(hr, pEnumerator, L"pEnumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator))")

            CComPtr<IMMDeviceCollection>pCollection = nullptr;
        hr = pEnumerator->EnumAudioEndpoints(eCapture, DEVICE_STATE_ACTIVE, &pCollection);
        FAILED_RETURN2(hr, pCollection, L"pEnumerator->EnumAudioEndpoints(eCapture, DEVICE_STATE_ACTIVE, &pCollection)")

            UINT  count = 0;
        hr = pCollection->GetCount(&count);
        FAILED_RETURN2(hr, count, L"Mic设备为0")

            for (ULONG i = 0; i < count; i++) {
                CComPtr<IMMDevice>pEndpoint = nullptr;
                hr = pCollection->Item(i, &pEndpoint);
                FAILED_CONTINUE2(hr, pEndpoint, L"pCollection->Item(i, &pEndpoint)")

                    //有时候某些程序独占时会初始化失败!!!，需要取消独占后重启电脑
                    CComPtr<IAudioClient>pClient = nullptr;
                hr = pEndpoint->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, (void**)&pClient);
                FAILED_CONTINUE2(hr, pClient, L"pEndpoint->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, (void**)&pClient)");
                WAVEFORMATEX* pwfx = nullptr;
                hr = pClient->GetMixFormat(&pwfx);//默认的支持格式
                FAILED_CONTINUE2(hr, pwfx, L"pClient->GetMixFormat(&pwfx)");
                CoTaskMemFree(pwfx);

                LPWSTR pwszID = nullptr;
                hr = pEndpoint->GetId(&pwszID);
                FAILED_CONTINUE2(hr, pwszID, L"pEndpoint->GetId(&pwszID)")

                    CComPtr<IPropertyStore>pProps = nullptr;
                hr = pEndpoint->OpenPropertyStore(STGM_READ, &pProps);
                FAILED_CONTINUE2(hr, pProps, L"pEndpoint->OpenPropertyStore(STGM_READ, &pProps)")

                    PROPVARIANT varName;
                PropVariantInit(&varName);
                hr = pProps->GetValue(PKEY_Device_FriendlyName, &varName);
                FAILED_CONTINUE1(hr, L"pProps->GetValue(PKEY_Device_FriendlyName, &varName)")

                    AudioDevInfo info;
                info.isDefalut = FALSE;
                info.isDefalutComm = FALSE;
                if (wcsicmp(m_strDefaultMicGuid.c_str(), pwszID) == 0) {
                    info.isDefalut = TRUE;
                }
                if (wcsicmp(m_strDefaultCommMicGuid.c_str(), pwszID) == 0) {
                    info.isDefalutComm = TRUE;
                }
                memset(info.m_strGuid, 0, sizeof(wchar_t) * MAX_PATH);;
                wcscpy(info.m_strGuid, pwszID);
                memset(info.m_strName, 0, sizeof(wchar_t) * MAX_PATH);;
                wcscpy(info.m_strName, varName.bstrVal);
                m_vecSoundCaptureInfo.push_back(info);
                CoTaskMemFree(pwszID);
                PropVariantClear(&varName);
            }
    }

    //关闭所有扬声器设备
    void CloseAllSoundDevice() {
        if (m_pListenSystemDevice) {
            m_pListenSystemDevice->Close();
            delete m_pListenSystemDevice;
            m_pListenSystemDevice = nullptr;
        }
        if (m_pListenMicDevice) {
            m_pListenMicDevice->Close();
            delete m_pListenMicDevice;
            m_pListenMicDevice = nullptr;
        }
    }

    void Reset() {
        ListSoundRenderDevice();
        ListSoundCaptureDevice();
    }

    void Clear() {
        CloseAllSoundDevice();
        m_vecSoundCaptureInfo.clear();
        m_vecSoundRenderInfo.clear();
    }

public:
    void ReleaseInstance(WasapiDevice* dev) {
        WXTcpAutoLock al(m_mutex);
        if (dev == nullptr)return;
        dev->Close();
        delete dev;
    }
    //打开一个声音设备
    //核心函数
    WasapiDevice* GetInstance(int bSystem, const wchar_t* guid,
        ScreenCapture* capture = nullptr, int nSampleRate = 48000, int nChannel = 2) {
        WXTcpAutoLock al(m_mutex);

        std::wstring strGuid = guid;
        if (wcscmp(guid, L"default") == 0 || wcscmp(guid, L"all") == 0) { //查询默认设备GUID
            strGuid = bSystem ? m_strDefaultSystemGuid.c_str() : m_strDefaultMicGuid.c_str();
        }

        if (wcscmp(guid, L"nullptr") == 0) { //没有设置正确的设备GUID
            return  nullptr;
        }


        WasapiDevice* dev = new WasapiDevice;
        if (dev->Open(bSystem, strGuid.c_str(), capture, nSampleRate, nChannel) != _WX_ERROR_SUCCESS) {
            delete dev;
            dev = nullptr;//根据实际GUID值来创建WASAPI设备
        }

        return dev;
    }
};

//设备管理的过程中会设置为false

static WasapiDeviceManager g_WasapiManager;

SCREENGRAB_CAPI void WXTcpWasapiInit() {
    g_WasapiManager.Clear();
    g_WasapiManager.Reset();
}

SCREENGRAB_CAPI void WXTcpWasapiDeinit() {

    g_WasapiManager.Clear();
}

SCREENGRAB_CAPI int  WXTcpWasapiGetRenderCount() {

    return (int)g_WasapiManager.m_vecSoundRenderInfo.size();
}

SCREENGRAB_CAPI AudioDevInfo* WXTcpWasapiGetRenderInfo(int index) {
    int nSize = (int)g_WasapiManager.m_vecSoundRenderInfo.size();
    if (index < 0 || index >= nSize)return nullptr;
    return &(g_WasapiManager.m_vecSoundRenderInfo[index]);
}

SCREENGRAB_CAPI int  WXTcpWasapiGetCaptureCount() {
    return (int)g_WasapiManager.m_vecSoundCaptureInfo.size();
}

SCREENGRAB_CAPI AudioDevInfo* WXTcpWasapiGetCaptureInfo(int index) {
    int nSize = (int)g_WasapiManager.m_vecSoundCaptureInfo.size();
    if (index < 0 || index >= nSize)return nullptr;
    return &(g_WasapiManager.m_vecSoundCaptureInfo[index]);
}


SCREENGRAB_CAPI  void   AudioDeviceResetDefault() {
    g_WasapiManager.GetDefaultName(TRUE);//获取默认设备名字
    g_WasapiManager.GetDefaultName(FALSE);//获取默认设备名字
}

//打开一个声音设备
WasapiDevice* WasapiDeviceGetInstance(int bSystem, const wchar_t* guid, ScreenCapture* capture/* = nullptr*/,
    int nSampleRate/* = 44100*/, int nChannel/* = 2*/) {
    return g_WasapiManager.GetInstance(bSystem, guid, capture, nSampleRate, nChannel);
}

//删除一个设备的引用
void  WasapiDeviceReleaseInstance(WasapiDevice* dev) {
    g_WasapiManager.ReleaseInstance(dev);
}

//默认设备名字
SCREENGRAB_CAPI  const wchar_t* WXTcpWasapiGetDefaultGuid(int bSystem) {
    g_WasapiManager.GetDefaultName(bSystem);
    return bSystem ? g_WasapiManager.m_strDefaultSystemGuid.c_str() : g_WasapiManager.m_strDefaultMicGuid.c_str();
}

SCREENGRAB_CAPI  const wchar_t* WXTcpWasapiGetDefaultName(int bSystem) {
    g_WasapiManager.GetDefaultName(bSystem);
    return bSystem ? g_WasapiManager.m_strDefaultSystemName.c_str() : g_WasapiManager.m_strDefaultMicName.c_str();
}

//默认通信设备名字
SCREENGRAB_CAPI  const wchar_t* WXTcpWasapiGetDefaultCommGuid(int bSystem) {
    g_WasapiManager.GetDefaultCommName(bSystem);
    return bSystem ? g_WasapiManager.m_strDefaultCommSystemGuid.c_str() : g_WasapiManager.m_strDefaultCommMicGuid.c_str();
}

SCREENGRAB_CAPI  const wchar_t* WXTcpWasapiGetDefaultCommName(int bSystem) {
    g_WasapiManager.GetDefaultCommName(bSystem);
    return bSystem ? g_WasapiManager.m_strDefaultCommSystemName.c_str() : g_WasapiManager.m_strDefaultCommMicName.c_str();
}

