

#include "ShareMemAPI.h"

#if __LINUX__
#include <sys/types.h> 
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <errno.h>
#elif __WINDOWS__
#include <windows.h>
#include "tchar.h"
#include <string>
#endif

#include "Assertx.h"
#include "Common.h"

namespace ShareMemAPI
{


SMHandle		CreateShareMem(SM_KEY key,UINT Size)
{
	__ENTER_FUNCTION

#if __LINUX__
	//key = ftok(keybuf,'w'); 
	SMHandle hd =  shmget(key , Size, IPC_CREAT|IPC_EXCL|0666); 
	NOTE_LOG("handle = %d ,key = %u ,error: %d \r\n",hd,key,errno);
	return hd;
#elif __WINDOWS__
	TCHAR keybuf[64];
	memset( keybuf, 0, sizeof( keybuf ));
#ifdef UNICODE
	_snwprintf( keybuf, sizeof( keybuf ), _T("%u"), key );
#else
	sprintf_s( keybuf, sizeof( keybuf ), _T("%u"), key );
#endif
	if (OpenFileMapping(FILE_MAP_ALL_ACCESS, TRUE, keybuf) != INVALID_SM_HANDLE)
	{
		ERROR_LOG("Already exist share memory %d", key);
		return INVALID_SM_HANDLE;
	}
	//第一个参数是指一个无效文件句柄，因此就不用CreateFile函数来获取句柄
	//这样系统将从系统分页文件中分配空间
	//分配的空间进程间可以共享，可以实现进程间的通信
	// NOTE: Window 创建共享内存后, 只要创建的句柄进程全部结束(包括异常结束), 共享内存将自动释放
	return  CreateFileMapping( (HANDLE)0xFFFFFFFFFFFFFFFF, NULL, PAGE_READWRITE, 0, Size, keybuf);
#endif
	__LEAVE_FUNCTION				
	return INVALID_SM_HANDLE;

}

SMHandle		OpenShareMem(SM_KEY key)
{
	__ENTER_FUNCTION
	//CHAR keybuf[64];
	//memset(keybuf,0,64);
	//sprintf(keybuf,"./%d",key);
#if __LINUX__
	//key = ftok(keybuf,'w'); 
	SMHandle hd =   shmget(key , 0, 0);  // shmget(key , size, 0);
	if (hd==-1)
		ERROR_LOG("handle = %d ,key = %d ,error: %d \r\n", hd, key, errno);
	return hd;
#elif __WINDOWS__
	TCHAR keybuf[64];
	memset( keybuf,0,sizeof( keybuf ) );
#ifdef UNICODE
	swprintf(keybuf,_T("%u"), key);
#else
	sprintf(keybuf,_T("%u"), key);
#endif
	return OpenFileMapping( FILE_MAP_ALL_ACCESS, TRUE, keybuf);
#endif
	__LEAVE_FUNCTION
		return INVALID_SM_HANDLE;
}

CHAR*			MapShareMem(SMHandle handle)
{
	__ENTER_FUNCTION

#if __LINUX__
	return  (CHAR*)shmat(handle,0,0);
#elif __WINDOWS__
	return (CHAR *)MapViewOfFile(handle, FILE_MAP_ALL_ACCESS, 0, 0, 0);
#endif
	__LEAVE_FUNCTION
		return 0;

}


VOID			UnMapShareMem(CHAR* MemoryPtr)
{
	__ENTER_FUNCTION
#if __LINUX__
	  shmdt(MemoryPtr);
#elif __WINDOWS__
	 UnmapViewOfFile(MemoryPtr);
#endif
	__LEAVE_FUNCTION
}


VOID			CloseShareMem(SMHandle handle)
{
	__ENTER_FUNCTION
#if __LINUX__
	shmctl(handle,IPC_RMID,0); 
#elif __WINDOWS__
	CloseHandle(handle);
#endif
	__LEAVE_FUNCTION
}

}