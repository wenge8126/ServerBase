
// ManagerToolDlg.h: 头文件
//

#pragma once

#include "GridCtrl.h"
#include "ComboGridCtrl.h"
#include "BaseTable.h"
#include "NetNode.h"
#include "DBUnit.h"
#include "ShowDBInfoDlg.h"
#include "RunConfigStruct.h"

using namespace NetCloud;


// CManagerToolDlg 对话框
class CManagerToolDlg : public CDialogEx
{
	ShowDBInfoDlg *mpShowDBDlg = NULL;

// 构造
public:
	CManagerToolDlg(CWnd* pParent = nullptr);	// 标准构造函数
	~CManagerToolDlg();

	virtual void OnCancel()
	{
		Clear();
		CDialogEx::OnCancel();
	}

	void NewCreateStateTable()
	{
		mStateTable = tBaseTable::NewBaseTable();
		mStateTable->AppendField("ID", FIELD_INT);
		mStateTable->AppendField("STATE", FIELD_STRING);
		mStateTable->AppendField("SERVER_ID", FIELD_INT);
		mStateTable->AppendField("THREAD", FIELD_STRING);
		mStateTable->AppendField("TICK", FIELD_STRING);
		mStateTable->AppendField("LOWTICK", FIELD_STRING);
		mStateTable->AppendField("USE_MEM", FIELD_FLOAT);
		mStateTable->AppendField("FREE_MEM", FIELD_STRING);
		mStateTable->AppendField("DISK", FIELD_STRING);
		mStateTable->AppendField("RESTART", FIELD_STRING);  
		mStateTable->AppendField("VER", FIELD_STRING);
		mStateTable->AppendField("INFO", FIELD_STRING);

		mDBInfoTable = tBaseTable::NewBaseTable();
		mDBInfoTable->AppendField("ID", FIELD_INT);
		mDBInfoTable->AppendField("SERVER_ID", FIELD_INT);
		mDBInfoTable->AppendField("TABLE_NAME", FIELD_STRING);
		mDBInfoTable->AppendField("DB_INFO", FIELD_STRING);

	}

	void  Clear();

	void ShowTable(AutoTable dataTable);

	void ClearLog() { mEditInfo.SetWindowText(""); }

	bool IsHttps()
	{
		return mbSelectHttps;
	}

	const char* Http()
	{
		return IsHttps() ? "Https" : "Http";
	}

	static void _AsyncTotalRunState(CManagerToolDlg *pDlg)
	{
		pDlg->AsyncTotalRunState();
	}

	void AsyncTotalRunState(AutoEvent waitEvent = AutoEvent());

	struct DBNetData;
	// 异步同时进行请求所有DB运行信息
	void AwaitRequestDBRunState(DBNetData *db);

	// 请求DBdump文件, 并保存到PDB对应的版本目录中
	// DB直接发送DUMP文件, 传输消息比较大, 所有此功能响应端实现在工具内, 开启在远程服务器上, 同时具有ws查询与报警功能
	void AwaitRequestDBDumpFile(DBNetData *db)
	{

	}

	// 通知到远程版本机上的管理工具, 用于主动扫描DUMP文件 (间隔60秒, 遍历检查一次DUMP), 发送到版本机上, 成功后删除(有效防止硬盘占用过大)
	void AsyncScanDumpFile();

	bool AWaitSend(const char *szDumpFile);

	bool ReceiveDumpFile(AutoEvent revEvent);

	static void _AsyncConnectAD(CManagerToolDlg *pDlg)
	{
		pDlg->AsyncConnectAD();
	}

	void AsyncStartUpdateClientServer();

	void AsyncStartScan();

	void AsyncConnectAD();

	void AsyncCloseServer();
	void AsyncRestoreBack();

	void AsyncMakeClientPacket();
	void AsyncUpdateClient();

	bool OnUpdateClientWeb(AutoData webPacketData, AString &info);

	void ResetTitle();

	AutoNice AsyncRequestDB(int serverID, const AString &opName, NiceData &param, int waitMilTime);

	void AsyncRequestExportFeedBack();

	void AsyncImportFeedBack();

	void AsyncExportBlackWhiteList();

	void AsyncImportBlackWhiteList();

public:
	AutoTable					mStateTable;
	AutoTable					mDBInfoTable;
	ANetNode					mUserNode;
	Hand<DBUnit>			mDBUnit;
	bool mbSelectHttps = false;
	bool mbStartScanDumpFile = false;
	UnitID							mReadyReceiveDumpID;

	AutoEvent					mDumpFileSendEvent;
	AutoNet						mWssNet;
	ANetNode					mAccountNode;
	Hand<DBUnit>			mAccountDBUnit;
	Hand<DBUnit>			mUpdateClientUnit;

	AutoData						mClientZipData;

	struct DBNetData
	{
		ANetNode				mDBNode;
		Hand<DBUnit>		mDBUnit;
		bool							mbConnected = false;
		LogicDBConfig			mDBConfig;
		AutoEvent				mWaitEvent;
	};

	Array<DBNetData>		mDBNetList;

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MANAGERTOOL_DIALOG };
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
	ShowInfoGrid mServerListGrid;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnRequestRunState();
	CEdit mEditLogicDBAddress;
	CEdit mEditWssPort;
	CEdit mEditHttpParam;
	CEdit mEditSDKIP;
	CEdit mEditSDKPort;
	CEdit mEditToolIp;
	CEdit mEditInfo;
	afx_msg void OnBnStartScanDumpFile();
	CButton mButtonStartScanDump;
	CEdit mEditLocalADUnitID;
	CEdit mEditRemoteMgrUnitID;
	afx_msg void OnBnStartWssServer();
	CEdit mEditBackDBFileName;
	afx_msg void OnBnRestoreDB();
	CEdit mEditDBRestoreServerID;
	CEdit mEditServerArea;
	afx_msg void OnBnCloseServer();


	afx_msg void OnBnClickedButtonHttpTest();
	CButton mSelectHttps;
	afx_msg void OnBnSelectHttps();
	CEdit mEditCloseServerPassword;
	afx_msg void OnBnConnectAccount();
	afx_msg void OnBnClickedButtonSelectServerPath();
	CEdit mEditServerRunPath;
	CButton mFlagShowSendDumpInfo;
	CButton mButtonStartWss;
	CEdit mEditClientWebPath;
	afx_msg void OnBnSelectClientWebPath();
	afx_msg void OnBnStartClientWebUpdate();
	afx_msg void OnBnUploadClientUpdate();
	afx_msg void OnBnClickedButtonUploadClientUpdate2();
	CButton mButtonUploadClient;
	CEdit mEditUpdateUnitID;
	CButton mButtonStartUpdateClientServer;
	afx_msg void OnBnExportFeedBackTable();
	afx_msg void OnBnImportFeedBack();
	afx_msg void OnBnClickedButtonTestFeedBackData();
	afx_msg void OnBnExportBlackWhiteList();
	afx_msg void OnBnImportBlackWhiteList();
	afx_msg void OnBnShowDBWindow();
	CButton mRadioShowDB;
};
