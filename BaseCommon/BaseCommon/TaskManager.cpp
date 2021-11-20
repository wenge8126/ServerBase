
#include "TaskManager.h"
#include "Common.h"

#include <stdarg.h>

#include "TimeManager.h"

//-------------------------------------------------------------------------
void Task::Release()
{
	if (mFactoryPtr->mpFactory != NULL)
		mFactoryPtr->mpFactory->OnTaskEnd(this);
	else
		delete this;
}

int Task::GetType()
{
	if (mFactoryPtr->mpFactory != NULL)
		return mFactoryPtr->mpFactory->GetType();
	return 0;
}

const char* Task::GetTypeName()
{
	if (mFactoryPtr->mpFactory != NULL)
		return mFactoryPtr->mpFactory->GetTypeName();
	return "Unkwon";
}

//-------------------------------------------------------------------------
//------------------------------------------------------------------------------
TaskManager::TaskManager() 
	: mHaveWillTask(false)
	, m_bHaveFinish(false)
	, m_bBackListEmpty(true)
	, mNowTaskCount(0)
{
	mWaitEvent = event_create(false, true);
	mTaskFactoryMap.resize(INIT_TASTFACTORY_COUNT);
//#if __WINDOWS__
//	mWaitEvent = (int)::CreateEvent(NULL, FALSE, FALSE, NULL);
//#endif
	//start();
}
//------------------------------------------------------------------------------
TaskManager::~TaskManager()
{
	Close();
	event_destroy(mWaitEvent);
	
//#if __WINDOWS__
//	::CloseHandle((HANDLE)mWaitEvent);
//	mWaitEvent = 0;
//#endif
}


//------------------------------------------------------------------------------
void TaskManager::Close(void)
{
	mClose = true;
	event_set(mWaitEvent);
	//stop();		
	WorkThread::Close();

	Lock();
	mMainThreadWaitTaskList.clear();
	mBackRunTaskList.clear();
	mBackThreadWaitFinishedList.clear();
	mMainThreadRunFinishList.clear();
	mMainThreadMoveToBackList.clear();
	for (size_t i = 0; i<mTaskFactoryMap.size(); ++i)
	{
		if (mTaskFactoryMap[i])
		{
			mTaskFactoryMap[i]->OnClearUp();
			mTaskFactoryMap[i]._free();		
		}
	}
	mTaskFactoryMap.clear();
	UnLock();

//#if __WINDOWS__
//	::SetEvent((HANDLE)mWaitEvent);
//#endif


}
//------------------------------------------------------------------------------
void TaskManager::RegisterTask( AutoTaskFactory hTaskFactory )
{
	int taskType = hTaskFactory->GetType();
	if 	(taskType<(int)mTaskFactoryMap.size())
	{
		if (mTaskFactoryMap[taskType])
		{
			Log("Warn: already exist task factory>>>[%d], now replace", taskType);
		}
	}
	else
	{
		size_t count = mTaskFactoryMap.size() * 2;
		while (taskType>=(int)count)
		{
			count *= 2;
		}
		mTaskFactoryMap.resize(count);			
	}
	mTaskFactoryMap[taskType] = hTaskFactory;
	Log("Info: Succeed register task [%d]", taskType);
}
//------------------------------------------------------------------------------

// Note: run in main thread. ����һ���µ�����, ͬʱ����뵽����Ҫִ���б���.
HandTask TaskManager::StartTask(int taskType)
{
	AutoTaskFactory hF = GetTaskFactory(taskType);
	if (hF)
	{
		HandTask  hTask = hF->NewTask();
#if	DEBUG_FOR_TASK
		static __declspec(thread) size_t code = 0;
		hTask->mCode = ++code;
#endif
		// ע��, ����������б����ǰ��, ����ת�Ƶ���̨�б�ʱ, 
		// �ִ���ǰ�濪ʼ��ȡ, Ȼ���ټӵ���ִ̨�е��б����ǰ��, ���Ժ����������,���ǻᱻ����ִ���б�������
		mMainThreadWaitTaskList.push(hTask);

		mHaveWillTask = true;

		++mNowTaskCount;

		return hTask;
	}
	return HandTask();
}
//------------------------------------------------------------------------------

// Note: run in main thread
AutoTaskFactory TaskManager::GetTaskFactory(int taskType)
{
	if (taskType>=0 && taskType<(int)mTaskFactoryMap.size())
		return mTaskFactoryMap[taskType];

	return AutoTaskFactory();
}
//------------------------------------------------------------------------------

// main or back
void TaskManager::Log(const char *szInfo, ... )
{
#if DEVELOP_MODE
	va_list va;
	va_start( va, szInfo);
	TableTool::DebugLog(va, szInfo);
#endif
}
//------------------------------------------------------------------------------


/// Note: run in main thread
void TaskManager::Process(void)
{
	bool bHave = false;
	if (mHaveWillTask && m_bBackListEmpty)
	{
		/// ����ת������ 1 �ȴ�����->ת�Ƶ���̨�б�, 2 ��̨�ȴ���ɵ��б�->����ִ����������б�		
		Lock();
		// step 2 ����������б��е�����[��>��]ת�Ƶ�ִ������б���
		mMainThreadWaitTaskList.swap(mMainThreadMoveToBackList);
		mHaveWillTask = false;
		m_bBackListEmpty = false;
		UnLock();
				
		event_set(mWaitEvent);
	}
	// step 3 �ȴ��������б��е��¼�

	if (m_bHaveFinish)
	{
		Lock();
		mMainThreadRunFinishList.swap(mBackThreadWaitFinishedList);
		m_bHaveFinish = false;
		UnLock();

		size_t count = mMainThreadRunFinishList.size();
		auto &list = mMainThreadRunFinishList._getList();
		for (size_t i=0; i<count; ++i)
		{
			HandTask hTask = list[i];
#if DEBUG_FOR_TASK
			Log(">>>>[%u]��ʼִ���������#########", hTask->mCode);
#endif
			--mNowTaskCount;
			hTask->resume();
			hTask->OnFinished(this);
		}

		mMainThreadRunFinishList.clear();
	}
}

//------------------------------------------------------------------------------


/// run in back thread.
VOID TaskManager::backWorkThread()
{
	while (IsActive())
	{				
		backProcess();
		//#if __WINDOWS__
		//		if (mBackRunTaskList.empty())
		//			WaitForSingleObject((HANDLE)mWaitEvent, INFINITE);
		//#endif		
		// step 3 ��ִ̨������
		bool bHave = false;
		Lock();
		if (!mMainThreadMoveToBackList.empty())
		{
			mBackRunTaskList.swap(mMainThreadMoveToBackList);
			bHave = true;
			m_bBackListEmpty = true;
		}
		UnLock();

		if (bHave)						
		{			
			size_t count = mBackRunTaskList.size();
			auto &list = mBackRunTaskList._getList();
			for (size_t i=0; i<count; ++i)
			{
				HandTask &hTask = list[i];
				if (!hTask)
				{
					continue;
				}
#if DEBUG_FOR_TASK
				Log(">>>>[%u]��ִ̨������############", hTask->mCode);
#endif
				try{
					hTask->Execute(this);		
				}
				catch (...)
				{
					ERROR_LOG("����ִ���쳣 >[%d][%s]", hTask->GetType(), hTask->GetTypeName());
				}
				Lock();
				mBackThreadWaitFinishedList.push(hTask);
				hTask.setNull();
				m_bHaveFinish = true;
				UnLock();
			}	
			mBackRunTaskList.clear();
		}
		else
		{
			//TimeManager::Sleep(1);
			// ��ͣ6��, ���µ������,��������				
			event_timedwait(mWaitEvent, 6000);
		}
	}
}


void TaskManager::onBeforeClose(void)
{
	event_set(mWaitEvent);
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
