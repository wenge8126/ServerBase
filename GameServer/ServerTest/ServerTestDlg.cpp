﻿
// ServerToolDlg.cpp: 实现文件
//

#include "framework.h"
#include "ServerTool.h"
#include "ServerTestDlg.h"
#include "afxdialogex.h"
#include "ToolActor.h"
#include "CommonDefine.h"
#include "XlsxTableLoader.h"
#include "ExportBase.h"
#include "FileDataStream.h"

#include "SDK.h"
#include "ServerEvent.h"

#ifdef _DEBUG
//#define new DEBUG_NEW
#endif


// CServerToolDlg 对话框



CServerTestDlg::CServerTestDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SERVERTOOL_DIALOG, pParent)
	, mGateIP(_T(""))
	, mConfigPath(_T(""))
	, mGatePort(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}



void CServerTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_GATE_IP, mGateIP);
	DDX_Text(pDX, IDC_EDIT_CONFIG_PATH, mConfigPath);
	DDX_Text(pDX, IDC_EDIT_GATE_PORT, mGatePort);
	DDX_Control(pDX, IDC_EDIT_INFO, mEditInfo);
}

BEGIN_MESSAGE_MAP(CServerTestDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &CServerTestDlg::OnBnClickedButtonConnect)
	ON_BN_CLICKED(IDC_BUTTON_CREATE_DB, &CServerTestDlg::OnBnCreateDB)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON3, &CServerTestDlg::OnBnClickedTestCreateAccount)
	ON_BN_CLICKED(IDC_BUTTON_EXPORT_RUN_CONFIG, &CServerTestDlg::OnBnExportRunConfig)
END_MESSAGE_MAP()

CServerTestDlg *gpToolDlg = NULL;

class ServerLog : public SaveTableLog
{
public:
	ServerLog() {}
	~ServerLog() {}

	virtual void logVa(va_list &vaList, const char*info) override
	{
		mLock.lock();

		//AString temp;
		//temp.Format(vaList, info);

		static  char szLogBuffer[8096];
		_vsnprintf_s(szLogBuffer, 8096 - 1, (const char*)info, vaList);

		va_end(vaList);
		//UInt64 now = TimeManager::SysMicrosecond();
		//UInt64 second = (UInt64)(now * 0.000001);

		//temp.Format("%s %d > %s\r\n", TimeManager::ToTimeString(second).c_str(), (int)(now*0.001 - second * 1000), szLogBuffer);
		int len = strlen(szLogBuffer);
		szLogBuffer[len] = '\r';
		szLogBuffer[len + 1] = '\n';
		szLogBuffer[len + 2] = '\0';

		Print(szLogBuffer);

		mLock.unlock();
	}
	//virtual void logVa(bool bPrint, va_list &va, const char*info) override;

	virtual void Print(const char *szInfo) override
	{
		mLock.lock();
		CString str;
		gpToolDlg->mEditInfo.GetWindowText(str);
		str += "\r\n";
		TimeManager::GetMe().SetTime();
		str += TimeManager::GetMe().GetTime().c_str();
		str += " > ";
		str += szInfo;
		gpToolDlg->mEditInfo.SetWindowText(str);

		gpToolDlg->mEditInfo.LineScroll(gpToolDlg->mEditInfo.GetLineCount());
		mLock.unlock();
	}

public:
	CsLock	mLock;
};

// CServerToolDlg 消息处理程序

BOOL CServerTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	gpToolDlg = this;

	TableTool::SetPrintLog(new ServerLog());

	TableTool::SetLog(new TableLog("./Log/ServerTool.log"));
	TableTool::SetErrorLog(new TableLog("./Error/ServerTool.log") );

	new TableManager();

	mReceiveData.resize(2048);
	mUPDNet.Bind("0.0.0.0", 2020);

	mToolConfig = TableManager::getSingleton().CreateNewTable("Config");
	mToolConfig->LoadCSV("./RunConfig/ServerToolConfig.csv");

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	mGateIP = "127.0.0.1";
	mGatePort = 10000;
	mConfigPath = "D:/Home_86/Config";

	UpdateData(FALSE);

	mActorManager = MEM_NEW NetCloud::ActorManager("127.0.0.1", 11003, 10, 2);

	mActorManager->RegisterActor(Actor_ToolWorker, MEM_NEW DefineActorFactory<ToolActor>());
	mActorManager->RegisterComponect("HttpReqeustComponent", MEM_NEW EventFactory< HttpReqeustComponent>());
	mToolActor = mActorManager->CreateActor(Actor_ToolWorker, 12);

	SetWindowText("模拟客户端");

	SetTimer(1, 10, NULL);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CServerTestDlg::OnPaint()
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
HCURSOR CServerTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void _RunConnectGate(CServerTestDlg *pDlg)
{
	pDlg->AsyncConnectGate();
}

void _RunTestConnectServer(CServerTestDlg *p);

void CServerTestDlg::OnBnClickedButtonConnect()
{
	// TODO: 在此添加控件通知处理程序代码

	CoroutineTool::AsyncCall(_RunTestConnectServer, this);

}

void CServerTestDlg::AsyncConnectGate()
{
	//if (!mActorManager->mNetNode->AwaitConnectGate(NetCloud::Address((LPCSTR)mGateIP, mGatePort)))
	//{
	//	ERROR_LOG("Connect gate fail");
	//	return;
	//}
	//else
	//	LOG("Connect succeed gate");
}

void _RunCreateDB(CServerTestDlg *pDlg)
{
	pDlg->AsyncCreateDB();
}


void CServerTestDlg::OnBnCreateDB()
{
	// TODO: 在此添加控件通知处理程序代码
	CoroutineTool::AsyncCall(_RunCreateDB, this);
}



void CServerTestDlg::AsyncCreateDB()
{
	ClearLog();
	UpdateData(TRUE);
	AString dbConfigFile = "DBList";
	//if (mToolConfig)
	//	dbConfigFile = mToolConfig->GetValue("account_config", 1).c_str();

	//if (dbConfigFile.length() <= 0)
	//{
	//	MessageBox("未配置DB > RunConfig/ToolConfig.csv>account_config", "", MB_ICONERROR);
	//	return;
	//}

	AString listFileName;
	listFileName.Format("%s/%s.xlsx", (LPCTSTR)mConfigPath, dbConfigFile.c_str());

	// 打开ConfigList
	AutoTable  configListTable = XlsxTableLoader::LoadXlsx(listFileName, dbConfigFile);
	if (!configListTable)
	{
		CString info;
		info.Format("配置目录内不存在: %s, sheet %s", listFileName.c_str(), dbConfigFile.c_str());

		MessageBox(info, "", MB_ICONERROR);
		return;
	}

	// TODO: 在此添加控件通知处理程序代码
	int bOK = MessageBox("新建帐号DB前需要清空当前所有帐号DB表, 是否重建帐号DB?", "特别提醒", MB_OKCANCEL | MB_ICONQUESTION);
	if (bOK != IDOK)
		return;

	RQ_CreateDBTable	msg;
	msg.mDBConfigData = MEM_NEW NiceData();
	for (auto it = configListTable->GetRecordIt(); *it; ++(*it))
	{
		ARecord record = it->Record();

		const char *szTableName = record["INDEX"].c_str();

		AString fileName;
		fileName.Format("%s/%s.xlsx", (LPCTSTR)mConfigPath, record["XLSX"].c_str());
		AutoTable table = XlsxTableLoader::LoadXlsx(fileName, record["SHEET"].string());
		if (table)
		{
			table->SetTableName(szTableName);

			msg.mDBConfigData[szTableName] = table.getPtr();

		}
		else
			LOG("Fail load xlsx  %s, sheet %s, index %s", record["XLSX"].c_str(), record["SHEET"].c_str(), record["INDEX"].c_str());
	}
	msg.mExportCodePath = "D:/Home_86/Server/GameServer/Common";
	RS_CreateDBTable resp;
	auto result = mToolActor->Await({ Actor_DBWorker, 1 }, msg, resp, 16000);
	if (result)
	{
		LOG("-----------------------------------------------------\r\n%s\r\n------------------------------------------", resp.mResultInfo.c_str());
	}
	else
		LOG("CExcelToolDlg : request create DB fail, may be connect fail");


	LOG("********************** finish all create db ******************************");
}



void CServerTestDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nIDEvent == 1)
	{
		mActorManager->Process();
		mActorManager->LowProcess();
		CoroutineTool::CheckFinish();
		UInt64 addr;
		mReceiveData.clear();
		if (mUPDNet.ProcessReceive(&mReceiveData, addr, false))
		{
			AutoNice d = MEM_NEW NiceData();
			d->FullJSON((char*)mReceiveData.data());
			AString localIP = d["LOCAL"];
			int port = d["PORT"];
			NetAddress addr(localIP.c_str(), port);
			UInt64 key = addr;
			if (mNetInfoList.exist(key))
			{
				LOG("严重错误, IP端口重复 :\r\n%s===============\r\n%s", mNetInfoList.find(key)->dump().c_str(), d->dump().c_str());
			}
			else
				LOG("%s", d->dump().c_str());
			mNetInfoList.insert(key, d);
			mAllNetList.push_back(d);
		}

		TimerCenter::GetMe()->Process();
	}

	CDialogEx::OnTimer(nIDEvent);
}

void _RunTestAccount(CServerTestDlg *p)
{
	RQ_CheckAndCreateAccount msg;
	msg.mAccount = "Testx";
	msg.mPassword = "333444";
	msg.mServerID = 1;
	RS_CheckAndCreateAccount resp2;
	bool b = p->mToolActor->Await( { Actor_Account, 1 }, msg, resp2, 100000);
	auto resp = &resp2;
	if (b)
	{
		LOG("Response account : \r\n%s", resp->dump().c_str());

		//if (resp->mID > 0)
		//{
		//	if (!p->mActorManager->mNetNode->AwaitCheckExistUnit({ Actor_Player, resp->mID }, 10000))
		//	{
		//		RQ_CreatePlayerActor  msg;
		//		msg.mPlayerID = resp->mID;
		//		auto resp = p->mToolActor->Await<RS_CreatePlayerActor>(msg, { Actor_DBWorker, 1 }, 100000);
		//		if (resp)
		//			LOG("Create player %s", resp->dump().c_str());
		//	}

		//	RQ_PlayerBaseData msg;
		//	
		//	auto responseMsg = p->mToolActor->Await<RS_PlayerBaseData>(msg, { Actor_Player, resp->mID }, 10000);
		//	if (responseMsg)
		//		LOG("Player %s data : \r\n%s", STRING(resp->mID), responseMsg->dump().c_str());
		//}

	}
	else
		ERROR_LOG("Reqest account fail");
}

//-------------------------------------------------------------------------
class Net_Export_H DefaultClientNet : public IOCPClientNet
{
public:
	DefaultClientNet(int threadNum = _IOCP_THREAD_NUM)
		: IOCPClientNet()
	{
		mEventCenter = MEM_NEW Logic::EventCenter();
	}
	~DefaultClientNet()
	{
		mEventCenter._free();
	}

	virtual Logic::tEventCenter* GetEventCenter(void) const { return ((DefaultClientNet*)this)->mEventCenter.getPtr(); }

	virtual void Process() override
	{
		mEventCenter->ProcessEvent();
		IOCPClientNet::Process();
	}

	int GetSafeCode() override { return 11; }

public:
	AutoEventCenter			mEventCenter;
};

bool gbTag = false;

class TestServerEvent : public Logic::tServerEvent
{
public:
	virtual void _OnResp(AutoEvent &respEvent) override
	{
		LOG("********** Test\r\n%s", respEvent->GetData().dump().c_str());
		gbTag = true;
	}

protected:
private:
};

class CheckAccountMsg : public Logic::tServerEvent
{
public:
	virtual void _OnResp(AutoEvent &respEvent) override
	{
		LOG("********** Test\r\n%s", respEvent->GetData().dump().c_str());
		gbTag = true;
	}

protected:
private:
};

void _RunTestConnectServer(CServerTestDlg *p)
{
	//Hand< HttpReqeustComponent> http = p->mToolActor->GetComponent("HttpReqeustComponent");
	//AString resp;
	//http->AwaitRequest("http://127.0.0.1:5000?x=99&y=66", resp);
	//LOG("Http result : %s", resp.c_str());

	AutoNet testNet = MEM_NEW DefaultClientNet();
	Hand<IOCPClientNet> net = testNet;
	net->Connect("127.0.0.1", 4001, 10000);

	net->GetEventCenter()->RegisterEvent("CS_ActorMsg", MEM_NEW Logic::EventFactory< TestServerEvent>());
	net->GetEventCenter()->RegisterEvent("CheckAccountMsg", MEM_NEW Logic::EventFactory< CheckAccountMsg>());

	while (true)
	{
		net->Process();
		net->LowProcess(100);
		if (net->IsConnected())
			break;

		tTimer::AWaitTime(100);

		//TimerCenter::GetMe()->Process();
	}
	NOTE_LOG("连接成功");

	AutoEvent evt = net->GetClientConnect()->StartEvent("CheckAccountMsg");
	evt["ACCOUNT"] = "yyyy";
	evt["PASSWORD"] = "zzz";	
	evt->DoEvent();

		while (true)
	{
		net->Process();
		//tTimer::AWaitTime(100);
		net->LowProcess(1);
		tTimer::AWaitTime(100);
		if (gbTag)
			break;
	}
	gbTag = false;

	NOTE_LOG("完成帐号测试");
}



//	AutoEvent evt = net->GetClientConnect()->StartEvent("CS_ActorMsg");
//	evt["MSG_NAME"] = "RQ_CheckAndCreateAccount";
//	evt["UNIT_TYPE"] = (int)Actor_Account;
//	evt["UNIT_ID"] = 1;
//	evt->DoEvent();
//
//	while (true)
//	{
//		net->Process();
//		//tTimer::AWaitTime(100);
//		net->LowProcess(1);
//		TimeManager::Sleep(10);
//		if (gbTag)
//			break;
//	}
//	gbTag = false;
//}

void CServerTestDlg::OnBnClickedTestCreateAccount()
{
	//CoroutineTool::AsyncCall(_RunTestHttp, this);

	CoroutineTool::AsyncCall(_RunTestAccount, this);

}




void CServerTestDlg::OnBnExportRunConfig()
{
	ClearLog();

	CString configPath = mConfigPath;
	//mEditConfigPath.GetWindowText(configPath);

	AString fileName;
	fileName.Format("%s/RunConfigProtocol.txt", (LPCTSTR)configPath);
	AString err;
	AutoNice info = GenerateProtocol(fileName, "", "../GameServer/Common", "RunConfigStruct", false, err);

	LOG("%s", err.c_str());

	if (info)
		MessageBox("成功生成运行配置协议");

	//mEditMakeConfigPath.GetWindowText(configPath);

	//AString path = (LPCSTR)configPath;
	//path.replace("/Config", "/RunConfig");

	//AString configFileName = path;
	//configFileName += "/RunInfo.json";

	//// 读取当前的
	//FileDataStream  f(configFileName.c_str(), FILE_CREATE);
	//if (f)
	//{
	//	AString infoString = info->ToJSON();
	//	f._write((void*)infoString.c_str(), infoString.length());
	//}
}
