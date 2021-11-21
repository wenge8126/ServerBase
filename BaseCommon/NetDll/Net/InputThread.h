
#ifndef _INCLUDE_INPUTTHREAD_H_
#define _INCLUDE_INPUTTHREAD_H_

#include "NetHead.h"
#include "WorkThread.h"

class BaseThread;
//-------------------------------------------------------------------------
// 用于控制台输入线程
// 一直阻塞在等待输入, 最大程序优化CPU占用
// 主线程只需要调用  BaseThread::MainWaitWork 等待, 可正常关闭程序
//-------------------------------------------------------------------------
class Net_Export InputThread : public WorkThread
{
public:
	virtual void OnInput(char ch){}

public:
	InputThread(BaseThread *pThread)
		: mpThread(pThread)
	{ }

	virtual void backWorkThread(void) override;

public:
	BaseThread *mpThread;
};
//-------------------------------------------------------------------------
// 监控工作线程
// 对工作线程监控, 比如是否死循环, 指定时间内无反应警告
//-------------------------------------------------------------------------
class Net_Export ControlThread : public InputThread
{
public:
	virtual void OnWorkThreadFault(BaseThread *pThread, int nowLoopUseTime, int maxLoopTime){}

public:
	ControlThread(BaseThread *pThread, int onceCheckSecond, float maxAllowLoopSecond)
		: InputThread(pThread)
		, mOnceMilSec(onceCheckSecond*10)
		, mCheckMaxTime((int)(maxAllowLoopSecond*1000))
	{ InitThread(); }

	virtual void backWorkThread(void) override;

public:
	int mOnceMilSec;
	int mCheckMaxTime;
};
//-------------------------------------------------------------------------
#endif //_INCLUDE_INPUTTHREAD_H_