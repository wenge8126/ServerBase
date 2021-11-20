#pragma once

#ifndef _INCLUDE_TASKSYSTEM_H_
#define _INCLUDE_TASKSYSTEM_H_

#include "TaskManager.h"
#include "Hand.h"
//-------------------------------------------------------------------------
// 用于处理密集计算逻辑的任务条件, 避免主线程阻塞
//-------------------------------------------------------------------------
class BaseCommon_Export_H TaskSystem : public Base<TaskSystem>
{
public:
	void RegisterTask(AutoTaskFactory hTaskFactory)
	{
		for (int i = 0; i < mTaskMgrList.size(); ++i)
		{
			mTaskMgrList[i].RegisterTask(hTaskFactory);
		}
	}

	HandTask	 StartTask(int taskType)
	{
		int x = CRand::Rand(mTaskMgrList.size() - 1);
		return mTaskMgrList[x].StartTask(taskType);
	}

	void Close()
	{
		for (int i = 0; i < mTaskMgrList.size(); ++i)
		{
			mTaskMgrList[i].Close();
		}
		mTaskMgrList.clear();
	}

	void Process()
	{
		for (int i = 0; i < mTaskMgrList.size(); ++i)
		{
			mTaskMgrList[i].Process();
		}
	}

public:
	TaskSystem(int initTaskThreadNum)		
	{
		mTaskMgrList.resize(initTaskThreadNum);

		for (int i=0; i<initTaskThreadNum; ++i)
		{
			mTaskMgrList[i].InitThread();
		}
	}

	~TaskSystem()
	{
		Close();
	}

	virtual void OnForceFreeBefore() override
	{
		Close();
	}

protected:
	Array<TaskManager>			mTaskMgrList;
};



#endif //_INCLUDE_TASKSYSTEM_H_
