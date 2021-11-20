#ifndef _SHAREMEM_API_H_
#define _SHAREMEM_API_H_

#include "BaseCommon.h"

#ifdef	__WINDOWS__
#define INVALID_SM_HANDLE	 ((VOID*)0)
#elif	 __LINUX__
#define INVALID_SM_HANDLE	 -1
#endif

//-------------------------------------------------------------------------
typedef	unsigned long SM_KEY;

#if defined(__LINUX__)
typedef		int		SMHandle;
#elif defined(__WINDOWS__)
typedef		void*	SMHandle;
#endif
struct SMHead
{
	SM_KEY			m_Key;
	unsigned long	m_Size;
#if __LINE__
	int					m_RefCont;			// 被创建及打开次数, 用于Linux下自动关闭 (WIN全部句柄关闭后, 自动销毁)
#endif
	SMHead()
	{
		m_Key = 0;
		m_Size = 0;
	}
};
//-------------------------------------------------------------------------
// 共享内存功能
// NOTE: 1 Window 创建共享内存后, 只要创建的句柄进程全部结束(包括异常结束), 共享内存将自动释放
//			2 Window 使用 PAGE_READWRITE 进行创建, 因此, 如果对应KEY的共享内存已经存在, 则直接打开
//			3 Linux 下 如果未使用 CloseShareMem > shmctl 释放, 那么分配的内存依然存在, 所以使用记数器来自动判断释放
//-------------------------------------------------------------------------
namespace	ShareMemAPI
{

	/*创建ShareMem 内存区
	 *	
	 *	key   创建ShareMem 的关键值
	 *
	 *  Size  创建大小
	 *
	 *	返回 对应ShareMem保持值
	 NOTE: Window 使用 PAGE_READWRITE 进行创建, 因此, 如果对应KEY的共享内存已经存在, 则直接打开
	 */
	SMHandle		CreateShareMem(SM_KEY key,UINT Size);
	/*打开ShareMem 内存区
	 *	
	 * key   打开ShareMem 的关键值
	 * 
	 * Size  打开大小
	 *
	 * 返回  对应ShareMem 保持值
	 */
	SMHandle		OpenShareMem(SM_KEY key);
	
	/*映射ShareMem 内存区
	 *	
	 *	handle 映射ShareMem 的保持值
	 *
	 *  返回 ShareMem 的数据指针
	 */
	CHAR*			MapShareMem(SMHandle handle);
	
	/*关闭映射 ShareMem 内存区
	 *
	 *	MemoryPtr			ShareMem 的数据指针
	 *	
	 *  
	 */	
	VOID			UnMapShareMem(CHAR* MemoryPtr);
	
	/*	关闭ShareMem
	 * 	handle  需要关闭的ShareMem 保持值
	 */
	VOID			CloseShareMem(SMHandle handle);


}


#endif

