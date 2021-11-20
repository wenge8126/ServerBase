
#include "Lock.h"

#ifdef	__LINUX__
#	include <pthread.h>
#else
#ifndef WINVER				// ����ʹ���ض��� Windows XP ����߰汾�Ĺ��ܡ�
#define WINVER 0x0501		// ����ֵ����Ϊ��Ӧ��ֵ���������� Windows �������汾��
#endif

#ifndef _WIN32_WINNT		// ����ʹ���ض��� Windows XP ����߰汾�Ĺ��ܡ�
#define _WIN32_WINNT 0x0501	// ����ֵ����Ϊ��Ӧ��ֵ���������� Windows �������汾��
#endif						

#ifndef _WIN32_WINDOWS		// ����ʹ���ض��� Windows 98 ����߰汾�Ĺ��ܡ�
#define _WIN32_WINDOWS 0x0410 // ����ֵ����Ϊ�ʵ���ֵ����ָ���� Windows Me ����߰汾��ΪĿ�ꡣ
#endif

#ifndef _WIN32_IE			// ����ʹ���ض��� IE 6.0 ����߰汾�Ĺ��ܡ�
#define _WIN32_IE 0x0600	// ����ֵ����Ϊ��Ӧ��ֵ���������� IE �������汾��
#endif
#	include "windows.h"

#endif

#include <exception>
#include <stdio.h>

//#include "AutoKey.h"
#include "Common.h"

//#include "TimeManager.h"

//-------------------------------------------------------------------------
#ifdef	__LINUX__

CsLock::CsLock()
{
	mLock = new pthread_mutex_t;
    pthread_mutex_init( (pthread_mutex_t*)mLock , NULL );
}

CsLock::~CsLock()
{
	pthread_mutex_destroy( (pthread_mutex_t*)mLock );
	delete ((pthread_mutex_t*)mLock);
}

void CsLock::lock()
{
	pthread_mutex_lock((pthread_mutex_t*)mLock );
}

void CsLock::unlock()
{
	pthread_mutex_unlock((pthread_mutex_t*) mLock );
}

bool CsLock::trylock()
{
	return pthread_mutex_trylock((pthread_mutex_t*)mLock )==0;	
}

//-------------------------------------------------------------------------
//Linux MtLock use System V, ʹ���ź���ʵ����
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/stat.h>


MtLock::MtLock(const CHAR* name)
{
	union semun
	{
		int val;
		struct semid_ds *buf;
		unsigned short *array;
	};

	union semun arg;
	int pid;
	arg.val = 1;
	semid = semget(MAKE_INDEX_ID(name), 1, IPC_CREAT | S_IRUSR | S_IWUSR); //�ź�������Ϊ1
	semctl(semid, 0, SETVAL, arg); //�����ź���0��ֵΪ1
}


MtLock::~MtLock()
{
	union semun
	{
		int val;
		struct semid_ds *buf;
		unsigned short *array;
	};
	union semun sem_union;
	if (semctl(semid, 0, IPC_RMID, sem_union) == -1) {
		perror("Sem delete");
	}
}


void MtLock::lock()
{
	struct sembuf cut_sop;
	cut_sop.sem_num = 0;
	cut_sop.sem_op = -1; //v����
	cut_sop.sem_flg = SEM_UNDO;

	semop(semid, &cut_sop, 1); // �ź���p����
}


void MtLock::unlock()
{
	struct sembuf add_sop;
	add_sop.sem_num = 0;
	add_sop.sem_op = 1; //p����
	add_sop.sem_flg = SEM_UNDO;

	semop(semid, &add_sop, 1); // �ź���v����
}



//-------------------------------------------------------------------------
#else
CsLock::CsLock()
{
	mLock = new CRITICAL_SECTION; InitializeCriticalSection( (CRITICAL_SECTION*)mLock );
}

CsLock::CsLock(const CsLock &other)
{
	mLock = new CRITICAL_SECTION; InitializeCriticalSection( (CRITICAL_SECTION*)mLock );
}

CsLock::~CsLock()
{
	DeleteCriticalSection( (CRITICAL_SECTION*)mLock ); 
	delete ((CRITICAL_SECTION*)mLock);
}

void CsLock::lock()
{
	EnterCriticalSection((CRITICAL_SECTION*)mLock );
}

void CsLock::unlock()
{
	LeaveCriticalSection((CRITICAL_SECTION*) mLock );
}

bool CsLock::trylock()
{
	return TryEnterCriticalSection((CRITICAL_SECTION*)mLock )==TRUE;
	return false;
}


//-------------------------------------------------------------------------
MtLock::MtLock(const CHAR*	name)
{
#ifdef __WINDOWS__
#ifdef _UNICODE
	AString temp = name;
	mLock = (void*)::CreateMutex(NULL, FALSE, temp.getWString().getPtr());
#else
	mLock = (void*)::CreateMutex(NULL, FALSE, name);
#endif
	if (mLock==0)
	{
		char szInfo[60];
		_snprintf_s(szInfo, 60, "Error code: %u", ::GetLastError());
		throw std::exception(szInfo);
	}
#endif
}

MtLock::MtLock()
{
	mLock = (void*)::CreateMutex(NULL, FALSE, NULL);
	if (mLock==0)
	{
		char szInfo[60];
		_snprintf_s(szInfo, 60, "Error code: %u", ::GetLastError());
		throw std::exception(szInfo);
	}
}

MtLock::MtLock(const MtLock &other)
{
	AssertNote(0, "�����߳�����֧�ָ�ֵ����");
}

MtLock::~MtLock()
{
	::ReleaseMutex((HANDLE)mLock);
	::CloseHandle((HANDLE)mLock);
}
void MtLock::lock()
{
	DWORD re = ::WaitForSingleObject((HANDLE)mLock, INFINITE);
    if (re==WAIT_FAILED)
    {
		throw std::exception("Wait for mutex error.");            
    }
	else if (re==WAIT_TIMEOUT)
	{
		throw std::exception("Wait for mutex time over."); 
	}
	//TableTool::Log("succss lock");
}
void MtLock::unlock()
{
	if (::ReleaseMutex((HANDLE)mLock)==0)
	{		
		ERROR_LOG("MtLock > ReleaseMutex error code: %u\n", ::GetLastError());
		//throw std::exception(szInfo);
	}
	//TableTool::Log("succss unlock");
}
bool MtLock::trylock()
{
	DWORD re = ::WaitForSingleObject((HANDLE)mLock, 0);
    if (re==WAIT_FAILED)
    {
		throw std::exception("Wait for mutex error.");            
    }
	else if (re==WAIT_TIMEOUT)
	{
		//throw std::exception("Wait for mutex time over."); 
	}
	else if (re==WAIT_OBJECT_0)
	{
		return true;
	}
	return false;
}

#endif
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

