#ifndef _SHARE_MEM_ACCESS_OBJECT_H_
#define _SHARE_MEM_ACCESS_OBJECT_H_

#include "BaseCommon.h"
#include "ShareMemAPI.h"
#include "MemBase.h"
//-------------------------------------------------------------------------

#define		SHMEM_LOG_PATH	"./Log/ShareMemory.log"

//-------------------------------------------------------------------------
/*
 *		共享内存访问对象
 *		ShareMemory	Access	Object
 *		NOTE: Window 创建共享内存后, 只要创建的句柄进程全部结束(包括异常结束), 共享内存将自动释放
 *		Linux 下 如果未使用 CloseShareMem > shmctl 释放, 那么分配的内存依然存在, 所以使用记数器来自动判断释放
 */
//-------------------------------------------------------------------------
class BaseCommon_Export ShareMemAO : public MemBase
{
public:
	ShareMemAO();
	~ShareMemAO() {  Destory();  };

	ShareMemAO(const ShareMemAO &other)
		: m_hold( INVALID_SM_HANDLE)
		, m_pHeader(NULL)
		, m_Size(0)
	{
		*this = other;
	}

	ShareMemAO& operator = (const ShareMemAO &other)
	{
		Destory();
		this->m_hold = other.m_hold;
		this->m_pHeader = other.m_pHeader;
		this->m_Size = other.m_Size;

		other.m_hold = INVALID_SM_HANDLE;
		other.m_pHeader = 0;
		other.m_Size = 0;

		return *this;
	}

	/*
	 *	创建ShareMem 访问对象(新创建)
	 *
	 *  SM_KEY	key		访问键值
	 *
	 *	UINT		Size	访问数据区字节个数
	 *
	 */
	BOOL	Create(SM_KEY key,UINT Size);
	/*
	 *	销毁对象
	 */
	VOID	Destory();

    // Linux 直接删除释放共享内存, 即使其他进程正在使用, 用于其他进程异常关闭后, 计数器不会为0时, 进行彻底销毁
    VOID    Delete();

	/*
	 *		附着ShareMem 访问对象(不是新创建)
	 *		SM_KEY	key		访问键值
	 *
	 *		UINT		Size	访问数据区字节个数
	 *		
	 */
	BOOL	Attach(SM_KEY, UINT &Size);

	/*
	 *	 获得数据区指针
	 */
	CHAR*	GetDataPtr()
	{
		if (m_pHeader!=NULL)
			return m_pHeader + sizeof(SMHead);
		return NULL;
	}

	SM_KEY GetKey()
	{
		if (m_pHeader != NULL)
			return ((SMHead*)m_pHeader)->m_Key;
		return 0;
	}
	/*
	 *	获得数据区总大小
	 */
	UINT	GetSize()
	{
		return m_Size;
	}

	BOOL	DumpToFile(CHAR* FilePath);
	BOOL	MergeFromFile(CHAR* FilePath);


private:
	//ShareMemory	内存大小
	mutable UINT				m_Size;
	// ShareMemory	内存头指针
	mutable CHAR*				m_pHeader;
	// ShareMemory	句柄	
	mutable SMHandle			m_hold;
	
};	





#endif