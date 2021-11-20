
#include "ShareMemAO.h"

#include "ShareMemAPI.h"
#include "Assertx.h"
#include <string>

//using namespace ShareMemAPI;

namespace Log
{
	void SaveLog(const char* , const char* szInfo, ...){}
}


ShareMemAO::ShareMemAO()
{
	m_hold = INVALID_SM_HANDLE;
	m_Size = 0;
	m_pHeader = 0;
}

BOOL	ShareMemAO::Create(SM_KEY key, UINT Size)
{
	__ENTER_FUNCTION
		Destory();
	
		m_hold	= ShareMemAPI::CreateShareMem(key, Size+ sizeof(SMHead));
		if(m_hold == INVALID_SM_HANDLE)
		{
			Log::SaveLog(SHMEM_LOG_PATH,"Create ShareMem Error SM_KET = %d",key);
			return FALSE;
		}
		m_pHeader = ShareMemAPI::MapShareMem(m_hold);

		if(m_pHeader)
		{	
			SMHead *pHeader= (SMHead*)m_pHeader;
			pHeader->m_Key	 =	key;
			pHeader->m_Size =	Size;
#if __LINUX__
			//Linux 下 如果未使用 CloseShareMem > shmctl 释放, 那么分配的内存依然存在, 所以使用记数器来自动判断释放
			((SMHead*)m_pHeader)->m_RefCont = 1;
#endif
			m_Size	=	Size;
			Log::SaveLog(SHMEM_LOG_PATH,"Create ShareMem Ok SM_KET = %d",key);
			return TRUE;
		}
		else
		{
			Log::SaveLog(SHMEM_LOG_PATH,"Map ShareMem Error SM_KET = %d",key);
			return FALSE;
		}
		
	
	
	__LEAVE_FUNCTION
		return FALSE ;
}

BOOL	ShareMemAO::Attach(SM_KEY key, UINT	&Size)
{
	__ENTER_FUNCTION
		Destory();
	m_hold	=	ShareMemAPI::OpenShareMem(key);

	if(m_hold == INVALID_SM_HANDLE)
	{
		Log::SaveLog(SHMEM_LOG_PATH,"Attach ShareMem Error SM_KET = %d",key);
		return FALSE;
	}
	
	m_pHeader = ShareMemAPI::MapShareMem(m_hold);

	if(m_pHeader)
	{
		Assert(((SMHead*)m_pHeader)->m_Key	 ==	key);
		//Assert(((SMHead*)m_pHeader)->m_Size  ==	Size);
		Size = ((SMHead*)m_pHeader)->m_Size;
#if __LINUX__
		(((SMHead*)m_pHeader)->m_RefCont)++;
#endif
		m_Size	=	Size;
		Log::SaveLog(SHMEM_LOG_PATH,"Attach ShareMem OK SM_KET = %d",key);
		return TRUE;
	}
	else
	{
		Log::SaveLog(SHMEM_LOG_PATH,"Map ShareMem Error SM_KET = %d",key);
		return FALSE;
	}

	__LEAVE_FUNCTION

		return FALSE;

}

VOID	ShareMemAO::Destory()
{
	__ENTER_FUNCTION
#if __LINUX__
		int nowRefCount = 0;
	if (m_pHeader != NULL)
		nowRefCount = --(((SMHead*)m_pHeader)->m_RefCont);
#endif
	if (m_pHeader)
	{
		ShareMemAPI::UnMapShareMem(m_pHeader);
		m_pHeader = 0;
	}
	if (m_hold != INVALID_SM_HANDLE)
	{
#if __LINUX__
		if (nowRefCount <= 0)
			ShareMemAPI::CloseShareMem(m_hold);
#else
		ShareMemAPI::CloseShareMem(m_hold);
#endif
		m_hold = INVALID_SM_HANDLE;
	}

	m_Size = 0;

	__LEAVE_FUNCTION
}

VOID ShareMemAO::Delete()
{
    __ENTER_FUNCTION

        if (m_pHeader)
        {
            ShareMemAPI::UnMapShareMem(m_pHeader);
            m_pHeader = 0;
        }
    if (m_hold != INVALID_SM_HANDLE)
    {
        ShareMemAPI::CloseShareMem(m_hold);
        m_hold = INVALID_SM_HANDLE;
    }

    m_Size = 0;

    __LEAVE_FUNCTION
}

BOOL	ShareMemAO::DumpToFile(CHAR* FilePath)
{
	__ENTER_FUNCTION	

		Assert(FilePath);
		
		FILE* f	= fopen(FilePath,"wb");
		if(!f)	
			return FALSE;
		fwrite(m_pHeader,1,m_Size,f);
		fclose(f);
		
		return TRUE;

	__LEAVE_FUNCTION

		return FALSE;
}

BOOL ShareMemAO::MergeFromFile(CHAR* FilePath)
{
	__ENTER_FUNCTION

		Assert(FilePath);
		
		FILE*	f = fopen(FilePath,"rb");
		if(!f)
			return FALSE;
		fseek(f,0L,SEEK_END);
		INT FileLength =ftell(f);
		fseek(f,0L,SEEK_SET);
		fread(m_pHeader,FileLength,1,f);
		fclose(f);

		return TRUE;

	__LEAVE_FUNCTION

		return FALSE;
}
