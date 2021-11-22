// CloudGate.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "Dump.h"
#include "GateThread.h"
#include "BaseServer.h"
#include <windows.h>
#include "resource.h"

#define RELEASE_RUN_VER  0
//#define SERVER_VERSION_FLAG "1.00.00"

int main()
{

	SetUnhandledExceptionFilter(TopLevelFilter);
	Dump::msDumpFileInfo = SERVER_VERSION_FLAG;

	//HWND   hwnd=GetForegroundWindow();//直接获得前景窗口的句柄   
	HWND   hwnd = GetConsoleWindow();//GetForegroundWindow();//直接获得前景窗口的句柄   
	EnableMenuItem(::GetSystemMenu(hwnd, FALSE), SC_CLOSE, MF_BYCOMMAND | MF_GRAYED);	//禁用关闭按钮
	SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)LoadIcon(NULL, "AccountIcon.ico"));

	if (strstr(GetCommandLine(), "account_gate_config")!=NULL)
	{
		SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(GetModuleHandle(0), MAKEINTRESOURCE(IDI_ICON_ACCOUNT_GATE)));
		SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)LoadIcon(GetModuleHandle(0), MAKEINTRESOURCE(IDI_ICON_ACCOUNT_GATE)));
	}
	else
	{
		SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(GetModuleHandle(0), MAKEINTRESOURCE(IDI_ICON_MAIN_GATE)));
		SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)LoadIcon(GetModuleHandle(0), MAKEINTRESOURCE(IDI_ICON_MAIN_GATE)));
	}

	AString logFileName, logErrorFileName;
	SYSTEMTIME nowtime; 
	GetLocalTime(&nowtime);

	CHECK_RUN_CONFIG(GateServerConfig, "account_gate_config")
			else
			INIT_RUN_CONFIG(GateServerConfig, "gate_config")

	logFileName.Format("./Log/CloudGate_%4d-%02d-%02d.log",
		nowtime.wYear, nowtime.wMonth, nowtime.wDay);

	TableTool::SetLog(new TableLog(logFileName.c_str()));


	logErrorFileName.Format("./Error/CloudGate_Error_%4d-%02d-%02d.log",
		nowtime.wYear, nowtime.wMonth, nowtime.wDay);

	Allot::setLogFile("MemCloudGate");
	TableTool::SetErrorLog(new TableLog(logErrorFileName.c_str()));


	tBaseTable::msCommonConfig.mRecordMemPoolBlockRecordCount = 1000;

	TableManager *pMgr = new TableManager();

	tBaseServer	*mDBServer = new tBaseServer();
	GateThread *gpThread = MEM_NEW GateThread();
	mDBServer->Start(gpThread, "CloudGate", logFileName.c_str(), "");

	//char szDir[MAX_PATH];
	//::GetCurrentDirectory(MAX_PATH - 1, szDir);
	//AString tile;
	//tile.Format("%s_%s版 v[%s] %s", CRunConfig<GateServerConfig>::mConfig.title.c_str(),
	//	, RELEASE_RUN_VER != 0 ? "运行" : "开发"
	//	, SERVER_VERSION_FLAG
	//	, szDir
	//);
	//SetConsoleTitle(tile.c_str());
#if DEVELOP_MODE
	mDBServer->WaitWorking(true, 1, false, 3600); // 1分钟检查程序超时
#else
	mDBServer->WaitWorking(true, 1, false, 30); // 1分钟检查程序超时
#endif

	if (gpThread->mbNeedRestart)
	{
		char szDir[MAX_PATH];
		::GetCurrentDirectory(MAX_PATH - 1, szDir);
		StartRun(GetCommandLine(), szDir);
	}
	//else
	//{
	//	for (int i = 1; i <= 100; ++i)
	//	{
	//		bool bNeedRestart = gpThread->IsException();
	//		int restartCount = gpThread->mRestartCount;
	//		if (bNeedRestart)
	//		{
	//			delete gpThread;
	//			gpThread = NULL;

	//			NOTE_LOG("\r\n\r\n***********************************************************\r\n*\r\n* Work thread exception exit \r\n*Now restart work thread after three second (Restart time %d)\r\n*\r\n***********************************************************\r\n", i);

	//			SYSTEMTIME nowtime;
	//			GetLocalTime(&nowtime);
	//			logFileName.Format("./Log/CloudGate_%4d-%02d-%02d.log",
	//				nowtime.wYear, nowtime.wMonth, nowtime.wDay);

	//			gpThread = MEM_NEW GateThread();
	//			gpThread->mRestartCount = restartCount + 1;
	//			mDBServer->Start(gpThread, "CloudGate", logFileName.c_str(), "");
	//			mDBServer->WaitWorking(true, 1, false, 60);
	//		}
	//		else
	//			break;
	//	}
	//}

	delete gpThread;
	gpThread = NULL;

	delete mDBServer;
	mDBServer = NULL;

	TableTool::CleanUp();
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
