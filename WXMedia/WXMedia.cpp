// Win32 平台专用 
#include "ScreenGrab.hpp"
#include <d3d11.h>
#include <direct.h> 


#pragma comment(lib,  "x264_common.lib")
#pragma comment(lib,  "x264_encoder.lib")


/* From Windows SDK */
#pragma comment(lib,  "winmm.lib")
#pragma comment(lib,  "strmiids.lib")
#pragma comment(lib,  "version.lib")
#pragma comment(lib,  "legacy_stdio_definitions.lib")
#pragma comment(lib,  "d3d9.lib")
#pragma comment(lib,  "dxva2.lib")
#pragma comment(lib,  "d3d11.lib")
#pragma comment(lib,  "dxgi.lib")
#pragma comment(lib,  "gdiplus.lib")
#pragma comment(lib,  "DbgHelp.lib")
#pragma comment(lib,  "Psapi.lib")

//设置日志文件
static BOOL s_bInitLog = FALSE;
static WXTcpLocker gLock;
static WXTcpLogInstance s_log;

#define MAX_LENGTH 1024
static wchar_t s_wszMsg[MAX_LENGTH];
static wchar_t s_wszLog[MAX_LENGTH];
SCREENGRAB_CAPI void  WXTcpSetLogFile(const wchar_t* wszFileName) {
	WXTcpAutoLock al(gLock);
	s_bInitLog = s_log.Open(wszFileName);
}
SCREENGRAB_CAPI void  WXTcpLog(const wchar_t *format, ...) {
	WXTcpAutoLock al(gLock);
	if (s_bInitLog) {
		memset(s_wszMsg, 0, MAX_LENGTH * sizeof(wchar_t));
		va_list marker = nullptr;
		va_start(marker, format);
		vswprintf(s_wszMsg, format, marker);
		va_end(marker);

		memset(s_wszLog, 0, MAX_LENGTH * sizeof(wchar_t));
		time_t t = time(nullptr);
		tm* local = localtime(&t);
		wsprintfW(s_wszLog, L"%04d-%02d-%02d %02d:%02d:%02d - %ws\r\n",
			local->tm_year + 1900, local->tm_mon + 1, local->tm_mday,
			local->tm_hour, local->tm_min, local->tm_sec, s_wszMsg);
		s_log.Write(s_wszLog);
	}
}


SCREENGRAB_CAPI void  WXTcpSleepMs(int ms) {
	if (ms > 0)::Sleep(ms);
}

extern void WXTcpMediaUtilsWin32Init(const wchar_t* logfile);
SCREENGRAB_CAPI void WXTcpUtilsInit(const wchar_t* logfile) {
	WXTcpMediaUtilsWin32Init(logfile);
}


//WXTcpMedia.dll 的初始化操作，因为有不少操作和COM操作相关，放在DllMain函数指定容易造成卡顿
//传入地址应该为全路径...否则crash
SCREENGRAB_CAPI void WXScreenGrabInit(const wchar_t* logfile) {
	if (logfile == nullptr)return;

	try {
		WXTcpUtilsInit(logfile);;
	}
	catch (...) {
		WXTcpLog(L"WXTcpUtilsInit Crash");
	}

	try {
		WXTcpWasapiInit();
	}
	catch (...) {
		WXTcpLog(L"WXTcpWasapiInit Crash");
	}

	try {
		WXTcpScreenInit();
	}
	catch (...) {
		WXTcpLog(L"WXTcpScreenInit Crash");
	}


	try {
		WXTcpSupportDXGI();
	}
	catch (...) {
		WXTcpLog(L"WXTcpSupportDXGI Crash");
	}
}

static std::atomic<int>sCheckDXGI = 0;
static std::atomic<int>sSupportDXGI = 0;

/*
判断本机是否支持DXGI采集屏幕，一般情况下Win8就可以支持
*/
SCREENGRAB_CAPI int WXTcpSupportDXGI() {

	if (sCheckDXGI == 1)
		return sSupportDXGI;

	if (sCheckDXGI == 0)
		sCheckDXGI = 1;

	if (WXTcpGetSystemVersion() <= 7) { //Win7  不支持
		sSupportDXGI = 0;
		return sSupportDXGI;
	}

	D3D_FEATURE_LEVEL FeatureLevel;

	// Driver types supported
	D3D_DRIVER_TYPE DriverTypes[3] = {
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT NumDriverTypes = 3;

	D3D_FEATURE_LEVEL FeatureLevels[4] = {
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_1
	};
	UINT NumFeatureLevels = 4;

	HRESULT hr = S_OK;

	CComPtr <ID3D11Device>pDevice = nullptr;
	CComPtr<ID3D11DeviceContext>pImmediateContext = nullptr;

	for (UINT DriverTypeIndex = 0; DriverTypeIndex < NumDriverTypes; ++DriverTypeIndex) {
		hr = D3D11CreateDevice(nullptr, DriverTypes[DriverTypeIndex], nullptr, 0, FeatureLevels, NumFeatureLevels,
			D3D11_SDK_VERSION, &pDevice, &FeatureLevel, &pImmediateContext);
		if (SUCCEEDED(hr)) {
			break;
		}
	}
	if (FAILED(hr)) {
		WXTcpLog(L"D3D11CreateDevice DX Error[%x]", hr);
		return 0;
	}

	// Get DXGI device
	CComPtr<IDXGIDevice>DxgiDevice = nullptr;
	hr = pDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&DxgiDevice));
	if (FAILED(hr)) {
		WXTcpLog(L"m_pDevice->QueryInterface(__uuidof(IDXGIDevice) DX Error[%x]",
			hr);
		return -1;
	}

	// Get DXGI adapter
	CComPtr<IDXGIAdapter>DxgiAdapter = nullptr;
	hr = DxgiDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&DxgiAdapter));
	if (FAILED(hr)) {
		WXTcpLog(L"DxgiDevice->GetParent(__uuidof(IDXGIAdapter) DX Error[%x]",
			hr);
		return 0;
	}

	// Get output
	CComPtr<IDXGIOutput>DxgiOutput = nullptr;
	hr = DxgiAdapter->EnumOutputs(0, &DxgiOutput);//0 是否表示第一个摄像头
	if (FAILED(hr)) {
		WXTcpLog(L"DxgiAdapter->EnumOutputs DX Error[%x]", hr);
		return 0;
	}

	// QI for Output 1
	CComPtr<IDXGIOutput1>DxgiOutput1 = nullptr;
	hr = DxgiOutput->QueryInterface(__uuidof(DxgiOutput1), reinterpret_cast<void**>(&DxgiOutput1));
	if (FAILED(hr)) {
		WXTcpLog(L"DxgiOutput->QueryInterface DX Error[%x]", hr);
		return 0;
	}
	CComPtr < IDXGIOutputDuplication>ppOutputDuplication = nullptr;
	hr = DxgiOutput1->DuplicateOutput(pDevice, &ppOutputDuplication);//固定对象
	if (FAILED(hr)) {
		WXTcpLog(L"DxgiOutput1->DuplicateOutput DX Error[%x]", hr);
		return 0;
	}
	sSupportDXGI = 1;
	WXTcpLog(L"Support DXGI Capture!!!!!");
	return sSupportDXGI;
}

/*
获取本机操作系统的版本号，6为XP，7为Win7，8为Win8.0,9为Win8.1，10为Win10
*/
static std::atomic<int> s_system_version = 0;
int  WXTcpGetSystemVersion() {
	if (s_system_version != 0)
		return s_system_version;

	//先判断是否为win8.1或win10  
	typedef void(__stdcall*NTPROC)(DWORD*, DWORD*, DWORD*);
	HINSTANCE hinst = LoadLibraryA("ntdll.dll");
	if (hinst) {
		NTPROC proc = (NTPROC)GetProcAddress(hinst, "RtlGetNtVersionNumbers");
		if (proc) {
			DWORD dwMajor, dwMinor, dwBuildNumber;
			proc(&dwMajor, &dwMinor, &dwBuildNumber);
			if (dwMajor == 5) {
				s_system_version = 6; //XP
			}
			if (dwMajor == 6 && dwMinor == 1) {
				s_system_version = 7;//Win7
			}
			if (dwMajor == 6 && dwMinor == 2) {
				s_system_version = 8;//win 8.0
			}
			if (dwMajor == 6 && dwMinor == 3) {
				s_system_version = 9;//win 8.1
			}
			if (dwMajor >= 10) {
				s_system_version = 10;//win 10  
			}
		}
		FreeLibrary(hinst);
	}

	if (s_system_version == 0) {
		//判断win8.1以下的版本  
		SYSTEM_INFO info;                //用SYSTEM_INFO结构判断64位AMD处理器    
		GetSystemInfo(&info);            //调用GetSystemInfo函数填充结构    
		OSVERSIONINFOEX os;
		os.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
		if (GetVersionEx((OSVERSIONINFO *)&os)) {
			if (os.dwMinorVersion == 5)
				s_system_version = 6;
			if (os.dwMinorVersion >= 6)
				s_system_version = 7;//Vista Win7 ...
		}
	}
	return s_system_version;
}

// 创建Dump文件  
static std::wstring s_strDump;
// 处理Unhandled Exception的回调函数  
LONG ApplicationCrashHandler(EXCEPTION_POINTERS *pException)
{
	// 创建Dump文件 
	HANDLE hDumpFile = CreateFile(s_strDump.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hDumpFile) {
		// Dump信息 
		MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
		dumpInfo.ExceptionPointers = pException;
		dumpInfo.ThreadId = GetCurrentThreadId();
		dumpInfo.ClientPointers = TRUE;

		// 写入Dump文件内容  
		MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &dumpInfo, NULL, NULL);
		CloseHandle(hDumpFile);
	}
	return EXCEPTION_EXECUTE_HANDLER;
}


/*当前exe程序路径*/
std::wstring g_strPath = L"";
SCREENGRAB_CAPI const wchar_t*  WXTcpGetPath() {
	return g_strPath.c_str();
}

static ULONG_PTR m_gdiplusToken = 0;

HINSTANCE g_wx_hInstance = nullptr;
void WXTcpMediaUtilsWin32Init(const wchar_t* logfile) {

	WXTcpSetLogFile(logfile);

	TCHAR szFilePath[MAX_PATH + 1] = { 0 };
	GetModuleFileName(NULL, szFilePath, MAX_PATH);
	(_tcsrchr(szFilePath, _T('\\')))[1] = 0;
	g_strPath = szFilePath;

	s_strDump = logfile;
	s_strDump += L".dmp";

	// 设置处理Unhandled Exception的回调函数 
	SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);

	CoInitializeEx(NULL, COINIT_MULTITHREADED | COINIT_SPEED_OVER_MEMORY);

	Gdiplus::GdiplusStartupInput StartupInput;//GDI+初始化
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &StartupInput, NULL);
}


BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
	WSADATA wsaData;
	BOOL ret = FALSE;
	switch (ul_reason_for_call) {

	case DLL_PROCESS_ATTACH:

		//ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
		//if (ret != 0){
		//	return FALSE;
		//}
		timeBeginPeriod(1); //设置Sleep的精度为1ms
		g_wx_hInstance = (HINSTANCE)hModule;
		break;

	case DLL_PROCESS_DETACH:
		timeEndPeriod(1);
		break;

	case DLL_THREAD_ATTACH:
		break;

	case DLL_THREAD_DETACH:
		break;
	}
	return (TRUE);
}

