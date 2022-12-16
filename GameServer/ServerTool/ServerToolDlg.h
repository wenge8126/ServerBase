
// ServerToolDlg.h: 头文件
//

#pragma once

#include "ActorManager.h"
#include "UDPNet.h"
#include "FastHash.h"

// CServerToolDlg 对话框
class CServerToolDlg : public CDialogEx
{
	UDPNet		mUPDNet;
	DataBuffer	mReceiveData;

	FastHash<UInt64, AutoNice>	mNetInfoList;
	Array<AutoNice>	mAllNetList;

// 构造
public:
	CServerToolDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SERVERTOOL_DIALOG };
#endif

	Auto<NetCloud::ActorManager> mActorManager;
	HandActor mToolActor;
	AutoTable mToolConfig;

	void AsyncConnectGate();
	void AsyncCreateDB();

	void ClearLog()
	{
		mEditInfo.SetWindowText("");
	}


	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonConnect();
	afx_msg void OnBnCreateDB();
	CString mGateIP;
	CString mConfigPath;
	int mGatePort;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CEdit mEditInfo;
	afx_msg void OnBnClickedTestCreateAccount();
	afx_msg void OnBnExportRunConfig();
	afx_msg void OnBnExportToolProtocol();
};
