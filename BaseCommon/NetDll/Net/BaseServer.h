
#ifndef _INCLUDE_BASESERVER_H_
#define _INCLUDE_BASESERVER_H_

#include "NetHead.h"

#include "InputThread.h"

class BaseThread;
//-------------------------------------------------------------------------
class Net_Export tBaseServer
{
public:
	virtual bool Start(BaseThread *pBaseThread, const char *szProcessName, const char* szLogFileName, const char* szConfigTableName);
	virtual void WaitWorking(bool bControlInput, int onceCheckSecond, bool bFinishFreeThread = true, float maxAllowLoopSecond = 6);

public:
	virtual bool InitConfig(const char* szConfigTableName);
	virtual bool InitStaticData(BaseThread *pBaseThread);
	virtual void ReleaseStaticData(void);

	virtual void RequestClose();
	virtual void Stop(void);
	virtual bool IsStop();
	virtual bool CanStop(){ return true; }

	virtual void ProcessCommand(const char *szCommand){}

	virtual void MainLoop(int onceCheckSecond, int checkMaxLoopSecond); 

	// 128 * 128 nFontSize = 100
	static bool SetIocn(const char *szIconBmpFile, const char *szText, const char *szFontName, int nFontSize, unsigned int color);

	static void RestartProgram(BaseThread *pThread, bool bException);

	static void RestartProgram(int restartCount, int exceptionCount, bool bException);

public:
	tBaseServer();
	virtual ~tBaseServer();

protected:
	BaseThread				*mBaseThread;
	InputThread				*mpInputThread;
	bool							mAlready;

};


//-------------------------------------------------------------------------
class ShareMemAO;

class Net_Export tProcess
{
public:
	tProcess();
	virtual ~tProcess();

public:
	virtual bool OnShareMemMsg(char *) = 0;

public:
	virtual bool InitShareMem(unsigned long memKey, unsigned int uSize, bool bCreate);
	virtual char* GetShareData();
	virtual bool Process();

protected:
	ShareMemAO						*mShareMem;			// 用于进程控制
	bool							mbReadyShareMemOk;
};




#endif //_INCLUDE_BASESERVER_H_
