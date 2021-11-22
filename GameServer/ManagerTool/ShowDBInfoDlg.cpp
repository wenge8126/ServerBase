// ShowDBInfoDlg.cpp: 实现文件
//

#include "pch.h"
#include "ManagerTool.h"
#include "ShowDBInfoDlg.h"
#include "afxdialogex.h"


// ShowDBInfoDlg 对话框

IMPLEMENT_DYNAMIC(ShowDBInfoDlg, CDialogEx)

ShowDBInfoDlg::ShowDBInfoDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_SHOW_DB, pParent)
{

}

ShowDBInfoDlg::~ShowDBInfoDlg()
{
}

void ShowDBInfoDlg::ShowTable(AutoTable dataTable)
{
	mGridShowDB.InitClear();
	mGridShowDB.mTable = dataTable;
	// 表头
	mGridShowDB.SetRowCount(1 + dataTable->GetRecordCount());
	mGridShowDB.SetFixedRowCount(1);
	mGridShowDB.SetFixedColumnCount(1);
	mGridShowDB.SetColumnCount(dataTable->GetField()->getCount());
	//	mGridShowDB.SetFixedColumnCount(1);
	mGridShowDB.SetRowResize(TRUE);
	mGridShowDB.SetEditable(FALSE);

	AutoField f = dataTable->GetField();

	int row = 0;
	for (int col = 0; col < mGridShowDB.GetColumnCount(); col++)
	{
		GV_ITEM Item;
		Item.mask = GVIF_TEXT | GVIF_FORMAT;
		Item.row = row;
		Item.col = col;
		if (row < mGridShowDB.GetFixedRowCount())
		{
			Item.nFormat = DT_CENTER | DT_WORDBREAK;
		}
		else if (col < mGridShowDB.GetFixedColumnCount()) {
			Item.nFormat = DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS;
		}
		Item.szText = f->getFieldInfo(col)->getName().c_str();
		mGridShowDB.SetItem(&Item);
	}

	// 表格内容
	for (TableIt tIt(dataTable); tIt; ++tIt)
	{
		++row;
		ARecord re = tIt.getCurrRecord();

		for (int col = 0; col < mGridShowDB.GetColumnCount(); col++)
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

			mGridShowDB.SetItem(&Item);
		}
	}
	//mGridShowDB.AutoSize();
	//mGridShowDB.ExpandToFit();
	//mGridShowDB.AutoSize();
	//mGridShowDB.SetRowResize(FALSE);

	//mGridShowDB.SetColumnWidth(1, 6000);
	for (int i = 0; i < dataTable->GetField()->getCount(); ++i)
		mGridShowDB.AutoSizeColumn(i);
	mGridShowDB.SetRedraw(TRUE, TRUE);
}

void ShowDBInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DB_LIST, mGridShowDB);
}


BEGIN_MESSAGE_MAP(ShowDBInfoDlg, CDialogEx)
END_MESSAGE_MAP()


// ShowDBInfoDlg 消息处理程序
