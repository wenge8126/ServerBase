/*/-------------------------------------------------------------------------/
�ٽ����߳���
����:	ʵ��1ǧ��μ�������� �� 1ǧ��οջ�ӷ�������ʱ���� 172 / 16, 
		ԼΪ1���ӷ������10��, �����Ҫ10��CPUָ��
//-------------------------------------------------------------------------*/
#ifndef _INCLUDE_CSLOCK_H_
#define _INCLUDE_CSLOCK_H_

#include "BaseCommon.h"


//----------------------------------------------------------------------------

class  BaseCommon_Export CsLock
{
protected:
	void  *mLock;

public:
	friend struct Lock;

	CsLock();
	~CsLock();

	void lock();
	void unlock();
	bool trylock();

	CsLock(const CsLock &other);
	CsLock& operator = (const CsLock &other)
	{
		return *this;
	}
};
//----------------------------------------------------------------------------
struct CsLockTool
{
	CsLock   *cs;
public:
	CsLockTool( CsLock  &lock )
	{
		cs = &lock; 
		cs->lock();
	}

	~CsLockTool()
	{
		cs->unlock();
	}
};
//----------------------------------------------------------------------------
struct CsTryLock
{
	CsLock   *cs;
	bool	bLock;
public:
	CsTryLock( CsLock  &lock )
	{
		cs = &lock; 
		bLock = cs->trylock();
	}
	
	~CsTryLock()
	{
		if (bLock)
			cs->unlock();
	}

	bool locked(){ return bLock; }

	operator bool () { return bLock; }
};
//----------------------------------------------------------------------------
#if __WINDOWS__
class BaseCommon_Export MtLock
{
protected:
	void  *mLock;

public:
	friend struct LockTool;
	MtLock(const CHAR* name);
	MtLock();
	~MtLock();
	void lock();
	void unlock();
	bool trylock();

	MtLock(const MtLock &other);
	MtLock& operator = (const MtLock &other)
	{
		return *this;
	}
};
//----------------------------------------------------------------------------

struct MtLockTool
{
	MtLock   *cs;
public:
	MtLockTool( MtLock  &lock )
	{
		cs = &lock; 
		cs->lock();
	}

	~MtLockTool()
	{
		cs->unlock();
	}
};


//----------------------------------------------------------------------------
struct MtTryLock
{
	MtLock   *cs;
	bool	bLock;
public:
	MtTryLock( MtLock  &lock )
	{
		cs = &lock; 
		bLock = cs->trylock();
	}
	
	~MtTryLock()
	{
		if (bLock)
			cs->unlock();
	}

	bool locked(){ return bLock; }

	operator bool () { return bLock; }
};
//----------------------------------------------------------------------------
#elif __LINUX__
class  MtLock
{
	int semid = 0;

public:
	MtLock(const CHAR* name);
	~MtLock();

	void lock();

	void unlock();

	bool trylock()
	{
		lock();
		return true;
	}
};
#endif //__WINDOWS__


#endif