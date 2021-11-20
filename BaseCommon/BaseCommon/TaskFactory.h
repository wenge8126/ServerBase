
#ifndef _INCLUDE_TASKFACTORY_H_
#define _INCLUDE_TASKFACTORY_H_

#include "Hand.h"
#include "Task.h"

class TaskFactory : public Base<TaskFactory>
{
public:
	TaskFactory()
	{
		mSelfPtr = MEM_NEW TastFactoryPtr();
		mSelfPtr->mpFactory = this;
	}
	virtual ~TaskFactory() { OnClearUp(); mSelfPtr->mpFactory = NULL; }

	virtual int GetType(void) = 0;
	virtual const char* GetTypeName(void) = 0;
	virtual HandTask _NewTask() = 0;

public:
	virtual HandTask NewTask(void) 
	{
		if (!mTaskList.empty())
		{
			HandTask t = mTaskList.back();
			mTaskList.pop_back();
			return t;
		}

		HandTask pNew = _NewTask();	
		pNew->mFactoryPtr = mSelfPtr;
		return pNew;
	}

public:
	virtual void OnTaskEnd(Task *hTask) 
	{
		hTask->InitFree();
		mTaskList.push_back(hTask);
	}

public:
	virtual void OnClearUp(void)
	{
		for (int i=0; i<mTaskList.size(); ++i)
		{
			//mTaskList[i]._free();
			delete mTaskList[i];
		}
		mTaskList.clear();
	}

protected:
	Array<Task*>		mTaskList;
	ATastFactory			mSelfPtr;
};

typedef Hand<TaskFactory>	AutoTaskFactory;


template<typename T, int type>
class DefineTaskFactory : public TaskFactory
{
public:
	virtual ~DefineTaskFactory(){}

	virtual int GetType(void) { return type; }
	virtual const char* GetTypeName(void) { return "DEFAULT"; }

public:
	virtual HandTask _NewTask(void) 
	{	
		return MEM_NEW T();	
	}
};



#endif //_INCLUDE_TASKFACTORY_H_