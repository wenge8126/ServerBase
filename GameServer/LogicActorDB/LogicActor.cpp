#include "Dump.h"
#include "CommonDefine.h"
#include <Windows.h>
#include "LogicActorThread.h"
#include "BaseServer.h"
#include "resource.h"

int main()
{
	SetUnhandledExceptionFilter(TopLevelFilter);
	Dump::msDumpFileInfo = SERVER_VERSION_FLAG;

	AString logFileName, logErrorFileName;
	SYSTEMTIME nowtime;
	GetLocalTime(&nowtime);
	HWND   hwnd = GetConsoleWindow();

	{
		SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(GetModuleHandle(0), MAKEINTRESOURCE(IDI_ICON_DATA_SHARE)));
		SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)LoadIcon(GetModuleHandle(0), MAKEINTRESOURCE(IDI_ICON_DATA_SHARE)));
	}

	INIT_RUN_CONFIG(LogicActorDBConfig, "logic_actor_config")

	logFileName.Format("./Log/%s_%4d-%02d-%02d_%02d-%02d-%02d.log", LogicActorThread::GetAppName()
		, nowtime.wYear, nowtime.wMonth, nowtime.wDay, nowtime.wHour, nowtime.wMinute, nowtime.wSecond);

	TableTool::SetLog(new TableLog(logFileName.c_str()));

	logErrorFileName.Format("./Error/%s_Error_%4d-%02d-%02d_%02d-%02d-%02d.log.log", LogicActorThread::GetAppName()
		, nowtime.wYear, nowtime.wMonth, nowtime.wDay, nowtime.wHour, nowtime.wMinute, nowtime.wSecond);

	Allot::setLogFile(LogicActorThread::GetAppName());
	TableTool::SetErrorLog(new TableLog(logErrorFileName.c_str()));

	//AString title = DBShareMemThread::GetAppName();
	//title += "  共享内存进程";
	//SetConsoleTitle(title);

	TimeManager::Sleep(1000);

	tBaseServer	*mDBShareMemServer = new tBaseServer();
	LogicActorThread *pThread = MEM_NEW LogicActorThread();
	mDBShareMemServer->Start(pThread, "DBShareMemServer", logFileName.c_str(), "");

#if DEVELOP_MODE
	mDBShareMemServer->WaitWorking(false, 1, false, 3600);
#else
	mDBShareMemServer->WaitWorking(false, 1, false, 30);
#endif

	if (pThread->mbNeedRestart)
	{
		int restartCount = pThread->mRestartCount;
		int exceptionCount = pThread->mExceptionCount;
		delete pThread;
		// 需要等待大约8秒, DB完全关闭, 才可以重用配置的通讯共享key
		LOG("Wait restart after 8 second ...");
		Sleep(8000);
		
		tBaseServer::RestartProgram(restartCount, exceptionCount, false);
	}
	else
		delete pThread;

	delete mDBShareMemServer;
	mDBShareMemServer = NULL;

	TableTool::CleanUp();
}
