#ifndef _SHARE_MEM_ACCESS_OBJECT_H_
#define _SHARE_MEM_ACCESS_OBJECT_H_

#include "BaseCommon.h"
#include "ShareMemAPI.h"
#include "MemBase.h"
//-------------------------------------------------------------------------

#define		SHMEM_LOG_PATH	"./Log/ShareMemory.log"

//-------------------------------------------------------------------------
/*
 *		�����ڴ���ʶ���
 *		ShareMemory	Access	Object
 *		NOTE: Window ���������ڴ��, ֻҪ�����ľ������ȫ������(�����쳣����), �����ڴ潫�Զ��ͷ�
 *		Linux �� ���δʹ�� CloseShareMem > shmctl �ͷ�, ��ô������ڴ���Ȼ����, ����ʹ�ü��������Զ��ж��ͷ�
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
	 *	����ShareMem ���ʶ���(�´���)
	 *
	 *  SM_KEY	key		���ʼ�ֵ
	 *
	 *	UINT		Size	�����������ֽڸ���
	 *
	 */
	BOOL	Create(SM_KEY key,UINT Size);
	/*
	 *	���ٶ���
	 */
	VOID	Destory();

    // Linux ֱ��ɾ���ͷŹ����ڴ�, ��ʹ������������ʹ��, �������������쳣�رպ�, ����������Ϊ0ʱ, ���г�������
    VOID    Delete();

	/*
	 *		����ShareMem ���ʶ���(�����´���)
	 *		SM_KEY	key		���ʼ�ֵ
	 *
	 *		UINT		Size	�����������ֽڸ���
	 *		
	 */
	BOOL	Attach(SM_KEY, UINT &Size);

	/*
	 *	 ���������ָ��
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
	 *	����������ܴ�С
	 */
	UINT	GetSize()
	{
		return m_Size;
	}

	BOOL	DumpToFile(CHAR* FilePath);
	BOOL	MergeFromFile(CHAR* FilePath);


private:
	//ShareMemory	�ڴ��С
	mutable UINT				m_Size;
	// ShareMemory	�ڴ�ͷָ��
	mutable CHAR*				m_pHeader;
	// ShareMemory	���	
	mutable SMHandle			m_hold;
	
};	





#endif