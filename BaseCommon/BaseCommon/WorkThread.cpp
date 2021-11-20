
#include "WorkThread.h"
#include "Common.h"

#if __WINDOWS__
#include <process.h>
#include <Windows.h>
#include <exception>
#include "Dump.h"

WorkThread::WorkThread()
{
	mClose = false;
	mBackTread = NULL;
	mThreadID = 0;
	mDebugName = "Defaul";
}

UINT WINAPI RunWorkThread( LPVOID lpParameter )
{
	try{
		_set_se_translator(throw_exception);
		((WorkThread*)lpParameter)->backWorkThread();
	}
	catch (std::exception &e)
	{
		ERROR_LOG("[%u]�߳��쳣> [%s]", ((WorkThread*)lpParameter)->GetThreadID(), e.what());
		throw e;
		//((WorkThread*)lpParameter)->onExceptionExist();
	}
	//catch (...)
	//{
	//	ERROR_LOG("�߳��쳣�˳� >[%u]", ((WorkThread*)lpParameter)->GetThreadID());
	//	((WorkThread*)lpParameter)->onExceptionExist();
	//}
	return 0;
}

void WorkThread::InitThread( void )
{
	if (mBackTread != NULL)
	{
		ERROR_LOG(" xxxxxxxxxxx [%s :%u] Now thread already init start, please try InitThread after closed xxxxxxxxxx", mDebugName.c_str(), mThreadID);
		return;
	}
	mClose = false;	
	mBackTread = _beginthreadex( NULL, 0, RunWorkThread, this, 0, &mThreadID );
	//GREEN_LOG("[%s] �����߳� >[%u]", mDebugName.c_str(), mThreadID);
}

void WorkThread::Close( void )
{
	mClose = true;
	if ( mBackTread )
	{
		onBeforeClose();
		if (WaitForSingleObject( (HANDLE)mBackTread, 2000 )!=WAIT_OBJECT_0)
		{
			//ERROR_LOG("[%s]�����ر��߳�ʧ��>[%u] (�رճ�ʱ6��), ��ǰǿ�ƹر�", mDebugName.c_str(), mThreadID);
			_ForceClose(10);
		}
		CloseHandle((HANDLE)mBackTread);
		mBackTread = NULL;
		mThreadID = 0;
	}
}


void WorkThread::Suspend()
{
#if __WINDOWS__
	::SuspendThread((HANDLE)mBackTread);
#else
	AssertNote(0, "δʵ��");
#endif
}


bool WorkThread::Resume()
{
#if __WINDOWS__
	while (true)
	{
		UINT re = ::ResumeThread((HANDLE)mBackTread);
		if (re==0xFFFFFFFF)
		{
			ERROR_LOG("�ָ̻߳�ʧ��");
			return false;
		}
		else if (re<=0)
			return true;
	}
#else
	AssertNote(0, "δʵ��");
#endif
	return false;
}

bool WorkThread::IsThreadQuit()
{
	return mBackTread==NULL;
}

bool WorkThread::_ForceClose( size_t reVal )
{
	if ( mBackTread)
    {
		TerminateThread( (HANDLE)mBackTread, reVal );
        CloseHandle((HANDLE)mBackTread);
		mBackTread = NULL;
    }
    return true;
}
void WorkThread::WaitTheadFinish()
{
	Close();
}


#else

#	if __LINUX__

#include <pthread.h>

WorkThread::WorkThread()
	: mThreadID(0)
{
	mClose = false;
}

VOID* RunWorkThread( VOID* lpParameter )
{
	// ʹ��unjoin, �˳����ͷ� https://www.cnblogs.com/huty/p/8518633.html
	((WorkThread*)lpParameter)->backWorkThread();
	return 0;
}

//void WorkThread::threadProcess()
//{
//	backWorkThread();  
//	pthread_detach(pthread_self());  
//	mThreadID = 0;
//}

void WorkThread::InitThread( void )
{
    if (mThreadID !=0)
	{
		ERROR_LOG(" xxxxxxxxxxx [%s :%llu] Now thread already init start, please try InitThread after closed xxxxxxxxxx", mDebugName.c_str(), mThreadID);
		return;
	}
    
	mClose = false;

	pthread_create( (pthread_t*)&mThreadID, NULL , RunWorkThread , (VOID*)this );
	//GREEN_LOG("[%s] �����߳� >[%u]", mDebugName.c_str(), mThreadID);
}

void WorkThread::Close( void )
{
	mClose = true;
    
    _ForceClose(NULL);
    //if (mThreadID!=0)
    //    pthread_kill((pthread_t)mThreadID, SIGKILL);
    //mThreadID = 0;	
}

bool WorkThread::IsThreadQuit()
{
	return (mThreadID == 0);
}


bool WorkThread::_ForceClose(size_t reVal)
{
    if (mThreadID!=0)
    {
        //mClose = true;
        //pthread_cancel((pthread_t)mThreadID);
		pthread_cancel((pthread_t)mThreadID);
		void *p = NULL;
		pthread_join((pthread_t)mThreadID, &p);
        //pthread_kill((pthread_t)mThreadID, SIGKILL);
        mThreadID = 0;
    }    
    return mThreadID==0;
}

void WorkThread::WaitTheadFinish()
{
	Close();
}

void WorkThread::Suspend()
{

	AssertNote(0, "δʵ��");

}


bool WorkThread::Resume()
{

	AssertNote(0, "δʵ��");

	return false;
}

#	endif

#endif