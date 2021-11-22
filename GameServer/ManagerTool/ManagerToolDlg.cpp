
// ManagerToolDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "ManagerTool.h"
#include "ManagerToolDlg.h"
#include "afxdialogex.h"
#include "TableManager.h"
#include "WebServerNet.h"
#include "TimeManager.h"
#include "HttpClient.h"
#include "Timer.h"
#include "Base64.h"
#include "NodeMsgEvent.h"
#include "DBUnit.h"
#include "CommonDefine.h"
#include "WebServerNet.h"
#include "CommonDefine.h"
#include "FastHash.h"
#include "RunConfigStruct.h"
#include "ConfigEnum.h"
#include "LogEvent.h"
#include "ShowDBInfoDlg.h"
#include <process.h>
#include <io.h>
#include "BigDataEvent.h"
#include "FileDataStream.h"
#include "Vesion.h"
#include "WssServer.h"
#include "ManagerWssMsg.h"
#include "ResoursePack.h"
#include "TableUpdateLoader.h"
#include "SendDuanXin.h"

using namespace NetCloud;

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif

#define ADDRESS_LIST_CSV		"AddressList.csv"

CManagerToolDlg  *gpManagerToolDlg = NULL;



class NodeNet : public NetCloud::DBUserNode
{
public:
	NodeNet(const char *szCloudNodeIp, int nCloudNodePort, int nSafeCheck, int threadNum = 2)
		: DBUserNode(szCloudNodeIp, nCloudNodePort, nSafeCheck, threadNum)
	{
		mbNeedTryReconnect = false;
	}


	virtual void OnConnected(const char *szIP, int port) { LOG("Connected [%s:%d]", szIP, port); }
	virtual void OnConnectFinish(const char *szIP, int port) {LOG("Connect finish [%s:%d]", szIP, port); }
	virtual void OnDisconnect(const char *szIP, int port) { LOG("Disconnect [%s:%d]", szIP, port); }
	virtual void OnConnectFail(const char *szIP, int port) { LOG("Connect fail [%s:%d]", szIP, port); }
};

// 大消息网络对象
class AccountUnit : public DBUnit
{
public:
	virtual void BeginReceive(EVENT_ID receiveMsgID, DSIZE allWaitSize, DSIZE partCount, DSIZE partSize, const AString &msgInfo) override
	{
		LOG("Start receive big msg [%d : %d] : %s", allWaitSize, partCount, msgInfo.c_str());
	}
	virtual void ReceiveProgress(EVENT_ID receiveMsgID, DSIZE revDize, DSIZE totalSize) override
	{
		AString info;
		info.Format("RevDump:\r\n%d/100", (int)((float)revDize/totalSize*100) );
		//LOG(info.c_str());
		gpManagerToolDlg->mFlagShowSendDumpInfo.SetWindowText(info.c_str());
	}
	virtual void OnFinishReceive(EVENT_ID receiveMsgID, const AString &msgInfo)
	{
		LOG("Receive finish big msg : %s", msgInfo.c_str());
		gpManagerToolDlg->mFlagShowSendDumpInfo.SetWindowText("<DB运行目录\r\n版本机ID>");
	}
};

void _AsyncScanDumpFile(CManagerToolDlg *pDlg)
{
	pDlg->AsyncScanDumpFile();
}

// 应答接收DUMP端测试
class _R_MG_TestExist : public NetCloud::tResponseEvent
{
public:
	virtual bool _DoEvent() override
	{
		CString runPath;
		gpManagerToolDlg->mEditServerRunPath.GetWindowText(runPath);
		if (runPath.GetLength() <= 0)
		{
			GetResponseEvent()["ERROR"] = "未设置DUMP保存目录";
		}
		else
		{
			GetResponseEvent()["RESULT"] = true;
			gpManagerToolDlg->mReadyReceiveDumpID = (UInt64)get("ID");
			gpManagerToolDlg->ResetTitle();
		}
		Finish();
		return true;
	}
};

// 主要用于大文件, 传递DUMP文件   
class MG_SendDumpFile : public NetCloud::tRequestEvent
{
public:
	AString	 mLocalDumpName;

	void InitShowInfo()
	{
		gpManagerToolDlg->mFlagShowSendDumpInfo.SetWindowText("<DB运行目录\r\n版本机ID>");
	}

public:
	virtual bool _DoEvent() override
	{
		LOG("********** Send big dump data , Wait a minute ... ");
		WaitTime(1200);
		//gpManagerToolDlg->mFlagShowSendDumpInfo .EnableWindow(FALSE);
		return true;
	}

	bool Send(int nType /* = 0 */, int nTarget /* = 0 */) override
	{
		return true;
	}

	virtual void _OnResp(AutoEvent &respEvent)
	{
		if (respEvent["RESULT"])
		{
			LOG("成功发送DUMP, 并删除本地 : %s", mLocalDumpName.c_str());
			::DeleteFile(mLocalDumpName.c_str());
		}
		else
		{
			LOG("Fail upload config > %s, load dll > %s", respEvent["ERROR"].c_str(), respEvent["DLL_INFO"].c_str());
		}

		LOG("%s", respEvent->GetData().dump().c_str());

		InitShowInfo();
		
		Free();		
	}

	virtual bool _OnEvent(AutoEvent &hEvent) override
	{
		Hand< Cloud_SendDataPartEvent> evt = hEvent;
		if (!evt)
			return Logic::tServerEvent::_OnEvent(hEvent);

		if (evt->IsSendFailed())
		{
			LOG("Upload fail, send data fail > %s", get("DUMP_NAME").c_str());
			InitShowInfo();
			Free();
		}
		else
		{
			int rate = evt->GetSendProgress();
			AString info;
			info.Format("Upload %d/100", rate);
			gpManagerToolDlg->mFlagShowSendDumpInfo.SetWindowText(info.c_str());
		}
		return true;
	}

	virtual void OnForceFreeBefore()
	{
		CoroutineTool::AsyncCall(_AsyncScanDumpFile, gpManagerToolDlg);
	}
};

// 用于接收DUMP文件
class _R_MG_SendDumpFile : public NetCloud::tResponseEvent
{
public:
	virtual bool _DoEvent() override
	{
		GetResponseEvent()["RESULT"] = gpManagerToolDlg->ReceiveDumpFile(this);
		Finish();
		return true;
	}

};

//-------------------------------------------------------------------------
// 上传客户端web文件包
class UploadClientPacketDataEvent : public NetCloud::tRequestEvent
{
public:
	virtual bool _DoEvent() override
	{
		LOG("********** Upload config data, send big data , Wait a minute ... ");
		WaitTime(120);
		gpManagerToolDlg->mButtonUploadClient.EnableWindow(FALSE);
		return true;
	}

	bool Send(int nType /* = 0 */, int nTarget /* = 0 */) override
	{
		return true;
	}

	virtual void _OnResp(AutoEvent &respEvent)
	{
		if (respEvent["RESULT"])
			LOG("Succeed upload and update client\r\n%s", respEvent["INFO"].c_str())
		else
		{
			LOG("Fail upload config > %s, Info > %s", respEvent["ERROR"].c_str(), respEvent["INFO"].c_str());
		}

		LOG("%s", respEvent->GetData().dump().c_str());

		gpManagerToolDlg->mButtonUploadClient.SetWindowText("上传更新\r\n客户端");
		gpManagerToolDlg->mButtonUploadClient.EnableWindow(TRUE);
	}

	virtual bool _OnEvent(AutoEvent &hEvent) override
	{
		Hand< Cloud_SendDataPartEvent> evt = hEvent;
		if (!evt)
			return Logic::tServerEvent::_OnEvent(hEvent);

		if (evt->IsSendFailed())
		{
			LOG("Upload fail, send data fail");
			gpManagerToolDlg->mButtonUploadClient.SetWindowText("上传更新\r\n客户端");
			gpManagerToolDlg->mButtonUploadClient.EnableWindow(TRUE);
			gpManagerToolDlg->MessageBox("远程服务器上可能开启了工具, 不可使用相同的本地127地址, 否则远程工具上传失败");
		}
		else
		{
			int rate = evt->GetSendProgress();
			AString info;
			info.Format("Upload %d/100", rate);
			gpManagerToolDlg->mButtonUploadClient.SetWindowText(info.c_str());
		}
		return true;
	}

	virtual bool _OnTimeOver() override
	{
		gpManagerToolDlg->MessageBox("上传更新客户端失败: 超时");
		gpManagerToolDlg->mButtonUploadClient.SetWindowText("上传更新\r\n客户端");
		gpManagerToolDlg->mButtonUploadClient.EnableWindow(TRUE);
		return true;
	}
};
//-------------------------------------------------------------------------
// 接收客户端更新包
class _R_UploadClientPacketDataEvent : public NetCloud::AsyncResponse
{
public:
	virtual void AsyncDo() override
	{
		Auto<DataBuffer> zipData = (DataStream*)get("DATA");
		if (!zipData)
		{
			GetResponseEvent()["INFO"] = "No set DATA";
			Finish();
			return;
		}
		AutoData packetData = MEM_NEW DataBuffer();
		if (!zipData->UnZipData(packetData, 0))
		{
			GetResponseEvent()["INFO"] = "Unzip client packet DATA";
			Finish();
			return;
		}

		AString info;
		GetResponseEvent()["RESULT"] = gpManagerToolDlg->OnUpdateClientWeb(packetData, info);
		GetResponseEvent()["INFO"] = info;
		Finish();
		return;
	}
};

//-------------------------------------------------------------------------

template<bool bSSL>
class ToolHttpWeb : public uWS::tWssServerNet<bSSL>
{
public:
	virtual void OnResponse(const AString &requestData, AString &response, bool bPost, const AString &requestAddress)
	{
		if (requestData == "state")
		{
			gpManagerToolDlg->OnBnRequestRunState();
			tTimer::AWaitTime(3000);

			DataBuffer data;
			gpManagerToolDlg->mStateTable->SaveData(&data);

			Base64Tool  tool;
			response = tool.encode((const char*)data.data(), data.dataSize());
			 

			return;
		}

		NiceData d;
		d.FullJSON(requestData);

		ARecord re = gpManagerToolDlg->mStateTable->CreateRecord(gpManagerToolDlg->mStateTable->GetRecordCount(), true);
		for (auto it = d.begin(); it; ++it)
		{
			re[it.key()] = Data(&it.get());
		}

		gpManagerToolDlg->ShowTable(gpManagerToolDlg->mStateTable);
	}

	virtual bool SendToClient(Int64 playerID, Logic::tEvent *pEvent) { return false; }
	virtual void BroatcastMsg(Logic::tEvent *pMsg, Int64 excludeID) {}
	virtual void InitClearAllPlayer() {}
};


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


// CManagerToolDlg 对话框



CManagerToolDlg::CManagerToolDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MANAGERTOOL_DIALOG, pParent)
{
	gpManagerToolDlg = this;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CManagerToolDlg::~CManagerToolDlg()
{
	Clear();

	TableTool::CleanUp();
	//delete TableManager::getSingletonPtr();

	gpManagerToolDlg = NULL;
}

void CManagerToolDlg::Clear()
{
	if (mWssNet)
		mWssNet->StopNet();
	mWssNet._free();
	SAFE_DELETE(mpShowDBDlg);

	mUserNode._free();
	mAccountNode._free();
	for (int i = 0; i < mDBNetList.size(); ++i)
	{
		mDBNetList[i].mDBNode._free();
		mDBNetList[i].mDBUnit._free();
	}


	TableTool::CleanUp();
}

void CManagerToolDlg::ShowTable(AutoTable dataTable)
{
	mServerListGrid.InitClear();
	mServerListGrid.mTable = dataTable;
	// 表头
	mServerListGrid.SetRowCount(1 + dataTable->GetRecordCount());	
	mServerListGrid.SetFixedRowCount(1);
	mServerListGrid.SetFixedColumnCount(4);
	mServerListGrid.SetColumnCount(dataTable->GetField()->getCount());
	//	mServerListGrid.SetFixedColumnCount(1);
	mServerListGrid.SetRowResize(TRUE);
	mServerListGrid.SetEditable(FALSE);

	AutoField f = dataTable->GetField();

	int row = 0;
	for (int col = 0; col < mServerListGrid.GetColumnCount(); col++)
	{
		GV_ITEM Item;
		Item.mask = GVIF_TEXT | GVIF_FORMAT;
		Item.row = row;
		Item.col = col;
		if (row < mServerListGrid.GetFixedRowCount())
		{
			Item.nFormat = DT_CENTER | DT_WORDBREAK;
		}
		else if (col < mServerListGrid.GetFixedColumnCount()) {
			Item.nFormat = DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS;
		}
		Item.szText = f->getFieldInfo(col)->getName().c_str();
		mServerListGrid.SetItem(&Item);
	}

	// 表格内容
	for (TableIt tIt(dataTable); tIt; ++tIt)
	{
		++row;
		ARecord re = tIt.getCurrRecord();

		for (int col = 0; col < mServerListGrid.GetColumnCount(); col++)
		{
			GV_ITEM Item;
			Item.mask = GVIF_TEXT | GVIF_FORMAT;
			Item.row = row;
			Item.col = col;
			Item.nFormat = DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS;

			if (f->getFieldInfo(col)->getType() == FIELD_FLOAT)
				Item.szText.Format("%.2f", (float)re[col]);
			else
				Item.szText = re[col].string().c_str(); // AString::getANIS(re[col].string().c_str()).c_str();

			mServerListGrid.SetItem(&Item);
		}
	}

	//mServerListGrid.AutoSize();
	for (int i=0; i<mStateTable->GetField()->getCount(); ++i)
		mServerListGrid.AutoSizeColumn(i);
	mServerListGrid.SetColumnWidth(mStateTable->GetField()->getFieldCol("INFO"), 10000);
	mServerListGrid.SetRedraw(TRUE, TRUE);
	//mServerListGrid.SetRowResize(FALSE);
	//mServerListGrid.ExpandToFit();
}



void CManagerToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SERVER_LIST, mServerListGrid);
	DDX_Control(pDX, IDC_EDIT_IP, mEditLogicDBAddress);
	DDX_Control(pDX, IDC_EDIT_PORT, mEditWssPort);
	DDX_Control(pDX, IDC_EDIT_HTTP_PARAM, mEditHttpParam);
	DDX_Control(pDX, IDC_EDIT_SDK_IP, mEditSDKIP);
	DDX_Control(pDX, IDC_EDIT_SDK_PORT, mEditSDKPort);
	DDX_Control(pDX, IDC_EDIT_TOOL_IP, mEditToolIp);
	DDX_Control(pDX, IDC_EDIT_TOOL_PORT2, mEditInfo);
	DDX_Control(pDX, IDC_BUTTON_GET_RUN_STATE, mButtonStartScanDump);
	DDX_Control(pDX, IDC_EDIT_REMOTE_IP, mEditLocalADUnitID);
	DDX_Control(pDX, IDC_EDIT_REMOTE_PORT, mEditRemoteMgrUnitID);
	DDX_Control(pDX, IDC_EDIT_BACK_FILE_NAME, mEditBackDBFileName);
	DDX_Control(pDX, IDC_EDIT_DB_UNIT_ID, mEditDBRestoreServerID);
	DDX_Control(pDX, IDC_EDIT_SERVER_AREA, mEditServerArea);

	DDX_Control(pDX, IDC_RADIO1, mSelectHttps);
	DDX_Control(pDX, IDC_EDIT_CLOSE_PASSWORD, mEditCloseServerPassword);
	DDX_Control(pDX, IDC_EDIT_SERVER_RUN_PATH, mEditServerRunPath);
	DDX_Control(pDX, IDC_BUTTON_SELECT_SERVER_PATH, mFlagShowSendDumpInfo);
	DDX_Control(pDX, IDC_BUTTON_CONNECT_DB, mButtonStartWss);
	DDX_Control(pDX, IDC_EDIT_CLIENT_WEB_PATH, mEditClientWebPath);
	DDX_Control(pDX, IDC_BUTTON_UPLOAD_CLIENT_UPDATE2, mButtonUploadClient);
	DDX_Control(pDX, IDC_EDIT_CLIENT_UPDATE_UNIT_ID, mEditUpdateUnitID);
	DDX_Control(pDX, IDC_BUTTON_START_WEB_UPDATE, mButtonStartUpdateClientServer);
	DDX_Control(pDX, IDC_RADIO_SHOW_DB, mRadioShowDB);
}

BEGIN_MESSAGE_MAP(CManagerToolDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_REQUEST_RUN_STATE, &CManagerToolDlg::OnBnRequestRunState)
	ON_BN_CLICKED(IDC_BUTTON_GET_RUN_STATE, &CManagerToolDlg::OnBnStartScanDumpFile)
	ON_BN_CLICKED(IDC_BUTTON_CONNECT_DB, &CManagerToolDlg::OnBnStartWssServer)
	ON_BN_CLICKED(IDC_BUTTON_RESTORE_DB, &CManagerToolDlg::OnBnRestoreDB)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE_SERVER, &CManagerToolDlg::OnBnCloseServer)
	ON_BN_CLICKED(IDC_BUTTON_HTTP_TEST, &CManagerToolDlg::OnBnClickedButtonHttpTest)
	ON_BN_CLICKED(IDC_RADIO1, &CManagerToolDlg::OnBnSelectHttps)
	ON_BN_CLICKED(IDC_BUTTON_CONNECT_ACCOUNT, &CManagerToolDlg::OnBnConnectAccount)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_SERVER_PATH, &CManagerToolDlg::OnBnClickedButtonSelectServerPath)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_WEB_PATH, &CManagerToolDlg::OnBnSelectClientWebPath)
	ON_BN_CLICKED(IDC_BUTTON_START_WEB_UPDATE, &CManagerToolDlg::OnBnStartClientWebUpdate)
	ON_BN_CLICKED(IDC_BUTTON_UPLOAD_CLIENT_UPDATE, &CManagerToolDlg::OnBnUploadClientUpdate)
	ON_BN_CLICKED(IDC_BUTTON_UPLOAD_CLIENT_UPDATE2, &CManagerToolDlg::OnBnClickedButtonUploadClientUpdate2)
	ON_BN_CLICKED(IDC_BUTTON_EXPORT_FEED_BACK_TABLE, &CManagerToolDlg::OnBnExportFeedBackTable)
	ON_BN_CLICKED(IDC_BUTTON_IMPORT_FEED_BACK, &CManagerToolDlg::OnBnImportFeedBack)
	ON_BN_CLICKED(IDC_BUTTON_TEST_FEED_BACK_DATA, &CManagerToolDlg::OnBnClickedButtonTestFeedBackData)
	ON_BN_CLICKED(IDC_BUTTON_EXPORT_BLACK_WHITE_LIST, &CManagerToolDlg::OnBnExportBlackWhiteList)
	ON_BN_CLICKED(IDC_BUTTON_IMPORT_BLACK_WHITE_LIST, &CManagerToolDlg::OnBnImportBlackWhiteList)
	ON_BN_CLICKED(IDC_RADIO_SHOW_DB, &CManagerToolDlg::OnBnShowDBWindow)
END_MESSAGE_MAP()

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
		if (gpManagerToolDlg != NULL)
		{

			CString str;
			gpManagerToolDlg->mEditInfo.GetWindowText(str);
			str += "\r\n";
			TimeManager::GetMe().SetTime();
			str += TimeManager::GetMe().GetTime().c_str();
			str += " > ";
			str += strInfo.c_str();
			gpManagerToolDlg->mEditInfo.SetWindowText(str);

			gpManagerToolDlg->mEditInfo.LineScroll(gpManagerToolDlg->mEditInfo.GetLineCount());
		}
		//TABLE_LOG(strInfo.c_str());
		((ThreadLog*)mLog)->log("%s\r\n", strInfo.c_str());
	}

	virtual void logVa(bool bPrint, va_list &va, const char*info) override
	{
		logVa(va, info);
	}
};
//-------------------------------------------------------------------------
// 请求恢复DB事件
class RequestRestoreDB : public NetCloud::tRequestEvent
{
public:
	virtual bool _DoEvent() override
	{
		WaitTime(10);
		return true;
	}

	virtual void _OnResp(AutoEvent &respEvent) override
	{
		NOTE_LOG("RequestRestoreDB response:\r\n%s", respEvent->GetData().dump().c_str());
	}
};

class NotifyRestoreDBResult : public NetCloud::tMsgEvent
{
public:
	virtual bool _DoEvent() override
	{
		bool bBack = get("BACK_RESULT");
		bool b = get("RESTORE_RESULT");
		NOTE_LOG("First back result %s\r\n%s", bBack ? "Succeed" : "Fail", get("BACK_INFO").c_str());
		NOTE_LOG("Restore DB %s %s\r\n%s", get("FILE_NAME").c_str(), b ? "Succeed" : "Fail", get("INFO").c_str());
		NOTE_LOG("RequestRestoreDB response:\r\n%s", GetData().dump().c_str());
		return true;
	}
};

// CManagerToolDlg 消息处理程序

BOOL CManagerToolDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	mpShowDBDlg = new ShowDBInfoDlg();
	mpShowDBDlg->Create(IDD_DIALOG_SHOW_DB, this);
	mpShowDBDlg->ShowWindow(SW_HIDE);

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

	Allot::setLogFile("ManagerTool");
	TableTool::SetLog(new WinLog("Log/ManagerTool.log"));
	TableTool::SetErrorLog(new WinLog("Error/ManagerToolError.log"));

	AutoTable t = TableManager::getSingleton().CreateNewTable("Config");
	t->LoadCSV("./RunConfig/ManagerToolConfig.csv");
	
	NewCreateStateTable();
	//ShowTable(t);

	mEditHttpParam.SetWindowText(t->GetValue("Param", "VALUE").c_str());

	mEditServerRunPath.SetWindowText(t->GetValue("RunPath", "VALUE").c_str());
	mEditClientWebPath.SetWindowText(t->GetValue("WebPath", "VALUE").c_str());

	mEditToolIp.SetWindowText(t->GetValue("HttpWeb", "VALUE").c_str());

	AString sslKey = t->GetValue("SSLKEY", "VALUE");
	AString sslCrt = t->GetValue("SSLCRT", "VALUE");

	mEditSDKIP.SetWindowText(t->GetValue("SDKWeb", "VALUE").c_str());
	mEditSDKPort.SetWindowText(t->GetValue("SDKWeb", "PORT").c_str());

	//mEditRemoteIP.SetWindowText(t->GetValue("Remote", "VALUE").c_str());
	mEditRemoteMgrUnitID.SetWindowText(t->GetValue("Remote", "PORT").c_str());

	AString dbGateIP = t->GetValue("DBGate", "VALUE");
	mEditLogicDBAddress.SetWindowText(dbGateIP.c_str());

	AString wssPort = t->GetValue("HttpWeb", "PORT");
	mEditWssPort.SetWindowText(wssPort.c_str());

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	 
	SetTimer(1, 10, NULL);
	// TODO: 在此添加额外的初始化代码
	 
	mUserNode = MEM_NEW NodeNet(t->GetValue("Node", "VALUE").c_str(), (int)t->GetValue("Node", "PORT")+1, -55);
	 
	REGISTER_REQUEST_EVENT(mUserNode->GetEventCenter(), RequestRestoreDB);
	REGISTER_EVENT(mUserNode->GetEventCenter(), NotifyRestoreDBResult);
	  
	mDBUnit = MEM_NEW DBUnit();
	mDBUnit->SetID(0, 33);
	mUserNode->AppendUnit(mDBUnit.getPtr());

	ResetTitle();

	mbSelectHttps = mSelectHttps.GetCheck();

	AutoTable addrTable = TableManager::getSingleton().CreateNewTable(ADDRESS_LIST_CSV);
	AString addrTablePath = "RunConfig/";
	addrTable->LoadCSV((addrTablePath + ADDRESS_LIST_CSV).c_str());

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CManagerToolDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CManagerToolDlg::OnPaint()
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
HCURSOR CManagerToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CManagerToolDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nIDEvent == 1)
	{
		if (mWssNet)
		{
			mWssNet->Process();
			mWssNet->LowProcess(10);
		}
		{
			TimerCenter::GetMe()->Process();

			mUserNode->Process();
			mUserNode->LowProcess(1);

			if (mAccountNode)
			{
				mAccountNode->Process();
				mAccountNode->LowProcess(1);
			}
			for (int i = 0; i < mDBNetList.size(); ++i)
			{
				DBNetData &d = mDBNetList[i];
				if (d.mDBNode)
				{
					d.mDBNode->Process();
					d.mDBNode->LowProcess(1);
				}
			}
			CoroutineTool::CheckFinish();
		}
	}
	else if(nIDEvent == 3)
	{
		if (!mDumpFileSendEvent)
			CoroutineTool::AsyncCall(_AsyncScanDumpFile, this);
	}
	CDialogEx::OnTimer(nIDEvent);
}


//间隔60秒, 遍历检查一次DUMP), 发送到版本机上, 成功后删除(有效防止硬盘占用过大)
void CManagerToolDlg::AsyncScanDumpFile()
{
	CString runPath;
	mEditServerRunPath.GetWindowText(runPath);

	AString path = (LPCSTR)runPath;
	path += "/";

	LOG("开始扫描一次DUMP : %s", path.c_str());

	_finddata_t file_info;
	AString current_path = path + "*.dmp"; //可以定义后面的后缀为*.exe，*.txt等来查找特定后缀的文件，*.*是通配符，匹配所有类型,路径连接符最好是左斜杠/，可跨平台
	//打开文件查找句柄
	intptr_t handle = _findfirst(current_path.c_str(), &file_info);
	//返回值为-1则查找失败
	if (-1 == handle)
		return ;
	do
	{
		//判断是否子目录
		if (file_info.attrib != _A_SUBDIR) //是目录
		{
			//NOTE_LOG("%s", file_info.name);
			if (FileDataStream::GetFileExtName(file_info.name) == ".dmp")
			{
				LOG("扫描到DUMP文件: %s", (path + file_info.name).c_str());
				if (AWaitSend((path + file_info.name).c_str()))
					break;
			}
		}
		//输出文件信息并计数,文件名(带后缀)、文件最后修改时间、文件字节数(文件夹显示0)、文件是否目录
		//cout<<file_info.name<<' '<<file_info.time_write<<' '<<file_info.size<<' '<<attribute<<endl; //获得的最后修改时间是time_t格式的长整型，需要用其他方法转成正常时间显示
		//file_num++;

	} while (!_findnext(handle, &file_info));  //返回0则遍历完
	//关闭文件句柄
	_findclose(handle);
}

bool CManagerToolDlg::AWaitSend(const char *szDumpFile)
{
	if (mDumpFileSendEvent)
		return false;

	if (!mAccountDBUnit)
	{
		ERROR_LOG("需要先连接AD");
		return false;
	}

	FileDataStream dumpFile(szDumpFile, FILE_READ);
	if (!dumpFile)
	{
		ERROR_LOG("打开DUMP 文件失败 %s", szDumpFile);
		return false;
	}

	CString unitID;
	mEditRemoteMgrUnitID.GetWindowText(unitID);

	if (TOINT((LPCSTR)unitID) <= 0)
	{
		ERROR_LOG("需要先设定远程管理ID");
		return true;
	}
	
	UnitID id(Unit_ManagerTool, TOINT((LPCSTR)unitID));
	mDumpFileSendEvent = mAccountDBUnit->StartEvent("MG_SendDumpFile", id);

	LOG("准备发送DUMP %s, 可能需要读取几十秒 ...", szDumpFile);
	tTimer::AWaitTime(10);


	Hand<Cloud_SendDataPartEvent> evt = mAccountDBUnit->StartEvent("Cloud_SendDataPartEvent", id);

	Hand<MG_SendDumpFile> sendEvt = mDumpFileSendEvent;

	int onceSize = 1024 * 1024 * 16;	// 一次处理16M
	AString key = "DATA_";
	UInt64 totalSize = dumpFile.size64();
	int nowPos = 0;
	dumpFile.seek(0);
	Auto<DataBuffer> tempData = MEM_NEW DataBuffer(onceSize+1);
	for (int i = 0; true; ++i)
	{
		UInt64 readSize = nowPos + onceSize > totalSize ? totalSize - nowPos : onceSize;
		if (readSize <= 0)
			break;
		if (!dumpFile._read(tempData->data(), (DSIZE)readSize))
		{
			ERROR_LOG("读取DUMP文件失败: %s", szDumpFile);
			return false;
		}
		tempData->setDataSize((DSIZE)readSize);

		AutoData zipData;
		if (tempData->ZipData(zipData) <= 0)
		{
			ERROR_LOG("压缩DUMP文件失败: %s", szDumpFile);
			return false;
		}
		sendEvt[key+i] = (DataStream*)zipData.getPtr();
		nowPos += (int)readSize;
		tTimer::AWaitTime(10);
	}

	AString info = "Send : ";
	info += TimeManager::ToDateTime(TimeManager::Now());
	sendEvt["INFO"] = info;
	sendEvt["DUMP_NAME"] = FileDataStream::GetFileName(szDumpFile);
	sendEvt->mLocalDumpName = szDumpFile;

	sendEvt->DoEvent();

	int allSendSize = 0, partCount = 0, partSize=0;
	AString msgInfo;
	msgInfo.Format("Dump file : %s", FileDataStream::GetFileName(szDumpFile).c_str());

	evt->SendBigEvent(sendEvt, msgInfo, &allSendSize, &partCount, &partSize);
	LOG("开始发送 DUMP [%llu, Part %d = %d / %d] :  %s"
		, totalSize
		, partCount
		, allSendSize		
		, partSize
		, szDumpFile
	);


	return true;
}

bool CManagerToolDlg::ReceiveDumpFile(AutoEvent revEvent)
{
	CString runPath;
	mEditServerRunPath.GetWindowText(runPath);

	AString path = (LPCSTR)runPath;
	AString dumpName = revEvent["DUMP_NAME"];
	
	// 解析出版本号
	AString ver = "Unkwon";
	StringArray strList;
	if (AString::Split(dumpName.c_str(), strList, "_", 3) >= 2)
		ver = strList[1];

	AString dumpFileName;
	dumpFileName.Format("%s/PDB/%s/%s", path.c_str(), ver.c_str(), dumpName.c_str());

	FileDataStream::CreateMultiDirectory(FileDataStream::GetPathName(dumpFileName.c_str()).c_str());
	FileDataStream f(dumpFileName.c_str(), FILE_CREATE);

	AString key = "DATA_";
	int i = 0; 
	for (; true; ++i)
	{
		Auto<DataBuffer> zipData = (DataStream*)revEvent[key + i];
		if (!zipData)
			break;

		AutoData scrData;
		if (!zipData->UnZipData(scrData, 0))
		{
			ERROR_LOG("解压DUMP文件失败 : %s", dumpFileName.c_str());
			return false;
		}
		if (!f._write(scrData->data(), scrData->dataSize()))
		{
			ERROR_LOG("写入DUMP文件失败 : %s", dumpFileName.c_str());
			return false;
		}
	}

	f.close();
	LOG("成功接收到DUMP文件 : %s", dumpFileName.c_str());

	return true;

}

void CManagerToolDlg::OnBnRequestRunState()
{
	ClearLog();
	
	CoroutineTool::AsyncCall(CManagerToolDlg::_AsyncTotalRunState, this);

}

void AppendRunState(AutoNice infoData, int serverID = 0)
{
	ARecord re = gpManagerToolDlg->mStateTable->CreateRecord(gpManagerToolDlg->mStateTable->GetRecordCount(), true);
	re["SERVER_ID"] = serverID;
	AutoField field = gpManagerToolDlg->mStateTable->GetField();
	for (int i=1; i<field->getCount(); ++i)
	{
		re[i] = infoData[field->getFieldInfo(i)->getName()];
	}
	StateDataType stateValue = (int)infoData["STATE"];
	StateData  stateData(stateValue);

	if (stateValue <= 0)
	{
		re["STATE"] = "ok";
		infoData["STATE"] = "ok";
	}
	else
	{				
			if (stateData.isOpen(EBASE_RUN_STATE::eStartError) || stateData.isOpen(EBASE_RUN_STATE::eLogicError))
				re["STATE"] = "error";
			else if (stateData.isOpen(EBASE_RUN_STATE::eLogicMaintain))
				re["STATE"] = "maintain";
			else if (stateData.isOpen(EBASE_RUN_STATE::eMemoryTooBad))
				re["STATE"] = "bad";
			else if (stateData.isOpen(EBASE_RUN_STATE::eDiskTooBad))
				re["STATE"] = "bad";
			else if (stateData.isOpen(EBASE_RUN_STATE::eLoopTooBad))
				re["STATE"] = "bad";
			else if (stateData.isOpen(EBASE_RUN_STATE::eLowLoopTooBad))
				re["STATE"] = "bad";
			else if ((stateValue & 0xFF) > 0)
				re["STATE"] = "warn";

#if ONLY_BAD_SEND
			if (re["STATE"].string() == "bad")
			{
				// 发送短信通知
				AString strState;
				strState.Format("%s_%s", re["THREAD"].c_str(), re["STATE"].c_str());
				DuanXin::Send(strState, 0, re["SERVER_ID"]);
			}
#else
			AString strState;
			strState.Format("%s_%s", re["THREAD"].c_str(), re["STATE"].c_str());
			DuanXin::Send(strState, 0, re["SERVER_ID"]);
#endif

			// 用于手机端显示
			infoData["STATE"] = re["STATE"];
		
		AString info;
		if (stateData.isOpen(EBASE_RUN_STATE::eMemoryWarn))
			info += u8"剩余内存警告 ";
		if (stateData.isOpen(EBASE_RUN_STATE::eDiskWarn))
			info += u8"剩余磁盘警告 ";
		if (stateData.isOpen(EBASE_RUN_STATE::eLoopWarn))
			info += u8"CPU警告 ";
		if (stateData.isOpen(EBASE_RUN_STATE::eLowLoopWarn))
			info += u8"低速循环警告 ";
		if (stateData.isOpen(EBASE_RUN_STATE::eMemoryTooBad))
			info += u8"剩余内存不足 ";
		if (stateData.isOpen(EBASE_RUN_STATE::eDiskTooBad))
			info += u8"剩余磁盘不足 ";
		if (stateData.isOpen(EBASE_RUN_STATE::eLoopTooBad))
			info += u8"CPU已满 ";
		if (stateData.isOpen(EBASE_RUN_STATE::eLowLoopTooBad))
			info += u8"低速循环过少 ";
		if (stateData.isOpen(EBASE_RUN_STATE::eStartError))
			info += u8"启动错误 ";
		if (stateData.isOpen(EBASE_RUN_STATE::eLogicError))
			info += u8"逻辑错误 ";
		if (stateData.isOpen(EBASE_RUN_STATE::eLogicMaintain))
			info += u8"系统维护 ";
		info += re["INFO"].string();
		infoData["INFO"] = info;
		re["INFO"] = info.ANIS();
	}
	
	AutoNice extInfo = (tNiceData*)infoData["EXT"];

	if (extInfo)
	{
		for (auto it = extInfo->begin(); it; ++it)
		{
			ARecord extRecord = gpManagerToolDlg->mDBInfoTable->CreateRecord(gpManagerToolDlg->mDBInfoTable->GetRecordCount() + 1, TRUE);
			extRecord["SERVER_ID"] = serverID;
			extRecord["TABLE_NAME"] = it.key();
			extRecord["DB_INFO"] = (AString)it.get();
		}
	}
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

void _ASyncRequestDBRunState(CManagerToolDlg::DBNetData *db)
{
	gpManagerToolDlg->AwaitRequestDBRunState(db);
}

void CManagerToolDlg::AsyncTotalRunState(AutoEvent waitEvent)
{
	if (!mAccountDBUnit)
	{
		MessageBox("先连接帐号服务器");
		return;
	}

	AfxGetMainWnd()->SetWindowText("运营管理工具  等待获取运行信息 ...");
	mpShowDBDlg->SetWindowText("等待获取DB信息 ...");

	tTimer::AWaitTime(100);

	mStateTable->ClearAll();
	mDBInfoTable->ClearAll();

	//mpShowDBDlg->ShowWindow(SW_SHOW);

	AutoNice resp = mAccountDBUnit->AwaitRequestDB("g_global_config", "", "g_get_all_run_state", waitEvent ? &waitEvent->GetData() : NULL, 20000);
	LOG("获取帐号运行状态 : \r\n%s", resp ? "Succeed" : "Fail");

	AutoNice infoData = (tNiceData*)resp["LOGIC_DB"];
	if (infoData)
		AppendRunState(infoData);

	infoData = (tNiceData*)resp["SHARE_DB"];
	if (infoData)
		AppendRunState(infoData);

	infoData = (tNiceData*)resp["GATE"];
	if (infoData)
		AppendRunState(infoData);

	AString loginKey = "LOGIN_";
	for (int i = 0; i < 20; ++i)
	{
		infoData = (tNiceData*)resp[loginKey + i];
		if (infoData)
			AppendRunState(infoData);
	}	

	bool bWait = false;
	// 遍历所有DB
	AutoNice dbList = (tNiceData*)resp["DB_LIST"];
	resp->remove("DB_LIST");
	if (waitEvent)
	{
		Hand<Logic::tClientEvent> evt = waitEvent;
		if (evt)
		{
			evt->GetResponseEvent()["0"] = resp.getPtr();
		}
	}

	if (dbList)
	{
		CString port, toolIP, toolPort;
		mEditToolIp.GetWindowText(toolIP);

		AutoTable configTable = TableManager::getSingleton().GetTable("Config");
		int beginWorkerID = configTable->GetValue("BeginWorkerID", "VALUE");
		if (beginWorkerID <= 0)
		{
			MessageBox("需要配置 RunConfig/ManagerToolConfig.csv 里增加 BeginWorkerID, VALUE 为工具开始的UnitID, 如 24000 ");
			return;
		}

		for (int i=0; i<dbList->count(); ++i)
		{
			AutoNice dbConfig = (tNiceData*)dbList[i];
			if (!dbConfig)
				continue;
			LogicDBConfig config;
			config.Full(dbConfig);
			DBNetData &net = mDBNetList[config.SERVER_ID];
			if (!net.mbConnected)
			{
				net.mDBConfig.Full(dbConfig);
				// 异步同时进行请求所有DB运行信息 (并行, 同时请求, 多个服务区时, 请求总时间约为 10秒)
			}
			net.mWaitEvent = waitEvent;
			CoroutineTool::AsyncCall(_ASyncRequestDBRunState, &net);
			bWait = true;
		}		
	}

	if (dbList->count() <= 0)
	{
		ShowTable(mStateTable);
		mpShowDBDlg->ShowTable(mDBInfoTable);

		ResetTitle();

		AString titleInfo;
		titleInfo.Format(" 显示DB运行信息 %s", TimeManager::GetMe().GetDateTime().c_str());
		mpShowDBDlg->SetWindowText(titleInfo.c_str());
	}

	if (!bWait && waitEvent)
	{
		waitEvent->Finish();
	}

}


void CManagerToolDlg::AwaitRequestDBRunState(DBNetData *db)
{
	AutoEvent waitEvent = db->mWaitEvent;
	db->mWaitEvent.setNull();

	DBNetData &net = *db;
	if (!net.mbConnected)
	{
		if (!net.mDBNode)
		{
			AString localIp = NetAddress::GetLocalIP();

			AutoTable configTable = TableManager::getSingleton().GetTable("Config");
			int beginWorkerID = configTable->GetValue("BeginWorkerID", "VALUE");

			net.mDBNode = MEM_NEW NodeNet(localIp.c_str(), 13000 + net.mDBConfig.SERVER_ID, -55, 1);
			net.mDBUnit = MEM_NEW DBUnit();
			net.mDBUnit->SetID(1000 + _getpid(), beginWorkerID + 25000 + net.mDBConfig.SERVER_ID);
			net.mDBNode->AppendUnit(net.mDBUnit.getPtr());
			EX_REGISTER_EVENT(net.mDBNode->GetEventCenter(), "TD_ReqeustAllRunState", NetCloud::AsyncRequestEvent);
		}
		NetAddress addr(LanToExtranet(net.mDBConfig.db_config.db_node.gate.ip).c_str(), net.mDBConfig.db_config.db_node.gate.port);
		bool b = net.mDBNode->AwaitConnectGate(addr);
		if (!b)
		{
			ERROR_LOG("Connect DB fail : %s", addr.dump().c_str());
			return;
		}
		tTimer::AWaitTime(10);
		net.mbConnected = true;
	}

	//AutoNice resp = net.mDBUnit->AwaitRequestDB("t_resource", "", "g_get_all_run_state", NULL, 20000);			
	Hand<AsyncRequestEvent> requestEvt = net.mDBUnit->StartEvent("TD_ReqeustAllRunState", UnitID());
	if (waitEvent)
		requestEvt->GetData().append(waitEvent->GetData(), true);
	AutoEvent resp = requestEvt->AwaitReqeust(net.mDBUnit, { Unit_DBWorker, net.mDBConfig.db_config.worker_id }, 20);
	if (!resp || !resp["RESULT"])
	{
		LOG("获取DB运行状态 : Fail\r\n");
		net.mbConnected = false;
		return;
	}

	AutoNice infoData = (tNiceData*)resp["LOGIC_DB"];
	if (infoData)
	{
		AppendRunState(infoData, net.mDBConfig.SERVER_ID);
		AString restartInfo = infoData["RESTART"];
		if (TOINT(restartInfo.c_str()) > 0)
		{ 
			AwaitRequestDBDumpFile(&net);
		}
	}

	infoData = (tNiceData*)resp["SHARE_DB"];
	if (infoData)
		AppendRunState(infoData, net.mDBConfig.SERVER_ID);

	infoData = (tNiceData*)resp["GATE"];
	if (infoData)
		AppendRunState(infoData, net.mDBConfig.SERVER_ID);

	AString loginKey = "LOGIN_";
	for (int i = 0; i < 20; ++i)
	{
		infoData = (tNiceData*)resp[loginKey + i];
		if (infoData)
			AppendRunState(infoData, net.mDBConfig.SERVER_ID);
	}

	if (waitEvent)
	{
		Hand<Logic::tClientEvent> evt = waitEvent;
		if (evt)
		{
			AutoNice d = MEM_NEW NiceData();
			d->append(resp->GetData(), true);
			evt->GetResponseEvent()[STRING(net.mDBConfig.SERVER_ID)] = d.getPtr();
			evt->Finish();
		}
	}

	ShowTable(mStateTable);
	mpShowDBDlg->ShowTable(mDBInfoTable);

	ResetTitle();

	AString titleInfo;
	titleInfo.Format(" 显示DB运行信息 %s", TimeManager::GetMe().GetDateTime().c_str());
	mpShowDBDlg->SetWindowText(titleInfo.c_str());
}

void _AsyncStartScan(CManagerToolDlg *pDlg)
{
	pDlg->AsyncStartScan();
}

void CManagerToolDlg::OnBnStartScanDumpFile()
{
	CoroutineTool::AsyncCall(_AsyncStartScan, this);
}

void CManagerToolDlg::AsyncStartScan()
{
	ClearLog();

	// 接收DUMP文件, 版本机上的连接ID
	CString unitID;
	mEditRemoteMgrUnitID.GetWindowText(unitID);

	if (TOINT((LPCSTR)unitID) <= 0)
	{
		MessageBox("需要先设定远程管理ID");
		return;
	}

	if (!mAccountDBUnit)
	{
		MessageBox("先连接帐号DB");
		return;
	}

	if (!mbStartScanDumpFile)
	{
		UnitID id(Unit_ManagerTool, TOINT((LPCSTR)unitID));
		// 测试接收端是否存在
		Hand<NetCloud::AsyncRequestEvent> evt = mAccountDBUnit->StartEvent("MG_TestExist", id);
		evt["ID"] = (UInt64)mAccountDBUnit->GetID();
		if (!evt->AwaitReqeust(mAccountDBUnit, id, 6)["RESULT"])
		{
			AString info;
			info.Format("接收端  [%s] no exist or error : %s", id.dump().c_str(), evt["ERROR"].c_str());
			LOG(info.c_str());
			MessageBox(info.c_str());
			return;
		}
	}

	mbStartScanDumpFile = !mbStartScanDumpFile;

	mButtonStartScanDump.SetWindowText(mbStartScanDumpFile ? "扫描DUMP中..." : "开启扫描DUMP");
	if (mbStartScanDumpFile)
	{
		LOG("开启扫描DUMP");
		SetTimer(3, 6000, NULL);
	}
	else
	{
		LOG("停止扫描DUMP");
		KillTimer(3);
	}
	ResetTitle();
}


// 恢复数据到指定的备份(恢复进程: 程序指令停止服务器运行, 完成备份当前数据成功后, 再进行恢复)
// 1 拷贝备份文件至 管理目录下
// 2 设置正确的游戏区连接地址
// 3 选择备份文件
// 4 确认恢复



void CManagerToolDlg::OnBnStartWssServer()
{
	// TODO: 在此添加控件通知处理程序代码
	CString gatePort;
	mEditWssPort.GetWindowText(gatePort);

	//mUserNode->Init(NetAddress((LPCSTR)gateIP, TOINT((LPCSTR)gatePort)));
	if (!mWssNet)
	{
		mWssNet = MEM_NEW ToolWssNet();
		ManagerWssMsg::Register((ToolWssNet*)mWssNet.getPtr());
	}

	Hand<ToolWssNet> net = mWssNet;
	net->StartNet("*.*.*.*", TOINT((LPCSTR)gatePort));

	LOG("Wss server start : ws://* : %s", (LPCSTR)gatePort);

	ResetTitle();

	mButtonStartWss.EnableWindow(FALSE);
	mButtonStartWss.SetWindowText("Wss服务已开启");
}

void _AsyncRestoreBack(CManagerToolDlg *pDlg)
{
	pDlg->AsyncRestoreBack();
}

void CManagerToolDlg::OnBnRestoreDB()
{
	CoroutineTool::AsyncCall(_AsyncRestoreBack, this);
}

void CManagerToolDlg::AsyncRestoreBack()
{

	if (!mAccountDBUnit)
	{
		MessageBox("先连接帐号DB服务器");
		return;
	}

	CString serverArea;
	mEditDBRestoreServerID.GetWindowText(serverArea);
	int serverID = TOINT((LPCSTR)serverArea);
	if (serverID <= 0)
	{
		MessageBox("指定游戏区号");
		return;
	}


	NiceData param;
	param["SERVER_ID"] = serverID;

	AutoNice resp = mAccountDBUnit->AwaitRequestDB("g_global_config", "", "g_get_logic_server_config", &param, 6000);
	LOG("获取Logic DB配置 : %d : %s", serverID, resp ? "Succeed" : "Fail");
	if (!resp)
	{
		MessageBox("需要先连接帐号DB");
		return;
	}
	AutoNice configData = (tNiceData*)resp["INFO"];
	if (!configData)
	{
		LOG("未回复DB配置, 可能游戏区不存在 : %d", serverID);
		return;
	}

	LogicDBConfig config;
	config.Full(configData);

	AString addrInfo;
	addrInfo.Format("%s : %d", LanToExtranet(config.db_config.db_node.gate.ip).c_str(), config.db_config.db_node.gate.port);

	mEditLogicDBAddress.SetWindowText(addrInfo.c_str());
	//mEditLogicDBPort.SetWindowText(STRING(config.db_config.db_node.gate.port));

	NetAddress  addr(LanToExtranet(config.db_config.db_node.gate.ip).c_str(), config.db_config.db_node.gate.port);
	if (!mUserNode->AwaitConnectGate(addr))
	{
		LOG("Connect DB fail >%s", addr.dump().c_str());
		return;
	}


	CString fileName;
	mEditBackDBFileName.GetWindowText(fileName);

	if (fileName.GetLength() <= 0)
	{
		MessageBox("需要正确的DB备份文件名称(先将拷贝到目标DB目录中)");
		return;
	}

	AutoEvent evt = mDBUnit->StartEvent("RequestRestoreDB", UnitID(Unit_DBWorker, config.db_config.worker_id) );
	evt["FILE_NAME"] = (LPCSTR)fileName;
	evt->DoEvent();

	ClearLog();
	NOTE_LOG("Do restore db, wait respon info ...");
}



void CManagerToolDlg::ResetTitle()
{
	AString wssInfo;
	if (mWssNet)
	{
		CString port;
		mEditWssPort.GetWindowText(port);
		wssInfo.Format("ws:// * : %s", (LPCSTR)port);
	}
	AString revDumpInfo;
	UInt64 id = mReadyReceiveDumpID;
	if (id > 0)
	{
		revDumpInfo.Format(" 准备接收%sDUMP", mReadyReceiveDumpID.dump().c_str());
	}
	AString title;
	title.Format("运营管理工具 ver: %s  %s %s %s", SERVER_VERSION_FLAG, wssInfo.c_str(), mbStartScanDumpFile ? "正在扫描DUMP服务 ..." : "", revDumpInfo.c_str());
	AfxGetMainWnd()->SetWindowText(title.c_str());
}



void ShowInfoGrid::OnLButtonDown()
{
	CCellID currCell = m_LeftClickDownCell; // GetFocusCell();
	if (!mTable || currCell.col > 1 || currCell.row <= 0) return;

	//if (mTable->GetField()->getFieldInfo(1)->getName()=="SERVER_NAME")
	{
		CString s = GetItemText(currCell.row, 0);
		ARecord re = mTable->GetRecord((LPCSTR)s);
		if (re["THREAD"].string() == "WssLogin")
		{
			gpManagerToolDlg->mEditServerArea.SetWindowText(re["SERVER_ID"].c_str());

			AString addr;
			addr.Format("%s : %s", re["WSS_IP"].c_str(), re["GATE_PORT"].c_str());

			gpManagerToolDlg->mEditLogicDBAddress.SetWindowText(addr.c_str());

			//g_pToolDlg->mGoodsFenLeiEdit.SetWindowText(TOANIS(re["FEN_LEI"].string().c_str()));
			//gCForeControlDlg->mServerInfoText.Format("<%d>[%s]", (int)re[0], AString::getANIS(re[1].string().c_str()).c_str());
			//gCForeControlDlg->mSelectStateValue = re["STATE"];
			//gCForeControlDlg->UpdateData(FALSE);
		}
	}
}

#include "Md5Tool.h"

void _AsyncCloseServer(CManagerToolDlg *pDlg)
{
	pDlg->AsyncCloseServer();
}

void CManagerToolDlg::OnBnCloseServer()
{
	// TODO: 在此添加控件通知处理程序代码
	CoroutineTool::AsyncCall(_AsyncCloseServer, this);
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

void CManagerToolDlg::AsyncCloseServer()
{
	if (!mAccountDBUnit)
	{
		MessageBox("先连接帐号DB服务器");
		return;
	}

	CString serverArea;
	mEditServerArea.GetWindowText(serverArea);
	int serverID = TOINT((LPCSTR)serverArea);
	if ( serverID== 0)
	{
		MessageBox("指定游戏区号, 帐号服务器为-1");
		return;
	}

	CString pass;
	mEditCloseServerPassword.GetWindowText(pass);

	AString checkInfo;
	checkInfo.Format("CLOSE2021-%s-%s", (LPCSTR)serverArea, (LPCSTR)pass);
	MD5 md(checkInfo.c_str());

	if (serverID == -1)
	{
		// 关闭帐号服务器
		NiceData param2;
		param2["PASSWORD"] = md.toString();
		param2["IS_ACCOUNT"] = true;
		AutoNice resp = mAccountDBUnit->AwaitRequestDB("g_global_config", "", "g_close_server", &param2, 10000);
		LOG("Close Account DB : \r\n%s", resp ? resp->dump().c_str() : "Request g_close_server fail");
		return;
	}


	NiceData param;
	param["SERVER_ID"] = serverID;

	AutoNice resp = mAccountDBUnit->AwaitRequestDB("g_global_config", "", "g_get_logic_server_config", &param, 6000);
	LOG("获取Logic DB配置 : %d : %s", serverID, resp ? "Succeed" : "Fail");
	if (!resp)
	{
		MessageBox("需要先连接帐号DB");
		return;
	}
	AutoNice configData = (tNiceData*)resp["INFO"];
	if (!configData)
	{
		LOG("未回复DB配置, 可能游戏区不存在 : %d", serverID);
		return;
	}

	LogicDBConfig config;
	config.Full(configData);

	AString addrInfo;
	addrInfo.Format("%s : %d", LanToExtranet(config.db_config.db_node.gate.ip).c_str(), config.db_config.db_node.gate.port);
	mEditLogicDBAddress.SetWindowText(addrInfo.c_str());

	NetAddress  addr(LanToExtranet(config.db_config.db_node.gate.ip).c_str(), config.db_config.db_node.gate.port);
	if (!mUserNode->AwaitConnectGate(addr))
	{
		LOG("Connect DB fail >%s", addr.dump().c_str());
		return;
	}
	NiceData param2;
	param2["PASSWORD"] = md.toString();
	resp = mDBUnit->AwaitRequestDB("t_resource", "", "g_close_server", &param2, 10000);
	LOG("Close Logic DB %d : %s > %s :\r\n%s", config.SERVER_ID, config.SERVER_NAME.ANIS().c_str(), addr.dump().c_str(), resp ? resp->dump().c_str() : "Request g_close_server fail");
}

#include "SendDuanXin.h"

void CManagerToolDlg::OnBnClickedButtonHttpTest()
{
	ClearLog();
	CString requestString;
	mEditHttpParam.GetWindowText(requestString);

	CHttpClient  client;
	std::string response;
	client.Get(AString::getUTF8((LPCSTR)requestString).c_str(), response, 10);

	NOTE_LOG("%s\r\nResponse: %s", (LPCSTR)requestString, AString::getANIS(response.c_str()).c_str());
}


void CManagerToolDlg::OnBnSelectHttps()
{
	mbSelectHttps = !mbSelectHttps;
	mSelectHttps.SetCheck(mbSelectHttps ? 1 : 0);

}



void CManagerToolDlg::OnBnConnectAccount()
{
	ClearLog();
	CoroutineTool::AsyncCall(CManagerToolDlg::_AsyncConnectAD, this);
}

void CManagerToolDlg::AsyncConnectAD()
{
	CString ip;
	CString port, toolIP, toolPort;
	mEditSDKIP.GetWindowText(ip);
	mEditSDKPort.GetWindowText(port);

	mEditToolIp.GetWindowText(toolIP);

	AString localIp = NetAddress::GetLocalIP();


	AutoTable configTable = TableManager::getSingleton().GetTable("Config");
	int beginWorkerID = configTable->GetValue("BeginWorkerID", "VALUE");
	if (beginWorkerID <= 0)
	{
		MessageBox("需要配置 RunConfig/ManagerToolConfig.csv 里增加 BeginWorkerID, VALUE 为工具开始的UnitID, 如 24000 ");
		return;
	}

	int localNodePort = configTable->GetValue("Node", "PORT");
	if (localNodePort <= 0)
	{
		MessageBox("需要配置 RunConfig/ManagerToolConfig.csv 里增加 Node, VALUE 为本地节点端口 ");
		return;
	}
		
	if (!mAccountNode)
	{
		mAccountNode = MEM_NEW NodeNet(localIp.c_str(), localNodePort, -10);

		mAccountDBUnit = MEM_NEW AccountUnit();
		mAccountDBUnit->SetID(Unit_ManagerTool, beginWorkerID);
		mAccountNode->AppendUnit(mAccountDBUnit.getPtr());

		REGISTER_REQUEST_EVENT(mAccountNode->GetEventCenter(), MG_SendDumpFile);
		REGISTER_EVENT(mAccountNode->GetEventCenter(), _R_MG_SendDumpFile);
		REGISTER_AWAIT_REQUEST(mAccountDBUnit->GetEventCenter(), "MG_TestExist");
		REGISTER_EVENT(mAccountDBUnit->GetEventCenter(), _R_MG_TestExist);
		REGISTER_EVENT(mAccountDBUnit->GetEventCenter(), _R_UploadClientPacketDataEvent);
		REGISTER_REQUEST_EVENT(mAccountDBUnit->GetEventCenter(), UploadClientPacketDataEvent);
	}
	AString localIDInfo = "AD连接ID : ";
	localIDInfo += beginWorkerID;
	mEditLocalADUnitID.SetWindowText(localIDInfo.c_str());

	NetCloud::Address  addr((LPCSTR)ip, TOINT((LPCSTR)port));
	bool b = mAccountNode->AwaitConnectGate(addr);

	LOG("Connect account %s %s", addr.dump().c_str(), b ? "succeed" : "fail");
}



void CManagerToolDlg::OnBnClickedButtonSelectServerPath()
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

	mEditServerRunPath.SetWindowText(szFolder);
}


void CManagerToolDlg::OnBnSelectClientWebPath()
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

	mEditClientWebPath.SetWindowText(szFolder);
}

void StartUpdateClientServer(CManagerToolDlg *p)
{
	p->AsyncStartUpdateClientServer();
}

void CManagerToolDlg::OnBnStartClientWebUpdate()
{
	ClearLog();
	CoroutineTool::AsyncCall(StartUpdateClientServer, this);
}

void CManagerToolDlg::AsyncStartUpdateClientServer()
{
	CString ip;
	CString port, toolIP, toolPort;
	mEditSDKIP.GetWindowText(ip);
	mEditSDKPort.GetWindowText(port);

	mEditToolIp.GetWindowText(toolIP);

	AString localIp = NetAddress::GetLocalIP();


	AutoTable configTable = TableManager::getSingleton().GetTable("Config");
	int beginWorkerID = configTable->GetValue("BeginWorkerID", "VALUE");
	if (beginWorkerID <= 0)
	{
		MessageBox("需要配置 RunConfig/ManagerToolConfig.csv 里增加 BeginWorkerID, VALUE 为工具开始的UnitID, 如 24000 ");
		return;
	}

	CString webPath;
	mEditClientWebPath.GetWindowText(webPath);
	if (webPath.GetLength() <= 0)
	{
		MessageBox("需要选择客户端WEB运行目录");
		return;
	}

	int localNodePort = configTable->GetValue("Node", "VALUE");
	if (localNodePort <= 0)
	{
		MessageBox("需要配置 RunConfig/ManagerToolConfig.csv 里增加 Node, VALUE 为本地节点端口 ");
		return;
	}

	// 先请求AD运行状态, 及所有LogicDB
	if (!mAccountNode)
	{
		mAccountNode = MEM_NEW NodeNet(localIp.c_str(), localNodePort, -10);

		mAccountDBUnit = MEM_NEW AccountUnit();
		mAccountDBUnit->SetID(Unit_ManagerTool, beginWorkerID);
		mAccountNode->AppendUnit(mAccountDBUnit.getPtr());

		REGISTER_REQUEST_EVENT(mAccountNode->GetEventCenter(), MG_SendDumpFile);
		REGISTER_EVENT(mAccountNode->GetEventCenter(), _R_MG_SendDumpFile);
		REGISTER_AWAIT_REQUEST(mAccountDBUnit->GetEventCenter(), "MG_TestExist");
		REGISTER_EVENT(mAccountDBUnit->GetEventCenter(), _R_MG_TestExist);

		REGISTER_REQUEST_EVENT(mAccountDBUnit->GetEventCenter(), UploadClientPacketDataEvent);
		REGISTER_EVENT(mAccountDBUnit->GetEventCenter(), _R_UploadClientPacketDataEvent);
	}

	if (!mUpdateClientUnit)
	{
		mUpdateClientUnit = MEM_NEW AccountUnit();
		mUpdateClientUnit->SetID(Unit_ClientUpdateServer, beginWorkerID);
		mAccountNode->AppendUnit(mUpdateClientUnit.getPtr());
	}

	AString localIDInfo = "更新ID: ";
	localIDInfo += mUpdateClientUnit->GetID().dump();
	mEditLocalADUnitID.SetWindowText(localIDInfo.c_str());

	NetCloud::Address  addr((LPCSTR)ip, TOINT((LPCSTR)port));
	bool b = mAccountNode->AwaitConnectGate(addr);

	if (b)
	{
		mButtonStartUpdateClientServer.EnableWindow(FALSE);
		mButtonStartUpdateClientServer.SetWindowText("正在接受\r\n更新客户端");
		AString title;
		title.Format("运营管理工具 ver: %s  已成功启动客户端更新服务 更新ID: %s", SERVER_VERSION_FLAG, mUpdateClientUnit->GetID().dump().c_str());
		AfxGetMainWnd()->SetWindowText(title.c_str());
	}

	AString info;
	info.Format("Connect account %s %s", addr.dump().c_str(), b ? "succeed" : "fail");
	LOG(info.c_str());
	MessageBox(info.c_str());
}

void StartMakeClientPacket(CManagerToolDlg *p)
{
	p->AsyncMakeClientPacket();
}

void CManagerToolDlg::OnBnUploadClientUpdate()
{
	ClearLog();
	CoroutineTool::AsyncCall(StartMakeClientPacket, this);
}

bool TraverseDeleteBackFiles(const std::string &path, Array<AString> &fileList)
{
	Array<AString> pathList;
	_finddata_t file_info;
	string current_path = path + "*.*"; //可以定义后面的后缀为*.exe，*.txt等来查找特定后缀的文件，*.*是通配符，匹配所有类型,路径连接符最好是左斜杠/，可跨平台
	//打开文件查找句柄
	intptr_t handle = _findfirst(current_path.c_str(), &file_info);
	//返回值为-1则查找失败
	if (-1 == handle)
		return false;
	do
	{
		//判断是否子目录
		string attribute;
		if (file_info.attrib == _A_SUBDIR) //是目录
		{
			AString name = file_info.name;
			if (name != "." && name != "..")
			{
				std::string  dirName = path + file_info.name;
				dirName += "/";
				pathList.push_back(dirName.c_str());
				//TraverseDeleteBackFiles(dirName, fileList);
			}
		}
		else
		{
			attribute = "file";
			//NOTE_LOG("%s", file_info.name);
			//if (FileDataStream::GetFileExtName(file_info.name) == ".bak")
			{
				fileList.push_back((path + file_info.name).c_str());
			}
		}
		//输出文件信息并计数,文件名(带后缀)、文件最后修改时间、文件字节数(文件夹显示0)、文件是否目录
		//cout<<file_info.name<<' '<<file_info.time_write<<' '<<file_info.size<<' '<<attribute<<endl; //获得的最后修改时间是time_t格式的长整型，需要用其他方法转成正常时间显示
		//file_num++;

	} while (!_findnext(handle, &file_info));  //返回0则遍历完
	//关闭文件句柄
	_findclose(handle);

	for (int i=0; i<pathList.size(); ++i)
	{
		TraverseDeleteBackFiles(pathList[i].c_str(), fileList);
	}

	return true;
}

AutoData ProcessIndexHtml(AutoData indexText, int webVer)
{
	//CHttpClient		client;

	//std::string response;
	//if (!client.Get("http://cdnsdk.hyuteach.com/jssdk/js/loadsdk.js", response, 10))
	//{
	//	gpManagerToolDlg->MessageBox("请求 https://cdnsdk.hyuteach.com/jssdk/js/loadsdk.js 失败 ");
	//	return false;
	//}

	//AString myCode = "window.checkVersion = function(nowVer)\r\n"
	//	"{\r\n"
	//	"	if (window.HuanYuGameSdk == null || window.HuanYuGameSdk.clearCache == null)\r\n"
	//	"	{\r\n"
	//	"		console.log(\"xxx no add jave HuanYuGameSdk object, or no clearCache function\");\r\n"
	//	"		return;\r\n"
	//	"	}\r\n"
	//	"	var v = localStorage.getItem(\"ver\");\r\n"
	//	"	if (v == null || v != nowVer)\r\n"
	//	"	{\r\n"
	//	"		localStorage.setItem(\"ver\", nowVer);\r\n"
	//	"		window.HuanYuGameSdk.clearCache(function() { console.log(\"oooooooooopppppppppppp clear finish\"); });\r\n"
	//	"		console.log(\"Old ver : \", v, \"Clear cache, now ver\", nowVer);\r\n"
	//	"	}\r\n"
	//	"	else\r\n"
	//	"	{\r\n"
	//	"		console.log(\"ver : \", parseInt(v));\r\n"
	//	"	}\r\n";
	//
	//AutoData sdkData = MEM_NEW DataBuffer();
	//sdkData->_write((void*)response.c_str(), response.length());
	//sdkData->_write((void*)"\r\n\r\n", 4);

	//sdkData->_write((void*)myCode.c_str(), myCode.length());

	FileDataStream sdkFile("./RunConfig/sdk.js", FILE_READ);
	AutoData sdkData = sdkFile.readAllData();

	if (!sdkData)
	{
		gpManagerToolDlg->MessageBox("不存在SDK文件 > ./RunConfig/sdk.js");
		return AutoData();
	}

	
	FileDataStream sdkTestFile("./ClientWeb/sdk.js", FILE_CREATE_UTF8);
	sdkTestFile._write(sdkData->data(), sdkData->dataSize());

	// 拆离原index.html文件, 用于插入代码
	AString indexString;
	indexString.set(indexText->data(), indexText->dataSize());

	const char *szBeginLoadScriptPos = strstr(indexString.c_str(), "loadScript(debug ?");

	AString lastString = szBeginLoadScriptPos;
	AString headString;
	headString.set(indexString.c_str(), szBeginLoadScriptPos - indexString.c_str());

	//https://278-test1.huanyutd.com:4000?cmd=2000
	CString getVersionAddress;
	gpManagerToolDlg->mEditHttpParam.GetWindowText(getVersionAddress);

	if (getVersionAddress.GetLength() <= 0)
	{
		gpManagerToolDlg->MessageBox("需要提供获取 WebClient Http 地址 如: https://278-test1.huanyutd.com:4000?cmd=2000");
		return AutoData();
	}

	AString insertCode;
	insertCode.Format("loadScript(\"sdk.js\", function(){ window.checkVersion(\"%s\");  window.loadZip(\"mainjs-%d.zip\",  function(){ window.boot(); } ); } ); \r\nreturn;\r\n\r\n", (LPCSTR)getVersionAddress, webVer);

	AString newIndexString;
	newIndexString.Format("%s%s            %s", headString.c_str(), insertCode.c_str(), lastString.c_str());

	indexText->clear(false);
	indexText->_write((void*)newIndexString.c_str(), newIndexString.length());

	FileDataStream indeTestFile("./ClientWeb/index.html", FILE_CREATE_UTF8);
	indeTestFile._write(indexText->data(), indexText->dataSize());

	return sdkData;
}

void CManagerToolDlg::AsyncMakeClientPacket()
{
	CString webPath;
	mEditClientWebPath.GetWindowText(webPath);
	if (webPath.GetLength() <= 0)
	{
		MessageBox("需要选择客户端WEB运行目录");
		return;
	}

	FileDataStream::CreateMultiDirectory("./ClientWeb");
	int webVer = 1000;
	FileDataStream	webVerFile("./ClientWeb/version.txt", FILE_READ);
	AString verString;
	if ( webVerFile.readLine(verString) )
	{
		webVer = TOINT(verString.c_str());
		if (webVer <= 0)
			webVer = 1000;
	}
	webVerFile.close();

	++webVer;
	FileDataStream	webVerFileW("./ClientWeb/version.txt", FILE_CREATE);
	verString = STRING(webVer);
	webVerFileW._write((void*)verString.c_str(), verString.length());
	webVerFileW._write((void*)"\r\n", 2);

	AString path = (LPCSTR)webPath;
	path += "/";

	// 递归打包所有的客户端文件
	Array<AString> fileList;
	TraverseDeleteBackFiles(path.c_str(), fileList);

	EasyGenerateResourcesPack  packetTool;

	Auto<DataBuffer> allJsText = MEM_NEW DataBuffer();
	for (int i=0; i<fileList.size(); ++i)
	{
		AString fileName = fileList[i];
		FileDataStream f(fileName.c_str(), FILE_READ);
		if (f)
		{
			AutoData data = f.readAllData();
			if (data)
			{
				// 特殊处理index.html
				if (FileDataStream::GetFileName(fileName.c_str()) == "index.html")
				{
					AutoData sdkData = ProcessIndexHtml(data, webVer);
					if (!sdkData)
					{
						MessageBox("处理 sdk.js 失败");
						return;
					}
					
					packetTool.AppendResourceData("sdk.js", sdkData);

				}

				fileName.replace(path.c_str(), "");
				// js 文件全部压缩到一个文件中
				if ( FileDataStream::GetFileExtName(fileName.c_str())==".js" && (strstr(fileName.c_str(), "cocos2d-js")==fileName.c_str() || FileDataStream::GetPathName(fileName.c_str()).length()>0 ) ) // strstr(fileName.c_str(), "assets/main/index.") == fileName.c_str()) )
				{
					if (strstr(fileName.c_str(), "src/settings.") != fileName.c_str())
					{
						AString x = "\r\n//---------------------------------------------\r\n";
						allJsText->_write((void*)x.c_str(), x.length());
						allJsText->_write(data->data(), data->dataSize());
						data->clear();
						data->_write("\r\n", 2);
						LOG("Zip js > %s", fileName.c_str());
					}
				}

				packetTool.AppendResourceData(fileName.c_str(), data);
				//LOG("Append client file : %s", fileName.c_str());
				continue;
			}
		}
		
		LOG("xxx Read fail >%s", fileName.c_str());
	}

	AutoData zipJs;
	allJsText->ZipData(zipJs, 0, 0, 0, MAX_ZIP_RATE);

	AString mainName;
	mainName.Format("mainjs-%d.zip", webVer);
	packetTool.AppendResourceData(mainName.c_str(), zipJs);

	packetTool.AppendResourceData("mainjs.js", allJsText);

	Auto<DataBuffer> packetData = MEM_NEW DataBuffer();
	AString errorInfo;
	int nCount = 0;
	if (packetTool.GeneratePack(NULL, errorInfo, packetData.getPtr(), &nCount))
	{
		if (packetData->ZipData(mClientZipData) > 0)
		{
			AString info;
			info.Format("Succeed generate client file packet %d", nCount);
				LOG(info.c_str());
			MessageBox(info.c_str());
			return;
		}
		else
			MessageBox("Zip packet data fail");
	}
	else
	{
		AString info;
		info.Format("Generate client file packet fail > %s", errorInfo.c_str());
		LOG(info.c_str());
		MessageBox(info.c_str());
		return;
	}

}

void StartUpdateClient(CManagerToolDlg *p)
{
	p->AsyncUpdateClient();
}

void CManagerToolDlg::OnBnClickedButtonUploadClientUpdate2()
{
	ClearLog();
	CoroutineTool::AsyncCall(StartUpdateClient, this);
}

void CManagerToolDlg::AsyncUpdateClient()
{
	if (!mClientZipData)
	{
		MessageBox("先完成客户端打包");
		return;
	}

	if (!mAccountDBUnit)
	{
		MessageBox("先连接到帐号服务器");
		return;
	}

	CString updateID;
	mEditUpdateUnitID.GetWindowText(updateID);
	int id = TOINT((LPCSTR)updateID);
	if (id <= 0)
	{
		MessageBox("需要指定客户端更新服务的ID");
		return;
	}

	UnitID	 webServerID(Unit_ClientUpdateServer, id);

	// 大数据上传
	Hand<Cloud_SendDataPartEvent> evt = mAccountDBUnit->StartEvent("Cloud_SendDataPartEvent", webServerID);

	AutoEvent sendEvt = mAccountDBUnit->StartEvent("UploadClientPacketDataEvent", webServerID);

	sendEvt["DATA"] = mClientZipData.getPtr();
	AString info = "Update:";
	info += TimeManager::ToDateTime(TimeManager::Now());
	sendEvt["INFO"] = info;
	sendEvt->DoEvent();
	evt->SendBigEvent(sendEvt);
	 

}

void DeletePath(const char *szPath)
{
	SHFILEOPSTRUCT DirRemove;

	DirRemove.fFlags = FOF_NOCONFIRMATION;

	DirRemove.hNameMappings = NULL;

	DirRemove.hwnd = NULL;

	DirRemove.lpszProgressTitle = NULL;

	DirRemove.pFrom = szPath;

	DirRemove.pTo = NULL;

	DirRemove.wFunc = FO_DELETE;

	SHFileOperation(&DirRemove);
}

// 删除config 目录之外的所有文件及目录
bool DeleteWebFiles(const std::string &path)
{
	_finddata_t file_info;
	string current_path = path + "*.*"; //可以定义后面的后缀为*.exe，*.txt等来查找特定后缀的文件，*.*是通配符，匹配所有类型,路径连接符最好是左斜杠/，可跨平台
	//打开文件查找句柄
	intptr_t handle = _findfirst(current_path.c_str(), &file_info);
	//返回值为-1则查找失败
	if (-1 == handle)
		return false;
	do
	{
		//判断是否子目录
		string attribute;
		if (file_info.attrib == _A_SUBDIR) //是目录
		{
			AString name = file_info.name;
			if (name != "." && name != ".." && name!="config")
			{
				std::string  dirName = path + file_info.name;
				DeletePath(dirName.c_str());				
			}
		}
		else
		{
			attribute = "file";
			//NOTE_LOG("%s", file_info.name);
			//if (FileDataStream::GetFileExtName(file_info.name) == ".bak")
			{
				::DeleteFile((path + file_info.name).c_str());
			}
		}
		//输出文件信息并计数,文件名(带后缀)、文件最后修改时间、文件字节数(文件夹显示0)、文件是否目录
		//cout<<file_info.name<<' '<<file_info.time_write<<' '<<file_info.size<<' '<<attribute<<endl; //获得的最后修改时间是time_t格式的长整型，需要用其他方法转成正常时间显示
		//file_num++;

	} while (!_findnext(handle, &file_info));  //返回0则遍历完
	//关闭文件句柄
	_findclose(handle);
	return true;
}

bool CManagerToolDlg::OnUpdateClientWeb(AutoData webPacketData, AString &info)
{
	EasyResourcesPack  webPacket;
	webPacketData->seek(0);
	if (!webPacket.load(webPacketData.getPtr()))
	{
		info.Format("File packet load fail");
		return false;
	}

	CString webPath;
	mEditClientWebPath.GetWindowText(webPath);

	AString path;
	path.Format("%s/", (LPCSTR)webPath);

	FileDataStream::CreateMultiDirectory(path.c_str());

	// 通知客户端开始更新
	{
		NiceData param;
		param["IS_FINISH"] = false;
		AutoNice resp = mAccountDBUnit->AwaitRequestDB("g_global_config", "", "g_notify_update_client", &param, 6000);
		LOG("通知客户端开始 : %s", resp ? "Succeed" : "Fail");
		if (!resp)
		{
			MessageBox("需要先连接帐号DB");			
		}
		
		if (!resp["RESULT"])
		{
			LOG("未通知成功");
			//return;
		}
	}

	// 清空当前的web, 删除后, 会一一部分缓存需要的文件, 然后停止在获取文件失败
	//??? DeleteWebFiles(path.c_str());

	bool b = webPacket.Export(path.c_str(), info);
	LOG("Update web file >%s, info : %s", b ? "succeed" : "fail", info.c_str());

	// 通知完成更新客户端, 客户端重启
	{
		NiceData param;
		param["IS_FINISH"] = true;
		AutoNice resp = mAccountDBUnit->AwaitRequestDB("g_global_config", "", "g_notify_update_client", &param, 6000);
		LOG("通知客户端开始 : %s", resp ? "Succeed" : "Fail");
		if (!resp)
		{
			MessageBox("需要先连接帐号DB");
		}

		if (!resp["RESULT"])
		{
			LOG("未通知成功");
		}
	}

	return b;
}

void RequestDBExportFeedBack(CManagerToolDlg *pDlg)
{
	pDlg->AsyncRequestExportFeedBack();
}

void CManagerToolDlg::OnBnExportFeedBackTable()
{
	CoroutineTool::AsyncCall(RequestDBExportFeedBack, this);
}

void CManagerToolDlg::AsyncRequestExportFeedBack()
{
	ClearLog();

	CString serverArea;
	mEditServerArea.GetWindowText(serverArea);
	int serverID = TOINT((LPCSTR)serverArea);
	if (serverID <= 0)
	{
		MessageBox("指定游戏区号");
		return;
	}
	NiceData p;
	AutoNice resp = AsyncRequestDB(serverID, "m_export_feed_back_table", p, 300000);
	if (!resp)
	{
		LOG("请求DB m_export_feed_back_table fail");
		return;
	}

	FileDataStream::CreateMultiDirectory("./FeedBack/");

	{
		Auto<DataBuffer> zipData = (DataStream*)resp["RMB_DATA"];
		AutoData data;
		if (zipData)
		{
			if (zipData->UnZipData(data, 0))
			{
				data->seek(0);
				AutoTable t = tBaseTable::NewBaseTable();
				if (t->LoadFromData(data.getPtr()))
				{
					t->SaveCSV("./FeedBack/rmb_back.csv");
					FileDataStream  f("./FeedBack/rmb_back.table", FILE_CREATE);
					if (f)
					{
						f._write(data->data(), data->dataSize());
						LOG("Succeed total rmb table > ./FeedBack/rmb_back.csv (.table), count : %d", t->GetRecordCount());
					}
					else
						LOG("Save rmb table fail");
				}
			}
			else
				LOG("RMB table data unzip fail");
		}
		else
			LOG("RMB_DATA no response");
	}

	{
		Auto<DataBuffer> zipData = (DataStream*)resp["LEVEL_DATA"];
		AutoData data;
		if (zipData)
		{
			if (zipData->UnZipData(data, 0))
			{
				data->seek(0);
				AutoTable t = tBaseTable::NewBaseTable();
				if (t->LoadFromData(data.getPtr()))
				{
					t->SaveCSV("./FeedBack/level_back.csv");
					FileDataStream  f("./FeedBack/level_back.table", FILE_CREATE);
					if (f)
					{
						f._write(data->data(), data->dataSize());
						LOG("Succeed total rmb table > ./FeedBack/level_back.csv (.table), count : %d", t->GetRecordCount());
					}
					else
						LOG("Save rmb table fail");
				}
			}
			else
				LOG("RMB table data unzip fail");
		}
		else
			LOG("RMB_DATA no response");
	}

	AString info = resp["RESULT"];
	if (info.length() <= 0)
		info = "汇总充值反馈数据成功";
	
	LOG(info.c_str());
}

AutoNice CManagerToolDlg::AsyncRequestDB(int serverID, const AString &opName, NiceData &requestParam, int waitMilTime)
{
	if (!mAccountDBUnit)
	{
		MessageBox("先连接帐号DB服务器");
		return AutoNice();
	}

	NiceData param;
	param["SERVER_ID"] = serverID;

	AutoNice resp = mAccountDBUnit->AwaitRequestDB("g_global_config", "", "g_get_logic_server_config", &param, 6000);
	LOG("获取Logic DB配置 : %d : %s", serverID, resp ? "Succeed" : "Fail");
	if (!resp)
	{
		MessageBox("需要先连接帐号DB");
		return AutoNice();
	}
	AutoNice configData = (tNiceData*)resp["INFO"];
	if (!configData)
	{
		LOG("未回复DB配置, 可能游戏区不存在 : %d", serverID);
		return AutoNice();
	}

	LogicDBConfig config;
	config.Full(configData);

	AString addrInfo;
	addrInfo.Format("%s : %d", LanToExtranet(config.db_config.db_node.gate.ip).c_str(), config.db_config.db_node.gate.port);
	mEditLogicDBAddress.SetWindowText(addrInfo.c_str());

	NetAddress  addr(LanToExtranet(config.db_config.db_node.gate.ip).c_str(), config.db_config.db_node.gate.port);
	if (!mUserNode->AwaitConnectGate(addr))
	{
		LOG("Connect DB fail >%s", addr.dump().c_str());
		return AutoNice();
	}

	int nResult = 0;
	resp = mDBUnit->AwaitRequestDB("t_resource", "", opName.c_str(), &requestParam, waitMilTime, &nResult);
	//LOG("Close Logic DB %d : %s > %s :\r\n%s", config.SERVER_ID, config.SERVER_NAME.ANIS().c_str(), addr.dump().c_str(), resp ? resp->dump().c_str() : "Request g_close_server fail");
	if (nResult != eNoneError)
	{
		LOG("Request DB %s fail : %d", opName.c_str(), nResult);
		return AutoNice();
	}
	return resp;
}

void ImportFeedBack(CManagerToolDlg *pDlg)
{
	pDlg->AsyncImportFeedBack();
}

void CManagerToolDlg::OnBnImportFeedBack()
{
	CoroutineTool::AsyncCall(ImportFeedBack, this);
}


AutoData ReadLocalFileZipData(const char *szFileName)
{
		FileDataStream f(szFileName, FILE_READ);
		Auto<DataBuffer> data = f.readAllData();
		if (data)
		{
			AutoData zip = MEM_NEW DataBuffer();
			if (data->ZipData(zip))
				return zip;
			else
				LOG("Zip data fail %s", szFileName);
		}
		else
			LOG("Read %s data fail", szFileName);

		return AutoData();
}

AutoData ReadCSVFileZipData(const char *szFileName)
{
	AutoTable t = tBaseTable::NewBaseTable();
	bool b = t->LoadCSV(szFileName);
	Auto<SaveDBRecordBuffer> data = MEM_NEW SaveDBRecordBuffer();
	if (t)
		t->SaveData(data.getPtr());
	if (b)
	{
		AutoData zip = MEM_NEW DataBuffer();
		if (data->ZipData(zip))
			return zip;
		else
			LOG("Zip data fail %s", szFileName);
	}
	else
		LOG("Read %s data fail", szFileName);

	return AutoData();
}

void CManagerToolDlg::AsyncImportFeedBack()
{
	if (!mAccountDBUnit)
	{
		MessageBox("选连接帐号服务器");
		return;
	}
	ClearLog();

	AutoData rmbData = ReadLocalFileZipData("./FeedBack/rmb_back.table");
	AutoData levelData = ReadLocalFileZipData("./FeedBack/level_back.table");

	if (!rmbData || !levelData)
	{
		LOG("No exist ./FeedBack/rmb_back.table or ./FeedBack/level_back.table");
		MessageBox("需要先正确导出反馈汇总数据");
	}

	NiceData param;
	param["RMB_DATA"] = rmbData.getPtr();
	param["LEVEL_DATA"] = levelData.getPtr();

	int nResult = 0;
	AutoNice resp = mAccountDBUnit->AwaitRequestDB("g_global_config", "", "m_import_feed_back_data", &param, 300000, &nResult);
	//LOG("获取Logic DB配置 : %d : %s", serverID, resp ? "Succeed" : "Fail");
	if (!resp || nResult!=eNoneError)
	{
		LOG("m_import_feed_back_data run fail > %d", nResult);
		return;
	}

	LOG("导入充值回馈数据: %s , Rmb feed back: %d, Level feed back: %d\r\n info: %s", resp["RESULT"] ? "Succeed" : "fail", (int)resp["RMB_COUNT"], (int)resp["LEVEL_COUNT"], resp["ERROR"].c_str());
}


void CManagerToolDlg::OnBnClickedButtonTestFeedBackData()
{
	// 将CSV表格 保存到table文件
	{
		AutoTable t = tBaseTable::NewBaseTable();
		if (!t->LoadCSV("./FeedBack/rmb_back.csv"))
		{
			MessageBox("./FeedBack/rmb_back.csv load fail, file no exist");
			return;
		}
		DataBuffer d;
		if (!t->SaveData(&d))
		{
			MessageBox("rmb table save data fail");
			return;
		}

		FileDataStream f("./FeedBack/rmb_back.table", FILE_CREATE);
		f._write(d.data(), d.dataSize());
		f.close();
	}

	{
		AutoTable t = tBaseTable::NewBaseTable();
		if (!t->LoadCSV("./FeedBack/level_back.csv"))
		{
			MessageBox("./FeedBack/level_back.csv load fail, file no exist");
			return;
		}
		DataBuffer d;
		if (!t->SaveData(&d))
		{
			MessageBox("rmb table save data fail");
			return;
		}

		FileDataStream f("./FeedBack/level_back.table", FILE_CREATE);
		f._write(d.data(), d.dataSize());
		f.close();
	}
	MessageBox("成功将 ./FeedBack/rmb_back.csv and ./FeedBack/level_back.csv, 转换为充值回馈数据");
}

void ExportWhiteList(CManagerToolDlg *p)
{
	p->AsyncExportBlackWhiteList();
}

void CManagerToolDlg::OnBnExportBlackWhiteList()
{
	CoroutineTool::AsyncCall(ExportWhiteList, this);
}


void ImportWhiteList(CManagerToolDlg *p)
{
	p->AsyncImportBlackWhiteList();
}


void CManagerToolDlg::OnBnImportBlackWhiteList()
{
	CoroutineTool::AsyncCall(ImportWhiteList, this);
}


void CManagerToolDlg::AsyncExportBlackWhiteList()
{
	if (!mAccountDBUnit)
	{
		MessageBox("选连接帐号服务器");
		return;
	}
	ClearLog();

	int nResult = 0;
	AutoNice resp = mAccountDBUnit->AwaitRequestDB("g_global_config", "", "m_export_black_white_list", NULL, 300000, &nResult);
	//LOG("获取Logic DB配置 : %d : %s", serverID, resp ? "Succeed" : "Fail");
	if (!resp || nResult != eNoneError)
	{
		LOG("m_import_feed_back_data run fail > %d", nResult);
		return;
	}

	FileDataStream::CreateMultiDirectory("./LoginAccount/");

	{
		Auto<DataBuffer> zipData = (DataStream*)resp["BLACK_DATA"];
		AutoData data;
		if (zipData)
		{
			if (zipData->UnZipData(data, 0))
			{
				data->seek(0);
				AutoTable t = tBaseTable::NewBaseTable();
				if (t->LoadFromData(data.getPtr()))
				{
					bool b = t->SaveCSV("./LoginAccount/black.csv");
					//FileDataStream  f("./LoginAccout/rmb_back.table", FILE_CREATE);
					if (b)
					{
						//f._write(data->data(), data->dataSize());
						LOG("Succeed total rmb table > ./LoginAccount/black.csv , count : %d", t->GetRecordCount());
					}
					else
						LOG("Save rmb table fail");
				}
			}
			else
				LOG("black table data unzip fail");
		}
		else
			LOG("black DATA no response");
	}

	{
		Auto<DataBuffer> zipData = (DataStream*)resp["WHITE_DATA"];
		AutoData data;
		if (zipData)
		{
			if (zipData->UnZipData(data, 0))
			{
				data->seek(0);
				AutoTable t = tBaseTable::NewBaseTable();
				if (t->LoadFromData(data.getPtr()))
				{
					bool b = t->SaveCSV("./LoginAccount/white.csv");
					//FileDataStream  f("./FeedBack/level_back.table", FILE_CREATE);
					if (b)
					{
						//f._write(data->data(), data->dataSize());
						LOG("Succeed total rmb table > ./LoginAccount/white.csv, count : %d", t->GetRecordCount());
					}
					else
						LOG("Save rmb table fail");
				}
			}
			else
				LOG("white table data unzip fail");
		}
		else
			LOG("white_DATA no response");
	}

	AString info = resp["RESULT"];
	if (info.length() <= 0)
		info = "获取黑白名单成功";

	LOG(info.c_str());
}

void CManagerToolDlg::AsyncImportBlackWhiteList()
{
	if (!mAccountDBUnit)
	{
		MessageBox("选连接帐号服务器");
		return;
	}
	ClearLog();

	AutoData rmbData = ReadCSVFileZipData("./LoginAccount/black.csv");
	AutoData levelData = ReadCSVFileZipData("./LoginAccount/white.csv");

	if (!rmbData || !levelData)
	{
		LOG("No exist ./LoginAccount/black.csv or ./LoginAccount/white.csv");
		MessageBox("需要先正确导出黑白名单");
	}

	NiceData param;
	param["BLACK_DATA"] = rmbData.getPtr();
	param["WHITE_DATA"] = levelData.getPtr();

	int nResult = 0;
	AutoNice resp = mAccountDBUnit->AwaitRequestDB("g_global_config", "", "m_import_black_white_list", &param, 300000, &nResult);
	//LOG("获取Logic DB配置 : %d : %s", serverID, resp ? "Succeed" : "Fail");
	if (!resp || nResult != eNoneError)
	{
		LOG("m_import_black_white_list run fail > %d", nResult);
		return;
	}

	LOG("导入黑白名单: %s , black: %d, white: %d\r\n info: %s", resp["RESULT"] ? "Succeed" : "fail", (int)resp["BLACK_COUNT"], (int)resp["WHITE_COUNT"], resp["ERROR"].c_str());
}

void CManagerToolDlg::OnBnShowDBWindow()
{
	static bool bNowShow = false;
	bNowShow = !bNowShow;
	mpShowDBDlg->ShowWindow(bNowShow ? SW_SHOW : SW_HIDE);

	mRadioShowDB.SetCheck(bNowShow ? 1 : 0);
}
