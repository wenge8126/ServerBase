// WebLogin.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "CenterThread.h"
#include <windows.h>
#include "Dump.h"
#include "BaseServer.h"
#include "resource.h"
#include "Vesion.h"


int main()
{
	SetUnhandledExceptionFilter(TopLevelFilter);
	Dump::msDumpFileInfo = SERVER_VERSION_FLAG;

	//HWND   hwnd=GetForegroundWindow();//直接获得前景窗口的句柄   
	HWND   hwnd = GetConsoleWindow();//GetForegroundWindow();//直接获得前景窗口的句柄   
	EnableMenuItem(::GetSystemMenu(hwnd, FALSE), SC_CLOSE, MF_BYCOMMAND | MF_GRAYED);	//禁用关闭按钮
	//SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)LoadIcon(NULL, "AccountIcon.ico"));

	if (CenterThread::IsAccountWeb())
	{
		SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(GetModuleHandle(0), MAKEINTRESOURCE(IDI_ICON_ACCOUNT_WEB)));
		SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)LoadIcon(GetModuleHandle(0), MAKEINTRESOURCE(IDI_ICON_ACCOUNT_WEB)));
	}
	else
	{
		SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(GetModuleHandle(0), MAKEINTRESOURCE(IDI_ICON_CENTER_SERVER)));
		SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)LoadIcon(GetModuleHandle(0), MAKEINTRESOURCE(IDI_ICON_CENTER_SERVER)));
	}

	AString logFileName, logErrorFileName;
	SYSTEMTIME nowtime;
	GetLocalTime(&nowtime);

			INIT_RUN_CONFIG(LoginConfig, "account_config")

	logFileName.Format("./Log/%s_%4d-%02d-%02d_%02d-%02d-%02d.log", CenterThread::GetAppName(),
		nowtime.wYear, nowtime.wMonth, nowtime.wDay, nowtime.wHour, nowtime.wMinute, nowtime.wSecond);

	TableTool::SetLog(new TableLog(logFileName.c_str()));

	logErrorFileName.Format("./Error/%s_Error_%4d-%02d-%02d_%02d-%02d-%02d.log", CenterThread::GetAppName(),
		nowtime.wYear, nowtime.wMonth, nowtime.wDay, nowtime.wHour, nowtime.wMinute, nowtime.wSecond);

	Allot::setLogFile(CenterThread::GetAppName());
	TableTool::SetErrorLog(new TableLog(logErrorFileName.c_str()));

	PRINT("Now command line >%s\r\n", GetCommandLine());
	if (strstr(GetCommandLine(), "run_account_web")!=NULL)
	{
		char szDir[MAX_PATH];
		::GetCurrentDirectory(MAX_PATH - 1, szDir);

		TCHAR		szModule_Name[MAX_PATH] = { 0 };
			GetModuleFileName(NULL, szModule_Name, MAX_PATH);
			AString line;
			line.Format("%s account_web_cofnig", szModule_Name);
			StartRun(line.c_str(), szDir);
	}

	TableManager *pMgr = new TableManager();
	::Sleep(2000);
	tBaseServer	*mServer = new tBaseServer();
	CenterThread *gpThread = MEM_NEW CenterThread();
	mServer->Start(gpThread, CenterThread::GetAppName(), logFileName.c_str(), NULL);

	//char szDir[MAX_PATH];
	//::GetCurrentDirectory(MAX_PATH - 1, szDir);
	//AString tile;
	//tile.Format("LogicCloudDB_%s版 v[%s] %s"
	//	, RELEASE_RUN_VER != 0 ? "运行" : "开发"
	//	, SERVER_VERSION_FLAG
	//	, szDir
	//);
	//SetConsoleTitle(tile.c_str());

#if DEVELOP_MODE
	mServer->WaitWorking(true, 1, false, 3600);
#else
	mServer->WaitWorking(true, 1, false, 30);
#endif

	if (gpThread->mbNeedRestart)
	{		
		tBaseServer::RestartProgram(gpThread, false);
	}
	else
	{
		// 由于在协程内dump, 无法跳到此处, 因此, 直接在异常回调中重启进程
		//for (int i = 1; i <= 100; ++i)
		//{
		//	bool bNeedRestart = gpThread->IsException();
		//	int restartCount = gpThread->mRestartCount;
		//	if (bNeedRestart)
		//	{
		//		delete gpThread;
		//		gpThread = NULL;

		//		NOTE_LOG("\r\n\r\n***********************************************************\r\n*\r\n* Work thread exception exit \r\n*Now restart work thread after three second (Restart time %d)\r\n*\r\n***********************************************************\r\n", i);

		//		SYSTEMTIME nowtime;
		//		GetLocalTime(&nowtime);
		//		logFileName.Format("./Log/%s_%4d-%02d-%02d.log", WebLoginThread::GetAppName(),
		//			nowtime.wYear, nowtime.wMonth, nowtime.wDay);

		//		gpThread = MEM_NEW WebLoginThread();
		//		gpThread->mRestartCount = restartCount + 1;
		//		mServer->Start(gpThread, WebLoginThread::GetAppName(), logFileName.c_str(), "");
		//		mServer->WaitWorking(true, 1, false, 60);
		//	}
		//	else
		//		break;
		//}
	}
	delete gpThread;
	gpThread = NULL;

	delete mServer;
	mServer = NULL;

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
