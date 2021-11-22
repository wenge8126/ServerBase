
// ExcelToolDlg.h: 头文件
//

#pragma once

#include "DBUnit.h"
#include "NetNode.h"

using namespace NetCloud;

typedef EasyMap<AString, AutoNice>  ProtocolMap;

// CExcelToolDlg 对话框
class CExcelToolDlg : public CDialogEx
{
public:
	ANetNode				mUserNode;
	Hand<DBUnit>			mDBUnit;
	Hand<NetUnit>		mBigMsgUnit;

	ANetNode				mAccountNode;
	Hand<DBUnit>		mAccountDBUnit;
	Hand<NetUnit>		mAccountBigUnit;
	bool							mbHttps = false;

	AutoTable					mToolConfig;

public:
	void AsyncNewCreateDB();

	void AsyncNewCreateAccountDB();

	void AsyncApplyUpdateConfig();

	void AsyncApplayUpdatePluginDll();

	static void AsyncUploadRunPacket(CExcelToolDlg*);

	static void AsyncCreateServerRun(CExcelToolDlg *pDlg);

	static void AsyncUploadGlobalConfig(CExcelToolDlg *pDlg);

	void _AsyncCreateServerRunEx();

	void _AsyncCreateServerRun();

	void GenerateConfigPacketAndCode(bool bGenerateCode);

	// 生成C++ 协议结构代码
	// 生成MSG的结构代码
	AString GenerateMsgProtocolCppCode(AutoNice proList, AutoNice proNotes, Array<AString> &structNameList, EasyHash<AString , bool> &exportRunHash, bool bFirst, bool bRunConfig = false);

	AString GenerateMsgProtocolTSCode(AutoNice proList, AutoNice proNotes, Array<AString> &structNameList, bool bFirst);

	AString GenerateConfigEnumTsCode(TableMap &tableMap, AString &cppEnumString);

	AutoNice GenerateProtocol(const AString &fileName, const AString &targetName, bool bTSCode = true);

	// 获取DBList配置包数据
	AutoData CreateDBListConfigPacket(AString &info);

	void UpdateServerRunFile();

	void AsyncConnectDB();

// 构造
public:
	CExcelToolDlg(CWnd* pParent = nullptr);	// 标准构造函数
	~CExcelToolDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EXCELTOOL_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

	virtual void OnCancel()
	{
		Clear();
		CDialogEx::OnCancel();
	}

	CString SelectPath();

	void ClearLog() { mEditInfo.SetWindowText(""); }

	void Clear();

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnExportTable();
	CEdit mEditConfigPath;
	CEdit mEditInfo;
	afx_msg void OnSelectConfigPath();
	CEdit mEditMakeJSPath;
	CEdit mEditMakeCPath;
	CEdit mEditMakeConfigPath;
	afx_msg void OnBnUpload();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnSelectTSPath();
	afx_msg void OnBnSelectCPath();
	afx_msg void OnBnSelectTargetConfigPath();
	afx_msg void OnBnNewCreateDB();
	afx_msg void OnBnGenerateProtocol();
	afx_msg void OnBnRestartLogicServer();
	afx_msg void OnBnReconnect();
	afx_msg void OnBGenerateDbStruct();
	CButton mMakeConfigButton;
	CButton mUploadConfigButton;
	afx_msg void OnBnClickedButtonGenerateProtoc2();
	CEdit mEditDBWorkerID;
	afx_msg void OnBnCreateAccountDB();
	CEdit mEditGateIP;
	CEdit mEditGatePort;
	afx_msg void OnBnGenrateRunconfig();
	afx_msg void OnBnUpdateRunconfig();
	afx_msg void OnBnMakeConfigPacket();
	CEdit mEditHttpParam;
	afx_msg void OnBnSendHttpTest();
	afx_msg void OnBnApplyConfig();
	CButton mButtonApplyConfig;
	afx_msg void OnBnUploadPluginDll();
	afx_msg void OnBnApplyPluginDll();
	CButton mButtonUploadPlugin;
	afx_msg void OnBnMakeRunPacket();
	afx_msg void OnBnUploadRunPacket();
	CButton mButtonUploadRunPacket;
	CEdit mEditSaftCode;
	CEdit mEditRunServerPath;
	afx_msg void OnBnSelectRunPath();
	afx_msg void OnBnCreateServerRun();
	CButton mButtonCreateServerRun;
	CEdit mEditLocalIP;
	CEdit mEditWssAddress;
	CEdit mEditMysqlIP;
	CEdit mEditMysqlUser;
	CEdit mEditMysqlPassword;
	CEdit mEditGameServerName;
	CEdit mEditSSLKeyFile;
	CEdit mEditAccountIP;
	CEdit mEditAccountPort;
	CEdit mEditAccountSafeCode;
	afx_msg void OnBnConnectAccountGate();
	afx_msg void OnBnUploadGlobalConfig();
	CButton mButtonGlobalConfig;
	afx_msg void OnBnClickedButtonSelectConfig5();
	afx_msg void OnBnClickedButtonUpdateServer();
	afx_msg void OnBnClickedSelectHttps();
	CButton mSelectHttps;
	CEdit mEditCloseServerPassword;
	CEdit mEditUpdateServerID;
	afx_msg void OnBnSendNotice();
	CButton mButtonShowDBConnectInfo;
	// 日志服ip和地址
	CEdit mLogInfo;
	CEdit mEditBackSqlIP;
};
