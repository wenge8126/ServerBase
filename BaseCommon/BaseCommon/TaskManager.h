/********************************************************************
	created:	2013/08/24
	created:	24:8:2013   13:41
	filename: 	C:\Work\BaseCommon\BaseCommon\TaskManager.h
	file path:	C:\Work\BaseCommon\BaseCommon
	file base:	TaskManager
	file ext:	h
	author:		Yang WenGe
	
	purpose:	�����̹߳���
			��������ƣ����������ں�ִ̨��һ�κ��ͻ����� 2016.2.28
*********************************************************************/

#ifndef _INCLUDE_TASKMANAGER_H_
#define _INCLUDE_TASKMANAGER_H_

#include "Array.h"
#include "PoolList.h"
#include "EasyStack.h"
#include "EasyMap.h"
#include "WorkThread.h"
#include "Task.h"
#include "TaskFactory.h"
#include "BaseCommon.h"

#include "ThreadEvent.h"
#include "Lock.h"
//---------------------------------------------------------------------------------

#define INIT_TASTFACTORY_COUNT		16


typedef PoolList<HandTask>			PoolTaskList;
typedef Array<AutoTaskFactory>		TaskFactoryMap;
typedef EasyStack<HandTask>		TaskList;

//---------------------------------------------------------------------------------
// ��̨�����̹߳���
// NOTE: 1 ��������һ�δ���Ƚ϶�ʱ, ������ɵȴ�ʱ���Ƚϳ�, ��ʹ����������������Խ��
//		 2 �����̨������ִ������, ������߳����� event_wait(mWaitEvent);
//---------------------------------------------------------------------------------
class BaseCommon_Export TaskManager : public WorkThread
{
public:
	TaskManager();
	virtual ~TaskManager();

public:
	virtual void Close(void);
	
	// �����һ������������ʱ����ڴ�ֵ, ������������һѭ��
	virtual int	OnceProcessOverTime(){ return 10; }

	virtual int GetNowTaskCount(){ return mNowTaskCount; }

public:
	// Note: run in main thread
	virtual void RegisterTask( AutoTaskFactory hTaskFactory );

	// Note: run in main thread. ����һ���µ�����, ͬʱ����뵽����Ҫִ���б���.
	virtual HandTask StartTask(int taskType);

	// Note: run in main thread
	virtual AutoTaskFactory GetTaskFactory(int taskType);
	// main or back
	virtual void Log(const char *szInfo, ... );

public:
	/// Note: run in main thread
	virtual void Process(void);

public:
	/// run in back thread.
	virtual VOID backWorkThread ();

	//NOTE: �˵���һ��� 6�����һ�� event_timedwait(mWaitEvent, 6000)
	virtual void backProcess(){}

	virtual void onBeforeClose(void);

protected:
	void Lock(void){ mLock.lock(); }
	void UnLock(void){ mLock.unlock(); }
	bool TryLock(void){ return mLock.trylock(); }

protected:
	//bool						mActive;
	bool						mHaveWillTask;
	bool						m_bNeedProcess;
	bool						m_bHaveFinish;
	bool						m_bBackListEmpty;
	CsLock						mLock;	

	
	// 1 ���߼�����, �ȴ��������б��е��¼�,��ת�Ƶ�����б��� 2 �ٽ�׷�ӵ�����ת�Ƶ���̨Ҫ������б���.
	// 2 ��̨�̼߳�����, �ȴ�ִ���б���,����������ɵ�����,���ƶ�������б���, Ȼ���ٰ�Ҫִ�е�����ӽ�ִ���б�
	TaskList					mMainThreadWaitTaskList;			// ����׷�ӵ�������ʱ�б�,������׷��
	TaskList					mMainThreadMoveToBackList;			// �����ټ��������ʱ�б����ƶ�������б�
	TaskList					mBackRunTaskList;					// ��̨�̼߳�����,�ȴ�����б��м�����ɵ�����, �ӵ���������б���, Ȼ���ٴ�����Ҫ�ƶ����б��м���

	TaskList					mBackThreadWaitFinishedList;		// ��̨�����������ɵ��б�
	TaskList					mMainThreadRunFinishList;			// ����Ҫ�������ɵ�����

	TaskFactoryMap				mTaskFactoryMap;					// ���񹤳�ӳ���, Ϊ��Ч��,��ǰʹ������

	event_handle				mWaitEvent;

	int							mNowTaskCount;
};


//---------------------------------------------------------------------------------


#endif //_INCLUDE_TASKMANAGER_H_