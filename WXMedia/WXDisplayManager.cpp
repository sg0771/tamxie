/**********************************************

文件描述：显示器管理类
说明：
1、此类只适用于任意多屏幕的控制，无论你有几块显卡，接了几个屏幕。
2、每个屏幕可配置不同分辨率，可配置不同的位置关系；
//win10 已经没有 DPI问题了
***********************************************/
#include <ScreenGrab.hpp>

class WXTcpDisplayManager {
	std::vector<PlayInfo>m_arrayInfo;
	PlayInfo m_defauleInfo; //默认显示器

	std::vector<int>m_arrayRotate;
public:
	void Reset() {
		m_arrayInfo.clear();

		//计算全屏大小
		int MinX = 10000;
		int MinY = 10000;
		int MaxX = -10000;
		int MaxY = -10000;

		DISPLAY_DEVICE dd, dd1;
		BOOL flag = true;
		DEVMODE dm;
		int DispNum = 0;

		do {
			ZeroMemory(&dm, sizeof(dm));
			dm.dmSize = sizeof(dm);

			ZeroMemory(&dd, sizeof(dd));
			dd.cb = sizeof(dd);

			ZeroMemory(&dd1, sizeof(dd1));
			dd1.cb = sizeof(dd1);

			flag = EnumDisplayDevices(nullptr, DispNum, &dd, 0);//获取第i个设备的名字
			if (!flag)break;

			DispNum++;

			std::wstring szName = dd.DeviceName;
			EnumDisplayDevices(dd.DeviceName, 0, &dd1, 0);//获取指定设备的信息

			if (!(dd.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP)) {
				continue;
			}

			EnumDisplaySettings(dd.DeviceName, ENUM_CURRENT_SETTINGS, &dm);//获取指定设备详细信息
			int nRotate = 90 * dm.dmDisplayOrientation;
			WXTcpLog(L"Display Device[%ws] Size=%dx%d Pos = %dx%d Rotate=%d",
				szName.c_str(),
				dm.dmPelsWidth,
				dm.dmPelsHeight,
				dm.dmPosition.x,
				dm.dmPosition.y,
				nRotate);

			if (dm.dmPelsWidth == 0 || dm.dmPelsHeight == 0) {
				WXTcpLog(L"DisplaySize is 0");
				continue;//无效的显示器
			}

			//显示器名字以及实际尺寸		
			if (dd.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP) {//是否有效的桌面
				PlayInfo info;
				info.left = dm.dmPosition.x;
				info.top = dm.dmPosition.y;
				info.width = dm.dmPelsWidth / 2 * 2;//实际分辨率
				info.height = dm.dmPelsHeight / 2 * 2;//实际分辨率
				wcscpy(info.wszName, dd.DeviceName);
				if ((dd1.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE) || (info.left == 0 && info.top == 0)) { //桌面主设备
					info.isPrimary = 1;
					WXTcpLog(L"Display Primary [%ws]", szName.c_str());
				}
				else {
					info.isPrimary = 0;
				}

				bool exist = false;
				for (int i = 0; i < m_arrayInfo.size(); i++) {
					if (info.left == m_arrayInfo[i].left &&
						info.top == m_arrayInfo[i].top &&
						info.width == m_arrayInfo[i].width &&
						info.height == m_arrayInfo[i].height) {
						exist = true;
						break;
					}
				}
				if (!exist) {

					if (MinX > info.left)
						MinX = info.left;

					if (MinY > info.top)
						MinY = info.top;

					if (MaxX < info.left + info.width)
						MaxX = info.left + info.width;

					if (MaxY < info.top + info.height)
						MaxY = info.top + info.height;

					m_arrayInfo.push_back(info);
					m_arrayRotate.push_back(nRotate);
					if (info.isPrimary) {
						memcpy(&m_defauleInfo, &info, sizeof(info));
					}
				}
			}
		} while (flag);

		if (m_arrayInfo.size() > 1) { //多屏录制！！
			PlayInfo info;
			info.left = MinX;
			info.top = MinY;
			info.width = (MaxX - MinX) / 2 * 2;//实际分辨率
			info.height = (MaxY - MinY) / 2 * 2;//实际分辨率
			wcscpy(info.wszName, L"FullScreen");
			m_arrayInfo.push_back(info);
			m_arrayRotate.push_back(0);
		}

		WXTcpLog(L"%ws Enum DisplayDevice WindowsMultiScreen DispNum = %d, Count = %d",
			__FUNCTIONW__, DispNum, (int)m_arrayInfo.size());
	}

	WXTcpDisplayManager() {
	}
	virtual ~WXTcpDisplayManager() {
		m_arrayInfo.clear();
	}

public:
	inline int GetScreenCount() {
		return (int)m_arrayInfo.size();
	}

	PlayInfo* GetInfo(int index) {
		if (index >= m_arrayInfo.size() || index < 0)
			return &m_defauleInfo;
		return &m_arrayInfo[index];
	}

	PlayInfo* GetDefaultInfo() {
		return &m_defauleInfo;
	}

	PlayInfo* GetInfo(const wchar_t* wszDevice) {
		for (int i = 0; i < m_arrayInfo.size(); i++) {
			PlayInfo* info = &m_arrayInfo[i];
			if (info && wcsicmp(wszDevice, info->wszName) == 0) {
				std::wstring strName = info->wszName;
				WXTcpLog(L"Screen Info [%ws] = [%d,%d,%d, %d]",
					strName.c_str(), info->left, info->top, info->width, info->height);
				return info;
			}
		}
		return &m_defauleInfo;
	}

	int GetRotate(const wchar_t* wszDevice) {
		for (int i = 0; i < m_arrayInfo.size(); i++) {
			PlayInfo* info = &m_arrayInfo[i];
			if (info && wcsicmp(wszDevice, info->wszName) == 0) {
				return m_arrayRotate[i];
			}
		}
		return m_arrayRotate[0];
	}
};

//------------- 显示器设备 --------------
static WXTcpLocker s_lockScreen;
static WXTcpDisplayManager s_VMS;


SCREENGRAB_CAPI int WXTcpScreenGetRotate(const wchar_t* wszDevice) {
	return s_VMS.GetRotate(wszDevice);
}

//初始化设备管理器
SCREENGRAB_CAPI void WXTcpScreenInit() {
	WXTcpAutoLock al(s_lockScreen);
	s_VMS.Reset();
}

SCREENGRAB_CAPI void WXTcpScreenDeinit() {

}

SCREENGRAB_CAPI int WXTcpScreenGetCount() {
	return s_VMS.GetScreenCount();
}

SCREENGRAB_CAPI PlayInfo* WXTcpScreenGetDefaultInfo() {
	return s_VMS.GetDefaultInfo();
}

SCREENGRAB_CAPI PlayInfo* WXTcpScreenGetInfo(int index) {
	return s_VMS.GetInfo(index);
}

SCREENGRAB_CAPI PlayInfo* WXTcpScreenGetInfoByName(const wchar_t* wszDevice) {
	return s_VMS.GetInfo(wszDevice);
}

