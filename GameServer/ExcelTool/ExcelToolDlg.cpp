
// ExcelToolDlg.cpp: 实现文件
//


#include "framework.h"
#include "ExcelTool.h"
#include "ExcelToolDlg.h"
#include "afxdialogex.h"

#include <iostream>
#include <afxdb.h>
#include <odbcinst.h>

#include "TableLog.h"
#include "Common.h"

#include "XlsxTableLoader.h"
#include "FileDataStream.h"

#include "TableManager.h"
#include "GenerateCodeTool.h"
#include "GenerateCodeTS.h"
#include "Md5Tool.h"
#include "ResoursePack.h"
#include "DataBuffer.h"

#include "BigDataEvent.h"
#include "TimeManager.h"
#include "CommonDefine.h"
#include "IndexBaseTable.h"
#include "base64.h"
#include "DBDefine.h"
#include "MySqlDBTool.h"
#include "ConfigEnum.h"
#include "RunConfig.h"
#include "Vesion.h"
#include "SystemInfo.h"

using namespace std;

#define DB_WORKER_ID  {Actor_DBWorker, GetWorkerID()}

#define ADDRESS_LIST_CSV		"AddressList.csv"

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif

CExcelToolDlg *gpExcelToolDlg = NULL;

int GetWorkerID()
{
	CString str;
	gpExcelToolDlg->mEditDBWorkerID.GetWindowText(str);
	return TOINT((LPCSTR)str);
}

class UploadConfigDataBigEvent : public NetCloud::tRequestEvent
{
public:
	virtual bool _DoEvent() override
	{
		LOG("********** Upload config data, send big data , Wait a minute ... ");
		WaitTime(360);
		gpExcelToolDlg->mUploadConfigButton.EnableWindow(FALSE);
		return true;
	}

	bool Send(int nType /* = 0 */, int nTarget /* = 0 */) override
	{
		return true;
	}

	virtual void _OnResp(AutoEvent &respEvent) 
	{
		if (respEvent["RESULT"])
			LOG("Succeed upload config\r\n%s", respEvent["DLL_INFO"].c_str())
		else
		{
			LOG("Fail upload config > %s, load dll > %s", respEvent["ERROR"].c_str(), respEvent["DLL_INFO"].c_str());			
		}

		LOG("%s", respEvent->GetData().dump().c_str());

		gpExcelToolDlg->mUploadConfigButton.SetWindowText("上传配置数据");
		gpExcelToolDlg->mUploadConfigButton.EnableWindow(TRUE);
	}

	virtual bool _OnEvent(AutoEvent &hEvent) override
	{
		Hand< Cloud_SendDataPartEvent> evt = hEvent;
		if (!evt)
			return Logic::tServerEvent::_OnEvent(hEvent);

		if (evt->IsSendFailed())
		{
			LOG("Upload fail, send data fail");
			gpExcelToolDlg->mUploadConfigButton.SetWindowText("上传配置数据");
			gpExcelToolDlg->mUploadConfigButton.EnableWindow(TRUE);
			gpExcelToolDlg->MessageBox("远程服务器上可能开启了工具, 不可使用相同的本地127地址, 否则远程工具上传失败");
		}
		else
		{
			int rate = evt->GetSendProgress();
			AString info;
			info.Format("Upload %d/100", rate);
			gpExcelToolDlg->mUploadConfigButton.SetWindowText(info.c_str());
		}
		return true;
	}
};

class TestBroatcastMsg : public tMsgEvent
{
	virtual bool _DoEvent() override
	{
		LOG("********** ExcelTool do receive broatcast msg event > %d:%d \r\n %s", (int)mUnit->GetID().type, (int)mUnit->GetID().id, GetData().dump().c_str());
		return true;
	}
};

// 上传DLL
class UploadPluginDllBigEvent : public NetCloud::tRequestEvent
{
public:
	virtual bool _DoEvent() override
	{
		LOG("********** Upload plugin dll, send big data , Wait a minute ... ");
		WaitTime(360);
		gpExcelToolDlg->mButtonUploadPlugin.EnableWindow(FALSE);
		return true;
	}

	bool Send(int nType /* = 0 */, int nTarget /* = 0 */) override
	{
		return true;
	}

	virtual void _OnResp(AutoEvent &respEvent)
	{
		if (respEvent["RESULT"])
			LOG("Succeed upload plugin\r\n%s", respEvent["DLL_INFO"].c_str())
		else
			LOG("Fail upload plugin > %s", respEvent["ERROR"].c_str());

		LOG("%s", respEvent->GetData().dump().c_str());

		gpExcelToolDlg->mButtonUploadPlugin.SetWindowText("上传逻辑插件");
		gpExcelToolDlg->mButtonUploadPlugin.EnableWindow(TRUE);
	}

	virtual bool _OnEvent(AutoEvent &hEvent) override
	{
		Hand< Cloud_SendDataPartEvent> evt = hEvent;
		if (!evt)
			return Logic::tServerEvent::_OnEvent(hEvent);

		if (evt->IsSendFailed())
		{
			LOG("Upload fail, send data fail");
			gpExcelToolDlg->mButtonUploadPlugin.SetWindowText("上传逻辑插件");
			gpExcelToolDlg->mButtonUploadPlugin.EnableWindow(TRUE);
		}
		else
		{
			int rate = evt->GetSendProgress();
			AString info;
			info.Format("Upload %d/100", rate);
			gpExcelToolDlg->mButtonUploadPlugin.SetWindowText(info.c_str());
		}
		return true;
	}
};

class ToolInfo : public Base<ToolInfo>
{
public:
	void OnResponse(NetCloud::RequestDBOperatePak *requestPak, AutoNice responseData, int nResult)
	{
		requestPak->mParamData.seek(0);
		AString opName, tableName, recordKey;
		requestPak->mParamData.readString(opName);
		requestPak->mParamData.readString(tableName);
		requestPak->mParamData.readString(recordKey);

		AString info;
		if (nResult == eNoneError)
			info.Format("ToolInfo: DB opereate : %s > result Sueeccd >\r\n%s", opName.c_str(), responseData->dump().c_str());
		else
			info.Format("ToolInfo : DB opereate : %s > result Fail : %d", opName.c_str(), nResult);

		LOG(info.c_str());
	}

	void OnUploadResourceResponse(NetCloud::RequestDBOperatePak *requestPak, AutoNice responseData, int nResult)
	{
		LOG("Upload resource %s result %s : %d", responseData["NAME"].c_str(), responseData["RESULT"] ? "succeed" : "fail", nResult);
	}

	void AwaitCreateDB()
	{
		gpExcelToolDlg->AsyncNewCreateDB();
	}

	void AwaitCreateAccountDB()
	{
		gpExcelToolDlg->AsyncNewCreateAccountDB();
	}

	void AwaitApplyConfig()
	{
		gpExcelToolDlg->AsyncApplyUpdateConfig();
	}

	void AwaitApplayUpdatePluginDll()
	{
		gpExcelToolDlg->AsyncApplayUpdatePluginDll();
	}

	void RestartLogicServer()
	{
/*		AutoEvent evt = gpExcelToolDlg->mBigMsgUnit->StartEvent("RestartLogicServer", DB_WORKER_ID);
		HAsyncRequestEvent request = evt;

		CString pass, serverID;
		gpExcelToolDlg->mEditCloseServerPassword.GetWindowText(pass);
		gpExcelToolDlg->mEditUpdateServerID.GetWindowText(serverID);

		AString checkInfo;
		checkInfo.Format("CLOSE2021-%s-%s", (LPCSTR)serverID, (LPCSTR)pass);
		MD5 md(checkInfo.c_str());
		evt["PASSWORD"] = md.toString();

		AutoEvent resultEvent = request->AwaitReqeust(gpExcelToolDlg->mBigMsgUnit, DB_WORKER_ID, 60000);
		if (resultEvent)
		{
			if (resultEvent["RESULT"])
				LOG("Reload result > \r\n %s", resultEvent->GetData().dump().c_str())
			else
				LOG("Fail reload result > %s", resultEvent["ERROR"].c_str());
		}
		else
			LOG("Fail reload request send fail, check net connect");*/	
	}

	void OnConnectGateCall(HandConnect conn, bool bConnect)
	{
		LOG("Connect gate [%s:%d] %s", conn->GetIp(), conn->GetPort(), bConnect ? "succeed" : "disconnect");
	}

};

static Hand<ToolInfo> msTool;


class WinLog : public TableLog
{
public:
	WinLog(const char *szLogFile)
		: TableLog(szLogFile)
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




CExcelToolDlg::CExcelToolDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_EXCELTOOL_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CExcelToolDlg::~CExcelToolDlg()
{
	Clear();
	//delete TableManager::getSingletonPtr();
}

void CExcelToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_CONFIG_PATH, mEditConfigPath);
	DDX_Control(pDX, IDC_EDIT_LOG, mEditInfo);
	DDX_Control(pDX, IDC_EDIT_TS_PATH, mEditMakeJSPath);
	DDX_Control(pDX, IDC_EDIT_C_PATH, mEditMakeCPath);
	DDX_Control(pDX, IDC_EDIT_TARGET_CONFIG_PATH, mEditMakeConfigPath);
	DDX_Control(pDX, IDC_BUTTON1, mMakeConfigButton);
	DDX_Control(pDX, IDC_BUTTON_UPLOAD, mUploadConfigButton);
	DDX_Control(pDX, IDC_EDIT_DB_WORKER_ID, mEditDBWorkerID);
	DDX_Control(pDX, IDC_EDIT_IP, mEditGateIP);
	DDX_Control(pDX, IDC_EDIT_PORT, mEditGatePort);
	DDX_Control(pDX, IDC_EDIT_HTTP_PARAM, mEditHttpParam);
	DDX_Control(pDX, IDC_BUTTON_APPLY_CONFIG, mButtonApplyConfig);
	DDX_Control(pDX, IDC_BUTTON_UPLOAD_PLUGIN_DLL, mButtonUploadPlugin);
	DDX_Control(pDX, IDC_BUTTON_UPLOAD_RUN_PACKET, mButtonUploadRunPacket);
	DDX_Control(pDX, IDC_EDIT_SAFT_CODE, mEditSaftCode);
	DDX_Control(pDX, IDC_EDIT_SERVER_PATH, mEditRunServerPath);
	DDX_Control(pDX, IDC_BUTTON_CREATE_SERVER, mButtonCreateServerRun);
	DDX_Control(pDX, IDC_EDIT_LOCAL_IP, mEditLocalIP);
	DDX_Control(pDX, IDC_EDIT_WSS_ADDRESS, mEditWssAddress);
	DDX_Control(pDX, IDC_EDIT_MYSQL_IP, mEditMysqlIP);
	DDX_Control(pDX, IDC_EDIT_MYSQL_USER, mEditMysqlUser);
	DDX_Control(pDX, IDC_EDIT_MYSQL_PASSWORD, mEditMysqlPassword);
	DDX_Control(pDX, IDC_EDIT_SERVER_NAME, mEditGameServerName);
	DDX_Control(pDX, IDC_EDIT_SSL_CRT, mEditSSLKeyFile);
	DDX_Control(pDX, IDC_EDIT_ACCOUNT_IP, mEditAccountIP);
	DDX_Control(pDX, IDC_EDIT_ACCOUNT_PORT, mEditAccountPort);
	DDX_Control(pDX, IDC_EDIT_ACCOUNT_SAFT_CODE, mEditAccountSafeCode);
	DDX_Control(pDX, IDC_BUTTON_UPLOAD_GLOBAL_CONFIG, mButtonGlobalConfig);
	DDX_Control(pDX, IDC_SELECT_HTTPS, mSelectHttps);
	DDX_Control(pDX, IDC_EDIT_UPDATE_PASSWORD, mEditCloseServerPassword);
	DDX_Control(pDX, IDC_EDIT_ACCOUNT_DNS, mEditUpdateServerID);
	DDX_Control(pDX, IDC_BUTTON_CONNECT_DB_INFO, mButtonShowDBConnectInfo);
	DDX_Control(pDX, IDC_EDIT_LOGIP, mLogInfo);
	DDX_Control(pDX, IDC_EDIT_MYSQL_BACK_IP, mEditBackSqlIP);
}

CString CExcelToolDlg::SelectPath()
{
	CString m_FileDir;
	BROWSEINFO bi;
	ZeroMemory(&bi, sizeof(BROWSEINFO));
	bi.hwndOwner = m_hWnd;
	bi.ulFlags = BIF_RETURNONLYFSDIRS;
	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
	BOOL bRet = FALSE;
	TCHAR szFolder[MAX_PATH * 2];
	szFolder[0] = ('\0');
	if (pidl)
	{
		if (SHGetPathFromIDList(pidl, szFolder))
			bRet = TRUE;
		IMalloc *pMalloc = NULL;
		if (SUCCEEDED(SHGetMalloc(&pMalloc)) && pMalloc)
		{
			pMalloc->Free(pidl);
			pMalloc->Release();
		}
	}
	if (bRet == FALSE)
		return "";
	return szFolder;
}


void CExcelToolDlg::Clear()
{
	msTool._free();
	mAccountNode._free();
	mUserNode._free();
	TableTool::CleanUp();
}

BEGIN_MESSAGE_MAP(CExcelToolDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CExcelToolDlg::OnBnExportTable)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_CONFIG, &CExcelToolDlg::OnSelectConfigPath)
	ON_BN_CLICKED(IDC_BUTTON_UPLOAD, &CExcelToolDlg::OnBnUpload)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_CONFIG2, &CExcelToolDlg::OnBnSelectTSPath)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_CONFIG3, &CExcelToolDlg::OnBnSelectCPath)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_CONFIG4, &CExcelToolDlg::OnBnSelectTargetConfigPath)
	ON_BN_CLICKED(IDC_BUTTON_GENERATE_SQL_DB, &CExcelToolDlg::OnBnNewCreateDB)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_GENERATE_PROTOC, &CExcelToolDlg::OnBnGenerateProtocol)
	ON_BN_CLICKED(IDC_BUTTON_RELOAD_PLUGIN_DLL, &CExcelToolDlg::OnBnRestartLogicServer)
	ON_BN_CLICKED(IDC_BUTTON_RECONNECT, &CExcelToolDlg::OnBnReconnect)
	
	ON_BN_CLICKED(IDC_BUTTON_GENERATE_PROTOC2, &CExcelToolDlg::OnBnClickedButtonGenerateProtoc2)
	ON_BN_CLICKED(IDC_BUTTON_CREATE_ACCOUNT_DB, &CExcelToolDlg::OnBnCreateAccountDB)
	ON_BN_CLICKED(IDC_BUTTON_GENRATE_RUNCONFIG, &CExcelToolDlg::OnBnGenrateRunconfig)
	ON_BN_CLICKED(IDC_BUTTON_UPDATE_RUNCONFIG, &CExcelToolDlg::OnBnUpdateRunconfig)
	ON_BN_CLICKED(IDC_MAKE_CONFIG_PACKET, &CExcelToolDlg::OnBnMakeConfigPacket)
	ON_BN_CLICKED(IDC_BUTTON_HTTP_TEST, &CExcelToolDlg::OnBnSendHttpTest)
	ON_BN_CLICKED(IDC_BUTTON_APPLY_CONFIG, &CExcelToolDlg::OnBnApplyConfig)
	ON_BN_CLICKED(IDC_BUTTON_UPLOAD_PLUGIN_DLL, &CExcelToolDlg::OnBnUploadPluginDll)
	ON_BN_CLICKED(IDC_BUTTON_APPLY_PLUGIN_DLL, &CExcelToolDlg::OnBnApplyPluginDll)
	ON_BN_CLICKED(IDC_BUTTON_MAKE_RUN_PACKET, &CExcelToolDlg::OnBnMakeRunPacket)
	ON_BN_CLICKED(IDC_BUTTON_UPLOAD_RUN_PACKET, &CExcelToolDlg::OnBnUploadRunPacket)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_RUN_PATH, &CExcelToolDlg::OnBnSelectRunPath)
	ON_BN_CLICKED(IDC_BUTTON_CREATE_SERVER, &CExcelToolDlg::OnBnCreateServerRun)
	ON_BN_CLICKED(IDC_BUTTON_RECONNECT_ACCOUNT, &CExcelToolDlg::OnBnConnectAccountGate)
	ON_BN_CLICKED(IDC_BUTTON_UPLOAD_GLOBAL_CONFIG, &CExcelToolDlg::OnBnUploadGlobalConfig)
	ON_BN_CLICKED(IDC_BUTTON_UPDATE_SERVER, &CExcelToolDlg::OnBnClickedButtonUpdateServer)
	ON_BN_CLICKED(IDC_SELECT_HTTPS, &CExcelToolDlg::OnBnClickedSelectHttps)
	ON_BN_CLICKED(IDC_BUTTON_SEND_NOTIC, &CExcelToolDlg::OnBnSendNotice)
END_MESSAGE_MAP()


// CExcelToolDlg 消息处理程序
#include <map>

BOOL CExcelToolDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	gpExcelToolDlg = this;

	Allot::setLogFile("DBView");
	TableTool::SetLog(new WinLog("Log/DBView.log"));
	TableTool::SetErrorLog(new WinLog("Log/DBViewError.log"));

	mEditUpdateServerID.SetWindowText("1");

	msTool = MEM_NEW ToolInfo();

	AutoTable configTable = TableManager::getSingleton().CreateNewTable("ToolConfig");
	if (!configTable->LoadCSV("RunConfig/ToolConfig.csv"))
	{
		MessageBox("No exist config > RunConfig/ToolConfig.csv");
		return TRUE;
	}

	mEditRunServerPath.SetWindowText(configTable->GetValue("RunPath", "VALUE").c_str());

	mEditMysqlUser.SetWindowText("root");
	mEditMysqlPassword.SetWindowText("root");

	mLogInfo.SetWindowText("127.0.0.1:7000");

	mEditWssAddress.SetWindowText("127.0.0.1:8000");
	mEditGameServerName.SetWindowText("1 校园一区 1");

	mEditCloseServerPassword.SetWindowText("AA");

	ARecord userRe = configTable->GetRecord("CloudNode");
	ARecord mainGate = configTable->GetRecord("MainGate");

	mUserNode = MEM_NEW DBUserNode(userRe["VALUE"].string().c_str(), userRe["PORT"], -55);
	Hand<DBUserNode> node = mUserNode;
	node->mbNeedTryReconnect = false;
	mAccountNode = MEM_NEW DBUserNode(userRe["VALUE"].string().c_str(), ((int)userRe["PORT"])+1, -10);
	node = mAccountNode;
	node->mbNeedTryReconnect = false;

	//mUserNode->Init(NetAddress(mainGate["VALUE"].string().c_str(), mainGate["PORT"]));
	mEditAccountIP.SetWindowText(mainGate["VALUE"].c_str());
	mEditAccountPort.SetWindowText("2000");
	mEditAccountSafeCode.SetWindowText("-10");

	int beginWorkerID = configTable->GetValue("BeginWorkerID", "VALUE");
	if (beginWorkerID <= 0)
	{
		MessageBox("需要配置 RunConfig/ToolConfig.csv 里增加 BeginWorkerID, VALUE 为工具开始的UnitID, 如 40000 ");
		beginWorkerID = 30000;
	}
	
	{
		mDBUnit = MEM_NEW DBUnit();
		mDBUnit->SetID(0, beginWorkerID);
		mUserNode->AppendUnit(mDBUnit.getPtr());

		mBigMsgUnit = MEM_NEW BigMsgUnit();
		mBigMsgUnit->SetID(0, beginWorkerID+1);
		mUserNode->AppendUnit(mBigMsgUnit.getPtr());

		mAccountBigUnit = MEM_NEW BigMsgUnit();
		mAccountBigUnit->SetID(0, beginWorkerID+2);
		mAccountNode->AppendUnit(mAccountBigUnit.getPtr());

		mAccountDBUnit = MEM_NEW DBUnit();
		mAccountDBUnit->SetID(0, beginWorkerID+3);
		mAccountNode->AppendUnit(mAccountDBUnit.getPtr());
	}
	REGISTER_EVENT(mBigMsgUnit->GetEventCenter(), UploadConfigDataBigEvent);
	REGISTER_EVENT(mBigMsgUnit->GetEventCenter(), UploadPluginDllBigEvent);
	
	REGISTER_EVENT(mBigMsgUnit->GetEventCenter(), TestBroatcastMsg);
	EX_REGISTER_EVENT(mBigMsgUnit->GetEventCenter(), "CreateDBMsg", AsyncRequestEvent);
	EX_REGISTER_EVENT(mBigMsgUnit->GetEventCenter(), "RestartLogicServer", AsyncRequestEvent);
	EX_REGISTER_EVENT(mBigMsgUnit->GetEventCenter(), "ApplayUploadConfigData", AsyncRequestEvent);
	EX_REGISTER_EVENT(mBigMsgUnit->GetEventCenter(), "ApplyUpdatePluginDll", AsyncRequestEvent);

	EX_REGISTER_EVENT(mAccountBigUnit->GetEventCenter(), "CreateDBMsg", AsyncRequestEvent);

	//if (confitTable->LoadCSV("RunConfig/ToolConfig.csv"))
	{
		mToolConfig = configTable;
		mEditConfigPath.SetWindowText(mToolConfig->GetValue("config_path", 1).c_str());
		mEditMakeJSPath.SetWindowText(mToolConfig->GetValue("ts_path", 1).c_str());
		mEditMakeCPath.SetWindowText(mToolConfig->GetValue("c_path", 1).c_str());
		mEditMakeConfigPath.SetWindowText(mToolConfig->GetValue("target_path", 1).c_str());

		mEditDBWorkerID.SetWindowText(mToolConfig->GetValue("db_config", "PORT").c_str());

		//ARecord mainGate = configTable->GetRecord("MainGate");
		//mEditGateIP.SetWindowText(mainGate["VALUE"].string().c_str());
		//mEditGatePort.SetWindowText( mainGate["PORT"].string().c_str());
	}
	mEditSaftCode.SetWindowText("-55");
	//OnBnReconnect();

	//else
	//{
	//	mEditConfigPath.SetWindowText("D:\\Work\\Config");
	//	mEditMakeJSPath.SetWindowText("D:/Work/Client/assets/scripts/Game");
	//	mEditMakeCPath.SetWindowText("D:/Work/Server/GameServer/Common");
	//	mEditMakeConfigPath.SetWindowText("D:/Work/Server/ServerRun/Config");
	//}	

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	SetTimer(1, 10, NULL);
	SetTimer(2, 2000, NULL);

	AString localIp = NetAddress::GetLocalIP();
	mEditLocalIP.SetWindowText(localIp.c_str());
	mEditMysqlIP.SetWindowText((localIp+":3306").c_str());

	AString title;
	title.Format("代码导出工具 ver [%s] Server [%s:%d]", SERVER_VERSION_FLAG, mainGate["VALUE"].string().c_str(), (int)mainGate["PORT"]);

	AfxGetMainWnd()->SetWindowText(title.c_str());

	AutoTable addrTable = TableManager::getSingleton().CreateNewTable(ADDRESS_LIST_CSV);
	AString addrTablePath = "RunConfig/";
	addrTable->LoadCSV((addrTablePath + ADDRESS_LIST_CSV).c_str());

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

AString LanToExtranet(const AString &lanIP)
{
	AutoTable t = TableManager::getSingleton().GetTable(ADDRESS_LIST_CSV);
	if (!t)
		return lanIP;

	ARecord re = t->GetRecord(lanIP);
	if (re)
		return re[1].string();

	return lanIP;
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CExcelToolDlg::OnPaint()
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
HCURSOR CExcelToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CExcelToolDlg::OnBnExportTable()
{
	GenerateConfigPacketAndCode(true);
}

void CExcelToolDlg::GenerateConfigPacketAndCode(bool bGenerateCode)
{
	ClearLog();

	CString configPath;

	mEditConfigPath.GetWindowText(configPath);

	AString listFileName = (LPCTSTR)configPath;
	listFileName += "/";
	listFileName += "ConfigList.xlsx";

	// 打开ConfigList
	AutoTable  configListTable = XlsxTableLoader::LoadXlsx(listFileName, "ConfigList");
	if (!configListTable)
	{
		MessageBox("配置目录内不存在 ConfigTable");
		return;
	}

	CString logFileName = configPath;
	logFileName += "/Warn.log";
	

	::DeleteFile((LPCSTR)logFileName);

	XlsxTableLoader::msLogFileName = (LPCSTR)logFileName;
	XlsxTableLoader::mbInitLogFile = true;

	//-------------------------------------------------------------------------
	// 读取本地导出MD5记录
	AutoTable recordTable;
	FileDataStream localRecordFile("RunConfig/_ExportRecord.table", FILE_READ);
	AutoData recordData = localRecordFile.readAllData();
	localRecordFile.close();
	if (recordData)
	{
		recordTable = tBaseTable::NewBaseTable();
		recordData->seek(0);
		if (!recordTable->LoadFromData(recordData.getPtr()))
			recordTable.setNull();
	}

	if (!recordTable)
	{
		recordTable = tBaseTable::NewBaseTable();
		recordTable->AppendField("INDEX", FIELD_STRING);
		recordTable->AppendField("MD5", FIELD_STRING);
		recordTable->AppendField("DATA", FIELD_DATA);
	}

	AutoTable xlsxFileIndexTable;

	AutoData listData = recordTable->GetValue("__xlsx_list__", "DATA");
	if (listData)
	{
		xlsxFileIndexTable = tBaseTable::NewBaseTable();
		listData->seek(0);
		if (!xlsxFileIndexTable->LoadFromData(listData.getPtr()))
			xlsxFileIndexTable.setNull();
	}
	if (!xlsxFileIndexTable)
	{
		xlsxFileIndexTable = tBaseTable::NewBaseTable();
		xlsxFileIndexTable->AppendField("INDEX", FIELD_STRING);
		xlsxFileIndexTable->AppendField("MD5", FIELD_STRING);		
	}

	EasyHash<AString, bool> checkList; // 已经检查过的列表
	int reloadCount = 0;
	int loadLocalCount = 0;
	UInt64 now = TimeManager::NowTick();
	//-------------------------------------------------------------------------

	mMakeConfigButton.EnableWindow(FALSE);
	mMakeConfigButton.SetWindowText("正在生成中...");

	CString makeTablePath;
	mEditMakeConfigPath.GetWindowText(makeTablePath);

	AString targetPath;
	targetPath.Format("%s/", (LPCTSTR)makeTablePath);
	FileDataStream::CreateMultiDirectory(targetPath.c_str());
	

	AString makeCSVPath;
	makeCSVPath.Format("%sMake/", targetPath.c_str());
	FileDataStream::CreateMultiDirectory(makeCSVPath.c_str());

	EasyGenerateResourcesPack  packetTool;

	DataTableManager  tempMgr;

	EasyMap<AString, AutoTable> enumTableList;

	// 导出协议的数据
	Array<AString> structNameList;
	AutoNice  proList = MEM_NEW NiceData();
	AutoNice proNotes = MEM_NEW NiceData();

	Array<ARecord>  configArray;
	for (auto it = configListTable->GetRecordIt(); *it; ++(*it))
	{
		ARecord record = it->Record();
		configArray[(int)record[0]] = record;
	}

	for (int i=0; i<configArray.size(); ++i)
	{
		ARecord record = configArray[i];
		if (!record)
			continue;

		const char *szTableName = record["INDEX"].c_str();
		AString xlsxFile = record["XLSX"];
		xlsxFile = xlsxFile.ANIS();
		AString fileName;
		fileName.Format("%s/%s.xlsx", (LPCTSTR)configPath, xlsxFile.c_str());
		
		AutoTable table;
		bool bSameLocal = false;
		if (checkList.exist(fileName))
		{
			bSameLocal = checkList.find(fileName);
		}
		else
		{
			FileDataStream xlsxFile(fileName.c_str(), FILE_READ);
			AutoData d = xlsxFile.readAllData();
			if (!d)
			{
				LOG("Read open fail > %s", fileName.c_str());
				continue;
			}

			MD5 md(d->data(), d->dataSize());
			AString md5String = md.toString();
			Data data = xlsxFileIndexTable->GetValue(fileName.c_str(), "MD5");
			AString existMD5;
			if (!data.empty())
				existMD5 = data;
			if (md5String == existMD5)
			{
				checkList.insert(fileName, true);
				bSameLocal = true;
			}
			else
			{
				ARecord re = xlsxFileIndexTable->CreateRecord(fileName, true);
				re["MD5"] = md5String;
				checkList.insert(fileName, false);
			}
		}
		if (bSameLocal)
		{
			Data d = recordTable->GetValue(szTableName, "DATA");
			AutoData tableData;
			if (!d.empty())
				tableData = (DataStream*)recordTable->GetValue(szTableName, "DATA");
			if (tableData)
			{
				table = tBaseTable::NewBaseTable();
				tableData->seek(0);
				if (!table->LoadFromData(tableData.getPtr()))
				{
					LOG("%s Load local data table fail", szTableName);
					table.setNull();
				}
				else
					loadLocalCount++;
			}
		}

		if (!table)
		{
			AutoTable xlxtable = XlsxTableLoader::LoadXlsx(fileName, record["SHEET"].string());
			if (xlxtable)
			{
				 table = MEM_NEW SkipBaseTable(INIT_FIELD_MODE::eInitCreateField);
				// 移除掉注释字段 *开关的
				bool bHave = false;
				for (int i = 0; i < xlxtable->GetField()->getCount(); ++i)
				{
					FieldInfo info = xlxtable->GetField()->getFieldInfo(i);
					if (info->getName().c_str()[0] != '*')
					{
						table->AppendField(info->getName().c_str(), (FIELD_TYPE)info->getType());
					}
					else
						bHave = true;
				}

				if (bHave)
				{
					for (auto it = xlxtable->GetRecordIt(); *it; ++(*it))
					{
						ARecord re = it->Record();
						ARecord newRe = table->CreateRecord(re[0].string(), true);
						for (int i = 1; i < table->GetField()->getCount(); ++i)
						{
							newRe[i] = re[table->GetField()->getFieldInfo(i)->getName()];
						}
					}
				}
				else
					table = xlxtable;

				table->SetTableName(szTableName);
				reloadCount++;
			}
			else
			{
				LOG("%s Load xlsx %s , sheet %s fail", szTableName, fileName.c_str(), record["SHEET"].string().ANIS().c_str());
				continue;
			}
		}

		if (table)
		{
			// 打包
			AutoData tableData = MEM_NEW DataBuffer(1024);
			if (table->SaveData(tableData.getPtr()))
			{
				packetTool.AppendResourceData(szTableName, tableData);
				MD5 md(tableData->data(), tableData->dataSize());
				record["MD5"] = md.toString();

				ARecord re = recordTable->CreateRecord(szTableName, true);
				re["MD5"] = record["MD5"];
				re["DATA"] = tableData.getPtr();

				// 上传到DB
				//NiceData param;
				//param["NAME"] = szTableName;
				//param["DATA"] = tableData.getPtr();
				//param["MD5"] = record["MD5"];
				//int nResult = 0;
				//bool b = mDBUnit->RequestDB("t_resource", "", "m_update_resource", &param, 10000, CloudDBCall(&ToolInfo::OnUploadResourceResponse, msTool.getPtr()) );
				//if (!b)
				//	LOG("Update config %s Upload fail : %d", szTableName, nResult);
			}
			else
			{
				LOG("%s save data fail", szTableName);
				continue;
			}

			AString name;
			name.Format("%s%s.csv", makeCSVPath.c_str(), szTableName);
			table->SaveCSV(name.c_str());

			// 导出协议结构(MSG_NAME 配置后, 不再导出配置表格
			AString msgClassName = record["STRUCT_NAME"];
			if (msgClassName.length() > 0)
			{
				if (record["STRUCT_CONFIG"])
				{
					if (!tempMgr.InsertTable(record["INDEX"].c_str(), table, false))
						LOG("Code error, no repeat %s", record["INDEX"].c_str());
				}

				if (table->GetField()->existField("TYPE") && table->GetField()->existField("MEMBER"))
				{
					structNameList.push_back(msgClassName);

					AutoNice structInfo = MEM_NEW NiceData();
					proList[msgClassName] = structInfo.getPtr();

					AutoNice noteInfo = MEM_NEW NiceData();
					proNotes[msgClassName] = noteInfo;

					int col = 0;
					for (auto it=table->GetRecordIt(); *it; ++(*it))
					{
						ARecord  memberRe = it->Record();
						if (!memberRe)
							break;
						AString memName = memberRe["MEMBER"];
						AString type = memberRe["TYPE"];
						if (memName.length() > 0 && type.length() > 0)
						{
							structInfo[memName] = type;
							AString infoString = memberRe["DESC"];
							
							if (memName == "eTask_GoWC")
							{
								col++;
							}
							if (infoString.length()<=0)
								infoString = memberRe["*DESC"];

							infoString.replace("\t", "");
							infoString.replace("\r", "");
							infoString.replace("\n", "");
							noteInfo[memName] = infoString;
							
						}
						else
							LOG("%s record %s no set MEMBER or TYPE", szTableName, record[0].c_str());
					}
				}
				else
					LOG("定义协议结构, 必须设置TYPE, ENUM字段, 定义成员的类型, 请正确配置后重新生成");
			}
			else if (record["ONLY_ENUM"])
			{
				enumTableList.insert(record["INDEX"].c_str(), table);
			}
			else	if (!tempMgr.InsertTable(record["INDEX"].c_str(), table, false))
				LOG("Code error, no repeat %s", record["INDEX"].c_str());
		}
		//else
			//LOG("Fail load xlsx  %s, sheet %s, index %s", record["XLSX"].c_str(), record["SHEET"].c_str(), record["INDEX"].c_str());
	}

	configListTable->SaveCSV((targetPath + "ConfigList.csv").c_str());
	LOG("Succeed make list csv table > %s", (targetPath + "ConfigList.csv").c_str());

	AutoData tableData = MEM_NEW DataBuffer(1024);
	if (configListTable->SaveData(tableData.getPtr()))
	{
		packetTool.AppendResourceData("ConfigList", tableData);
	}
	else
		LOG("ConfigList save data fail");

	AString err;
	if (packetTool.GeneratePack((targetPath + "t_configtable.pak").c_str(), err))
		LOG("Succeed generate config table packet >%s", (targetPath + "t_configtable.pak").c_str())
	else
		LOG("File generate config > %s", err.c_str());

	if (bGenerateCode)
	{
		CString path;
		mEditMakeCPath.GetWindowText(path);

		AString codePath; // = targetPath + "Code/";
		//FileDataStream::CreateMultiDirectory(codePath.c_str());
		codePath.Format("%s/", (LPCTSTR)path);

		AString info = GenerateCodeTool::generateDBManager(tempMgr.GetTableMap(), codePath, "ConfigDB");
		//LOG("Generate C++ config code > %s", info.c_str());

		mEditMakeJSPath.GetWindowText(path);
		codePath.Format("%s/", (LPCTSTR)path);

		AString infoTS = GenerateCodeTS::GenerateCode(tempMgr.GetTableMap(), codePath, "ConfigDB");
		//LOG("Generate config TS code > %s", infoTS.c_str());

		// 导出枚举
		for (int i = 0; i < enumTableList.size(); ++i)
		{
			tempMgr.InsertTable(enumTableList.getKey(i).c_str(), enumTableList.get(i), true);
		}
		AString enumCppCode;
		AString enumTsCode = GenerateConfigEnumTsCode(tempMgr.GetTableMap(), enumCppCode);

		// ts 枚举文件
		{
			AString enumFileName = codePath;
			enumFileName += "ConfigEnum.ts";

			FileDataStream enumFile(enumFileName.c_str(), FILE_CREATE_UTF8);
			enumFile._write((void*)enumTsCode.c_str(), enumTsCode.length());
			enumFile.close();

			LOG("Succeed generate ts enum > %s", enumFileName.c_str());
		}
		{
			mEditMakeCPath.GetWindowText(path);
			codePath.Format("%s/", (LPCTSTR)path);
			AString enumFileName = codePath;
			enumFileName += "ConfigEnum.h";

			FileDataStream enumFile(enumFileName.c_str(), FILE_CREATE_UTF8);
			enumFile._write((void*)enumCppCode.c_str(), enumCppCode.length());
			enumFile.close();

			LOG("Succeed generate ts enum > %s", enumFileName.c_str());
		}

		// 导出协议
		{
			AString tsCode = GenerateMsgProtocolTSCode(proList, proNotes, structNameList, true);

			mEditMakeJSPath.GetWindowText(path);
			codePath.Format("%s/MsgStruct.ts", (LPCTSTR)path);

			FileDataStream tsCodeFile(codePath.c_str(), FILE_CREATE_UTF8);
			tsCodeFile._write((void*)tsCode.c_str(), tsCode.length());
			tsCodeFile.close();

			LOG("Succend generate ts msg struct > %s", codePath.c_str());

			EasyHash<AString, bool>  temp;
			AString cppCode = GenerateMsgProtocolCppCode(proList, proNotes, structNameList, temp, true);

			mEditMakeCPath.GetWindowText(path);
			codePath.Format("%s/MsgStruct.h", (LPCTSTR)path);

			FileDataStream cppCodeFile(codePath.c_str(), FILE_CREATE_UTF8);
			cppCodeFile._write((void*)cppCode.c_str(), cppCode.length());
			cppCodeFile.close();

			LOG("Succend generate c++ msg struct > %s", codePath.c_str());
		}
	}

	// 保存到本地
	{
		AutoData d = MEM_NEW DataBuffer(1024);
		xlsxFileIndexTable->SaveData(d.getPtr());
		ARecord re = recordTable->CreateRecord("__xlsx_list__", true);
		re["DATA"] = d.getPtr();

		DataBuffer allData(16*1024*1024);	
		if (!recordTable->SaveData(&allData))
			LOG("Save all table data  fail");
		FileDataStream localRecordFile("RunConfig/_ExportRecord.table", FILE_CREATE);
		localRecordFile._write(allData.data(), allData.dataSize());
		localRecordFile.close();
		 
		CString path;
		mEditMakeJSPath.GetWindowText(path);
		AString tsPath = (LPCSTR)path;
		tsPath.replace("scripts/Game", "resources/config_table.bin");
		FileDataStream configResourceFile(tsPath.c_str(), FILE_CREATE);

		// 完全压缩
		AutoData zipData = MEM_NEW DataBuffer(1024*1024*16);
		int x = allData.ZipData(zipData, 0, 0, 0, MAX_SPEED);

		configResourceFile._write(zipData->data(), zipData->dataSize());
		configResourceFile.close();
		LOG("*** Generate config resource file > %s", tsPath.c_str());
	}
	mMakeConfigButton.EnableWindow(TRUE);
	mMakeConfigButton.SetWindowText("生成配置");

	LOG("%s Finish export update reload xlsx %d + local %d = %d, use time %llu", bGenerateCode ? "Generate code : ":"Only make packet :", reloadCount, loadLocalCount, reloadCount+loadLocalCount, TimeManager::NowTick() - now);
}


AString CExcelToolDlg::GenerateConfigEnumTsCode(TableMap &tableMap, AString &cppEnumString)
{	
	AString tsCode = "//Auto generate from ExcelTool ";
	tsCode += VERSION;
	tsCode += "\r\n//";
	// tsCode += TimeManager::GetMe().GetDateTime();

	cppEnumString = tsCode;
	cppEnumString += "\r\n#pragma once\r\n\r\n";

	tsCode += "\r\n\r\n";

	AString code;
	for (auto it=tableMap.begin(); it; ++it)
	{
		AString configIndex = it.key();
		AutoTable t = it.get();
		if (!t->GetField()->existField("ENUM"))
			continue;
		FieldInfo info = t->GetField()->getFieldInfo("ENUM");
		if (info != NULL)
		{
			int descCol = -1;
			if (t->GetField()->existField("DESC"))
				descCol = t->GetField()->getFieldCol("DESC");
			code.Format("enum Enum%s\r\n{\r\n", configIndex.c_str());
			tsCode += "export ";
			tsCode += code;
			cppEnumString += code;

			EasyHash<AString, bool>  memberList;
			EasyMap<AString, bool> repeatList;
			int repeatCount = 0;
			for (auto recordIt = t->GetRecordIt(); *recordIt; ++(*recordIt))
			{
				ARecord re = recordIt->Record();
				AString enumString = re["ENUM"];
				if (enumString.length() > 0)
				{
					if (memberList.exist(enumString))
					{
						repeatList.insert(enumString, true);
						++repeatCount;
						continue;
					}
					memberList.insert(enumString, true);

					code.Format("    %s = %d,", enumString.c_str(), (int)re[0]);
					tsCode += code;
					cppEnumString += code;

					code.setNull();
					if (descCol >=0)
					{
						AString desc = re[descCol];
						if (desc.length() > 0)
						{
							code.Format("    //%s", desc.c_str());
						}						
					}
					code += "\r\n";

					tsCode += code;
					cppEnumString += code;
				}
			}
			if (repeatCount > 0)
			{
				AString info;
				for (int i = 0; i < repeatList.size(); ++i)
				{
					info + " ";
					info += repeatList.getKey(i);
				}
				//WARN_LOG("%s  repeat member %s : %d", configIndex.c_str(), info.c_str(), repeatCount);
			}
			tsCode += "};\r\n\r\n";
			cppEnumString += "};\r\n\r\n";
		}
	}
	return tsCode;
}


void CExcelToolDlg::OnSelectConfigPath()
{
	CString m_FileDir;
	BROWSEINFO bi;
	ZeroMemory(&bi, sizeof(BROWSEINFO));
	bi.hwndOwner = m_hWnd;
	bi.ulFlags = BIF_RETURNONLYFSDIRS;
	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
	BOOL bRet = FALSE;
	TCHAR szFolder[MAX_PATH * 2];
	szFolder[0] = ('\0');
	if (pidl)
	{
		if (SHGetPathFromIDList(pidl, szFolder))
			bRet = TRUE;
		IMalloc *pMalloc = NULL;
		if (SUCCEEDED(SHGetMalloc(&pMalloc)) && pMalloc)
		{
			pMalloc->Free(pidl);
			pMalloc->Release();
		}
	}

	mEditConfigPath.SetWindowText(szFolder);
}


void CExcelToolDlg::OnBnUpload()
{
	// TODO: 在此添加控件通知处理程序代码
	ClearLog();

	CString makeTablePath;
	mEditMakeConfigPath.GetWindowText(makeTablePath);

	AString targetPath;
	targetPath.Format("%s/t_configtable.pak", (LPCTSTR)makeTablePath);

	FileDataStream		pakFile(targetPath.c_str(), FILE_READ);
	Auto<DataBuffer> data = pakFile.readAllData();

	AutoData zipData = MEM_NEW DataBuffer(1024);
	if (!data->ZipData(zipData))
	{
		LOG("Zip data fail");
		return;
	}

	Hand<Cloud_SendDataPartEvent> evt = mBigMsgUnit->StartEvent("Cloud_SendDataPartEvent", DB_WORKER_ID);

	AutoEvent sendEvt = mBigMsgUnit->StartEvent("UploadConfigDataBigEvent", DB_WORKER_ID);
	
	sendEvt["CONFIG_DATA"] = zipData.getPtr();
	AString info = "Update:";
	info += TimeManager::ToDateTime(TimeManager::Now());
	sendEvt["INFO"] = info;
	sendEvt->DoEvent();
	evt->SendBigEvent(sendEvt);
}

void CExcelToolDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nIDEvent == 1)
	{
		mUserNode->Process();
		mUserNode->LowProcess(1);
		TimerCenter::GetMe()->Process();
		CoroutineTool::CheckFinish();
		TimerCenter::GetMe()->Process();

		mAccountNode->Process();
		mAccountNode->LowProcess(1);
	
	}
	else if (nIDEvent == 2)
	{
		size_t totalSize = 0;
		size_t activePhys = 0;
		SystemInfo::SystemMem(totalSize, activePhys);
		AString info;
		size_t useMem = SystemInfo::ProcessMemory();
		info.Format("Use mem %.0fM, %.0f/%.0fM", (useMem / 1024) / 1024.0f, (activePhys / 1024) / 1024.0f, (totalSize / 1024) / 1024.0f);

		mEditHttpParam.SetWindowText(info.c_str());
	}

	CDialogEx::OnTimer(nIDEvent);
}

void CExcelToolDlg::OnBnSelectTSPath()
{
	// TODO: 在此添加控件通知处理程序代码

	mEditMakeJSPath.SetWindowText(SelectPath());

}


void CExcelToolDlg::OnBnSelectCPath()
{
	// TODO: 在此添加控件通知处理程序代码
	mEditMakeCPath.SetWindowText(SelectPath());
}


void CExcelToolDlg::OnBnSelectTargetConfigPath()
{
	// TODO: 在此添加控件通知处理程序代码
	mEditMakeConfigPath.SetWindowText(SelectPath());
}



void CExcelToolDlg::OnBnNewCreateDB()
{
	ASYNC(&ToolInfo::AwaitCreateDB, msTool.getPtr());
}

void CExcelToolDlg::AsyncNewCreateDB()
{
	ClearLog();

	AString dbConfigFile;
	if (mToolConfig)
		dbConfigFile = mToolConfig->GetValue("db_config", 1).c_str();

	if (dbConfigFile.length() <= 0)
	{
		MessageBox("未配置DB > RunConfig/ToolConfig.csv>db_config", "", MB_ICONERROR);
		return;
	}

	CString configPath;

	mEditConfigPath.GetWindowText(configPath);

	AString listFileName;
	listFileName.Format("%s/%s.xlsx", (LPCTSTR)configPath, dbConfigFile.c_str());

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
	int bOK = MessageBox("新建DB前需要清空当前所有DB表, 是否重建DB?", "注意", MB_OKCANCEL | MB_ICONQUESTION);
	if (bOK!= IDOK)
		return;

	CString makeTablePath;
	mEditMakeConfigPath.GetWindowText(makeTablePath);

	AString targetPath;
	targetPath.Format("%s/", (LPCTSTR)makeTablePath);
	FileDataStream::CreateMultiDirectory(targetPath.c_str());


	AString name;
	name.Format("%s%s.csv", targetPath.c_str(), dbConfigFile.c_str());
	configListTable->SaveCSV(name.c_str());

	CString path;
	mEditMakeJSPath.GetWindowText(path);
	AString codePath;
	codePath.Format("%s/", (LPCTSTR)path);

	CString cCodePath;
	mEditMakeCPath.GetWindowText(cCodePath);

	for (auto it = configListTable->GetRecordIt(); *it; ++(*it))
	{
		ARecord record = it->Record();

		const char *szTableName = record["INDEX"].c_str();

		AString fileName;
		fileName.Format("%s/%s.xlsx", (LPCTSTR)configPath, record["XLSX"].c_str());
		AutoTable table = XlsxTableLoader::LoadXlsx(fileName, record["SHEET"].string());
		if (table)
		{
			table->SetTableName(szTableName);					

			AString name;
			name.Format("%s%s.csv", targetPath.c_str(), szTableName);
			table->SaveCSV(name.c_str());

			// 创建TS脚本
			if (record["EXPORT_TS"])
			{
				AutoTable db = tBaseTable::NewBaseTable(false);
				int i = 0;
				while (true)
				{					
					ARecord fieldInfo = table->GetRecord(i++);
					if (fieldInfo)
					{
						db->AppendField(fieldInfo["NAME"].c_str(), fieldInfo["TYPE"].c_str());
					} 
					else
						break;
				}
				if (db->GetField()->check())
				{
					db->SetTableName(szTableName);
					AString tsCode = GenerateCodeTS::GenerateConfigTS(db, true);

					AString tsFile;
					tsFile.Format("%s/%s.ts", codePath.c_str(), szTableName);
					FileDataStream  f(tsFile.c_str(), FILE_CREATE_UTF8);
					f._write((void*)tsCode.c_str(), tsCode.length());
					f.close();
					LOG("Succeed export ts code > %s", tsFile.c_str());
				}
			}


			// 发送到DB 进行创建
			//NiceData  param;
			//param["TABLE"] = table.getPtr();
			//mDBUnit->RequestDB("", "", "t_create_db", &param, 6000, CloudDBCall( &ToolInfo::OnResponse, msTool.getPtr()) );

			AutoEvent evt = mBigMsgUnit->StartEvent("CreateDBMsg", DB_WORKER_ID);
			HAsyncRequestEvent request = evt;
			request["TABLE"] = table.getPtr();
			request["TABLE_NAME"] = szTableName;
			request["DB"] = "MemDB";
			request["CODE_PATH"] = (LPCSTR)cCodePath;
			AutoEvent resultEvent = request->AwaitReqeust(mBigMsgUnit, DB_WORKER_ID, 6);
			if (resultEvent)
			{
				if (resultEvent["RESULT"])
				{
					LOG("-----------------------------------------------------\r\nCExcelToolDlg: DB opereate : Create %s table : result Sueeccd >\r\n%s : Slot %s\r\n------------------------------------------", szTableName, resultEvent->GetData()["ERROR"].c_str(),
						resultEvent["INFO"].c_str()
						)
				}
				else
					LOG("----------------------------------------------------\r\nCExcelToolDlg: DB opereate : Create %s table : result fail >\r\n%s\r\n-----------------------------------------", szTableName, resultEvent->GetData()["ERROR"].c_str());
			}
			else
				LOG("CExcelToolDlg : request create DB fail > [%s], may be connect fail", szTableName);
		}
		else
			LOG("Fail load xlsx  %s, sheet %s, index %s", record["XLSX"].c_str(), record["SHEET"].c_str(), record["INDEX"].c_str());
	}
	LOG("********************** finish all create db ******************************");
}



FieldInfo GetFieldInfoByTypeString(AString typeString)
{
	AString tempType = typeString;
	AString leftType = tempType.SplitLeft("<");
	leftType.toUpper();
	FieldInfo info = NULL;
	if (leftType.length() > 0)
		info = FieldInfoManager::getFieldInfo(leftType.c_str());
	else
	{
		tempType.toUpper();
		info = FieldInfoManager::getFieldInfo(tempType.c_str());
	}
	//if (info == NULL)		
	//	LOG("Can not use type > %s", typeString.c_str());

	return info;
}

AString splitTemplateType(AString type, bool bTs, bool *bIsBaseType = NULL)
{	
	AString tempType = type.c_str();
	 AString templateType = tempType.SplitBlock('<', '>');
	 FieldInfo info = GetFieldInfoByTypeString(templateType);
	 if (bTs)
	 {
		 if (info == NULL)
			 return templateType;
	 
		 const char *szType = "";
		 switch (info->getType())
		 {

		 case FIELD_STRING:
		 case FIELD_CHAR_STRING:
			 szType = "string";
			 break;

		 case FIELD_INT:
		 case FIELD_SHORT:
		 case FIELD_INT64:
		 case FIELD_UINT64:
		 case FIELD_FLOAT:
		 case FIELD_DOUBLE:
			 szType = "number";
			 break;

		 case  FIELD_BOOL:
			 szType = "boolean";
			 break;

		 case FIELD_ARRAYDATA:
			 szType = "[]";
			 break;
		 }

		 if (szType == "")
		 {
			 LOG("Type > %s can not to TS", info->getTypeString());
		 }

		 return szType;
	 }
	

	 // c++
	 if (info == NULL)
		 return templateType;

	 const char *szType = "";
	 switch (info->getType())
	 {

	 case FIELD_STRING:
	 case FIELD_CHAR_STRING:
		 szType = "AString";
		 break;

	 case FIELD_INT:
		 szType = "int";
		 break;
	 case FIELD_SHORT:
		 szType = "short";
		 break;
	 case FIELD_INT64:
		 szType = "Int64";
		 break;
	 case FIELD_UINT64:
		 szType = "UInt64";
		 break;
	 case FIELD_FLOAT:
		 szType = "float";
		 break;
	 case FIELD_DOUBLE:
		 szType = "double";
		 break;

	 case  FIELD_BOOL:
		 szType = "bool";
		 break;

	 case FIELD_ARRAYDATA:
		 szType = "";		// 不允许嵌套
		 LOG("C++ Array 不允许再次嵌套 数组");
		 break;
	 }

	 if (szType == "")
	 {
		 LOG("Type > %s can not to CPP", info->getTypeString());
	 }
	 else if (bIsBaseType != NULL)
		 *bIsBaseType = true;

	 return szType;		
}

AString GetNoteString(AString &scrString)
{	
	AString noteString = scrString;
	bool bHave = strstr(scrString.c_str(), "//") != NULL;
	AString temp = noteString.SplitLeft("//");
	if (temp.length() > 0)
	{
		scrString = temp;
		scrString.replace(" ", "");
		scrString.replace("\t", "");
		return noteString;
	}
	if (bHave)
	{
		scrString.setNull();
		return noteString;
	}
	scrString.replace(" ", "");
	scrString.replace("\t", "");
	return AString();
}



AString GetMemberNote(AutoNice noteData, const AString &memberName)
{
	AString strNote = noteData[memberName];
	if (strNote.length() > 0)
	{
		AString temp = "// ";
		return temp + strNote;
	}
	return AString();
}

void ClearSpaceString(AString &infoStr)
{
	while (infoStr.length() > 0)
	{
		if (*infoStr.c_str() == ' ' || *infoStr.c_str() == '\t')
		{
			AString temp = infoStr.c_str() + 1;
			infoStr = temp;
		}
		else
			break;
	}
}

void CExcelToolDlg::OnBnGenerateProtocol()
{
	ClearLog();

	CString configPath;
	mEditConfigPath.GetWindowText(configPath);

	AString fileName;
	fileName.Format("%s/Protocol.txt", (LPCTSTR)configPath);
	GenerateProtocol(fileName, "MsgData");
}

AutoNice CExcelToolDlg::GenerateProtocol(const AString &fileName, const AString &targetName, bool bTSCode)
{
	FileDataStream  f(fileName.c_str(), FILE_READ);
	AutoData  d = f.readAllData();
	if (!d || d->dataSize()<=0)
	{
		CString info;
		info.Format("%s 不存在或内容为空", fileName.c_str());
		MessageBox(info);
		return AutoNice();
	}
	d->resize(d->dataSize() + 3);
	d->data()[d->size() - 3] = '\r';
	d->data()[d->size() - 2] = '\n';
	d->data()[d->size() - 1] = '\0';

	Array<AString> structNameList;

	AutoNice  proList = MEM_NEW NiceData();

	AutoNice proNotes = MEM_NEW NiceData();

	AString  strPro = (const char*)d->data();
	//strPro.replace("\r\n", "");
	EasyHash<AString, bool> exportRunHash;

	EasyHash<AString, bool> runConfigList;

	AString lastStructInfo;

	while (true)
	{
		if (strPro.length() <= 0)
			break;
		
		AString lineString = strPro.SplitLeft("\r\n");
		if (lineString.length()<=0)
		{
			if (strstr(strPro.c_str(), "\r\n") == NULL)
			{
				lineString = strPro;
				strPro.setNull();
			}
			else
				continue;
		}

		AString strInfo = GetNoteString(lineString);
		
		lineString.replace(" ", "");
		lineString.replace("\t", "");

		if (lineString.length() <= 0)
		{
			if (strInfo.length() > 0)
				lastStructInfo = strInfo;
			continue;
		}

		AString name = lineString;
		if (strstr(name.c_str(), ":DB") != NULL)
		{
			name = lineString.SplitLeft(":DB");
			exportRunHash.insert(name, true);
		}
		else if (strstr(name.c_str(), ":ARRAY") != NULL)		// 分析是否直接导出为一个数组结构
		{
			name = lineString.SplitLeft(":ARRAY");
			exportRunHash.insert(name, false);				// 插入到需要导出 Run 列表, 但值为false
		}
		else if (strstr(name.c_str(), ":CONFIG") != NULL)		// 分析是否直接导出为一个数组结构
		{
			name = lineString.SplitLeft(":CONFIG");
			runConfigList.insert(name, true);				// 插入到需要导出 Run 列表, 但值为false
		}

		AString dataString = strPro.SplitBlock();

		if (dataString.length()<=0)
			continue;

		AutoNice noteData = MEM_NEW NiceData();
		proNotes[name] = noteData.getPtr();
		AutoNice orderNameList = MEM_NEW ArrayNiceData();
		proNotes[name+"__list__"] = orderNameList.getPtr();

		noteData["_struct_"] = strInfo;

		if (lineString.length() > 0)
			proNotes[name + "_info_"] = lastStructInfo;
		lastStructInfo.setNull();

		structNameList.push_back(name);

		AutoNice field = MEM_NEW NiceData();
		int i = 0;
		while(true)
		{			
			if (dataString.length() <= 0)
				break;
			// int kkk;  //说明\r\n
			AString lineString = dataString.SplitLeft("\r\n");
			if (lineString.length() <= 0)
				continue;

			ClearSpaceString(lineString);

			AString noteStr;
			AString infoStr = lineString.SplitLeft("//");
			if (infoStr.length() <= 0)
				infoStr = lineString.SplitLeft(";");
			else
			{
				AString temp = infoStr.SplitLeft(";");
				infoStr = temp;
				noteStr = lineString;
			}

			if (infoStr.length() <= 0)
				continue;

			// 去掉前面的空格或Tab符号
			while (infoStr.length()>0)
			{
				if (*infoStr.c_str() == ' ' || *infoStr.c_str() == '\t')
				{
					AString temp = infoStr.c_str() + 1;
					infoStr = temp;
				}
				else
					break;
			}

			infoStr.replace('\t', ' ');
			AString type = infoStr.SplitLeft(" ");
			if (type.length() <= 0 || infoStr.length()<=0)
				break;
			
			orderNameList[orderNameList->count()] = infoStr;
			if (proList->exist(type.c_str()))
			{
				AutoNice existType = (tNiceData*)proList->get(type);
				if (existType)
				{
					field[infoStr] = type;
					if (noteStr.length() > 0)
					{
						noteData[infoStr] = noteStr;
					}
					noteData[infoStr + "__type__"] = type;
					continue;
				}
			}
			
			FieldInfo info = GetFieldInfoByTypeString(type);
			if (info != NULL)
				field[infoStr] = type;
			else
			{
				//LOG("Can not use type > %s, to %s", type.c_str(), infoStr.c_str());
				// 有可能是MsgStruct 中的协议
				field[infoStr] = type;
			}

			if (noteStr.length() > 0)
			{
				noteData[infoStr] = noteStr;
			}
		}

		proList[name]  = field.getPtr();

	}

	//LOG("-------------------\r\n%s", proList->dump().c_str());
	// 生成TS
	
	CString path;
	mEditMakeJSPath.GetWindowText(path);

	AString tsFileName = (LPCTSTR)path;
	tsFileName += "/";
	tsFileName += targetName;
	tsFileName += ".ts";
	if (bTSCode)
	{
		AString tsCode = GenerateMsgProtocolTSCode(proList, proNotes, structNameList, false);

		//LOG(" code > \r\n%s", tsCode.c_str());
		FileDataStream  tsFile(tsFileName.c_str(), FILE_CREATE_UTF8);

		tsFile._write((void*)tsCode.c_str(), tsCode.length());
		tsFile.close();

		LOG("Genereate ts code > n%s", tsFileName.c_str());
	}
	//------------------------------------------------------------------------
	// 生成C++
	AString cppCode = GenerateMsgProtocolCppCode(proList, proNotes, structNameList, exportRunHash, false, !bTSCode);

	//LOG("C++ ----------------------\r\n%s", proNotes->dump().c_str());

	mEditMakeCPath.GetWindowText(path);
	AString codeFileName = (LPCTSTR)path;
	codeFileName += "/";
	codeFileName += targetName;
	codeFileName += ".h";
	FileDataStream  cppCodeFile(codeFileName.c_str(), FILE_CREATE_UTF8);
	cppCodeFile._write((void*)cppCode.c_str(), cppCode.length());
	cppCodeFile.close();

	LOG("Genereate c++ code > n%s", codeFileName.c_str());
	
	return proNotes;
}

AutoData CExcelToolDlg::CreateDBListConfigPacket(AString &info)
{
	AString dbConfigFile;
	if (mToolConfig)
		dbConfigFile = mToolConfig->GetValue("db_config", 1).c_str();

	if (dbConfigFile.length() <= 0)
	{
		MessageBox("未配置DB > RunConfig/ToolConfig.csv>db_config", "", MB_ICONERROR);
		return AutoData();
	}

	CString configPath;

	mEditConfigPath.GetWindowText(configPath);

	AString listFileName;
	listFileName.Format("%s/%s.xlsx", (LPCTSTR)configPath, dbConfigFile.c_str());

	// 打开ConfigList
	AutoTable  configListTable = XlsxTableLoader::LoadXlsx(listFileName, dbConfigFile);
	if (!configListTable)
	{
		CString info;
		info.Format("配置目录内不存在: %s, sheet %s", listFileName.c_str(), dbConfigFile.c_str());

		MessageBox(info, "", MB_ICONERROR);
		return AutoData();
	}

	CString makeTablePath;
	mEditMakeConfigPath.GetWindowText(makeTablePath);

	AString targetPath;
	targetPath.Format("%s/", (LPCTSTR)makeTablePath);
	FileDataStream::CreateMultiDirectory(targetPath.c_str());


	AString name;
	name.Format("%s%s.csv", targetPath.c_str(), dbConfigFile.c_str());
	configListTable->SaveCSV(name.c_str());

	CString path;
	mEditMakeJSPath.GetWindowText(path);
	AString codePath;
	codePath.Format("%s/", (LPCTSTR)path);

	EasyGenerateResourcesPack packetTool;

	for (auto it = configListTable->GetRecordIt(); *it; ++(*it))
	{
		ARecord record = it->Record();

		const char *szTableName = record["INDEX"].c_str();

		AString fileName;
		fileName.Format("%s/%s.xlsx", (LPCTSTR)configPath, record["XLSX"].c_str());
		AutoTable table = XlsxTableLoader::LoadXlsx(fileName, record["SHEET"].string());
		if (table)
		{
			table->SetTableName(szTableName);

			AutoData tableData = MEM_NEW DataBuffer();
			table->SaveData(tableData.getPtr());
			packetTool.AppendResourceData(szTableName, tableData);
		}
	}

	AutoData tableData = MEM_NEW DataBuffer();
	configListTable->SaveData(tableData.getPtr());
	packetTool.AppendResourceData("DBLIST", tableData);

	AutoData packetData = MEM_NEW DataBuffer();

	if (packetTool.GeneratePack(NULL, info, packetData.getPtr()))
		return packetData;
	return AutoData();
}

//-------------------------------------------------------------------------


AString CExcelToolDlg::GenerateMsgProtocolCppCode(AutoNice proList, AutoNice proNotes, Array<AString> &structNameList, EasyHash<AString, bool> &exportRunHash, bool bFirst, bool bRunConfig)
{
	AString cppCode = "//Auto genereate msg data code		";
	//cppCode += TimeManager::GetMe().GetDateTime();
	cppCode += "\r\n#pragma once\r\n\r\n";

	cppCode += "#include \"BaseMsg.h\"\r\n#include \"Array.h\"\r\n#include \"ArrayList.h\"\r\n\r\n";

	if (!bFirst && !bRunConfig)
		cppCode += "#include \"MsgStruct.h\"\r\n\r\n";

	for (int i = 0; i < structNameList.size(); ++i)
	{
		AString name = structNameList[i];

		AutoNice d = (tNiceData*)proList[name];

		if (!d)
			continue;

		// 如果开头为 CC_ 则不导出C++, 表示客户端 TS专用
		if (name.c_str()[0] == 'C' && name.c_str()[1] == 'C')
			continue;

		AutoNice noteData = proNotes[name];

		AString strInfo = proNotes[name + "_info_"];
		if (strInfo.length() > 0)
		{
			cppCode += "// ";
			cppCode += strInfo;
			cppCode += "\r\n";
		}

		cppCode += "class ";
		cppCode += name;
		cppCode += " : public tBaseMsg";
		AString structNote = GetMemberNote(noteData, "_struct_");
		if (structNote.length() > 0)
		{
			cppCode += "	";
			cppCode += structNote;
		}
		cppCode += "\r\n{ \r\npublic:\r\n";

		AString getCode;
		AString code;
		AString toDataCode;

		AString clearCode;
		AString saveCode;
		AString copyCode;

		AString getIntTypeCode;
		AString setIntTypeCode;

		bool bHaveArray = false;
		bool bOnlyOneArray = exportRunHash.exist(name) && !exportRunHash.find(name);
		if (bOnlyOneArray && d->count() != 1)
		{
			LOG("如果只导出为一个数组结构, 只能定义一个arraydata<>成员, 当前按一般结构处理");
			bOnlyOneArray = false;
		}

		for (auto m = d->begin(); m; ++m)
		{
			AString key = m.key();
			AString type = m.get();

			// 找不到类型, 即作为存在的结构处理
			FieldInfo info = GetFieldInfoByTypeString(type);
			if (info == NULL || proList->exist(type.c_str()))
			{
				if (type.sub(0, 3) == "map")
				{
					bool bIsBase = false;
					AString subType = splitTemplateType(type, false, &bIsBase);
					if (subType.length() > 0)
					{
					}
					LOG("Type error > %s ", type.c_str());
					continue;
				}

				code.Format("    %s %s;		%s\r\n", type.c_str(), m.key().c_str(), GetMemberNote(noteData, m.key()).c_str());
				cppCode += code;

				code.Format("        {\r\n            AutoNice d = (tNiceData*)scrData[\"%s\"];\r\n            if (d) %s.Full(d); else LOG(\"No exist data %s\");\r\n        }\r\n", m.key().c_str(), m.key().c_str(), m.key().c_str());
				getCode += code;

				code.Format("        {\r\n            AutoNice d = destData->NewNice(); d->msKey = \"%s\"; %s.ToData(d);\r\n            destData[\"%s\"] = d.getPtr();\r\n        }\r\n",  type.c_str(),  m.key().c_str(), m.key().c_str());
				toDataCode += code;

				code.Format("        %s.clear(false);\r\n", key.c_str());
				clearCode += code;
				                                                                                                                                                           
				code.Format("        SAVE_MSG_STRUCT(%s);\r\n", key.c_str());
				saveCode += code;

				code.Format("        %s.copy(other.%s);\r\n", key.c_str(), key.c_str());
				copyCode += code;

				continue;
			}

			//FieldInfo info = GetFieldInfoByTypeString(type);
			//if (info == NULL)
			//{
			//	LOG("Table %s > %s : %s can not to c++", name.c_str(), m.key().c_str(), type.c_str());
			//	continue;
			//}

			const char *szType = NULL;
			AString clearString;
			switch (info->getType())
			{
			case FIELD_STRING:
			case FIELD_CHAR_STRING:
				szType = "AString";
				clearString = ".setNull()";				
				break;

			case FIELD_BYTE:
				szType = "byte";
				clearString = " = 0";
				break;

			case FIELD_INT:
				szType = "int";
				clearString = " = 0";
				break;

			case FIELD_SHORT:
				szType = "short";
				clearString = " = 0";
				break;

			case FIELD_INT64:
				szType = "Int64";
				clearString = " = 0";
				break;

			case FIELD_UINT64:
				szType = "UInt64";
				clearString = " = 0";
				break;

			case FIELD_FLOAT:
				szType = "float";
				clearString = " = 0";
				break;

			case FIELD_DOUBLE:
				szType = "double";
				clearString = " = 0";
				break;

			case  FIELD_BOOL:
				szType = "bool";
				clearString = " = false";
				break;

			case FIELD_ARRAYDATA:
				szType = "ArrayList";
				clearString = ".clear(false)";
				break;

			case  FIELD_NICEDATA:
			{
				szType = "AutoNice";
				clearString = ".setNull()";
				break;
			}
			}
			if (szType == NULL)
			{
				LOG("Table %s > %s : %s can not to c++", name.c_str(), m.key().c_str(), info->getTypeString());
				continue;
			}
			else
			{
				if (info->getType() == FIELD_NICEDATA)
					code.Format("        if (%s) %s%s;\r\n", key.c_str(), key.c_str(), clearString.c_str());
				else
				{
					code.Format("        %s%s;\r\n", key.c_str(), clearString.c_str());
					if (info->getType() != FIELD_ARRAYDATA)
					{
						AString str;
						str.Format("        if (strcmp(szMember, \"%s\")==0) { AData value; value = %s; return value; }\r\n", key.c_str(), key.c_str());
						getIntTypeCode += str;

						str.Format("        if (strcmp(szMember, \"%s\")==0) { %s = value; return true; };\r\n", key.c_str(), key.c_str());
						setIntTypeCode += str;
					}
				}
				clearCode += code;

				if (info->getType() == FIELD_ARRAYDATA)
				{
					bHaveArray = true;
					bool bIsBase = false;
					code.Format("    %s<%s> %s;	%s\r\n", szType, splitTemplateType(type, false, &bIsBase).c_str(), m.key().c_str(), GetMemberNote(noteData, m.key()).c_str());
					cppCode += code;
					if (bIsBase)
					{
						if (bOnlyOneArray)
							code.Format("        FullArray(scrData, %s);\r\n", m.key().c_str());
						else
							code.Format("        FullArray(scrData, %s, \"%s\");\r\n", m.key().c_str(), m.key().c_str());
					}
					else
						code.Format("        FullStuctArray(scrData, %s, \"%s\");\r\n", m.key().c_str(), m.key().c_str());

					getCode += code;

					if (bIsBase)
					{
						if (bOnlyOneArray)
							code.Format("        ArrayToData(destData, %s);\r\n", m.key().c_str() );
						else
							code.Format("        ArrayToData(destData, %s, \"%s\");\r\n", m.key().c_str(), m.key().c_str());
					}
					else
						code.Format("        StuctArrayToData(destData, %s, \"%s\");\r\n", m.key().c_str(), m.key().c_str());

					toDataCode += code;

					// ArrayNiceData:: serialize
					if (bIsBase)
					{
						if (bOnlyOneArray)
							code.Format("        SaveArray(%s, destData);\r\n", key.c_str() );
						else
							code.Format("        SaveArray(\"%s\", %s, destData);\r\n", key.c_str(), key.c_str());
					}
					else
						code.Format("        SaveMsgArray(\"%s\", %s, destData);\r\n", key.c_str(), key.c_str());
					saveCode += code;

					if (bIsBase)
						code.Format("        %s = other.%s;\r\n", key.c_str(), key.c_str());
					else
						code.Format("        CopyMsgArray(other.%s, %s);\r\n", key.c_str(), key.c_str());
					copyCode += code;
				}
				else
				{
					code.Format("    %s %s;		%s\r\n", szType, m.key().c_str(), GetMemberNote(noteData, m.key()).c_str());
					cppCode += code;

					if (info->getType() == FIELD_NICEDATA)
					{
						code.Format("        %s = (tNiceData*)scrData[\"%s\"];\r\n", m.key().c_str(), m.key().c_str());
						getCode += code;

						code.Format("        destData[\"%s\"] = %s.getPtr();\r\n", m.key().c_str(), m.key().c_str());
						toDataCode += code;

						// AutoNiceFieldInfo::saveData
						code.Format("        SAVE_MSG_NICE(%s);\r\n", key.c_str());
						saveCode += code;

						code.Format("        COPY_MSG_NICE(other.%s, %s);\r\n", key.c_str(), key.c_str());
						copyCode += code;
					}
					else
					{
						code.Format("        CheckGet(scrData, %s);\r\n", m.key().c_str(), m.key().c_str());
						getCode += code;

						code.Format("        destData[\"%s\"] = %s;\r\n", m.key().c_str(), m.key().c_str());
						toDataCode += code;

						code.Format("        SAVE_MSG_VALUE(%s, %d);\r\n", key.c_str(), info->getType());
						saveCode += code;

						code.Format("        %s = other.%s;\r\n", key.c_str(), key.c_str());
						copyCode += code;
					}
				}
			}
		}

		code.Format("\r\npublic:\r\n    %s() { clear(false); };\r\n\r\n", name.c_str());
		cppCode += code;

		cppCode += "\r\n    void Full(AutoNice scrData)\r\n    {\r\n        clear(false);\r\n";
		cppCode += getCode;
		cppCode += "    }\r\n\r\n";

		cppCode += "    virtual void ToData(AutoNice &destData) override\r\n    {\r\n";
		cppCode += toDataCode;
		cppCode += "    }\r\n\r\n";

		cppCode += "    bool serialize(DataStream *destData) const override\r\n    {\r\n";
		if (bOnlyOneArray)
		{
			code.setNull();
			if (!bHaveArray)
				LOG("只导出数组时, 必须定义为数据类型成员 arraydata<>");
		}
		else	
			code.Format("        destData->write((short)%d);\r\n\r\n", (int)d->count());

		cppCode += code;
		cppCode += saveCode;
		cppCode += "        return true;\r\n    }\r\n\r\n";

		cppCode += "    void clear(bool bClearBuffer=false) override \r\n    {\r\n";
		cppCode += clearCode;
		cppCode += "    }\r\n\r\n";
		 
		code.Format("    void copy(const tBaseMsg &otherMsg) override \r\n    {\r\n        if (strcmp(\"%s\", otherMsg.GetMsgName())!=0) { LOG(\"%%s is not %s\", otherMsg.GetMsgName()); return; }; const %s &other = *(const %s*)(&otherMsg);\r\n", name.c_str(), name.c_str(), name.c_str(), name.c_str());
		cppCode += code;
		cppCode += copyCode;
		cppCode += "    }\r\n\r\n";

		code.Format("    virtual const char* GetMsgName() const override { return \"%s\"; }\r\n\r\n", name.c_str());
		cppCode += code;

		if (exportRunHash.find(name))
			cppCode += "    virtual int Run(ShareMemCloudDBNode *pNode, const AString &tableName, AutoNice &paramData, AutoNice &resultData) override;\r\n\r\n";

		cppCode += "    AData get(const char *szMember) const \r\n    {\r\n";
		cppCode += getIntTypeCode;
		cppCode += "        return AData();\r\n    }\r\n\r\n";

		cppCode += "    bool set(const char *szMember, AData value) \r\n    {\r\n";
		cppCode += setIntTypeCode;
		cppCode += "        LOG(\"No exist > %%s\", szMember);  return false;\r\n    }\r\n\r\n";

		cppCode += "    AData operator [] (const char *szMember) const \r\n    {\r\n";
		cppCode += "        return get(szMember);\r\n";
		cppCode += "    }\r\n\r\n";

		cppCode += "    AData operator [] (const AString &member) const \r\n    {\r\n";
		cppCode += "        return get(member.c_str());\r\n";
		cppCode += "    }\r\n\r\n";

		cppCode += "};\r\n\r\n";
	}

	if (bRunConfig)
	{
		AString infoString = proNotes->ToJSON();

		AString str = Base64Tool::encode(infoString.c_str(), infoString.length());

		cppCode += "class _comment {\r\npublic:\r\n    static const char* CommentString()\r\n     {\r\n        static const char *gCommentString = \r\n                \"";
		cppCode += str;
		cppCode += "\";\r\n        return gCommentString; \r\n    }\r\n };\r\n\r\n";
	}
	return cppCode;
}

AString GetTsType(AString typeString, AutoNice proList, bool bSetDefault = true);

AString CExcelToolDlg::GenerateMsgProtocolTSCode(AutoNice proList, AutoNice proNotes, Array<AString> &structNameList, bool bFirst)
{
	AString headCode = "//Auto genereate msg data code		";
	//headCode += TimeManager::GetMe().GetDateTime();
	headCode += "\r\n\r\n";

	headCode += "import BaseMsg from \"../Base/BaseMsg\"\r\n";
	headCode += "import NiceData, { AData } from \"../Base/NiceData\"\r\n\r\n";

	AString secordString;

	EasyHash<AString, bool>  tempImportList;

	AString tsCode;

	if (bFirst)
	{
		secordString = "export default class tBaseMsg extends BaseMsg\r\n"
			"{\r\n"		
			"    public static CreateData(className) :tBaseMsg\r\n    {\r\n"
			"        switch (className)\r\n        {\r\n";
		//		case StudentProperty.name:
		//			return new StudentProperty();
		//		case BaseProperty.name:
		//			return new BaseProperty();
		//	}
		//	return null;
		//}
	}
	else
		tsCode += "import tBaseMsg from \"./MsgStruct\"\r\n\r\n";

	//for (auto it = proList->begin(); it; ++it)
	for (int i = 0; i < structNameList.size(); ++i)
	{
		AString name = structNameList[i];

		AutoNice d = (tNiceData*)proList[name];

		AString typeInfoCode;
		AString arrayElementCode;
		AString isMsgCode;
		AString resetClearCode;

		if (!d)
			continue;

		if (bFirst)
		{
			AString code;
			code.Format("            case \"%s\":\r\n                return new %s();\r\n", name.c_str(), name.c_str());
			secordString += code;
		}

		AString strInfo = proNotes[name + "_info_"];
		if (strInfo.length() > 0)
		{
			tsCode += "// ";
			tsCode += strInfo;
			tsCode += "\r\n";
		}

		tsCode += "export class ";
		tsCode += name;
		tsCode += "  extends tBaseMsg";

		AutoNice noteData = (tNiceData*)proNotes[name];
		AString structNote = GetMemberNote(noteData, "_struct_");
		if (structNote.length() > 0)
		{
			tsCode += "	";
			tsCode += structNote;
		}

		tsCode += "\r\n{\r\n";
		{
			AString code;
			code.Format("    public static msMsgName = \"%s\";\r\n\r\n", name.c_str());
			tsCode += code;
		}

		AString code;
		for (auto m = d->begin(); m; ++m)
		{
			AString type = m.get();

			// 找不到类型, 即作为存在的结构处理
			FieldInfo info = GetFieldInfoByTypeString(type);
			if (info==NULL || proList->exist(type.c_str()))
			{
				if (type == "any")
				{
					code.Format("    public %s :any = null;		%s\r\n", m.key().c_str(),  GetMemberNote(noteData, m.key()).c_str());
					tsCode += code;
					continue;
				}
				if (type.sub(0, 3) == "map")
				{
					AString temp = type;
					AString subType = temp.SplitBlock('<', '>');
					if (subType.length() > 0)
					{
						AString tsType = GetTsType(subType, proList, false);
						if (tsType.length() > 0)
						{
							code.Format("    public %s :{[key:number]:%s} = {};		%s\r\n", m.key().c_str(), tsType.c_str(), GetMemberNote(noteData, m.key()).c_str());
							tsCode += code;
							continue;
						}
					}
					LOG(" type error > %s", type);
					continue;
				}

				code.Format("    public %s:{} = new %s();	// Msg	%s\r\n", m.key().c_str(), type.c_str(), GetMemberNote(noteData, m.key()).c_str());
				tsCode += code;

				code.Format("        this.%s = new %s();\r\n", m.key().c_str(), type.c_str());
				resetClearCode += code;

				// 加到是否为消息类型的函数代码
				AString tempIsMsg;
				tempIsMsg.Format("           case \"%s\": return true;\r\n", m.key().c_str());
				isMsgCode += tempIsMsg;

				// 加入到类型信息中
				AString tempTypeString;
				tempTypeString.Format("            case \"%s\":  return NiceData._getType(%d);		// Object\r\n", m.key().c_str(), FIELD_NICEDATA);
				typeInfoCode += tempTypeString;

				if (!proList->exist(type.c_str()))
				{
					code.Format("import {%s} from \"./MsgStruct\"\r\n", type.c_str());
					if (!tempImportList.exist(code))
					{
						tempImportList.insert(code, true);
						headCode += code;						
					}
				}

				continue;
			}		

			AString tempTypeString;
			tempTypeString.Format("            case \"%s\":  return NiceData._getType(%d);		// %s\r\n", m.key().c_str(), info->getType(), info->getTypeString());
			typeInfoCode += tempTypeString;

			//if (info == NULL)
			//{
			//	LOG("Table %s > %s : %s can not to TS", name.c_str(), m.key().c_str(), type.c_str());
			//	continue;
			//}
			AString szType = "";
			switch (info->getType())
			{
			case FIELD_STRING:
			case FIELD_CHAR_STRING:
				szType = ": string = \"\"";
				break;

			case FIELD_BYTE:
			case FIELD_INT:
			case FIELD_SHORT:
			case FIELD_INT64:
			case FIELD_UINT64:
			case FIELD_FLOAT:
			case FIELD_DOUBLE:
				szType = ": number = 0";
				break;

			case  FIELD_BOOL:
				szType = ": boolean = false";
				break;				

			case FIELD_ARRAYDATA:
			{
				AString tempTemp = type;
				AString str = splitTemplateType(type, true);

				szType.Format(": %s[] = []", str.c_str());

				AString templateType = tempTemp.SplitBlock('<', '>');
				FieldInfo info = GetFieldInfoByTypeString(templateType);
				if (info==NULL && !proList->exist(str.c_str()))
				{
					code.Format("import {%s} from \"./MsgStruct\"\r\n", str.c_str());
					if (!tempImportList.exist(code))
					{
						tempImportList.insert(code, true);
						headCode += code;
					}
				}

				if (info == NULL)
				{
					// 追加上消息数组新建元素
					AString elementString;
					elementString.Format("            case \"%s\": return new  %s();\r\n", m.key().c_str(), str.c_str());
					arrayElementCode += elementString;
				}

				break;
			}
			case  FIELD_NICEDATA:
			{
				szType.Format(": Object = {}");
				break;
			}
			}
			if (szType == "")
			{
				LOG("Table %s > %s : %s can not to TS", name.c_str(), info->getName().c_str(), info->getTypeString());
				continue;
			}
			else
			{
				code.Format("    public %s %s;	// %s	%s\r\n", m.key().c_str(), szType.c_str(), info->getTypeString(),  GetMemberNote(noteData, m.key()).c_str());
				tsCode += code;
				
				szType.SplitLeft("=");
				code.Format("        this.%s =%s;\r\n", m.key().c_str(), szType.c_str());
				resetClearCode += code;
			}
		}

		tsCode += "\r\n";
		tsCode += "    public GetType(memberName:string):AData\r\n    {\r\n";
		tsCode += "        switch (memberName)\r\n        {\r\n";
		tsCode += typeInfoCode;
		tsCode += "        }\r\n        return null;\r\n";	
		tsCode += "    }\r\n";

		// 新建数组元素
		tsCode += "\r\n";
		tsCode += "    public NewArrayElement(arrayName:string): BaseMsg\r\n    {\r\n";
		tsCode += "        switch (arrayName)\r\n        {\r\n";
		tsCode += arrayElementCode;
		tsCode += "        }\r\n        return null;\r\n";
		tsCode += "    }\r\n";

		// 是否为子消息类型
		tsCode += "\r\n";
		tsCode += "    public IsMsg(memberName) :boolean\r\n    {\r\n";
		tsCode += "        switch (memberName)\r\n        {\r\n";
		tsCode += isMsgCode;
		tsCode += "        }\r\n        return false;\r\n";
		tsCode += "    }\r\n";

		// 初始清理
		tsCode += "\r\n";
		tsCode += "    public ResetClear()\r\n    {\r\n";
		tsCode += resetClearCode;
		tsCode += "    }\r\n";
		

		tsCode += "}\r\n\r\n";

	}	
	headCode += "\r\n";
	if (bFirst)
		secordString += "        }\r\n        return null;\r\n    }\r\n}\r\n\r\n";
	AString comString = headCode + secordString;
	return comString + tsCode;
}

// 获取TS数据类型, 如 "int>", 或者 int , arraydata ...
AString GetTsType(AString typeString, AutoNice proList, bool bSetDefault )
{
	AString tempType = typeString;
	AString leftType = tempType.SplitLeft(">");
	FieldInfo info = NULL;
	if (leftType.length() > 0)
	{
		if (proList->exist(leftType.c_str()))
			return leftType;

		info = FieldInfoManager::getFieldInfo(leftType.c_str());
	}
	else
	{
		if (proList->exist(typeString.c_str()))
			return typeString;
		info = FieldInfoManager::getFieldInfo(typeString.c_str());
	}
	if (info == NULL)
	{
		LOG("Type > %s can not to TS", typeString.c_str());
		return "";
	}
	const char *szType = "";
	switch (info->getType())
	{

	case FIELD_STRING:
	case FIELD_CHAR_STRING:
		szType = bSetDefault ? ": string = \"\"" : "string";
		break;

	case FIELD_BYTE:
	case FIELD_INT:
	case FIELD_SHORT:
	case FIELD_INT64:
	case FIELD_UINT64:
	case FIELD_FLOAT:
	case FIELD_DOUBLE:
		szType = bSetDefault ? ": number = 0":"number";
		break;

	case  FIELD_BOOL:
		szType = bSetDefault ? ": boolean = false" : "boolean";
		break;

	case FIELD_ARRAYDATA:
		szType = bSetDefault ? " = []" : "[]";
		break;
	}

	if (szType == "")
	{
		LOG("Type > %s can not to TS", info->getTypeString());
	}

	return szType;
}
//-------------------------------------------------------------------------

void CExcelToolDlg::OnBnRestartLogicServer()
{
	ClearLog();
	ASYNC(&ToolInfo::RestartLogicServer, msTool.getPtr());
}

void _AsyncConnectDB(CExcelToolDlg *pDlg)
{
	pDlg->AsyncConnectDB();
}

void CExcelToolDlg::OnBnReconnect()
{	
	CoroutineTool::AsyncCall(_AsyncConnectDB, this);

}

void CExcelToolDlg::AsyncConnectDB()
{
	ClearLog();

	CString strServerID;
	mEditUpdateServerID.GetWindowText(strServerID);

	int serverID = TOINT((LPCSTR)strServerID);
	if (serverID <= 0)
	{
		MessageBox("输入需要更新的区号");
		return;
	}

	NiceData param;
	param["SERVER_ID"] = serverID;

	int nResult = 0;
	AutoNice resp = mAccountDBUnit->AwaitRequestDB("g_global_config", "", "g_get_logic_server_config", &param, 6000, &nResult);
	if (!resp || nResult== eDBWaitTimeOver)
	{
		LOG("需要先连接帐号服务器");
		MessageBox("先连接帐号服务器");
		return;
	}
	AutoNice configData = (tNiceData*)resp["INFO"];
	if (!configData)
	{
		LOG("未回复DB配置, 可能游戏区不存在 : %d", serverID);
		return;
	}

	LogicDBConfig		config;
	config.Full(configData);
	LOG("获取Logic DB配置 : %d : %s", serverID, config.SERVER_NAME.ANIS().c_str());

	mEditGateIP.SetWindowText(LanToExtranet(config.db_config.db_node.gate.ip).c_str());
	mEditGatePort.SetWindowText(STRING(config.db_config.db_node.gate.port));

	mEditSaftCode.SetWindowText(STRING(config.db_config.db_node.gate.saft_code));

	mEditDBWorkerID.SetWindowText(STRING(config.db_config.worker_id));

	NetAddress gateAddr(LanToExtranet(config.db_config.db_node.gate.ip).c_str(), config.db_config.db_node.gate.port);
	if (mUserNode->AwaitConnectGate(gateAddr))
	{
		mButtonShowDBConnectInfo.SetWindowText("DB连接成功");
		LOG("DB连接成功: %d: %s > %s", serverID, config.SERVER_NAME.ANIS().c_str(), gateAddr.dump().c_str());
	}
	else
	{
		mButtonShowDBConnectInfo.SetWindowText("连接失败");
		LOG("DB连接失败: %d: %s > %s", serverID, config.SERVER_NAME.ANIS().c_str(), gateAddr.dump().c_str());
	}
}


void CExcelToolDlg::OnBnClickedButtonGenerateProtoc2()
{
	ClearLog();

	CString configPath;
	mEditConfigPath.GetWindowText(configPath);

	AString fileName;
	fileName.Format("%s/LoginProtocol.txt", (LPCTSTR)configPath);
	GenerateProtocol(fileName, "LoginMsgData");
}


void CExcelToolDlg::OnBnCreateAccountDB()
{
	ASYNC(&ToolInfo::AwaitCreateAccountDB, msTool.getPtr());
}


void CExcelToolDlg::AsyncNewCreateAccountDB()
{
	ClearLog();

	AString dbConfigFile;
	if (mToolConfig)
		dbConfigFile = mToolConfig->GetValue("account_config", 1).c_str();

	if (dbConfigFile.length() <= 0)
	{
		MessageBox("未配置DB > RunConfig/ToolConfig.csv>account_config", "", MB_ICONERROR);
		return;
	}

	CString configPath;

	mEditConfigPath.GetWindowText(configPath);

	AString listFileName;
	listFileName.Format("%s/%s.xlsx", (LPCTSTR)configPath, dbConfigFile.c_str());

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

	CString makeTablePath;
	mEditMakeConfigPath.GetWindowText(makeTablePath);

	AString targetPath;
	targetPath.Format("%s/", (LPCTSTR)makeTablePath);
	FileDataStream::CreateMultiDirectory(targetPath.c_str());


	AString name;
	name.Format("%s%s.csv", targetPath.c_str(), dbConfigFile.c_str());
	configListTable->SaveCSV(name.c_str());

	CString path;
	mEditMakeJSPath.GetWindowText(path);
	AString codePath;
	codePath.Format("%s/", (LPCTSTR)path);

	for (auto it = configListTable->GetRecordIt(); *it; ++(*it))
	{
		ARecord record = it->Record();

		const char *szTableName = record["INDEX"].c_str();

		AString fileName;
		fileName.Format("%s/%s.xlsx", (LPCTSTR)configPath, record["XLSX"].c_str());
		AutoTable table = XlsxTableLoader::LoadXlsx(fileName, record["SHEET"].string());
		if (table)
		{
			table->SetTableName(szTableName);

			AString name;
			name.Format("%s%s.csv", targetPath.c_str(), szTableName);
			table->SaveCSV(name.c_str());

			// 创建TS脚本
			//if (record["EXPORT_TS"])
			//{
			//	AutoTable db = tBaseTable::NewBaseTable(false);
			//	int i = 0;
			//	while (true)
			//	{
			//		ARecord fieldInfo = table->GetRecord(i++);
			//		if (fieldInfo)
			//		{
			//			db->AppendField(fieldInfo["NAME"].c_str(), fieldInfo["TYPE"].c_str());
			//		}
			//		else
			//			break;
			//	}
			//	if (db->GetField()->check())
			//	{
			//		db->SetTableName(szTableName);
			//		AString tsCode = GenerateCodeTS::GenerateConfigTS(db, true);

			//		AString tsFile;
			//		tsFile.Format("%s/%s.ts", codePath.c_str(), szTableName);
			//		FileDataStream  f(tsFile.c_str(), FILE_CREATE_UTF8);
			//		f._write((void*)tsCode.c_str(), tsCode.length());
			//		f.close();
			//		LOG("Succeed export ts code > %s", tsFile.c_str());
			//	}
			//}


			// 发送到DB 进行创建
			//NiceData  param;
			//param["TABLE"] = table.getPtr();
			//mDBUnit->RequestDB("", "", "t_create_db", &param, 6000, CloudDBCall( &ToolInfo::OnResponse, msTool.getPtr()) );

			AutoEvent evt = mAccountBigUnit->StartEvent("CreateDBMsg", { Actor_DBWorker, 0});
			HAsyncRequestEvent request = evt;
			request["TABLE"] = table.getPtr();
			request["TABLE_NAME"] = szTableName;
			request["DB"] = "AccountDB";
			AutoEvent resultEvent = request->AwaitReqeust(mAccountBigUnit, {Unit_DBWorker, 0}, 6);
			if (resultEvent)
			{
				if (resultEvent["RESULT"])
					LOG("-----------------------------------------------------\r\nCExcelToolDlg: DB opereate : Create %s table : result Sueeccd >\r\n%s\r\n------------------------------------------", szTableName, resultEvent->GetData()["ERROR"].c_str())
				else
					LOG("----------------------------------------------------\r\nCExcelToolDlg: DB opereate : Create %s table : result fail >\r\n%s\r\n-----------------------------------------", szTableName, resultEvent->GetData()["ERROR"].c_str());
			}
			else
				LOG("CExcelToolDlg : request create DB fail > [%s], may be connect fail", szTableName);
		}
		else
			LOG("Fail load xlsx  %s, sheet %s, index %s", record["XLSX"].c_str(), record["SHEET"].c_str(), record["INDEX"].c_str());
	}
	LOG("********************** finish all create db ******************************");
}




void CExcelToolDlg::OnBnGenrateRunconfig()
{
	ClearLog();

	CString configPath;
	mEditConfigPath.GetWindowText(configPath);

	AString fileName;
	fileName.Format("%s/RunConfigProtocol.txt", (LPCTSTR)configPath);
	AutoNice info =  GenerateProtocol(fileName, "RunConfigStruct", false);

	//LOG("%s", info->dump().c_str());

	mEditMakeConfigPath.GetWindowText(configPath);

	AString path = (LPCSTR)configPath;
	path.replace("/Config", "/RunConfig");

	AString configFileName = path;
	configFileName += "/RunInfo.json";

	// 读取当前的
	FileDataStream  f(configFileName.c_str(), FILE_CREATE);
	if (f)
	{
		AString infoString = info->ToJSON();
		f._write((void*)infoString.c_str(), infoString.length());
	}
}

#define  UPDATE_RUNCONFIG		1

#if UPDATE_RUNCONFIG
// 如果未生成, 则注释生成后, 再打开此功能

void CExcelToolDlg::OnBnUpdateRunconfig()
{
	ClearLog();

	AString info;
	RunConfig::GenerateRunConfigJson(info);
	LOG("%s", info.c_str());
	//info.setNull();
	//if (!RunConfig::LoadCheckConfig("gate_config", info, "GateServerConfig"))
	//	LOG("%s", info.c_str())
	//else
	//	LOG("Check ok");

	//CString configPath;
	//mEditMakeConfigPath.GetWindowText(configPath);

	//AString path = (LPCSTR)configPath;
	//path.replace("/Config", "/RunConfig");

	//// 读取注释
	//AString commPath = (LPCSTR)configPath;
	//commPath.replace("/Config", "/RunConfig");
	//commPath += "/RunInfo.json";
	////AutoNice commInfo = RunConfig::ReadJson(commPath.c_str());
	//AutoNice commInfo = MEM_NEW NiceData();
	//AString str = base64_decode( gCommentString ).c_str();
	//
	//commInfo->FullJSON(str);
	//LOG("info >\r\n%s", commInfo->dump().c_str());

	//AString configFileName = path;
	//configFileName += "/Run.json";

	//RunConfigData	config;

	//// 读取当前的
	//AutoNice existConfig = RunConfig::ReadJson(configFileName.c_str());
	//if (existConfig)
	//	config.Full(existConfig);
	////LOG("info >\r\n%s", existConfig->dump().c_str());

	////DataBuffer  d;
	////config.serialize(&d);

	////NiceData resultData;
	////d.seek(0);
	////resultData.restore(&d);
	//Auto<NiceData> resultData = MEM_NEW NiceData();
	//AutoNice d = resultData;
	//config.ToData(d);
	//d->msKey = "RunConfigData";
	////LOG("%s", resultData->dump().c_str());
	//AString resultJson = resultData->ToJSONEx(commInfo.getPtr());
	//FileDataStream  wf(configFileName.c_str(), FILE_CREATE);
	//if (wf)
	//{
	//	wf._write((void*)resultJson.c_str(), resultJson.length());
	//	LOG("Update run config > %s", configFileName.c_str());
	//}
	//else
	//	LOG("Fail create run config > %s", configFileName.c_str());
}

#else
void CExcelToolDlg::OnBnUpdateRunconfig()
{}
#endif

void CExcelToolDlg::OnBnMakeConfigPacket()
{
	GenerateConfigPacketAndCode(false);
}

#include "HttpClient.h"

void CExcelToolDlg::OnBnSendHttpTest()
{
	AutoTable myTable = XlsxTableLoader::LoadXlsx("e:/my.xlsx", "1");
	AutoTable  infoTable = XlsxTableLoader::LoadXlsx("e:/info.xls", "2");

	AutoTable resultTable = tBaseTable::NewBaseTable();
	resultTable->AppendField("INDEX", FIELD_INT);
	resultTable->AppendField("NAME", FIELD_STRING);
	resultTable->AppendField("ZY", FIELD_STRING);
	resultTable->AppendField("LIMIT", FIELD_INT);
	resultTable->AppendField("NUM", FIELD_INT);
	resultTable->AppendField("PASS", FIELD_STRING);

	int sortIndex = 75598;
	resultTable->CreateRecord(0, true)["PASS"] = sortIndex;


	//for (auto it = myTable->GetRecordIt(); *it; ++(*it))
	//{
	//	ARecord re = it->Record();
	//	AString code = re["CODE"];
	//	AString zy = re["ZY"];

	//	for (auto nIt=infoTable->GetRecordIt(); *nIt; ++(*nIt))
	//	{
	//		ARecord infoRe = nIt->Record();
	//		AString zyInfo;
	//		zyInfo.set(infoRe["ZY"].string().c_str(), 2);
	//		AString nameInfo;
	//		nameInfo.set( infoRe["NAME"].c_str(), 4);

	//		if (nameInfo == code && zyInfo == zy)
	//		{
	//			ARecord result = resultTable->CreateRecord((int)re[0], true);
	//			result["NAME"] = infoRe["NAME"];
	//			result["ZY"] = infoRe["ZY"];
	//			result["LIMIT"] = infoRe["LIMIT"];
	//			result["NUM"] = infoRe["NUM"];
	//			if ((int)infoRe["LIMIT"] >= sortIndex)
	//				result["PASS"] = "OK";
	//		}
	//	}
	//}


	for (auto nIt = infoTable->GetRecordIt(); *nIt; ++(*nIt))
	{
		ARecord infoRe = nIt->Record();
		AString zyInfo;
		zyInfo.set(infoRe["ZY"].string().c_str(), 2);
		AString nameInfo;
		nameInfo.set(infoRe["NAME"].c_str(), 4);

		if ((int)infoRe["LIMIT"] >= sortIndex)
		{
			AString zyName = infoRe["ZY"];
			//if (strstr(zyName.c_str(), u8"计算机") != NULL || strstr(zyName.c_str(), u8"信息") != NULL || strstr(zyName.c_str(), u8"物理学") != NULL)
			{
				ARecord result = resultTable->CreateRecord((int)infoRe[0], true);
				result["NAME"] = infoRe["NAME"];
				result["ZY"] = infoRe["ZY"];
				result["LIMIT"] = infoRe["LIMIT"];
				result["NUM"] = infoRe["NUM"];
				//if ((int)infoRe["LIMIT"] >= sortIndex)
				result["PASS"] = "OK";
			}
		}
	}

	resultTable->SaveCSV("e:/Result_ALL.csv");

	LOG("Finish");

	return;

	CString info;
	CString ip;
	CString port;
	mEditHttpParam.GetWindowText(info);

	mEditUpdateServerID.GetWindowText(ip);
	if (ip.GetLength()<=0)
		mEditGateIP.GetWindowText(ip);
	mEditGatePort.GetWindowText(port);
	CString addr;
	addr.Format("%s://%s:%s", mbHttps ? "https" : "http", (LPCSTR)ip, (LPCSTR)port);
	
	CHttpClient http;
	std::string resp;
	WString  wString;
	AString ddd;
	ddd.Format("%s", (LPCSTR)info); // AString::getUTF8((LPCTSTR)info);
	//ddd += (const wchar_t*)(LPCSTR)info;
	

	//wString.resize(ddd.length() * 3);
	//strcpy((char*)wString.str(), ddd.c_str(), ddd.length());

	//ddd = 

	//ddd.toANIS();

	//ddd.toUTF8();

	http.Post((LPCSTR)addr, ddd.UTF8().c_str(), resp, 10);

	ClearLog();
	LOG("%s\r\n%s\r\n *** respon > %s", (LPCSTR)addr, ddd.c_str(), AString::getANIS( resp.c_str() ).c_str());
}


void CExcelToolDlg::OnBnApplyConfig()
{
	ClearLog();
	mButtonApplyConfig.EnableWindow(FALSE);
	mButtonApplyConfig.SetWindowText("配置应用中...");
	ASYNC(&ToolInfo::AwaitApplyConfig, msTool.getPtr());
}


void CExcelToolDlg::AsyncApplyUpdateConfig()
{
	AutoEvent evt = mBigMsgUnit->StartEvent("ApplayUploadConfigData", { Unit_DBWorker, 0 });
	HAsyncRequestEvent request = evt;

	AutoEvent resultEvent = request->AwaitReqeust(mBigMsgUnit, DB_WORKER_ID, 30);
	if (resultEvent)
		LOG("AsyncApplyUpdateConfig >\r\n %s", resultEvent->GetData().dump().c_str())
	else
		LOG("AsyncApplyUpdateConfig request fail");

	mButtonApplyConfig.EnableWindow(TRUE);
	mButtonApplyConfig.SetWindowText("确认应用配置");

	AString state = resultEvent["RUN_STATE"];
	if (state.length() > 0)
		MessageBox(state.ANIS().c_str());
}




void CExcelToolDlg::OnBnUploadPluginDll()
{
	
	CFileDialog    dlgFile(TRUE, NULL, NULL, OFN_HIDEREADONLY, _T("Describe Files (*.dll)|*.dll|All Files (*.*)|*.*||"), NULL);

	if (dlgFile.DoModal())
	{
		CString strFile = dlgFile.GetPathName();

		FileDataStream		pakFile((LPCSTR)strFile, FILE_READ);
		if (pakFile)
		{
			Auto<DataBuffer> data = pakFile.readAllData();

			Hand<Cloud_SendDataPartEvent> evt = mBigMsgUnit->StartEvent("Cloud_SendDataPartEvent", DB_WORKER_ID);

			AutoEvent sendEvt = mBigMsgUnit->StartEvent("UploadPluginDllBigEvent", DB_WORKER_ID);

			AutoData zipData = MEM_NEW DataBuffer(1024);
			if (!data->ZipData(zipData))
			{
				LOG("Zip data fail");
				return;
			}

			sendEvt["DLL_DATA"] = zipData.getPtr();
			sendEvt->DoEvent();
			evt->SendBigEvent(sendEvt);
			return;
		}
	}
	MessageBox("需要选择插件DLL文件");
}

#include "FileNetTransfer.h"

void CExcelToolDlg::OnBnApplyPluginDll()
{
	ClearLog();

	ASYNC(&ToolInfo::AwaitApplayUpdatePluginDll, msTool.getPtr());
}
void CExcelToolDlg::AsyncApplayUpdatePluginDll()
{
	AutoEvent evt = mBigMsgUnit->StartEvent("ApplyUpdatePluginDll", { Unit_DBWorker, 0 });
	HAsyncRequestEvent request = evt;

	AutoEvent resultEvent = request->AwaitReqeust(mBigMsgUnit, DB_WORKER_ID, 30);
	if (resultEvent)
		LOG("ApplyUpdatePluginDll >\r\n %s", resultEvent->GetData().dump().c_str())
	else
		LOG("ApplyUpdatePluginDll request fail");
}



void CExcelToolDlg::OnBnMakeRunPacket()
{
	ClearLog();
	//CString m_FileDir;
	//BROWSEINFO bi;
	//ZeroMemory(&bi, sizeof(BROWSEINFO));
	//bi.hwndOwner = m_hWnd;
	//bi.ulFlags = BIF_RETURNONLYFSDIRS;
	//LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
	//BOOL bRet = FALSE;
	//TCHAR szFolder[MAX_PATH * 2];
	//szFolder[0] = ('\0');
	//if (pidl)
	//{
	//	if (SHGetPathFromIDList(pidl, szFolder))
	//		bRet = TRUE;
	//	IMalloc *pMalloc = NULL;
	//	if (SUCCEEDED(SHGetMalloc(&pMalloc)) && pMalloc)
	//	{
	//		pMalloc->Free(pidl);
	//		pMalloc->Release();
	//	}
	//}
	CString path;
	mEditRunServerPath.GetWindowText(path);
	AString runPath = (LPCSTR)path;
	if (runPath.length() <= 0)
	{
		MessageBox("选择运行目录");
		return;
	}
	runPath += "/";
	LOG("开始检查所需运行文件: %s", runPath.c_str());

	AutoTable listTable = tBaseTable::NewBaseTable();
	if (!listTable->LoadCSV("RunConfig/RunFileList.csv"))
	{
		MessageBox("未设置运行文件列表: RunConfig/RunFileList.csv");
		return;
	}

	CString cppCodePath;
	mEditMakeCPath.GetWindowText(cppCodePath);
	FileDataStream  verFile(cppCodePath + "/Vesion.h", FILE_READ);
	AString verInfo = "#define SERVER_VERSION_FLAG \"Unkwon\"";
	verFile.readLine(verInfo);
	AString left = verInfo.SplitLeft("SERVER_VERSION_FLAG \"");
	AString ver = verInfo.SplitLeft("\"");
	ver.replace(" ", "");

	// 创建运行文件目录, 在运行目录下面
	AString pdbPath;
	pdbPath.Format("%sPDB/%s/", runPath.c_str(), ver.c_str());

	EasyGenerateResourcesPack runPacketTool;
	bool bHaveError = false;
	for (auto it=listTable->GetRecordIt(); *it; ++(*it))
	{
		AString fileName = runPath;
		AString name;
		it->GetKey(name);
		fileName += name;
		FileDataStream f(fileName.c_str(), FILE_READ);
		if (f)
		{
			AutoData data = f.readAllData();
			runPacketTool.AppendResourceData(name.c_str(), data);

			// 创建到PDB备份目录 中
			AString path;
			path.Format("%s%s", pdbPath.c_str(), name.c_str());
			FileDataStream::CreateMultiDirectory(FileDataStream::GetPathName(path.c_str()).c_str());
			FileDataStream bakPdb(path.c_str(), FILE_CREATE);
			bakPdb._write(data->data(), data->dataSize());
			bakPdb.close();
		}
		else
		{
			LOG("No exist run file > %s", fileName.c_str());
			bHaveError = true;
		}
	}
	if (bHaveError)
	{
		MessageBox("缺少运行文件");
		return;
	}

	// 把配置文件也备份到PDB目录中
	CString makeTablePath;
	mEditMakeConfigPath.GetWindowText(makeTablePath);

	AString targetPath;
	targetPath.Format("%s/t_configtable.pak", (LPCTSTR)makeTablePath);

	FileDataStream f(targetPath.c_str(), FILE_READ);
	Auto<DataBuffer> data = f.readAllData();
	if (data)
	{
		AString path;
		path.Format("%sConfig/t_configtable.pak", pdbPath.c_str());
		FileDataStream::CreateMultiDirectory(FileDataStream::GetPathName(path.c_str()).c_str());
		FileDataStream bakPdb(path.c_str(), FILE_CREATE);
		bakPdb._write(data->data(), data->dataSize());
		bakPdb.close();
	}
	else
	{
		CString info;
		info.Format("未打包运行文件: %s", targetPath.c_str());
		MessageBox(info);		
	}

	AString errorInfo;
	if (runPacketTool.GeneratePack("RunConfig/server_run.pak", errorInfo))
		LOG("成功打包运行文件 [%s] : RunConfig/server_run.pak", ver.c_str())
	else
		LOG("打包失败 > %s", errorInfo.c_str());
}


void CExcelToolDlg::OnBnUploadRunPacket()
{
	CoroutineTool::AsyncCall<CExcelToolDlg*>(AsyncUploadRunPacket, this);
}

void CExcelToolDlg::AsyncUploadRunPacket(CExcelToolDlg *pDlg)
{
	FileDataStream f("RunConfig/server_run.pak", FILE_READ);

	Auto<DataBuffer> data = f.readAllData();
	f.close();
	if (!data)
	{
		pDlg->MessageBox("未打包运行文件: RunConfig/server_run.pak");
		return;
	}
	pDlg->mButtonUploadRunPacket.EnableWindow(FALSE);

	pDlg->ClearLog();
	LOG("Now zip run packet,  Wait a minute ... ")

	FileNetTransferSendControl  sendTool(2 * 1024*1024);
	if (!sendTool.ReadySendData(data))
	{
		pDlg->MessageBox("准备发送数据失败, 可能压缩失败, 重试几次");
		return;
	}

	// 长时间阻塞, 可能已经断开连接, 重新连接
	//pDlg->OnBnReconnect();
	LOG("Now reconnect,  Wait  ... ")
	{
		CString ip;
		CString port;
		pDlg->mEditAccountIP.GetWindowText(ip);
		pDlg->mEditAccountPort.GetWindowText(port);

		CString safe;
		pDlg->mEditAccountSafeCode.GetWindowText(safe);

		pDlg->mAccountNode->SetSafeCode(TOINT((LPCSTR)safe));
		if (!pDlg->mAccountNode->AwaitConnectGate(NetAddress((LPCSTR)ip, TOINT((LPCSTR)port))))
		{
			pDlg->mButtonUploadRunPacket.EnableWindow(TRUE);
			CString info;
			info.Format("连接帐号网关失败 : [%s:%d]", (LPCSTR)ip, TOINT((LPCSTR)port));
			return;
		}
	}

	// 使用ID为1保存索引
	NiceData param;

	param[RECORD_KEY] = SERVER_RUN_RESOURCE;
	param["TIME"] = TimeManager::Now();

	int nResult = 0;
	AutoNice resp = pDlg->mAccountDBUnit->AwaitRequestDB("g_global_resourse", SERVER_RUN_RESOURCE, "t_save_record", &param, 6000, &nResult);
	if (nResult!=eNoneError)
	{
			LOG("t_save_record_data run fail > %d", nResult);			
			pDlg->mButtonUploadRunPacket.SetWindowText("上传运行文件");
			pDlg->mButtonUploadRunPacket.EnableWindow(TRUE);
			pDlg->MessageBox("远程服务器上可能开启了工具, 不可使用相同的本地127地址, 否则远程工具上传失败");
			return;		
	}

	Array<int> mDataIndexList;
	mDataIndexList.resize(sendTool.mZipPartDataList.size());
	bool bHaveError = false;
	for (int i=0; i<sendTool.mZipPartDataList.size(); ++i)
	{
		AutoData d = sendTool.mZipPartDataList[i];

		NiceData param;

		param["DATA"] = d.getPtr();
		param[RECORD_KEY] = i + 2;

		int nResult = 0;
		AutoNice resp = pDlg->mAccountDBUnit->AwaitRequestDB("g_global_resourse", "", "t_save_record", &param, 6000, &nResult);
		if (!resp && nResult!=eNoneError)
		{
			LOG("t_save_record_data run fail > %d", nResult);
			bHaveError = true;
		}
		else
			mDataIndexList[i] = i + 2;
		CString txt;
		txt.Format("upload %.2f/100", (float)i / sendTool.mZipPartDataList.size() * 100);
		pDlg->mButtonUploadRunPacket.SetWindowText(txt);
	}

	pDlg->mButtonUploadRunPacket.SetWindowText("上传运行文件");
	pDlg->mButtonUploadRunPacket.EnableWindow(TRUE);

	if (bHaveError)
		pDlg->MessageBox("上传运行文件失败");
	else
	{
		AutoData listData = MEM_NEW DataBuffer();
		if (listData->writeArray(mDataIndexList))
		{
			NiceData param;

			param["DATA"] = listData.getPtr();
			param[RECORD_KEY] = SERVER_RUN_RESOURCE;
			resp = pDlg->mAccountDBUnit->AwaitRequestDB("g_global_resourse", "1", "t_save_record", &param, 6000, &nResult);
			if (nResult!=eNoneError)
			{
				LOG("t_save_record_data run fail > %d", nResult);
				bHaveError = true;
			}
		}
		else
			LOG("Save index array fail");
	}

	LOG("%s 上传运行文件", bHaveError ? "失败":"成功");
}




void CExcelToolDlg::OnBnSelectRunPath()
{
	CString m_FileDir;
	BROWSEINFO bi;
	ZeroMemory(&bi, sizeof(BROWSEINFO));
	bi.hwndOwner = m_hWnd;
	bi.ulFlags = BIF_RETURNONLYFSDIRS;
	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
	BOOL bRet = FALSE;
	TCHAR szFolder[MAX_PATH * 2];
	szFolder[0] = ('\0');
	if (pidl)
	{
		if (SHGetPathFromIDList(pidl, szFolder))
			bRet = TRUE;
		IMalloc *pMalloc = NULL;
		if (SUCCEEDED(SHGetMalloc(&pMalloc)) && pMalloc)
		{
			pMalloc->Free(pidl);
			pMalloc->Release();
		}
	}

	mEditRunServerPath.SetWindowText(szFolder);
}


void CExcelToolDlg::OnBnCreateServerRun()
{
	mButtonCreateServerRun.EnableWindow(FALSE);
	CoroutineTool::AsyncCall<CExcelToolDlg*>(AsyncCreateServerRun, this);
}

void CExcelToolDlg::AsyncCreateServerRun(CExcelToolDlg *pDlg)
{
	pDlg->_AsyncCreateServerRun();
	//pDlg->_AsyncCreateServerRunEx();
}

#include "Dump.h"


void CExcelToolDlg::_AsyncCreateServerRun()
{
	CExcelToolDlg *pDlg = this;

	CString runPath;
	pDlg->mEditRunServerPath.GetWindowText(runPath);
	if (runPath.GetLength() <= 0)
	{
		pDlg->MessageBox("请先选择正确的运行目录");
		pDlg->mButtonCreateServerRun.EnableWindow(TRUE);
		return;
	}

	pDlg->ClearLog();

	// 运行配置
	CString localIP, wssAddr, serverInfo, logInfo;
	mEditLocalIP.GetWindowText(localIP);
	mEditWssAddress.GetWindowText(wssAddr);
	mEditGameServerName.GetWindowText(serverInfo);
	mLogInfo.GetWindowText(logInfo);

	AString ip = (LPCSTR)localIP;


	AString temp = (LPCSTR)serverInfo;
	AString server = temp.SplitLeft(" ");
	int serverIndex = TOINT(server.c_str());
	server = temp.SplitLeft(" ");
	int serverType = TOINT(temp.c_str());
	if (serverIndex <= 0 || server.length() <= 0)
	{
		pDlg->MessageBox("需要设置正确的游戏区信息, 如: 1 校园一区 1");
		pDlg->mButtonCreateServerRun.SetWindowText("一键部署服务器");
		pDlg->mButtonCreateServerRun.EnableWindow(TRUE);
		return;
	}

	AString logInfos = (LPCSTR)logInfo;
	AString logIP = logInfos.SplitLeft(":");
	int logPort = TOINT(logInfos.c_str());
	
	AString wssInfo = (LPCSTR)wssAddr;
	AString wss = wssInfo.SplitLeft(":");
	int wssPort = TOINT(wssInfo.c_str());
	if (wssPort <= 0)
		wssPort = 8000 + serverIndex;

	// 先连接MYSQL创建库
	CString mysqlAddress, mysqlUser, mysqlPassword, backAddress;
	mEditMysqlIP.GetWindowText(mysqlAddress);
	mEditMysqlUser.GetWindowText(mysqlUser);
	mEditMysqlPassword.GetWindowText(mysqlPassword);
	mEditBackSqlIP.GetWindowText(backAddress);

	AString tempAddress = (LPCSTR)mysqlAddress;
	AString sqlIP = tempAddress.SplitLeft(":");
	int port = TOINT(tempAddress.c_str());
	if (port <= 0)
		port = 3306;

	tempAddress = (LPCSTR)backAddress;
	AString backSqlIP = tempAddress.SplitLeft(":");
	int backPort = TOINT(tempAddress.c_str());
	if (backPort <= 0)
		backPort = 3306;

	NiceData mysqlParam;
	
	mysqlParam[DBIP] = sqlIP;
	mysqlParam[DBPORT] = port;
	mysqlParam[DBUSER] = (LPCSTR)mysqlUser;
	mysqlParam[DBPASSWORD] = (LPCSTR)mysqlPassword;

	MySqlDBTool	mysqlTool;
	if (!mysqlTool.InitStart(mysqlParam))
	{
		LOG("MYSQL参数错误:\r\n%s", mysqlParam.dump().c_str());
		pDlg->mButtonCreateServerRun.EnableWindow(TRUE);
		return;
	}

	AString dbName = "school_db_";
	dbName += serverIndex;

	const char *DATA_DB_NAME = dbName.c_str();

	AString query;
	query.Format("CREATE DATABASE IF NOT EXISTS %s default character set utf8mb4 collate utf8mb4_unicode_ci;", DATA_DB_NAME);	
	if (!mysqlTool.exeSql(query, false))
	{
		LOG("MYSQL创建DB库%s失败:\r\n%s", DATA_DB_NAME, mysqlTool.getErrorMsg());
		pDlg->mButtonCreateServerRun.EnableWindow(TRUE);
		return;
	}

	LOG("Now load run packet file ...");

	NiceData param;
	
	param[RECORD_KEY] = SERVER_RUN_RESOURCE;
	int nResult = 0;
	AutoNice resp = pDlg->mAccountDBUnit->AwaitRequestDB("g_global_resourse", SERVER_RUN_RESOURCE, "t_load_record", &param, 6000, &nResult);
	AutoData listData = (DataStream*)resp["DATA"];
	if (!resp || !listData)
	{
		LOG("t_load_record_data run fail > %d", nResult);
		pDlg->mButtonCreateServerRun.EnableWindow(TRUE);
		return;
	}

	Array<int> indexList;
	listData->seek(0);
	if (!listData->readArray(indexList))
	{
		LOG("Read index list fail");
		pDlg->mButtonCreateServerRun.EnableWindow(TRUE);
		return;
	}

	Auto<DataBuffer> zipData = MEM_NEW DataBuffer();

	for (int i=0; i<indexList.size(); ++i)
	{
		NiceData param;
		param["FIELD"] = "DATA";
		param[RECORD_KEY] = indexList[i];
		int nResult = 0;
		AutoNice resp = pDlg->mAccountDBUnit->AwaitRequestDB("g_global_resourse", "", "t_load_record", &param, 6000, &nResult);
		AutoData listData = (DataStream*)resp["DATA"];
		if (!resp || !listData)
		{
			LOG("t_load_record run fail > %d", nResult);
			pDlg->mButtonCreateServerRun.SetWindowText("一键部署服务器");
			pDlg->mButtonCreateServerRun.EnableWindow(TRUE);
			return;
		}
		zipData->_write(listData->data(), listData->dataSize());
	}

	AString serverRunPath = (LPCSTR)runPath;
	serverRunPath += "/";

	AutoData packetData = MEM_NEW DataBuffer();
	if (zipData->UnZipData(packetData, 0))
	{
		EasyResourcesPack  packet;
		packet.load(packetData.getPtr());

		AString errorInfo;
		packet.Export(serverRunPath.c_str(), errorInfo);

		LOG("Create run file > %s", errorInfo.c_str());
	}
	else
		LOG("Unzip packet data fail");

	FileDataStream::CreateMultiDirectory((serverRunPath + "Log").c_str());
	FileDataStream::CreateMultiDirectory((serverRunPath + "Error").c_str());
	
	// 生成配置 RunConfig/Run.json
	AString configPath = serverRunPath + "RunConfig";

	// 读取注释
	AutoNice commInfo = MEM_NEW NiceData();
	AString str = Base64Tool::decode(_comment::CommentString());
	commInfo->FullJSON(str);

	AutoNice childComm = (tNiceData*)commInfo->get("RunConfigData");
	if (!childComm)
		childComm = commInfo;

	AString configFileName = configPath;
	configFileName += "/Run.json";

	RunConfigData	config;

	// 设置配置
	CString sslKeyName;
	mEditSSLKeyFile.GetWindowText(sslKeyName);
	AString sslKey = (LPCSTR)sslKeyName;
	AString sslFileName = FileDataStream::GetBaseFileName(sslKey.c_str());
	AString crtFileName;
	if (sslFileName.length() > 0)
	{
		crtFileName.Format("./RunConfig/%s_public.crt", sslFileName.c_str());
		sslKey.Format("./RunConfig/%s.key", sslFileName.c_str());
	}

	CString accountGateIP, accountGatePort;
	mEditAccountIP.GetWindowText(accountGateIP);
	mEditAccountPort.GetWindowText(accountGatePort);
	/*
	// 1 Gate
	config.gate_config.title = "MainGate";
	config.gate_config.gate_node.fix_count = 1;
	config.gate_config.gate_node.gate_code = 0;
	config.gate_config.gate_node.address.ip = ip;
	config.gate_config.gate_node.address.port = 5000 + config.gate_config.gate_node.gate_code;
	config.gate_config.gate_node.address.saft_code = -55;

	int gateServerPort = 6000 + serverIndex;

	config.gate_config.gate_server.ip = ip;
	config.gate_config.gate_server.port = gateServerPort;
	config.gate_config.gate_server.saft_code = -55;

	config.gate_config.main_gate.ip = ip;
	config.gate_config.main_gate.port = 5000;
	config.gate_config.main_gate.saft_code = -55;

	//2 Data Share Config
	config.data_share_config.title = "DataShareMemory";
	config.data_share_config.share_key.key = 500 + serverIndex;
	config.data_share_config.share_key.size = 2100000;							// 大约2M, 收发各1M

	//config.data_share_config.sql_db.mDBNAME = "DataDB";
	config.data_share_config.sql_db.mDBBASE = DATA_DB_NAME;
	config.data_share_config.sql_db.mDBIP = sqlIP;
	config.data_share_config.sql_db.mDBPORT = port;
	config.data_share_config.sql_db.mDBUSER = (LPCSTR)mysqlUser;
	config.data_share_config.sql_db.mDBPASSWORD = (LPCSTR)mysqlPassword;
	config.data_share_config.sql_db.mTABLE_LIST = "t_tablelist";

	AString backName = "back_";
	backName += DATA_DB_NAME;
	config.data_share_config.sql_back_db.mDBBASE = backName;
	config.data_share_config.sql_back_db.mDBIP = backSqlIP;
	config.data_share_config.sql_back_db.mDBPORT = backPort;
	config.data_share_config.sql_back_db.mDBUSER = (LPCSTR)mysqlUser;
	config.data_share_config.sql_back_db.mDBPASSWORD = (LPCSTR)mysqlPassword;
	config.data_share_config.sql_back_db.mTABLE_LIST = "t_tablelist";

	// 创建备份库
	if (backSqlIP.length()>0)
	{
		NiceData mysqlParam;

		mysqlParam[DBIP] = backSqlIP;
		mysqlParam[DBPORT] = backPort;
		mysqlParam[DBUSER] = (LPCSTR)mysqlUser;
		mysqlParam[DBPASSWORD] = (LPCSTR)mysqlPassword;

		MySqlDBTool	mysqlTool;
		if (!mysqlTool.InitStart(mysqlParam))
		{
			LOG("备份DB MYSQL参数错误 : 需要手动进行创建\r\n%s", mysqlParam.dump().c_str());			
		}
		else
		{
			AString query;
			query.Format("CREATE DATABASE IF NOT EXISTS %s default character set utf8mb4 collate utf8mb4_unicode_ci;", backName.c_str());
			if (!mysqlTool.exeSql(query, false))
			{
				LOG("MYSQL创建DB库%s失败 : 需要手动进行创建\r\n%s", backName.c_str(), mysqlTool.getErrorMsg());
			}
		}
	}

	//3 Data DB Config
	config.data_db_config.title = "LogicDB";
	config.data_db_config.db_config.db_code = 0;
	config.data_db_config.db_config.db_node.node.ip = ip;
	config.data_db_config.db_config.db_node.node.port = 6100 + serverIndex;
	config.data_db_config.db_config.db_node.node.saft_code = -55;

	config.data_db_config.db_config.db_node.gate.ip = ip;
	config.data_db_config.db_config.db_node.gate.port = gateServerPort;
	config.data_db_config.db_config.db_node.gate.saft_code = -55;

	config.data_db_config.db_config.share_key.key = 500 + serverIndex;
	config.data_db_config.db_config.share_key.size = 4096;
	//config.data_db_config.db_config.update_sql_key = 1000 + serverIndex; // 使用进程ID
	config.data_db_config.db_config.worker_id = serverIndex;
	config.data_db_config.db_config.record_update_milsecond = 20000;

	//config.data_db_config.log_sql.mDBNAME = "Log";
	config.data_db_config.log_sql.mDBBASE = "log";

	config.data_db_config.log_sql.mDBIP = logIP;
	config.data_db_config.log_sql.mDBPORT = logPort;
	config.data_db_config.log_sql.mDBUSER = (LPCSTR)mysqlUser;
	config.data_db_config.log_sql.mDBPASSWORD = (LPCSTR)mysqlPassword;
	config.data_db_config.log_sql.mTABLE_LIST = "";

	config.data_db_config.SERVER_ID = serverIndex;
	config.data_db_config.SERVER_NAME = server.UTF8();
	config.data_db_config.SERVER_TYPE = serverType;
	auto &timeObj = TimeManager::GetMe();
	config.data_db_config.OPEN_TIME.Format("%d/%d/%d 00:00:00", timeObj.GetYear(), timeObj.GetMonth(), timeObj.GetDay());
	config.data_db_config.manager_csv_file = "ManagerList.csv";

	//4 Login Config
	config.login_config.title = "WssLogin";
	config.login_config.data_db_node.node.ip = ip;
	config.login_config.data_db_node.node.port = 3000+serverIndex;
	config.login_config.data_db_node.node.saft_code = -55;

	config.login_config.data_db_node.gate.ip = ip;
	config.login_config.data_db_node.gate.port = gateServerPort;
	config.login_config.data_db_node.gate.saft_code = -55;

	config.login_config.db_user_id = 9000 + serverIndex;

	//5 log server config
	config.log_config.title = "LogServer";
	config.log_config.mIp = "";
	config.log_config.mPort = logPort;
	config.log_config.mDBNAME = "log";
	config.log_config.mDBIP = sqlIP;
	config.log_config.mDBPORT = 3306;
	config.log_config.mDBUSER = (LPCSTR)mysqlUser;
	config.log_config.mDBPASSWORD = (LPCSTR)mysqlPassword;

	config.log_config.AUTHORIZATION1 = "212.129.136.13"; // 平台ip

	CString accountGateSafe;
	mEditAccountSafeCode.GetWindowText(accountGateSafe);
	
	config.login_config.account_db_node.node.saft_code = TOINT((LPCSTR)accountGateSafe);
	config.login_config.account_db_node.node.ip = ip;
	config.login_config.account_db_node.node.port = 2000+serverIndex;

	config.login_config.account_db_node.gate.ip = (LPCSTR)accountGateIP;
	config.login_config.account_db_node.gate.port = TOINT((LPCSTR)accountGatePort);
	config.login_config.account_db_node.gate.saft_code = TOINT((LPCSTR)accountGateSafe);

	config.login_config.account_user_id = 2000 + serverIndex;
	config.login_config.ws_ip = wss;
	config.login_config.ws_port = wssPort;
	config.login_config.key_file = sslKey;
	config.login_config.cert_file = crtFileName;
	config.login_config.worker_id = 20000 + serverIndex;

	//6 BackDB
	config.db_back_config.title = "BackDB";
	config.db_back_config.sql_config.mDBBASE = backName;
	config.db_back_config.sql_config.mDBIP = "127.0.0.1";
	config.db_back_config.sql_config.mDBPORT = backPort;
	config.db_back_config.sql_config.mDBUSER = (LPCSTR)mysqlUser;
	config.db_back_config.sql_config.mDBPASSWORD = (LPCSTR)mysqlPassword;
	*/
	Auto<NiceData> resultData = MEM_NEW NiceData();
	AutoNice d = resultData;
	config.ToData(d);
	d->msKey = "RunConfigData";

	AString resultJson = resultData->ToJSONEx(commInfo.getPtr(), childComm.getPtr());
	FileDataStream::CreateMultiDirectory(configPath.c_str());
	FileDataStream  wf(configFileName.c_str(), FILE_CREATE);
	if (wf)
	{
		wf._write((void*)resultJson.c_str(), resultJson.length());
		LOG("Generate run config > %s", configFileName.c_str());
	}
	else
	{
		LOG("Fail create run config > %s", configFileName.c_str());
	}
	wf.close();
	// 启动DB进程
	StartRun((serverRunPath + "CloudGate.exe").c_str(), (LPCSTR)runPath);
	StartRun((serverRunPath+"DBShareMemory.exe").c_str(), (LPCSTR)runPath);

	tTimer::AWaitTime(1000);
	
	StartRun((serverRunPath+"LogicCloudDB.exe").c_str(), (LPCSTR)runPath);
	tTimer::AWaitTime(2000);

	// 连接DB
	if (mUserNode->AwaitConnectGate(NetAddress(ip.c_str(), gateServerPort)))
	{
		mEditGateIP.SetWindowText(ip.c_str());
		mEditGatePort.SetWindowText(STRING(gateServerPort));
		mEditDBWorkerID.SetWindowText(STRING(serverIndex));

		// 创建DB表
		NiceData param;
		
		param[RECORD_KEY] = GOLBAL_DBLIST_KEY;
		int nResult = 0;
		AutoNice resp = pDlg->mAccountDBUnit->AwaitRequestDB("g_global_config", GOLBAL_DBLIST_KEY, "t_load_record", &param, 6000, &nResult);
		Auto<DataBuffer> listData = (DataStream*)resp["DATA"];
		if (!resp || !listData)
		{
			LOG("t_load_record_data run fail > %d", nResult);
			goto _finish;
		}
		AutoData configData;
		if (!listData->UnZipData(configData, 0))
		{
			LOG("Unzip DB list config fail");
			goto _finish;
		}
		configData->seek(0);
		EasyResourcesPack packet;
		if (!packet.load(configData.getPtr()))
		{
			LOG("Load DB list config packet fail");
			goto _finish;
		}

		//AString err;
		//packet.Export("E:/TestTable/", err);

		DataTableManager  dbMgr;
		if (!dbMgr.LoadTable(&packet, "DBLIST", "DBListPacket"))
		{
			LOG("Load DB list config table  fail");
			goto _finish;
		}

		//  评测等待 DB 启动OK
		UInt64 now = TimeManager::NowTick();
		while (true)
		{
			AutoEvent evt = mBigMsgUnit->StartEvent("ApplayUploadConfigData", { Unit_DBWorker, 0 });
			HAsyncRequestEvent request = evt;

			AutoEvent resultEvent = request->AwaitReqeust(mBigMsgUnit, DB_WORKER_ID, 3);
			if (resultEvent)
				break;
			if (TimeManager::NowTick() - now > 20000)
			{
				LOG("Wait LogicDB start fail, over time 20 Sec, now exit");
				goto _finish;
			}
			LOG("Try check LogicDB start finish, wait try 3 Sec ...");
		}		

		AutoTable listTable = dbMgr.GetTable("DBLIST");
		for (auto it = listTable->GetRecordIt(); *it; ++(*it))
		{
			ARecord record = it->Record();
			AString tableName = record["INDEX"];

			AutoTable table = dbMgr.GetTable(tableName.c_str());

			AutoEvent evt = mBigMsgUnit->StartEvent("CreateDBMsg", DB_WORKER_ID);
			HAsyncRequestEvent request = evt;
			request["TABLE"] = table.getPtr();
			request["TABLE_NAME"] = tableName;
			request["DB"] = "MemDB";
			AutoEvent resultEvent = request->AwaitReqeust(mBigMsgUnit, DB_WORKER_ID, 6);
			if (resultEvent)
			{
				if (resultEvent["RESULT"])
				{
					LOG("-----------------------------------------------------\r\nCExcelToolDlg: DB opereate : Create %s table : result Sueeccd >\r\n%s : Slot %s\r\n------------------------------------------", tableName.c_str(), resultEvent->GetData()["ERROR"].c_str(),
						resultEvent["INFO"].c_str()
					)
				}
				else
					LOG("----------------------------------------------------\r\nCExcelToolDlg: DB opereate : Create %s table : result fail >\r\n%s\r\n-----------------------------------------", tableName.c_str(), resultEvent->GetData()["ERROR"].c_str());
			}
			else
				LOG("CExcelToolDlg : request create DB fail > [%s], may be connect fail", tableName.c_str());
		}

		// 上传游戏配置
		{
			
			param[RECORD_KEY] = GOLBAL_CONFIG_KEY;
			int nResult = 0;
			AutoNice resp = pDlg->mAccountDBUnit->AwaitRequestDB("g_global_config", GOLBAL_CONFIG_KEY, "t_load_record", &param, 6000, &nResult);
			AutoData listData = (DataStream*)resp["DATA"];
			if (!resp || !listData)
			{
				LOG("t_load_record run fail > %d", nResult);
				goto _finish;
			}

			Hand<Cloud_SendDataPartEvent> evt = mBigMsgUnit->StartEvent("Cloud_SendDataPartEvent", DB_WORKER_ID);

			AutoEvent sendEvt = mBigMsgUnit->StartEvent("UploadConfigDataBigEvent", DB_WORKER_ID);
			sendEvt["NEED_APPLAY"] = true;
			sendEvt["CONFIG_DATA"] = (DataStream*)listData.getPtr();
			sendEvt["INFO"] = TimeManager::ToDateTime(resp["TIME"], true);
			sendEvt->DoEvent();
			evt->SendBigEvent(sendEvt);
		}

		LOG("成功完成运行环境创建 : %s, 请查看LogicDB 初始结果", serverRunPath.c_str());
		pDlg->mButtonCreateServerRun.SetWindowText("一键部署服务器");
		pDlg->mButtonCreateServerRun.EnableWindow(TRUE);

		StartRun((serverRunPath + "WebLogin.exe").c_str(), (LPCSTR)runPath);

		return;

	}
	else
		LOG("连接DB Gate 失败");

_finish:
	pDlg->mButtonCreateServerRun.SetWindowText("一键部署服务器");
	pDlg->mButtonCreateServerRun.EnableWindow(TRUE);


}




void CExcelToolDlg::OnBnConnectAccountGate()
{
	ClearLog();

	CString ip;
	CString port;
	mEditAccountIP.GetWindowText(ip);
	mEditAccountPort.GetWindowText(port);

	CString safe;
	mEditAccountSafeCode.GetWindowText(safe);

	mAccountNode->SetSafeCode(TOINT((LPCSTR)safe));

	Hand<ToolInfo> tool = MEM_NEW ToolInfo();
	mAccountNode->Init(NetAddress((LPCSTR)ip, TOINT((LPCSTR)port)), CallBack<HandConnect>(&ToolInfo::OnConnectGateCall, tool.getPtr()));
}


void CExcelToolDlg::OnBnUploadGlobalConfig()
{
	CoroutineTool::AsyncCall<CExcelToolDlg*>(AsyncUploadGlobalConfig, this);
}

void CExcelToolDlg::AsyncUploadGlobalConfig(CExcelToolDlg *pDlg)
{
	CString makeTablePath;
	pDlg->mEditMakeConfigPath.GetWindowText(makeTablePath);

	AString targetPath;
	targetPath.Format("%s/t_configtable.pak", (LPCTSTR)makeTablePath);

	FileDataStream f(targetPath.c_str(), FILE_READ);

	Auto<DataBuffer> data = f.readAllData();

	if (!data)
	{
		CString info;
		info.Format("未打包运行文件: %s", targetPath.c_str());
		pDlg->MessageBox(info);
		return;
	}
	pDlg->mButtonGlobalConfig.EnableWindow(FALSE);

	pDlg->ClearLog();


	// 长时间阻塞, 可能已经断开连接, 重新连接
	//pDlg->OnBnReconnect();
	LOG("Now reconnect,  Wait  ... ")
	{
		CString ip;
		CString port;
		pDlg->mEditAccountIP.GetWindowText(ip);
		pDlg->mEditAccountPort.GetWindowText(port);

		CString safe;
		pDlg->mEditAccountSafeCode.GetWindowText(safe);

		pDlg->mAccountNode->SetSafeCode(TOINT((LPCSTR)safe));
		if (!pDlg->mAccountNode->AwaitConnectGate(NetAddress((LPCSTR)ip, TOINT((LPCSTR)port))))
		{
			pDlg->mButtonGlobalConfig.EnableWindow(TRUE);
			CString info;
			info.Format("连接帐号网关失败 : [%s:%d]", (LPCSTR)ip, TOINT((LPCSTR)port));
			pDlg->MessageBox(info);
			return;
		}
	}

	{
		// 使用ID为1保存 ConfigData
		
		AutoData zipData = MEM_NEW DataBuffer();
		if (!data->ZipData(zipData))
		{
			pDlg->MessageBox("Zip config data fail");
			return;
		}

		NiceData param;
		
		param[RECORD_KEY] = GOLBAL_CONFIG_KEY;
		param["DATA"] = zipData.getPtr();
		param["TIME"] = TimeManager::Now();

		MD5 md(zipData->data(), zipData->dataSize());
		param["MD5"] = md.toString().c_str();

		int nResult = 0;
		AutoNice resp = pDlg->mAccountDBUnit->AwaitRequestDB("g_global_config", GOLBAL_CONFIG_KEY, "t_save_record", &param, 6000, &nResult);
		if (!resp)
		{
			LOG("t_save_record run fail > %d", nResult);
			pDlg->MessageBox("远程服务器上可能开启了工具, 不可使用相同的本地127地址, 否则远程工具上传失败");
		}
		else
		{
			LOG("Succeed upload save config data\r\n%s", resp->dump().c_str());			
		}
	}

	{
		// 使用ID为0保存 DBList, 用于创建DB
		AString info;
		Auto<DataBuffer> dbListData = pDlg->CreateDBListConfigPacket(info);
		if (!dbListData)
		{
			CString str;
			str.Format("Create DB list config packet fail : %s", info.c_str());
			pDlg->MessageBox(str);		
			return;
		}
		AutoData zipData = MEM_NEW DataBuffer();
		if (!dbListData->ZipData(zipData))
		{
			pDlg->MessageBox("Zip DB list data fail");
			return;
		}

		NiceData param;

		param[RECORD_KEY] = GOLBAL_DBLIST_KEY;
		param["DATA"] = zipData.getPtr();
		param["TIME"] = TimeManager::Now();

		int nResult = 0;
		AutoNice resp = pDlg->mAccountDBUnit->AwaitRequestDB("g_global_config", GOLBAL_DBLIST_KEY, "t_save_record", &param, 6000, &nResult);
		if (!resp)
		{
			LOG("t_save_record run fail > %d", nResult);
		}
		else
		{
			LOG("Succeed upload save DB list config\r\n%s", resp->dump().c_str());
		}
	}

	pDlg->mButtonGlobalConfig.EnableWindow(TRUE);

}



void CExcelToolDlg::OnBnClickedButtonSelectConfig5()
{
	char szDir[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH - 1, szDir);
	StartRun("E:/ServerRunTest/LogicCloudDB.exe", "E:/ServerRunTest");
}



AutoNice AnalysisHttpResult(const AString &responseString)
{
	DataBuffer data;
	Base64Tool::decode(responseString.c_str(), data);
	AutoNice httpData = MEM_NEW NiceData();
	data.seek(0);
	httpData->restore(&data);
	return httpData;
}

void _UpdateServerRunFile(CExcelToolDlg *pDlg)
{
	pDlg->UpdateServerRunFile();
}

void CExcelToolDlg::OnBnClickedButtonUpdateServer()
{
	CString ip;
	CString serverID;
	mEditUpdateServerID.GetWindowText(ip);

	mEditGameServerName.GetWindowText(serverID);
	int nID = TOINT((LPCSTR)serverID);
	if (nID <= 0)
	{
		MessageBox("需要设置 游戏区 ID");
		return;
	}

	CoroutineTool::AsyncCall(_UpdateServerRunFile, this);


}

void CExcelToolDlg::UpdateServerRunFile()
{

	ClearLog();

	CString strServerID;
	mEditUpdateServerID.GetWindowText(strServerID);

	int serverID = TOINT((LPCSTR)strServerID);
	if (serverID == 0)
	{
		MessageBox("输入需要更新的区号");
		return;
	}

	NiceData param;
	param["SERVER_ID"] = serverID;
	int nResult = 0;
	AutoNice resp = mAccountDBUnit->AwaitRequestDB("g_global_config", "", "g_get_logic_server_config", &param, 6000, &nResult);
	if (!resp || nResult == eDBWaitTimeOver)
	{
		MessageBox("需要先连接帐号DB");
		return;
	}

	// 区号 -1 表示更新帐号运行程序
	if (serverID==-1)
	{
		CString pass;
		gpExcelToolDlg->mEditCloseServerPassword.GetWindowText(pass);

		AString checkInfo;
		checkInfo.Format("CLOSE2021-%d-%s", serverID, (LPCSTR)pass);
		MD5 md(checkInfo.c_str());

		NiceData param;
		param["HTTP"] = resp["HTTP"];
		param["RESTART"] = true;
		param["PASSWORD"] = md.toString();
		param["IS_ACCOUNT"] = true;

		int nResult = 0;
		AutoNice resp = mAccountDBUnit->AwaitRequestDB("g_global_config", "", "g_update_server_http", &param, 60000, &nResult);
		if (resp && nResult == eNoneError)
		{
			LOG("Succeed finish update server : \r\n%s", resp->dump().c_str());
		}
		else
			ERROR_LOG("Fail : g_update_server_http > %d", nResult);
		return;
	}

	AutoNice configData = (tNiceData*)resp["INFO"];
	if (!configData)
	{
		LOG("未回复DB配置, 可能游戏区不存在 : %d", serverID);
		return;
	}

	LogicDBConfig		config;
	config.Full(configData);

	LOG("开始更新 Logic DB : %d : %s, Wait a long time ...", serverID, config.SERVER_NAME.ANIS().c_str());

	NetAddress addr(LanToExtranet(config.db_config.db_node.gate.ip).c_str(), config.db_config.db_node.gate.port);
	if (mUserNode->AwaitConnectGate(addr))
	{
		mEditGateIP.SetWindowText(config.db_config.db_node.gate.ip.c_str());
		mEditGatePort.SetWindowText(STRING(config.db_config.db_node.gate.port));
		mEditDBWorkerID.SetWindowText(STRING(config.db_config.worker_id));

		CString pass;
		gpExcelToolDlg->mEditCloseServerPassword.GetWindowText(pass);

		AString checkInfo;
		checkInfo.Format("CLOSE2021-%d-%s", serverID, (LPCSTR)pass);
		MD5 md(checkInfo.c_str());

		NiceData param;
		param["HTTP"] = resp["HTTP"];
		param["RESTART"] = true;
		param["PASSWORD"] = md.toString();

		int nResult = 0;
		AutoNice resp = mDBUnit->AwaitRequestDB("t_resource", "", "g_update_server_http", &param, 60000, &nResult);
		if (resp && nResult == eNoneError)
		{
			LOG("Succeed finish update server : \r\n%s", resp->dump().c_str());
		}
		else
			ERROR_LOG("Fail : g_update_server_http > %d", nResult);
	}
	else
		ERROR_LOG("Connect db fail : %s", addr.dump().c_str());
}




void CExcelToolDlg::OnBnClickedSelectHttps()
{
	mbHttps = !mbHttps;
	mSelectHttps.SetCheck(mbHttps ? 1 : 0);
}


void CExcelToolDlg::OnBnSendNotice()
{
	FileDataStream textFile("./RunConfig/notice.txt", FILE_READ);
	AutoData d = textFile.readAllData();
	if (!d)
	{
		MessageBox("需要将公告内容填写在 RunConfig/notice.txt");
		return;
	}
	AString info;
	info.set(d->data(), d->dataSize());

	AString text;
	text.Format("cmd=1000&content=%s&package_id=1", info.UTF8().c_str());

	mEditHttpParam.SetWindowText(text.c_str());
	OnBnSendHttpTest();
}
