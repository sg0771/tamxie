
// TestCompressDlg.h: 头文件
//

#pragma once

#include <thread>
#include <timeapi.h>

#define MAX_COUNT  100

// CTestCompressDlg 对话框
class CTestCompressDlg : public CDialogEx
{
	HKEY m_hKey = nullptr;//注册表操作
	DWORD m_nCount = 0;//最大次数
	//CString m_strKey = TEXT("Software\\WangXu\\WXImageSDK");
	CString m_strKey = TEXT("Software\\ApowerSoft\\WXImageSDK");
	REGSAM m_sam = KEY_READ | KEY_WRITE | KEY_WOW64_64KEY;
	CString m_strValue = TEXT("value");

// 构造
	UINT_PTR m_idTime = 1001;
	BOOL m_bStartProcess = 0;

	std::thread* m_thread = nullptr;
	DWORD m_tsStart = 0;
public:
	CTestCompressDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TESTCOMPRESS_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:

	afx_msg void OnBnClickedInput();
	afx_msg void OnBnClickedOutput();


	CString m_strInput;
	CString m_strOutput;
	int m_iWidth;
	int m_iHeight;
	int m_iFileSize;
	int m_iQuality;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CComboBox m_cbType1;
	CComboBox m_cbType2;
	afx_msg void OnBnClickedQuality();
	afx_msg void OnBnClickedSize();
	afx_msg void OnBnClickedGetquality();
	afx_msg void OnBnClickedToPng();
	afx_msg void OnBnClickedToJpg();
	afx_msg void OnBnClickedReadIcc();
};
