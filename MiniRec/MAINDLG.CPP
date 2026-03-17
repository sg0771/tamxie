#include "stdafx.h"
#include "MainDlg.h"
#include <direct.h>
#include <io.h>
#pragma comment(lib, "winmm.lib")

LRESULT CMainDlg::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/){
    CSimpleDialog<IDD_ABOUTBOX, FALSE> dlg;
    dlg.DoModal();
    return 0;
}

LRESULT CMainDlg::OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/){
    ::ShowWindow(m_hWnd, SW_HIDE);
    return 0;
}

LRESULT CMainDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/){
	Stop();
	DelTray();
	UnregisterHotKey(m_hWnd, m_atomStartAndStop);
	GlobalDeleteAtom(m_atomStartAndStop);
	UnregisterHotKey(m_hWnd, m_atomPauseAndResume);
	GlobalDeleteAtom(m_atomPauseAndResume);

	EndDialog(wID);

    return 0;
}

//------------------  菜单 --------------------------------------------------
LRESULT CMainDlg::OnPopupAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/){
    // TODO: 在此添加命令处理程序代码
    CSimpleDialog<IDD_ABOUTBOX, FALSE> dlg;
    dlg.DoModal();
    return 0;
}


LRESULT CMainDlg::OnPopupStart(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    Start();
    return 0;
}


LRESULT CMainDlg::OnPopupStop(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/){
    Stop();
    return 0;
}


LRESULT CMainDlg::OnPopupPause(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/){
    Pause();
    return 0;
}


LRESULT CMainDlg::OnPopupResume(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/){
    Resume();
    return 0;
}


LRESULT CMainDlg::OnPopupShow(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/){
    if(m_pCapture == nullptr)
        ::ShowWindow(m_hWnd, SW_SHOW);
    return 0;
}


LRESULT CMainDlg::OnPopupHide(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/){
    ::ShowWindow(m_hWnd, SW_HIDE);
    return 0;
}

LRESULT CMainDlg::OnPopupExit(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/){
    Stop();
    DelTray();
	UnregisterHotKey(m_hWnd, m_atomStartAndStop);
	GlobalDeleteAtom(m_atomStartAndStop);
	UnregisterHotKey(m_hWnd, m_atomPauseAndResume);
	GlobalDeleteAtom(m_atomPauseAndResume);
    EndDialog(wID);
    return 0;
}

//热键
LRESULT CMainDlg::OnHotKey(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	if (wParam == m_atomStartAndStop) {
		m_pCapture == nullptr ? Start() : Stop();
	}
	if (wParam == m_atomPauseAndResume && m_pCapture) {
		m_bPause ? Resume() :	Pause();
	}
	return 0;
}

LRESULT CMainDlg::OnTaskbarRestart(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
    AddTray();
    return 0;
}

LRESULT CMainDlg::OnNotifyIcon(UINT uMsg, WPARAM uID, LPARAM uMouseMsg, BOOL& bHandled) {
    if (uID != IDR_MAINFRAME)
        return 0L;
	if (uMouseMsg == WM_LBUTTONDOWN){ //左键点击
		if (m_pCapture == nullptr) {
			::ShowWindow(m_hWnd, SW_SHOW);
		}
	}else if (uMouseMsg == WM_RBUTTONDOWN){ //右键单击
        POINT ptCursor;
		::GetCursorPos(&ptCursor);//获取当前鼠标位置 
        CMenuHandle pSubMenu = m_Menu.GetSubMenu(0);
        ::SetForegroundWindow(m_hWnd);
        ::TrackPopupMenu(pSubMenu.m_hMenu, TPM_LEFTBUTTON, ptCursor.x, ptCursor.y, 0, m_hWnd, NULL);
        ::PostMessage(m_hWnd, WM_NULL, 0, 0);
	}
	return 0;
}

void CMainDlg::ChangeTray() {
    if (m_pCapture && !m_bPause) {
        m_nIndexTray++;
        if (m_nIndexTray >= 4)
            m_nIndexTray = 0;
        m_notifyIconData.hIcon = m_arrIcons[m_nIndexTray];
        SetIcon(m_notifyIconData.hIcon, FALSE);
        Shell_NotifyIcon(NIM_MODIFY, &m_notifyIconData);		//发送更新图标的消息;
        Shell_NotifyIcon(NIM_ADD, &m_notifyIconData);		//发送增加系统托盘的消息;
    }
}

void CMainDlg::AddTray() {
    DelTray();
    if (!m_bAddTray) {
        m_bAddTray = TRUE;
        m_notifyIconData.cbSize = sizeof(NOTIFYICONDATA);//结构体的大小，以字节为单位。 
        m_notifyIconData.hWnd = this->m_hWnd;//窗口句柄，标示的窗口用来接收与托盘图标相关的消息。 
        m_notifyIconData.uID = IDR_MAINFRAME;// 应用程序定义的任务栏图标的标识符。Shell_NotifyIcon函数调用时，hWnd和uID成员用来标示具体要操作的图标
        m_notifyIconData.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;//指示具体哪些其他成员为合法数据
        m_notifyIconData.uCallbackMessage = WM_LIBEN;//应用程序定义的消息标示，当托盘图标区域发生鼠标事件或者使用键盘选择或激活图标时，系统将使用此标示向由hWnd成员标示的窗口发送消息。 
        m_notifyIconData.hIcon = m_hIcon;//图标ID 
        wcscpy(m_notifyIconData.szTip, L"Apoweroft-ScreenGrab");//托盘图标提示文本 
        Shell_NotifyIcon(NIM_ADD, &m_notifyIconData);//安装托盘图标
    }
}

void CMainDlg::DelTray() {
    if (m_bAddTray) {
        Shell_NotifyIcon(NIM_DELETE, &m_notifyIconData);//卸载图标
        m_bAddTray = FALSE;
    }
}


extern HINSTANCE g_hInstance /*= nullptr*/;
LRESULT CMainDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	//DoDataExchange(FALSE);
	//GetDlgItem(IDC_IP).EnableWindow(FALSE);
	//GetDlgItem(IDC_PORT1).EnableWindow(FALSE);

	m_atomStartAndStop = GlobalAddAtom(L"Mini Rec Start And Stop");   // 启动/结束 热键  Ctrl+Alt+1
	m_atomPauseAndResume = GlobalAddAtom(L"Mini Rec Pause And Resume");    //暂停/恢复 热键   Ctrl+Alt+2

	BOOL m_isKeyRegistered;
	m_isKeyRegistered = RegisterHotKey(m_hWnd, m_atomStartAndStop, MOD_CONTROL | MOD_SHIFT, '1');
	assert(m_isKeyRegistered != FALSE);     //调试时用(当前热键已经被注册时会返回失败)

	m_isKeyRegistered = RegisterHotKey(m_hWnd, m_atomPauseAndResume, MOD_CONTROL | MOD_SHIFT, '2');
	assert(m_isKeyRegistered != FALSE);     //调试时用(当前热键已经被注册时会返回失败)


	WXScreenGrabInit(L"MiniRec.log");
	// center the dialog on the screen
	CenterWindow();

    m_arrIcons[0] = ::LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_ICON1));
    m_arrIcons[1] = ::LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_ICON2));
    m_arrIcons[2] = ::LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_ICON3));
    m_arrIcons[3] = ::LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_ICON4));

	// set icons
    m_hIcon      = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
	SetIcon(m_hIcon, TRUE);

    m_hIconSmall = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
	SetIcon(m_hIconSmall, FALSE);

	AddTray();

    m_Menu.LoadMenu(IDR_MENU1);//载入右键快捷菜单

	m_strDir = WXTcpGetPath();

	m_strDir += L"Videos\\";

	ProcDir(m_strDir);
	GetDlgItem(IDC_START).EnableWindow(TRUE);
	
	//CEdit m_fr_sn = (CEdit)GetDlgItem(IDC_LOG);
	//m_font.Attach(m_fr_sn.GetFont());
	//m_font.CreatePointFont(400, _T("微软雅黑"));
	//m_fr_sn.SetFont(m_font.m_hFont);


	return TRUE;
}

void CMainDlg::SetDir() {
    if (m_pCapture)
        return;
    TCHAR szBuffer[MAX_PATH] = { 0 };
    BROWSEINFO bi;
    ZeroMemory(&bi, sizeof(BROWSEINFO));
    bi.hwndOwner = NULL;
    bi.pszDisplayName = szBuffer;
    bi.lpszTitle = _T("从下面选文件夹目录:");
    bi.ulFlags = BIF_RETURNFSANCESTORS;
    LPITEMIDLIST idl = SHBrowseForFolder(&bi);
    if (NULL != idl){
        SHGetPathFromIDList(idl, szBuffer);
        m_strDir = szBuffer;
    }

}

void CMainDlg::OpenDir() {
    ::ShellExecute(NULL, L"explore", m_strDir, NULL, NULL, SW_SHOWDEFAULT);
}

LRESULT CMainDlg::OnPopupSetDir(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/){
    SetDir();
    return 0;
}

LRESULT CMainDlg::OnPopupOpenDir(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/){
    OpenDir();
    return 0;
}

LRESULT CMainDlg::OnBnClickedFile(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/){
    SetDir();
	return 0;
}

LRESULT CMainDlg::OnBnClickedDir(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/){
    OpenDir();
	return 0;
}



//判断目录是否存在，不存在则创建
void CMainDlg::ProcDir(const wchar_t * wszDir) {
    int iSize;
    //返回接受字符串所需缓冲区的大小，已经包含字符结尾符'\0'
    iSize = WideCharToMultiByte(CP_ACP, 0, wszDir, -1, NULL, 0, NULL, NULL); //iSize =wcslen(pwsUnicode)+1=6
    char* szDir = (char*)malloc(iSize * sizeof(char)); //不需要 pszMultiByte = (char*)malloc(iSize*sizeof(char)+1);
    WideCharToMultiByte(CP_ACP, 0, wszDir, -1, szDir, iSize, NULL, NULL);

    if (::_access(szDir, 0) == -1)
    {
        ::_mkdir(szDir);
    }
    free(szDir);
}

LRESULT CMainDlg::OnBnClickedStart(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    Start();
    return 0;
}

//录屏操作
void CMainDlg::Start() {
	//DoDataExchange(TRUE);
    ::ShowWindow(m_hWnd, SW_HIDE);
    if (m_pCapture == nullptr) {
		m_bPause = FALSE;
        WXScreenGrabParam s_param;
		WXScreenGrabDefaultConfig(&s_param);

		wcscpy(s_param.m_systemName, L"all");
		//wcscpy(s_param.m_micName, L"all");
		wcscpy(s_param.m_wszDevName, L"default");


		m_strFileName = m_strDir;
		time_t t1 = time(NULL);
		tm *t = localtime(&t1);
		CString strTime;
		strTime.Format(L"%04d%02d%02d-%02d%02d%02d.ts", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
		m_strFileName += strTime;
		s_param.m_nPort = 0;
		wcscpy(s_param.m_wszName, m_strFileName);
		
        int error = 0;

       // s_param.m_text.m_bUsed = TRUE;
		m_tsStart = ::timeGetTime();
        m_pCapture = WXScreenGrabStart(&s_param, &error);

        if (m_pCapture == nullptr) {
            CString XX;

            if (error & _WX_ERROR_OPEN_FILE)XX += L"Crate File Failed\r\n";
            if (error & _WX_WARNING_NO_VIDEO_DISPLAY_DATA)XX += L"Not Screen Shot Data\r\n";
            if (error & _WX_WARNING_NO_VIDEO_CAREMA_DATA)XX += L"Not Camera Shot Data\r\n";
            if (error & _WX_WARNING_NO_VIDEO_DATA)XX += L"Not Video Data\r\n";

            if (error & _WX_WARNING_NO_IMAGE_WATERMARK_DATA)XX += L"No Image Watermark\r\n";
            if (error & _WX_WARNING_NO_TEXT_WATERMARK_DATA)XX += L"No Text Watermark\r\n";

            if (error & _WX_WARNING_NO_SOUND_SYSTEM_DATA)XX += L"No System Sound Data\r\n";
            if (error & _WX_WARNING_NO_SOUND_MIC_DATA)XX += L"No Micphone Sound Data\r\n";
            if (error & _WX_WARNING_NO_SOUND_DATA)XX += L"No Audio Data\r\n";

            if (error & _WX_WARNING_VIDEO_RECT)XX += L"/Video locale setting error\r\n";
            if (error & _WX_ERROR_VIDEO_NO_DEVICE)XX += L"Specified device name does not exist\r\n";
            if (error & _WX_ERROR_VIDEO_NO_PARAM)XX += L"The camera device does not support the format parameters width/height/fps\r\n";
            if (error & _WX_ERROR_VIDEO_DEVICE_OPEN)XX += L"Video Device Open Failed\r\n";

            if (error & _WX_ERROR_SOUND_SYSTEM_OPEN)XX += L"System Sound Open Failed\r\n";
            if (error & _WX_ERROR_SOUND_MIC_OPEN)XX += L"Micphone Sound Open Failed\r\n";
            ::MessageBox(NULL, XX, L"ScreenGrab Error", MB_OK);
        } else {
            GetDlgItem(IDC_START).EnableWindow(TRUE);
            SetTimer(m_idTime, 100, NULL);//OK
        }
    }
}

void CMainDlg::Stop() {
    if (m_pCapture) {
        KillTimer(m_idTime);
        WXScreenGrabStop(m_pCapture);
        m_pCapture = nullptr;
		m_bPause = FALSE;
        GetDlgItem(IDC_START).EnableWindow(TRUE);
        ::ShowWindow(m_hWnd, SW_SHOW);
        AddTray();
		::ShellExecute(NULL, L"explore", m_strDir, NULL, NULL, SW_SHOWDEFAULT);
    }
}

void CMainDlg::Pause() {
    if (m_pCapture) {
		WXScreenGrabPause(m_pCapture);
		m_bPause = TRUE;
        GetDlgItem(IDC_START).EnableWindow(TRUE);
    }
}

void CMainDlg::Resume() {
    if (m_pCapture) {
		WXScreenGrabPause(m_pCapture);
		m_bPause = FALSE;
        GetDlgItem(IDC_START).EnableWindow(TRUE);
    }
}

LRESULT CMainDlg::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
	Stop();
	DelTray();
    return 0;
}




LRESULT CMainDlg::OnBnClickedStop(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: 在此添加控件通知处理程序代码
	Stop();
	return 0;
}


LRESULT CMainDlg::OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    ChangeTray();
    return 0;
}
