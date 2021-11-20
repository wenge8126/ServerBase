/********************************************************************
	created:	2010/09/06
	created:	6:9:2010   11:13
	filename: 	f:\NewTest\common\Auto.h
	file path:	f:\NewTest\common
	file base:	AutoPtr
	file ext:	h
	author:		���ĸ�
	
	purpose:	�����ͨ�����ܶ���
				�ص���ֻ���ĸ��ֽڿռ�, ��ͬ��һ��������, ��ֵЧ�ʸߵ�
*********************************************************************/

#ifndef _AUTOPTR_H_
#define _AUTOPTR_H_

#include  "Assertx.h"
#include <typeinfo>
#include "MemBase.h"
#include "BaseCommon.h"
//-----------------------------------------------------------------------------------------

#define USE_DEBUG_HAND_LOG		0

#if USE_DEBUG_HAND_LOG

#	include "AutoDebug.h"
#	define NEW_HANDLER_LOG(T,P)	AutoDebugLog::GetMe().NewLog(typeid(T),P);
#	define FREE_HANDLER_LOG(T,P) AutoDebugLog::GetMe().FreeLog(typeid(T),P);

#	define NEW_USE_LOG	AutoDebugLog::GetMe().NewUse();
#	define FREE_USE_LOG  AutoDebugLog::GetMe().FreeUse();
#else
#	define NEW_HANDLER_LOG(T,P)
#	define FREE_HANDLER_LOG(T,P)
#	define NEW_USE_LOG
#	define FREE_USE_LOG
#endif

//--------------------------------------------------------------------------------------
extern BaseCommon_Export Bucket	mUseMemPool;
template<typename T>
class UseInfo //: public MemBase
{
	UseInfo():classObj(NULL),count(0){}
public:
	T*	classObj;
	int count;

	void Release()
	{
		FREE_USE_LOG
			mUseMemPool.deletePtr(this);
		//MEM_DELETE this;		
	}
	static UseInfo* New()
	{
		NEW_USE_LOG
			return new (mUseMemPool.newPtr()) UseInfo<T>();
		//return MEM_NEW  UseInfo<T>();		
	}
};
#	define USE_FLAG <T>

#if DEVELOP_MODE
#define CHECK_CLASS_OBJECT	AssertNote(NULL!=mUse && NULL!=mUse->classObj,  "[%s] class's hand is null", typeid(T).name());
	//if (NULL==mUse || NULL==mUse->classObj)	\
	//{\
	//	char szInfo[1024];\
	//	sprintf(szInfo, "[%s] class's hand is null", typeid(T).name());\
	//	AssertNote(0, szInfo);\
	//}
	
#else
#	define CHECK_CLASS_OBJECT
#endif

//-----------------------------------------------------------------------------------------

template<typename T> 
class  AutoPtr 
{

protected:
	mutable UseInfo USE_FLAG             *mUse;

public:
	inline T&   operator*()			{  CHECK_CLASS_OBJECT; return *((T*)mUse->classObj);	} 
	inline T*   getPtr()		    {  if (  mUse ) return (T*)mUse->classObj; return NULL;				}
	inline const T& operator*()const{  CHECK_CLASS_OBJECT; return *((const T*)mUse->classObj);	}
	inline const T* getPtr() const  {  if (  mUse ) return (const T*)mUse->classObj; return NULL;		}
	inline int  getUseCount() const {  if (  mUse ) return mUse->count; return 0;						}
	inline bool is() const          {  return ( mUse && mUse->classObj );								}
	inline bool isNull() const      {  return !is();													}
	inline operator bool () const   {  return is();														}
	// inline operator T& ()           {  CHECK_CLASS_OBJECT; return *((T*)mUse->classObj);				}
	inline void setNull()			{  _setnull();														}
	inline T*   operator->()		
	{
#pragma warning(push)
#pragma warning(disable:6011)
		CHECK_CLASS_OBJECT; 
		return  (T*)mUse->classObj;		
#pragma warning(pop)
	}
	inline const T*operator->()const
	{  
#pragma warning(push)
#pragma warning(disable:6011)
		CHECK_CLASS_OBJECT; 
		return  (T*)mUse->classObj;		
#pragma warning(pop)
	}
	//----------------------------------------------------------------------------
public:
	~AutoPtr()
	{
		_setnull();
	}	
	
	//----------------------------------------------------------------------------
	AutoPtr() { mUse = NULL; }
	//----------------------------------------------------------------------------

	AutoPtr( const AutoPtr &objhand ) 
	{
		mUse = NULL;
		*this = objhand;
	}
	//----------------------------------------------------------------------------

	AutoPtr( T* obj ) 
		: mUse(NULL)
	{		
		if ( obj )
		{
			_new(obj);
		}
	};


	//template<typename B>
	//AutoPtr( AutoPtr<B> &obj )
	//	: mUse(NULL)
	//{
	//	if (obj && dynamic_cast<T*>(obj.getPtr())!=NULL)
	//	{
	//		mUse =  (UseInfo USE_FLAG*)obj.mUse;
	//		mUse->count++;
	//	}
	//}

	//template<typename B>
	//AutoPtr& operator = ( const AutoPtr<B> &obj ) 
	//{
	//	if (dynamic_cast<const T*>(obj.getPtr())==getPtr())
	//		return *this;

	//	setNull();
	//	if (obj && dynamic_cast<const T*>(obj.getPtr())!=NULL)
	//	{
	//		mUse =  (UseInfo USE_FLAG*)obj.mUse;
	//		mUse->count++;
	//	}
	//	return *this;
	//}

	//----------------------------------------------------------------------------
	// ������ڿ�ʱ,ֱ�ӽ��������ǰ����Ĳ���
	//inline AutoPtr & operator=( int zero ) 
	//{ 
	//	_setnull();
	//	return *this;
	//}
	//----------------------------------------------------------------------------

	inline AutoPtr & operator=( T* obj ) 
	{ 
		if ( is() )
		{
			if ( getPtr() == obj ) 
				return *this;
		}

		_setnull();
		if ( obj )
		{ 
			_new(obj);
		}

		return *this;
	}

	//----------------------------------------------------------------------------
	// ʵ�����ܶ���֮ǰ���д�ֵ
	inline AutoPtr & operator=( const AutoPtr &obj ) 
	{ 
		if (*this==obj)
			return *this;
		_setnull();			
		if ( obj )  
		{
			mUse = obj.mUse;
			++mUse->count;
		}
		else
		{
			mUse = NULL;
		}
		return  *this;
	}

	//----------------------------------------------------------------------------

	void _free()
	{
		if ( is() )
		{
			if ( (mUse->count)==1 )
			{
				_setnull();
			}
			else
			{
				FREE_HANDLER_LOG(T, mUse->classObj);
				T *pPtr = mUse->classObj;				
				mUse->classObj = NULL;
				mUse->count = -(mUse->count-1);
				mUse = NULL;
				FreeClass(pPtr);
			}
		}
	}
	//----------------------------------------------------------------------------
	void swap(AutoPtr &other)
	{
		UseInfo USE_FLAG *use = mUse;
		mUse = other.mUse;
		other.mUse = use;
	}
	//----------------------------------------------------------------------------
private:
	void _new(T *obj)
	{
		NEW_HANDLER_LOG(T,obj)
		mUse = NewUseInfo();
		mUse->classObj = obj; 
		mUse->count = 1;
	}
	//----------------------------------------------------------------------------
	void _setnull(void)
	{
		if ( is() )
		{
			//�����һ���������ֱ�������ͷ�ʱ,�Ű�������ʵ���ͷ��ڴ沢���
			if (--(mUse->count) == 0)      
			{
				FREE_HANDLER_LOG(T, mUse->classObj);
				T *pPtr = mUse->classObj;			
				mUse->classObj = NULL;
				FreeUse();
				mUse = NULL;
				FreeClass(pPtr);
			}
			//ע��: �������Լ�ʱ,�ͷ��Լ���, û���������д���ʱ, ��ִ�е�����
		}
		else      
		{
			// ǿ���ͷź�,ֻ�������������
			if ( mUse )
			{
				if ( ++(mUse->count)==0 )
				{
					FreeUse();
					mUse = NULL;				
				}		
			}
		}		
		mUse = NULL;
	}

protected:
	//----------------------------------------------------------------------------
	inline void FreeClass( T *pPtr )
	{
		delete pPtr;
	}
	//----------------------------------------------------------------------------
	inline UseInfo USE_FLAG * NewUseInfo()
	{		
		return UseInfo USE_FLAG::New();
	}
	//----------------------------------------------------------------------------
	inline void FreeUse()
	{		
		mUse->Release();
	}
	//----------------------------------------------------------------------------
public:
	inline bool operator==( const AutoPtr& b) const
	{
		if (mUse==NULL)
		{
			if (b.mUse==NULL)
				return true;
			return false;
		}
		return mUse==b.mUse && getPtr() == b.getPtr();
	}
	//----------------------------------------------------------------------------
	bool operator!=( const AutoPtr& b)  const
	{
		return mUse!=b.mUse || getPtr() != b.getPtr();
	}
	//----------------------------------------------------------------------------
	bool operator < (const AutoPtr& b) const
	{
		return mUse<b.mUse;
	}
};
//-----------------------------------------------------------------------------------------
#ifndef __LINUX__
//�ػ��ַ�����
template<>
void AutoPtr<char>::FreeClass(char *sz)
{
	delete[] sz;
}
#endif



//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
#endif	//_AUTO_H_
