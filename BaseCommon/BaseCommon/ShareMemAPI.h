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
	int					m_RefCont;			// ���������򿪴���, ����Linux���Զ��ر� (WINȫ������رպ�, �Զ�����)
#endif
	SMHead()
	{
		m_Key = 0;
		m_Size = 0;
	}
};
//-------------------------------------------------------------------------
// �����ڴ湦��
// NOTE: 1 Window ���������ڴ��, ֻҪ�����ľ������ȫ������(�����쳣����), �����ڴ潫�Զ��ͷ�
//			2 Window ʹ�� PAGE_READWRITE ���д���, ���, �����ӦKEY�Ĺ����ڴ��Ѿ�����, ��ֱ�Ӵ�
//			3 Linux �� ���δʹ�� CloseShareMem > shmctl �ͷ�, ��ô������ڴ���Ȼ����, ����ʹ�ü��������Զ��ж��ͷ�
//-------------------------------------------------------------------------
namespace	ShareMemAPI
{

	/*����ShareMem �ڴ���
	 *	
	 *	key   ����ShareMem �Ĺؼ�ֵ
	 *
	 *  Size  ������С
	 *
	 *	���� ��ӦShareMem����ֵ
	 NOTE: Window ʹ�� PAGE_READWRITE ���д���, ���, �����ӦKEY�Ĺ����ڴ��Ѿ�����, ��ֱ�Ӵ�
	 */
	SMHandle		CreateShareMem(SM_KEY key,UINT Size);
	/*��ShareMem �ڴ���
	 *	
	 * key   ��ShareMem �Ĺؼ�ֵ
	 * 
	 * Size  �򿪴�С
	 *
	 * ����  ��ӦShareMem ����ֵ
	 */
	SMHandle		OpenShareMem(SM_KEY key);
	
	/*ӳ��ShareMem �ڴ���
	 *	
	 *	handle ӳ��ShareMem �ı���ֵ
	 *
	 *  ���� ShareMem ������ָ��
	 */
	CHAR*			MapShareMem(SMHandle handle);
	
	/*�ر�ӳ�� ShareMem �ڴ���
	 *
	 *	MemoryPtr			ShareMem ������ָ��
	 *	
	 *  
	 */	
	VOID			UnMapShareMem(CHAR* MemoryPtr);
	
	/*	�ر�ShareMem
	 * 	handle  ��Ҫ�رյ�ShareMem ����ֵ
	 */
	VOID			CloseShareMem(SMHandle handle);


}


#endif

