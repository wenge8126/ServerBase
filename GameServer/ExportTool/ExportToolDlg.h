
// ExportToolDlg.h: 头文件
//

#pragma once

#include "framework.h"
#include "HttpPostNet.h"
#include "TableManager.h"



// CExportToolDlg 对话框
class CExportToolDlg : public CDialogEx
{
public:	
	void GenerateConfigPacketAndCode(bool bGenerateCode);
	AString GenerateConfigEnumTsCode(TableMap &tableMap, AString &cppEnumString);

public:
	HttpPostNet		mHttpClient;
	AutoTable mToolConfig;

public:
	template<typename RESP_MSG>
	Auto< RESP_MSG> Reqeust(int actorType, Int64 actorID, tBaseMsg &requestMsg)
	{
		AString url;
		url.Format("%s:%s", GetText(mWebIP).c_str(), GetText(mWebPort).c_str() );
		AutoNice resp = mHttpClient.AwaitRequest(url.c_str(), actorType, actorID, &requestMsg, 6, 30);
		if (resp)
		{
			Auto<RESP_MSG> msg = MEM_NEW RESP_MSG();
			msg->Full(resp);
			return msg;
		}
		return Auto< RESP_MSG>();
	}

	void ClearLog() { mEditInfo.SetWindowText(""); }

// 构造
public:
	CExportToolDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EXPORTTOOL_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


	static AString GetText(CEdit &edit)
	{
		CString  x;
		edit.GetWindowText(x);
		AString re = (LPCSTR)x;
		return re;
	}

	static void SetText(CEdit &edit, const AString &text)
	{
		edit.SetWindowText(text.c_str());
	}


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
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnExportConfigCode();
	CEdit mWebIP;
	CEdit mWebPort;
	afx_msg void OnBnUploadSaveConfig();
	CEdit mEditInfo;
	CEdit mEditConfigPath;
	CButton mMakeConfigButton;
	CEdit mEditMakeConfigPath;
	CEdit mEditMakeCPath;
	CEdit mEditMakeJSPath;
	afx_msg void OnBnMakeConfigPacket();
};
