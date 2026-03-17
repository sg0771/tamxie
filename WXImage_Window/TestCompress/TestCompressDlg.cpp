
// TestCompressDlg.cpp: 实现文件
//

#pragma warning(disable: 4996)
#include "pch.h"
#include "framework.h"
#include "TestCompress.h"
#include "TestCompressDlg.h"
#include "afxdialogex.h"

#include <thread>

#pragma comment(lib,"WX_Image.lib")

#include "../WXImage/WXImage.h"
#include <memory>

#include <gdiplus.h>
#pragma comment(lib,"gdiplus.lib")

static std::shared_ptr<uint8_t>  _ReadFile(const wchar_t* wszName, int* length) {
	FILE* fin = _wfopen(wszName, L"rb");
	if (fin) {
		fseek(fin, 0, SEEK_END);
		int size = (int)ftell(fin);
		if (size) {
			std::shared_ptr<uint8_t> pData = std::shared_ptr<uint8_t>(new uint8_t[size]);
			fseek(fin, 0, SEEK_SET);
			fread(pData.get(), 1, size, fin);
			*length = size;
			return pData;
		}
	}
	return nullptr;
}



#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CTestCompressDlg 对话框



CTestCompressDlg::CTestCompressDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TESTCOMPRESS_DIALOG, pParent)
	, m_strInput(_T("1.jpg"))
	, m_strOutput(_T("output.jpg"))
	, m_iWidth(0)
	, m_iHeight(0)
	, m_iFileSize(1500)
	, m_iQuality(75)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTestCompressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strInput);
	DDX_Text(pDX, IDC_EDIT2, m_strOutput);
	DDX_Text(pDX, IDC_EDIT3, m_iWidth);
	DDX_Text(pDX, IDC_EDIT4, m_iHeight);
	DDX_Text(pDX, IDC_EDIT5, m_iFileSize);
	DDX_Text(pDX, IDC_EDIT6, m_iQuality);
	DDX_Control(pDX, IDC_COMBO1, m_cbType1);
	DDX_Control(pDX, IDC_COMBO2, m_cbType2);
}

BEGIN_MESSAGE_MAP(CTestCompressDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_INPUT, &CTestCompressDlg::OnBnClickedInput)
	ON_BN_CLICKED(IDC_OUTPUT, &CTestCompressDlg::OnBnClickedOutput)
	ON_BN_CLICKED(IDC_IMGQUALITY, &CTestCompressDlg::OnBnClickedQuality)
	ON_BN_CLICKED(IDC_IMGSIZE, &CTestCompressDlg::OnBnClickedSize)
	ON_BN_CLICKED(IDC_GetQuality, &CTestCompressDlg::OnBnClickedGetquality)
	ON_BN_CLICKED(IDC_TO_PNG, &CTestCompressDlg::OnBnClickedToPng)
	ON_BN_CLICKED(IDC_TO_JPG, &CTestCompressDlg::OnBnClickedToJpg)
	ON_BN_CLICKED(IDC_READ_ICC, &CTestCompressDlg::OnBnClickedReadIcc)
END_MESSAGE_MAP()


// CTestCompressDlg 消息处理程序

BOOL CTestCompressDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}



	LSTATUS  err =  ::RegOpenKeyEx(HKEY_CURRENT_USER, m_strKey, 0, m_sam, &m_hKey);
	if (m_hKey == nullptr) {
		err = RegCreateKeyEx(HKEY_CURRENT_USER, m_strKey,(DWORD)0,NULL,0, m_sam,NULL, &m_hKey,NULL);
		if (m_hKey == nullptr) {
			AfxMessageBox(L"请以管理员启动程序");
			exit(-1);
		}
	}

	DWORD dwKeyType = 0;
	DWORD dwKeySize = sizeof(DWORD);
	DWORD dwKeyValue = 0;
	err = RegQueryValueEx(m_hKey, m_strValue, NULL,&dwKeyType,(LPBYTE)&dwKeyValue,&dwKeySize);

	if (err != ERROR_SUCCESS) { //查询不到值
		dwKeyValue = 0;
		err = RegSetValueEx(m_hKey, m_strValue,0,REG_DWORD, (PBYTE)&dwKeyValue,sizeof(DWORD));

		if (err != ERROR_SUCCESS) {
			AfxMessageBox(L"请以管理员启动程序");
			exit(-1);
		}
		m_nCount = 0;
	}
	else {
		m_nCount = dwKeyValue;
	}

	if (m_nCount >= MAX_COUNT) {
		AfxMessageBox(L"试用次数超过限制");
		exit(-1);
	}

	WXImage_InitLibrary();//静态库初始化

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	m_cbType1.InsertString(0, L"Original");
	m_cbType1.InsertString(1, L"Jpeg");
	m_cbType1.InsertString(2, L"Webp");
	m_cbType1.InsertString(3, L"Mozjpeg");
	m_cbType1.SetCurSel(0);

	m_cbType2.InsertString(0, L"Original");
	m_cbType2.InsertString(1, L"Jpeg");
	m_cbType2.InsertString(2, L"Png");
	m_cbType2.InsertString(3, L"Webp");
	m_cbType2.InsertString(4, L"Mozjpeg");
	m_cbType2.InsertString(5, L"PNG8");
	m_cbType2.SetCurSel(0);

	static ULONG_PTR m_gdiplusToken = 0;
	if (m_gdiplusToken == 0) {
		Gdiplus::GdiplusStartupInput StartupInput;//GDI+初始化
		Gdiplus::GdiplusStartup(&m_gdiplusToken, &StartupInput, NULL);
	}

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CTestCompressDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CTestCompressDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CTestCompressDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

static void GeyRect(int rect_width, int rect_height, int src_width, int src_height, int* dX, int* dY) {
	*dX = 0;
	*dY = 0;
	int sw1 = rect_height * src_width / src_height;
	int sh1 = rect_width * src_height / src_width;
	if (sw1 <= rect_width) {
		*dX = (rect_width - sw1) / 2;
	}
	else {
		*dY = (rect_height - sh1) / 2;
	}
}

// 将BYTE*数据显示到指定窗口
static bool DisplayImageToHWND(HWND hWnd, BYTE* data, int width, int height, int pitch,int channel)
{
	if (!hWnd || !data || width <= 0 || height <= 0 || pitch <= 0)
		return false;

	// 获取窗口客户区DC
	HDC hdc = GetDC(hWnd);
	if (!hdc)
		return false;

	RECT rcDst;
	GetClientRect(hWnd, &rcDst);

	int WndW = rcDst.right;
	int WndH = rcDst.bottom;
	int desX = 0;
	int desY = 0;

	GeyRect(WndW, WndH, width, height, &desX, &desY);
	rcDst.left += desX;
	rcDst.right -= desX;
	rcDst.top += desY;
	rcDst.bottom -= desY;


	// 创建GDI+ Bitmap对象
	// 假设数据格式为24位RGB (每个像素3字节)
	Gdiplus::Bitmap bitmap(width, height, pitch, channel==3 ? PixelFormat24bppRGB : PixelFormat32bppARGB, data);

	// 创建GDI+ Graphics对象
	Gdiplus::Graphics graphics(hdc);

	// 绘制图像
	Gdiplus::Rect rc(rcDst.left, rcDst.top, rcDst.right - rcDst.left, rcDst.bottom - rcDst.top);

	Gdiplus::Status status = graphics.DrawImage(&bitmap, rc, 0, 0, width,height, Gdiplus::UnitPixel);

	// 释放资源
	ReleaseDC(hWnd, hdc);

	return status == Gdiplus::Ok;
}


void CTestCompressDlg::OnBnClickedInput()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strMsg;
	CFileDialog dlg(TRUE);
	if (IDOK == dlg.DoModal()) {
		m_strInput = dlg.GetPathName();

		int src_size = 0;
		std::shared_ptr<uint8_t>src_buffer = _ReadFile(m_strInput, &src_size);
		if (src_buffer) {

			//从内存buffer解码图像
			std::shared_ptr<void>pBitmap = std::shared_ptr<void>(
				WXImage_Load(src_buffer.get(), src_size),
				[](void* p) {  
					if (p) { WXImage_Unload(p); p = nullptr; }
				}
			);

			if (pBitmap.get() != nullptr) {
				//开始编码
				int   nWidth  = WXImage_GetWidth(pBitmap.get()); //图像宽度
				int   nHeight = WXImage_GetHeight(pBitmap.get());//图像高度
				int   resX    = WXImage_GetDotsPerMeterX(pBitmap.get());
				int   resY    = WXImage_GetDotsPerMeterX(pBitmap.get());

				int channel = WXImage_GetChannel(pBitmap.get());
				int Pitch   = WXImage_GetPitch(pBitmap.get());
				uint8_t* pData = WXImage_GetBits(pBitmap.get());

				strMsg.Format(L"[%d KB][%dx%d] [%dx%d]", (src_size + 1023) / 1024, nWidth, nHeight, resX, resY);
				HWND hwnd = GetDlgItem(IDC_PIC)->GetSafeHwnd();
				DisplayImageToHWND(hwnd, pData, nWidth, nHeight, Pitch, channel);
			}
		}else {
			strMsg.Format(L"不是图像文件");
		}
		UpdateData(FALSE);
	}
	this->SetWindowText(strMsg);
}


void CTestCompressDlg::OnBnClickedOutput()
{
	// TODO: 在此添加控件通知处理程序代码
	CFileDialog dlg(FALSE);
	if (IDOK == dlg.DoModal()) {
		m_strOutput = dlg.GetPathName();
		UpdateData(FALSE);
	}
}

void CTestCompressDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialogEx::OnTimer(nIDEvent);
	if (nIDEvent == m_idTime) {
		DWORD ts = timeGetTime() - m_tsStart;
		CString str;
		if (m_bStartProcess) {
			str.Format(L"正在转换， %d ms", (int)ts);
		}else{
			str.Format(L"总的转换时间  [%d ms]", (int)ts);
			KillTimer(m_idTime);
			if (m_thread) {
				m_thread->join();
				delete m_thread;
				m_thread = nullptr;

				RegSetValueEx(m_hKey, m_strValue, 0, REG_DWORD, (PBYTE)&m_nCount, sizeof(DWORD));
			}
		}
		GetDlgItem(IDC_LOG)->SetWindowText(str);
	}
}


void CTestCompressDlg::OnBnClickedSize()
{
	if (m_nCount >= MAX_COUNT) {
		AfxMessageBox(L"试用次数超过限制");
		exit(-1);
	}
	if (m_thread == nullptr) {
		m_nCount++;
		UpdateData(TRUE);
		m_bStartProcess = TRUE;//转换开始
		m_thread = new std::thread([this] {
			m_tsStart = timeGetTime();
			SetTimer(m_idTime, 100, NULL);

			int imageType = WXIMAGE_TYPE_ORIGINAL;
			int cur = m_cbType1.GetCurSel(); //正好和 WXIMAGE_TYPE_ 对应
			if (cur == 0) {
				imageType = WXIMAGE_TYPE_ORIGINAL;
			}
			else if (cur == 1) {
				imageType = WXIMAGE_TYPE_JPEG;
			}
			else if (cur == 2) {
				imageType = WXIMAGE_TYPE_WEBP;
			}
			else if (cur == 3) {
				imageType = WXIMAGE_TYPE_MOZJPEG;
			}
			int ret = CompressSizeU_FileToFile(m_strInput, m_strOutput,
				imageType, m_iFileSize, m_iWidth, m_iHeight);
			CString str;
			str.Format(L"CompressJpegU =%d", ret);
			this->SetWindowText(str);
			m_bStartProcess = FALSE;//转换结束
			});
	}
}

void CTestCompressDlg::OnBnClickedQuality()
{
	if (m_nCount >= MAX_COUNT) {
		AfxMessageBox(L"试用次数超过限制");
		exit(-1);
	}
	UpdateData(TRUE);
	if (m_thread == nullptr) {
		m_nCount++;
		UpdateData(TRUE);
		m_bStartProcess = TRUE;
		m_thread = new std::thread([this] {
			m_tsStart = timeGetTime();
			SetTimer(m_idTime, 100, NULL);
			int imageType = m_cbType2.GetCurSel(); //正好和 WXIMAGE_TYPE_ 对应
			int ret = CompressQualityU_FileToFile(m_strInput, m_strOutput,
				imageType, m_iQuality, m_iWidth, m_iHeight);
			CString str;
			str.Format(L"CompressQualityU[%d] = %d", imageType, ret);
			this->SetWindowText(str);
			m_bStartProcess = FALSE;
		});
	}
}




void CTestCompressDlg::OnBnClickedGetquality()
{
	UpdateData(TRUE);
	int nQuality = GetJpegQualityU(m_strInput);
	CString str;
	str.Format(L"Jpeg Quality = %d", nQuality);
	AfxMessageBox(str);
}

void CTestCompressDlg::OnBnClickedToPng()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	if (m_thread == nullptr) {
		m_nCount++;
		UpdateData(TRUE);
		m_bStartProcess = TRUE;
		m_thread = new std::thread([this] {
			m_tsStart = timeGetTime();
			SetTimer(m_idTime, 100, NULL);
			int imageType = WXIMAGE_TYPE_PNG; //正好和 WXIMAGE_TYPE_ 对应
			int ret = 0;
			//解码图像
			std::shared_ptr<void>bitmap = std::shared_ptr<void>(
				WXImage_LoadFromFileU(m_strInput),
				[](void* p) {  if (p) { WXImage_Unload(p); p = nullptr; } });
			if (bitmap.get()) {

				int      _nWidth  = WXImage_GetWidth(bitmap.get()); //图像宽度
				int      _nHeight = WXImage_GetHeight(bitmap.get());//图像高度
				uint8_t*   _pData  = WXImage_GetBits(bitmap.get());//解码图像数据
				int      _nPitch = WXImage_GetPitch(bitmap.get());//解码图像数据每行字节数量
				int      _nTypeRGB = WXImage_GetChannel(bitmap.get());//每个像素的数据位数
				uint8_t* _pIccData = WXImage_GetIccData(bitmap.get());//ICC 数据
				int _nIccSize = WXImage_GetIccSize(bitmap.get());//ICC 数据
				//int _nDotsPerMeterX = WXImage_GetDotsPerMeterX(bitmap.get());
				//int _nDotsPerMeterY = WXImage_GetDotsPerMeterY(bitmap.get());
				void* hanlde = HandlerCreate();
				int ret = RGBtoPNG(hanlde, 80, _nTypeRGB, _pData, _nWidth, _nHeight, _nPitch, _pIccData, _nIccSize);
				if (ret > 0) {
					uint8_t* tmp = new uint8_t[ret];
					HandlerGetData(hanlde, tmp);
					FILE* fout = _tfopen(m_strOutput, L"wb");
					if (fout) {
						fwrite(tmp, ret, 1, fout);
						fclose(fout);
					}
				}
			}

			CString str;
			str.Format(L"CompressQualityU[%d] = %d", imageType, ret);
			this->SetWindowText(str);
			m_bStartProcess = FALSE;
		});
	}
}


void CTestCompressDlg::OnBnClickedToJpg()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	if (m_thread == nullptr) {
		m_nCount++;
		UpdateData(TRUE);
		m_bStartProcess = TRUE;
		m_thread = new std::thread([this] {
			m_tsStart = timeGetTime();
			SetTimer(m_idTime, 100, NULL);
			int imageType = WXIMAGE_TYPE_JPEG; //正好和 WXIMAGE_TYPE_ 对应
			int ret = 0;
			//解码图像
			std::shared_ptr<void>bitmap = std::shared_ptr<void>(
				WXImage_LoadFromFileU(m_strInput),
				[](void* p) {  if (p) { WXImage_Unload(p); p = nullptr; } });
			if (bitmap.get()) {

				int      _nWidth = WXImage_GetWidth(bitmap.get()); //图像宽度
				int      _nHeight = WXImage_GetHeight(bitmap.get());//图像高度
				uint8_t* _pData = WXImage_GetBits(bitmap.get());//解码图像数据
				int      _nPitch = WXImage_GetPitch(bitmap.get());//解码图像数据每行字节数量
				int      _nTypeRGB = WXImage_GetChannel(bitmap.get());//每个像素的数据位数
				uint8_t* _pIccData = WXImage_GetIccData(bitmap.get());//ICC 数据
				int _nIccSize = WXImage_GetIccSize(bitmap.get());//ICC 数据
				//int _nDotsPerMeterX = WXImage_GetDotsPerMeterX(bitmap.get());
				//int _nDotsPerMeterY = WXImage_GetDotsPerMeterY(bitmap.get());
				void* hanlde = HandlerCreate();
				int ret = RGBtoJPG(hanlde, 80, _nTypeRGB, _pData, _nWidth, _nHeight, _nPitch, _pIccData, _nIccSize);
				if (ret > 0) {
					uint8_t* tmp = new uint8_t[ret];
					HandlerGetData(hanlde, tmp);
					FILE* fout = _tfopen(m_strOutput, L"wb");
					if (fout) {
						fwrite(tmp, ret, 1, fout);
						fclose(fout);
					}
				}
			}

			CString str;
			str.Format(L"CompressQualityU[%d] = %d", imageType, ret);
			this->SetWindowText(str);
			m_bStartProcess = FALSE;
			});
	}
}

void CTestCompressDlg::OnBnClickedReadIcc()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	int src_size = 0;
	std::shared_ptr<uint8_t>src_buffer = _ReadFile(m_strInput, &src_size);
	if (src_buffer) {
		int icc_size = ReadICC(nullptr, src_buffer.get(), src_size);

		CString str;
		if (icc_size) {
			str.Format(L"icc size %d", icc_size);
		}
		else {
			str = L"No icc";
		}
		AfxMessageBox(str);
	}

}
