/********************************************************************
	created:	2010/09/06
	created:	6:9:2010   11:13
	filename: 	f:\NewTest\common\Hand.h
	file path:	f:\NewTest\common
	file base:	Hand
	file ext:	h
	author:		���ĸ�
	
	purpose:	���Ա������������ָ�����,�ɱ����ظ���������
				�ص���ֻ���ĸ��ֽڿռ�, ��ͬ��һ��������, ��ֵЧ�ʸߵ�
	Note:		1 ����װ�Ķ��󲻿�����ʹ��ָ��������ʽ����, ���������ͷŴ���
				 ��: Hand<X>  b;   �����ٳ���   X* m_pObj = b; �� X mObj;
				2 �����ǰָ�����ֻ������ʹ�ù�, ���󽫲����Զ��ͷ�
				 ��: X *p = new X(); Hand<X> &k = p->GetSelf();
*********************************************************************/

#ifndef _INCLUDE_HAND_H_
#define _INCLUDE_HAND_H_

#include "MemBase.h"
#include "BaseCommon.h" 
#include "AutoPtr.h"
#include "Assertx.h"
#include "Data.h"
#include "Common.h"
//-----------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------

template<typename T> 
class  Hand 
{

public:
	mutable UseInfo USE_FLAG             *mUse;

public:
	inline T&   operator*()			{  CHECK_CLASS_OBJECT; return *((T*)mUse->classObj);				}
	inline T*   getPtr()		    {  if (  mUse ) return (T*)mUse->classObj; return NULL;				}
	inline const T& operator*()const{  CHECK_CLASS_OBJECT; return *((const T*)mUse->classObj);			}
	inline const T* getPtr() const  {  if (  mUse ) return (const T*)mUse->classObj; return NULL;		}
	inline int  getUseCount() const {  if (  mUse ) return mUse->count; return 0;						}
	inline bool is() const          {  return ( mUse && mUse->classObj );								}
	inline bool isNull() const      {  return !is();													}
	inline operator bool () const   {  return is();														}
	//inline operator T* ()           {  return	getPtr();												}
	inline void setNull()			{  _setnull();														}
	inline T*   operator->()		
	{
#pragma warning(push)
#pragma warning(disable:6011)
#if DEVELOP_MODE
		if (mUse==NULL || mUse->classObj==NULL || (mUse->classObj!=NULL && mUse->count==0))
		{ 
			int b = 2; 
		}
#endif
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

	Data operator [] ( const char *szKey ) const
	{ 
		if (is()) 
			return getPtr()->_getData(szKey); 
		ERROR_LOG("\r\nERROR: Data object [%s] is NULL, when get >[%s]\r\n", typeid(T).name(), szKey);
		return Data();
	}
	Data operator [] ( const AString &szKey ) const
	{
		return (*this)[szKey.c_str()];
	}
	Data operator [] ( int field ) const
	{ 
		if (is()) 
			return getPtr()->_getData(field); 
		ERROR_LOG("\r\nERROR: Data object [%s] is NULL, when get field >[%d]\r\n", typeid(T).name(), field);
		return Data();
	}
	//----------------------------------------------------------------------------
public:
	~Hand()
	{
		_setnull();
	}	
	
	//----------------------------------------------------------------------------
	Hand() 
	{ 
		mUse = NULL; 
	}
	//----------------------------------------------------------------------------

	Hand( const Hand &objhand ) 
	{
		mUse = NULL;
		*this = objhand;
	}
	//----------------------------------------------------------------------------

	Hand( T* obj ) 
		: mUse(NULL)
	{		
		if ( obj )
		{
			_new(obj);
		}
	};

	template<typename B>
	Hand(B *obj)
		: mUse(NULL)
	{
		*this = obj;
	}

	template<typename B>
	Hand& operator = (B *pB)
	{
		setNull();
		T *pT = dynamic_cast<T*>(pB);
		if (pT!=NULL)
		{
			*this = pT;
		}
		//else
		//{
		//	AssertEx(0 , "b class is not T 's  child");
		//}
		return *this;
	}

	template<typename B>
	Hand& operator = (const B *pB)
	{
		setNull();
		T *pT = (T*)dynamic_cast<const T*>(pB);
		if (pT!=NULL)
		{
			*this = pT;
		}
		//else
		//{
		//	AssertEx(0 , "b class is not T 's  child");
		//}
		return *this;
	}

	template<typename B>
	Hand( const Hand<B> &obj )
		: mUse(NULL)
	{
		if (obj && dynamic_cast<const T*>(obj.getPtr())!=NULL)
		{
			mUse =  (UseInfo USE_FLAG*)obj.mUse;
			mUse->count++;
		}
	}

	template<typename B>
	Hand& operator = ( const Hand<B> &obj ) 
	{
		if (dynamic_cast<const T*>(obj.getPtr())==getPtr())
			return *this;

		setNull();
		if (obj && dynamic_cast<const T*>(obj.getPtr())!=NULL)
		{
			mUse =  (UseInfo USE_FLAG*)obj.mUse;
			mUse->count++;
		}
		return *this;
	}


	//----------------------------------------------------------------------------
	// ������ڿ�ʱ,ֱ�ӽ��������ǰ����Ĳ���
	//inline Hand & operator=( int zero ) 
	//{ 
	//	AssertEx(zero==NULL, "ֻ������� == NULL");
	//	_setnull();
	//	return *this;
	//}
	//----------------------------------------------------------------------------

	inline Hand & operator=( T* obj ) 
	{ 
		if ( is() )
		{
			if ( getPtr() == obj ) 
				return *this;
		}

		_setnull();
		if ( obj!=NULL )
		{ 
			_new(obj);
		}

		return *this;
	}

	//----------------------------------------------------------------------------
	// ʵ�����ܶ���֮ǰ���д�ֵ
	inline Hand & operator=( const Hand &obj ) 
	{ 
		if (*this==obj)
			return *this;
		_setnull();			
		if ( obj.is() )  
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
			mUse->classObj->OnForceFreeBefore();
			FREE_HANDLER_LOG(T, mUse->classObj)								
				T  *p = mUse->classObj;				

			mUse->classObj = NULL;			
			// ��Ϊ��������һ������ָ�����
			// 1 ����ǰ���������ָ�����ʱ, ִ���������ִ�� _setNull(), �������Ϊ�����ͷż�����
			// 2 ����Ϊ����ָ�����ʱ, �����ض�����1, �Ѿ���1, ����������1, ֮�����Ϊ������������				

			FreeClass(p);					
			setNull();		
			mUse = NULL;			
		}		
	}
	//----------------------------------------------------------------------------
	void swap(Hand &other)
	{
		UseInfo USE_FLAG *use = mUse;
		mUse = other.mUse;
		other.mUse = use;
	}
	//----------------------------------------------------------------------------
private:
	void _new(T *obj)
	{
		*this = obj->GetSelf();
	}
	//----------------------------------------------------------------------------
	void _setnull(void)
	{
		if ( is() )
		{
			// ����ʹ������ָ����������������ִ������Ϊ�չ���, ������ֻ���ö����ָ�����
			AssertEx ( (const Hand*)&(mUse->classObj->GetSelf())!=this, "�߼�����:��������ܶ������������");
			//�����һ���������ֱ�������ͷ�ʱ,�Ű�������ʵ���ͷ��ڴ沢���
			if (--(mUse->count) == 1)      
			{
				FREE_HANDLER_LOG(T, mUse->classObj)
				
				T  *p = mUse->classObj;
				mUse->classObj = NULL;							

				FreeClass(p);							
			}
			//ע��: �������Լ�ʱ,�ͷ��Լ���, û���������д���ʱ, ��ִ�е�����
		}
		else      
		{
			// ǿ���ͷź�,ֻ�������������
			if ( mUse )
			{
				AssertEx(mUse->count>0, "Error: no exist logic, check." );
				if ( --(mUse->count)==0 )
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
	inline void FreeClass(T *p)
	{
		p->Release();
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
	template<typename B>
	inline bool operator==( const Hand<B>& b) const
	{
		if (mUse==NULL)
		{
			if (b.mUse==NULL)
				return typeid(T)==typeid(B);
			return false;
		}
		return (void*)mUse==(void*)b.mUse && getPtr() == b.getPtr();
	}
	//----------------------------------------------------------------------------
	template<typename B>
	bool operator!=( const Hand<B>& b)  const
	{
		return !(*this==b);
		//return mUse!=b.mUse || getPtr() != b.getPtr();
	}
	//----------------------------------------------------------------------------
	bool operator < (const Hand& b) const
	{
		return mUse<b.mUse;
	}
};
//-----------------------------------------------------------------------------------------
#ifndef __LINUX__
//�ػ��ַ�����
template<>
void Hand<char>::FreeClass(char * p)
{
	delete[] p;
}

#endif

//��������п��Ա����Լ������ܶ���, ȡ������ռ��, �����ʱ��������ʱ,��ʹ�ü�������1
template<typename N>
class Base : public MemBase
{
public:
	virtual Data _getData(const char *szKey) const 
	{ 
		ERROR_LOG("\r\nERROR: [%s] No overwrite Base _getData function, or load error data object>[%s]\r\n", szKey, typeid(N).name()); 
		AssertEx(0, "ERROR: [%s] No overwrite Base _getData function, or load error data object>[%s]\r\n", szKey, typeid(N).name());
		return Data(); 
	}
	virtual Data _getData(int field) const 
	{ 
		ERROR_LOG("\r\nERROR: [%d] No overwrite Base _getData function, or load error data object>[%s]\r\n", field, typeid(N).name()); 
		AssertEx(0, "ERROR: [%d] No overwrite Base _getData function, or load error data object>[%s]\r\n", field, typeid(N).name());
		return Data(); 
	}

public:
	typedef Hand<N>	ObjHand;

	ObjHand& GetSelf()
	{
		if (!mSelf.is())
		{
			// ���ͷŶ���û��ִ������ʱ��δ�ͷ�Use, ���ֱ���½�����ԭ�еı�й¶ ��ʹ���ڴ�ʹ�ñ����ҵ������⣩		
			if (mSelf.mUse!=NULL)
			{
				NEW_HANDLER_LOG(N, this)
				mSelf.setNull();
			}
			mSelf.mUse = UseInfo<N>::New();
						
			mSelf.mUse->classObj = (N*)this;
			mSelf.mUse->count = 1;
		}
		return mSelf; 
	}

public:
	virtual ~Base()
	{
		if (mSelf.is())
		{
			FREE_HANDLER_LOG(N, this);
			mSelf.mUse->classObj = NULL;			
			mSelf.setNull();
		}
	}

	virtual void Release() { MEM_DELETE this; }
	virtual void OnForceFreeBefore() { }

protected:
	ObjHand		mSelf;	
};


//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
#endif	//_INCLUDE_HAND_H_
