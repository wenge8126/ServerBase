
#include "BaseServer.h"
#include "ShareMemAO.h"
//-------------------------------------------------------------------------
#include "TableManager.h"
//#include "EventCenterManager.h"


#include "BaseThread.h"

#ifdef __WINDOWS__
#include <Windows.h>
#include "Dump.h"
#include <conio.h>
#endif
#include "Dump.h"
#include "SystemInfo.h"

tBaseServer::tBaseServer() 
	: mBaseThread(NULL)	
	, mAlready(false)
	, mpInputThread(NULL)
{
	
}


tBaseServer::~tBaseServer()
{
	//if (TableManager::getSingletonPtr())
	//delete TableManager::getSingletonPtr();
	SAFE_DELETE(mpInputThread);
}


bool tBaseServer::InitConfig( const char* szConfigTableName )
{
	if (szConfigTableName==NULL || strcmp(szConfigTableName, "")==0)
		return false;
	TableManager::SetDefaultPath("./");
	return TableManager::getSingleton().LoadTable(szConfigTableName, "CSV");
}



bool tBaseServer::InitStaticData(BaseThread	 *pBaseThread)
{
	ReleaseStaticData();

	mBaseThread = pBaseThread;
	mBaseThread->InitEventCenter(AutoEventCenter());

	mAlready = true;
	return true;
}

void tBaseServer::ReleaseStaticData(void)
{
	if (!mAlready)
		return;

	delete TableManager::getSingletonPtr();

	mAlready = false;
	return;
}



bool tBaseServer::Start(BaseThread *pBaseThread, const char *szProcessName, const char* szLogFileName, const char* szConfigTableName)
{
	if (!TableManager::Exist())
		new TableManager();

	//Allot::setLogFile(szProcessName);
	//TableManager::SetLog( new TableLog(szLogFileName) );

	InitConfig(szConfigTableName);
	
	InitStaticData(pBaseThread);

	mBaseThread->start();

	return true;
}


void tBaseServer::WaitWorking(bool bControlInput, int onceCheckSecond, bool bFinishFreeThread, float maxAllowLoopSecond)
{
	SAFE_DELETE(mpInputThread);
	if (bControlInput)
	{
		//mpInputThread = MEM_NEW ControlThread(mBaseThread, onceCheckSecond, maxAllowLoopSecond);
		//mpInputThread->backWorkThread();
		MainLoop(onceCheckSecond, (int)maxAllowLoopSecond); 
	}
	else
	{
		mpInputThread = MEM_NEW InputThread(mBaseThread);

		mpInputThread->InitThread();
		// 等待工作线程
		mBaseThread->MainWaitWork();

		mpInputThread->_ForceClose(0);

		SAFE_DELETE(mpInputThread);
	}

	NOTE_LOG("*3 Work thread already stop, now close work thread ...");
    mBaseThread->mCloseLock.lock();
    mBaseThread->Close();
    mBaseThread->mCloseLock.unlock();
	NOTE_LOG("*2 Work thread already closed, start close server thread ...");
	NOTE_LOG("*1 Server thread already closed, Main thread stoping ...");
	Stop();
	
	if (bFinishFreeThread)
	{
		delete mBaseThread;
		mBaseThread = NULL;
	}
	NOTE_LOG("*0 Server thread already stoped and delete work thread succeed, server finished exit");
}

void tBaseServer::RequestClose()
{
	if (mBaseThread!=NULL)
		mBaseThread->RequestClose();
}

void tBaseServer::Stop(void)
{
	mBaseThread->Close();
	ReleaseStaticData();
}

bool tBaseServer::IsStop()
{
	if (mBaseThread!=NULL) return mBaseThread->IsStoped(); return false;
}


void tBaseServer::MainLoop(int onceCheckSecond, int checkMaxLoopSecond)
{
	static const int nInputMaxLength = 256;
	char szInputString[nInputMaxLength+1];
	while (true)
	{
		if (mBaseThread->IsStoped())
		{
			if (mBaseThread->IsException())
				mBaseThread->Close();
			break;
		}
		TimeManager::Sleep(onceCheckSecond*1000);

		if (mBaseThread->ControlCheckLoopSecond()> checkMaxLoopSecond) 
		{
			UInt64 x = mBaseThread->ControlCheckLoopSecond();
			ERROR_LOG("严重问题: 工作线程当前循环时间%d超过最大检查时间%d", mBaseThread->ControlCheckLoopSecond(), checkMaxLoopSecond);
			mBaseThread->mbException = true;
			tProcessException(NULL);
			mBaseThread->_ForceClose(-2);
			mBaseThread->NotifyException();
			break;
			//OnWorkThreadFault(mpThread, mpThread->ControlCheck(), mpThread->GetMaxLoopTime());
		}
		//else
			//DEBUG_LOG("Work average time %d, max loop time %d", mpThread->GetAverageLoopTime(), mpThread->GetMaxLoopTime());

		if (kbhit())
		{
			fgets(szInputString, nInputMaxLength, stdin);
			// NOTE: 最后会包含一个回车符, 必须去掉
			if (strlen(szInputString) <= 0)
				continue;
			szInputString[strlen(szInputString) - 1] = '\0';
			if (strcmp(szInputString, "`") == 0)
			{
				mBaseThread->RequestClose();
			}
			else if (strcmp(szInputString, "state") == 0)
			{
				//NOTE_LOG("*Now state: work average time %d, max loop time %d", mpThread->GetAverageLoopTime(), mpThread->GetMaxLoopTime());
			}
			else
			{
				mBaseThread->ProcessCommand(szInputString);
			}
		}

	}
}
#ifdef __WINDOWS__

bool tBaseServer::SetIocn(const char *szIconBmpFile, const char *szText, const char *szFontName, int nFontSize, unsigned int color)
{
	if (szIconBmpFile==NULL)
		return false;

	HMODULE m = GetModuleHandle(NULL);
	HANDLE hbitmap = LoadImage(NULL,szIconBmpFile,IMAGE_BITMAP,0,0,LR_LOADFROMFILE); 

	if (hbitmap==(HANDLE)INVALID_HANDLE || hbitmap==NULL)
		return false;

	HDC hMemDC = NULL;
	HFONT f = NULL;
	if (szText!=NULL)
	{	
		if (szFontName!=NULL)
			f = CreateFont(nFontSize,0,0,0,100,FALSE,FALSE,0,ANSI_CHARSET,OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,FF_SWISS, szFontName);

		HDC hDC = ::GetDC(GetConsoleWindow());
		//CFont * pOldFont = hDC->SelectObject();
		hMemDC = CreateCompatibleDC(hDC);
		if (f!=NULL)
			SelectObject(hMemDC, f);
		SelectObject(hMemDC, hbitmap);
		//在位图上写字
		RECT rect = {0, 0, 600, 600};
		SetTextColor(hMemDC, RGB(color>>16 & 0xFF, color>>8 & 0xFF, color&0xFF));
		SetBkMode(hMemDC, TRANSPARENT);
		DrawText(hMemDC, szText, -1, &rect, DT_VCENTER);
	}
	BITMAP bmp;  
	GetObject(hbitmap,sizeof(BITMAP),&bmp);

	HBITMAP hbmMask = ::CreateCompatibleBitmap(::GetDC(NULL), 
		bmp.bmWidth, bmp.bmHeight);

	ICONINFO ii = {0};
	ii.fIcon = TRUE;
	ii.hbmColor = (HBITMAP)hbitmap;
	ii.hbmMask = hbmMask;

	HICON hIcon = ::CreateIconIndirect(&ii);//一旦不再需要，注意用DestroyIcon函数释放占用的内存及资源
	if (hMemDC!=NULL)
		DeleteDC(hMemDC);
	if (f!=NULL)
		DeleteObject(f);
	::DeleteObject(hbmMask);

	HWND   hwnd=GetConsoleWindow();//直接获得前景窗口的句柄   

	SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);   
	SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
	return true;
}

void tBaseServer::RestartProgram(BaseThread *pThread, bool bException)
{
	NiceData lineParam;
	SystemInfo::ReadCommandLineParam(::GetCommandLine(), lineParam);
	lineParam["restart"] = (pThread->mRestartCount + 1);
	lineParam["exception"] = (pThread->mExceptionCount + (bException?1:0) );

	AString strCommand = SystemInfo::AppendCommandLineParam(SystemInfo::RemoveCommandLineParam(::GetCommandLine()).c_str(), lineParam);
	LOG("Command line >%s, Now >%s\r\n%s", ::GetCommandLine(), strCommand.c_str(), lineParam.dump().c_str());

	char szDir[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH - 1, szDir);
	StartRun(strCommand.c_str(), szDir);

	NiceData state;
	int x = (int)pThread->GetRunStateInfo(state);
	LOG("Restart before state %d:\r\n%s", x, state.ToJSON().c_str());

	UInt64 now = TimeManager::SysMicrosecond();
	UInt64 second = (UInt64)(now * 0.000001);
	AString logString;
	logString.Format("%s %d > %s restart : %s", TimeManager::ToTimeString(second).c_str(), (int)(now*0.001 - second * 1000), pThread->GetThreadName().c_str(), strCommand.c_str());
	pThread->LogRun("./RunConfig/Start.log", logString.c_str());
}

void tBaseServer::RestartProgram(int restartCount, int exceptionCount, bool bException)
{
	NiceData lineParam;
	SystemInfo::ReadCommandLineParam(::GetCommandLine(), lineParam);
	lineParam["restart"] = (restartCount + 1);
	lineParam["exception"] = (exceptionCount + (bException ? 1 : 0) );

	AString strCommand = SystemInfo::AppendCommandLineParam(SystemInfo::RemoveCommandLineParam(::GetCommandLine()).c_str(), lineParam);
	LOG("Command line >%s, Now >%s\r\n%s", ::GetCommandLine(), strCommand.c_str(), lineParam.dump().c_str());

	char szDir[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH - 1, szDir);
	StartRun(strCommand.c_str(), szDir);

	UInt64 now = TimeManager::SysMicrosecond();
	UInt64 second = (UInt64)(now * 0.000001);
	AString logString;
	logString.Format("%s %d > Restart : %s", TimeManager::ToTimeString(second).c_str(), (int)(now*0.001 - second * 1000), strCommand.c_str());
	BaseThread::LogRun("./RunConfig/Start.log", logString.c_str());
}

#else
bool tBaseServer::SetIocn(const char *szIconBmpFile, const char *szText, const char *szFontName, int nFontSize, unsigned int color)
{
	return false;
}

void tBaseServer::RestartProgram(BaseThread *pThread)
{
	PRINT("Restart program fail\r\n");
}

void tBaseServer::RestartProgram(int restartCount, int exceptionCount)
{
	PRINT("Restart program fail\r\n");
}

#endif



//-------------------------------------------------------------------------
tProcess::tProcess() 
	: mShareMem(NULL)
	, mbReadyShareMemOk(false)
{

}

tProcess::~tProcess()
{
	if (mShareMem)
		delete mShareMem;
	mShareMem = NULL;
}

bool tProcess::InitShareMem( unsigned long memKey, unsigned int uSize, bool bCreate )
{
	if (mShareMem==NULL)
		mShareMem = MEM_NEW ShareMemAO();

	if (bCreate)
		mbReadyShareMemOk = mShareMem->Create(memKey, uSize)==TRUE;
	else
		mbReadyShareMemOk = mShareMem->Attach(memKey, uSize)==TRUE;

	return mbReadyShareMemOk;
}

bool tProcess::Process()
{
	// 检查共享内存消息
	if (mbReadyShareMemOk && mShareMem!=NULL)
	{
		char *pMsg = mShareMem->GetDataPtr();
		if (pMsg!=NULL)
			return OnShareMemMsg(pMsg);		
	}
	return true;
}

char* tProcess::GetShareData()
{
	if (mShareMem!=NULL)
		return mShareMem->GetDataPtr();
	return NULL;
}
