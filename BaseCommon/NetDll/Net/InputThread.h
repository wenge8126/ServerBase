
#ifndef _INCLUDE_INPUTTHREAD_H_
#define _INCLUDE_INPUTTHREAD_H_

#include "NetHead.h"
#include "WorkThread.h"

class BaseThread;
//-------------------------------------------------------------------------
// ���ڿ���̨�����߳�
// һֱ�����ڵȴ�����, �������Ż�CPUռ��
// ���߳�ֻ��Ҫ����  BaseThread::MainWaitWork �ȴ�, �������رճ���
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
// ��ع����߳�
// �Թ����̼߳��, �����Ƿ���ѭ��, ָ��ʱ�����޷�Ӧ����
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