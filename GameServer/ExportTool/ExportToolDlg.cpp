
// ExportToolDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "ExportTool.h"
#include "ExportToolDlg.h"
#include "afxdialogex.h"

#include "ServerToolMsg.h"
#include "CommonDefine.h"
#include "TimeManager.h"
#include "Timer.h"
#include "FileDataStream.h"
#include "ResoursePack.h"
#include "Md5Tool.h"

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif


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
public:
	
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


class WinLog : public TableLog
{
	CExportToolDlg *gpExcelToolDlg = NULL;

public:
	WinLog(CExportToolDlg *pDlg, const char *szLogFile)
		: TableLog(szLogFile)
		, gpExcelToolDlg(pDlg)
	{

	}

public:
	virtual void logVa(va_list &vaList, const char*info) override
	{
		AString strInfo;
		strInfo.Format(vaList, info);

		CString str;
		gpExcelToolDlg->mEditInfo.GetWindowText(str);
		str += "\r\n";
		TimeManager::GetMe().SetTime();
		str += TimeManager::GetMe().GetTime().c_str();
		str += " > ";
		str += strInfo.c_str();
		gpExcelToolDlg->mEditInfo.SetWindowText(str);

		gpExcelToolDlg->mEditInfo.LineScroll(gpExcelToolDlg->mEditInfo.GetLineCount());

		//TABLE_LOG(strInfo.c_str());
		//((ThreadLog*)mLog)->logVa(vaList, info);
	}

	virtual void logVa(bool bPrint, va_list &va, const char*info) override
	{
		logVa(va, info);
	}
};


// CExportToolDlg 对话框



CExportToolDlg::CExportToolDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_EXPORTTOOL_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CExportToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_ACCOUNT_IP, mWebIP);
	DDX_Control(pDX, IDC_EDIT_ACCOUNT_PORT, mWebPort);
	DDX_Control(pDX, IDC_EDIT_LOG, mEditInfo);
	DDX_Control(pDX, IDC_EDIT_CONFIG_PATH, mEditConfigPath);
	DDX_Control(pDX, IDC_BUTTON1, mMakeConfigButton);
	DDX_Control(pDX, IDC_EDIT_TARGET_CONFIG_PATH, mEditMakeConfigPath);
	DDX_Control(pDX, IDC_EDIT_C_PATH, mEditMakeCPath);
	DDX_Control(pDX, IDC_EDIT_TS_PATH, mEditMakeJSPath);
}

BEGIN_MESSAGE_MAP(CExportToolDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON1, &CExportToolDlg::OnBnExportConfigCode)
	ON_BN_CLICKED(IDC_BUTTON_UPLOAD, &CExportToolDlg::OnBnUploadSaveConfig)
	ON_BN_CLICKED(IDC_BUTTON5, &CExportToolDlg::OnBnMakeConfigPacket)
END_MESSAGE_MAP()


// CExportToolDlg 消息处理程序

BOOL CExportToolDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();


	Allot::setLogFile("ExportTool");
	TableTool::SetLog(new WinLog(this, "Log/ExportTool.log"));
	TableTool::SetErrorLog(new WinLog(this, "Log/ExportToolError.log"));

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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	//ShowWindow(SW_MAXIMIZE);

	//ShowWindow(SW_MINIMIZE);

	// TODO: 在此添加额外的初始化代码

	mHttpClient.InitThread();

	SetTimer(1, 10, NULL);

	AutoTable configTable = TableManager::getSingleton().CreateNewTable("ToolConfig");
	if (!configTable->LoadCSV("RunConfig/ToolConfig.csv"))
	{
		MessageBox("No exist config > RunConfig/ToolConfig.csv");
		return TRUE;
	}

	SetText(mWebIP, "127.0.0.1");
	SetText(mWebPort, "1080");

	mToolConfig = configTable;
	mEditConfigPath.SetWindowText(mToolConfig->GetValue("config_path", 1).c_str());
	mEditMakeJSPath.SetWindowText(mToolConfig->GetValue("ts_path", 1).c_str());
	mEditMakeCPath.SetWindowText(mToolConfig->GetValue("c_path", 1).c_str());
	mEditMakeConfigPath.SetWindowText(mToolConfig->GetValue("target_path", 1).c_str());

	//mEditDBWorkerID.SetWindowText(mToolConfig->GetValue("db_config", "PORT").c_str());

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CExportToolDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CExportToolDlg::OnPaint()
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
HCURSOR CExportToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CExportToolDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nIDEvent == 1)
	{
		mHttpClient.Process();

		TimerCenter::GetMe()->Process();
		CoroutineTool::CheckFinish();
	}
	CDialogEx::OnTimer(nIDEvent);
}


void CExportToolDlg::OnBnExportConfigCode()
{
	GenerateConfigPacketAndCode(true);
}


void CExportToolDlg::OnBnUploadSaveConfig()
{
	CoroutineTool::AsyncCall([=]()
	{
		AString targetPath = GetText(mEditMakeConfigPath);
		targetPath += "/t_configtable.pak";

		FileDataStream		pakFile(targetPath.c_str(), FILE_READ);
		Auto<DataBuffer> data = pakFile.readAllData();
		data->seek(0);
		EasyResourcesPack  configPacket;
		if (!configPacket.load(data.getPtr()))
		{
			MessageBox("恢复加载配置包失败");
			return;
		}

		DataTableManager	tableMgr;
		if (!tableMgr.LoadTable(&configPacket, CONFIG_LIST_INDEX))
		{
			MessageBox("加载配置表格失败");
			return;
		}

		for (auto it = tableMgr.GetTableMap().begin(); it; ++it)
		{				
			AString tableName = it.key();
			Auto<DataBuffer> tableData = configPacket.loadResource(tableName.c_str());

			AutoData zipTableData = MEM_NEW DataBuffer();
			if (tableData->ZipData(zipTableData) <= 0)
			{
				ERROR_LOG("Zip data fail : %s", tableName.c_str());
				continue;
			}

			MD5 md(tableData->data(), tableData->dataSize());
			

			TR_UpdateSaveResource x;
			x.mIndexName = tableName;
			x.mResourceData = zipTableData;
			x.mMD5 = md.toString();

			auto re = Reqeust<RT_ReponseSaveResult>(Actor_AccountCenter, 1, x);
			if (re)
				NOTE_LOG("Reqeuest save %s ok : %s", tableName.c_str(), re->dump().c_str())
			else
				ERROR_LOG("Request fail %s", x.GetMsgName());
		}

		

	}
	);
}




void CExportToolDlg::OnBnMakeConfigPacket()
{
	GenerateConfigPacketAndCode(false);
}
