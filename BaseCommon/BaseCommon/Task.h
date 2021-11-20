
#ifndef _INCLUDE_TASK_H_
#define _INCLUDE_TASK_H_

#include "Auto.h"
#include "EasyStack.h"
#include "CoroutineTool.h"

#define  DEBUG_FOR_TASK		0

class TaskManager;
class TaskFactory;

class TastFactoryPtr : public AutoBase
{
public:
	TaskFactory *mpFactory = NULL;
};
typedef Auto< TastFactoryPtr> ATastFactory;


class BaseCommon_Export Task : public AutoBase
{
	friend class TaskManager;
	friend class TaskFactory;

public:
	virtual void InitFree() = 0;
	// back thread run
	virtual void Execute(TaskManager *pTaskMgr) = 0;

	// main thread run
	virtual void OnFinished(TaskManager *pTaskMgr) = 0;

	virtual int GetType();
	virtual const char* GetTypeName();

	// 异步等待完成, 必须在协程函数内. 开启任务后, 设置任务状态, 调用些函数, 异步等待完成
	bool AWait()
	{
		mCoreCode = CORO;
		if (mCoreCode == 0)
		{
			ERROR_LOG("Must await in coroutine");
			return false;
		}
		CoroutineTool::yield();
		return true;
	}

	void resume()
	{
		if (mCoreCode != 0)
			CoroutineTool::Resume(mCoreCode);
	}

public:
	Task()
		: mCoreCode(0)
	{	}

	Auto<Task> GetSelf(){ return Auto<Task>(this); }

	virtual void Release() override;

protected:
#if DEBUG_FOR_TASK
	size_t		mCode;
#endif
	ATastFactory	 mFactoryPtr;

	unsigned int mCoreCode;

};

typedef Auto<Task>		HandTask;


#endif //_INCLUDE_TASK_H_