#pragma once

#include "GridCtrl.h"
#include "ComboGridCtrl.h"
#include "BaseTable.h"

class ShowInfoGrid : public CComboGridCtrl
{
	void OnLButtonDown();

public:
	ARecord GetSelectLoginServer()
	{

	}
	void InitClear()
	{
		m_LeftClickDownCell.col = -1;
		m_LeftClickDownCell.row = -1;
		ResetSelectedRange();
	}

public:
	AutoTable mTable;
};

// ShowDBInfoDlg 对话框

class ShowDBInfoDlg : public CDialogEx
{
	DECLARE_DYNAMIC(ShowDBInfoDlg)

public:
	ShowDBInfoDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~ShowDBInfoDlg();

	void ShowTable(AutoTable dataTable);


// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_SHOW_DB };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	ShowInfoGrid mGridShowDB;
};
