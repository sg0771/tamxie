// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "stdafx.h"
#include "resource.h"
#include <map>

#include <ScreenGrab.h>

#pragma comment(lib,  "ScreenGrab.lib")

#define WM_LIBEN  WM_USER+100
class CMainDlg : public CDialogImpl<CMainDlg>//, public CWinDataExchange<CMainDlg>
{
	DWORD m_tsStart = 0;

	BOOL m_bPause = FALSE;
	ATOM m_atomStartAndStop = 0;   // 启动/结束热键  Ctrl+Alt+1
	ATOM m_atomPauseAndResume = 0;    //暂停/恢复热键   Ctrl+Alt+2

	CString m_strDir = L"";
	CString m_strFileName = L"";

	void *m_pCapture = nullptr;
	void ProcDir(const wchar_t * dir);//判断目录是否存在，不存在则创建

	HICON m_arrIcons[4] = {nullptr};
    NOTIFYICONDATA m_notifyIconData;//NOTIFYICONDATA 结构声明 
    UINT m_msgTaskbarRestart;//任务栏被刷新
    CMenu  m_Menu;

    BOOL m_bAddTray = FALSE;
    HICON m_hIcon = nullptr;
    HICON m_hIconSmall = nullptr;

    int  m_nIndexTray = 0;
    void SetDir();//设置输出文件夹
    void OpenDir();//打开输出文件夹
    void Start();//启动录制
    void Stop();//结束录制
    void Pause();//暂停录制
    void Resume();//恢复录制

    void AddTray();
    void DelTray();
    void ChangeTray();

	CFont  m_font = NULL;

    int m_idTime = 1003;

public:
	enum { IDD = IDD_MAINDLG };

	BEGIN_MSG_MAP(CMainDlg)
		MESSAGE_HANDLER(WM_HOTKEY, OnHotKey)
        MESSAGE_HANDLER(m_msgTaskbarRestart, OnTaskbarRestart) //任务栏重建
		MESSAGE_HANDLER(WM_LIBEN, OnNotifyIcon)   //WM_LIBEN消息映射 
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
        MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
        COMMAND_ID_HANDLER(ID_POPUP_ABOUT, OnPopupAbout)
        COMMAND_ID_HANDLER(ID_POPUP_32776, OnPopupStart)
        COMMAND_ID_HANDLER(ID_POPUP_32777, OnPopupStop)
        COMMAND_ID_HANDLER(ID_POPUP_32778, OnPopupPause)
        COMMAND_ID_HANDLER(ID_POPUP_32779, OnPopupResume)
        COMMAND_ID_HANDLER(ID_POPUP_32780, OnPopupShow)
        COMMAND_ID_HANDLER(ID_POPUP_32781, OnPopupExit)
        COMMAND_ID_HANDLER(ID_POPUP_32782, OnPopupHide)
        COMMAND_ID_HANDLER(ID_POPUP_32783, OnPopupSetDir)
        COMMAND_ID_HANDLER(ID_POPUP_32784, OnPopupOpenDir)
        COMMAND_HANDLER(IDC_FILE, BN_CLICKED, OnBnClickedFile)
        COMMAND_HANDLER(IDC_DIR, BN_CLICKED, OnBnClickedDir)
        COMMAND_HANDLER(IDC_START, BN_CLICKED, OnBnClickedStart)
		COMMAND_HANDLER(IDC_STOP, BN_CLICKED, OnBnClickedStop)
        MESSAGE_HANDLER(WM_TIMER, OnTimer)
    END_MSG_MAP()

	
	LRESULT OnHotKey(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);//热键事件
    //Explorer外壳崩溃后任务栏重建
    LRESULT OnTaskbarRestart(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);//鼠标点击事件
	LRESULT OnNotifyIcon(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);//鼠标点击事件


	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

    LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	LRESULT OnBnClickedFile(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedDir(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedStart(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnPopupAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnPopupStart(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnPopupStop(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnPopupPause(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnPopupResume(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnPopupShow(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnPopupExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnPopupHide(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnPopupSetDir(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnPopupOpenDir(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedStop(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
};
