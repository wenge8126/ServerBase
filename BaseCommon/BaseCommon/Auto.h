/********************************************************************
	created:	2013/12/24
	created:	24:12:2013   15:19
	filename: 	D:\Work\BaseCommon\BaseCommon\AutoPtr.h
	file path:	D:\Work\BaseCommon\BaseCommon
	file base:	Auto
	file ext:	h
	author:		Wenge Yang
	
	purpose:	内嵌式智能指针对象
				1 内存使用上比较合理
				2 可以与指针混用
	NOTE:		此对象解决不了互相包含死锁状态
				当保存此对象的指针时, 一定注意不要传值生成AutpPtr, 
				否则会自动释放使保存的指针已经释放状态, 症状为 mUseCount = 0
*********************************************************************/

#ifndef _INCLUDE_AUTO_H_
#define _INCLUDE_AUTO_H_

#include "MemBase.h"
#include "Data.h"
#include "Common.h"


class AutoBase : public MemBase
{
public:
	virtual Data _getData(const char *szKey) const { ERROR_LOG("\r\nNo overwrite AutoBase _getData function, or load error data object [%s] of key [%s]\r\n", typeid(this).name(), szKey); return Data(); }
	virtual Data _getData(int nKey) const { ERROR_LOG("\r\nNo overwrite AutoBase _getData function, or load error data object [%s] of key [%d]\r\n", typeid(this).name(), nKey); return Data(); }

public:
	virtual ~AutoBase(){}

	AutoBase()
		: mUseCount(0)
		, mID(0)
	{

	}

	virtual void Release() { delete this; }
	virtual bool IsNull() const { return false; }

	virtual void OnUseChanged(int useCount){}

public:
	int mUseCount;

protected:
	int mID;						// 64位8字节对齐
};


template <typename T>
struct Auto
{
protected:
	T		*m_pBase = nullptr;

public:
	T&	operator * ()
	{ 
#if DEVELOP_MODE
		AssertEx(m_pBase!=nullptr, "object is nullptr"); 
#endif
		return *m_pBase; 
	}

	T* operator -> ()
	{
        AssertNote(m_pBase!=nullptr,  "[%s] class's hand is null", typeid(T).name());
		return m_pBase; 
	}

	const T* operator -> () const
	{
        AssertNote(m_pBase!=nullptr,  "[%s] class's hand is null", typeid(T).name());
		return m_pBase; 
	}

	const T& operator * () const
	{
#if DEVELOP_MODE
		AssertEx(m_pBase!=nullptr, "object is nullptr"); 
#endif
		return *m_pBase; 
	}

	operator bool() const { if (m_pBase == nullptr) return false;  return  !m_pBase->IsNull(); }

	Data operator [] ( const char *szKey ) const
	{ 
		if (m_pBase!=nullptr) 
			return getPtr()->_getData(szKey); 
		//ERROR_LOG("\r\nERROR: Data object [%s] is nullptr, when get >[%s]\r\n", typeid(T).name(), szKey);
		return Data();
	}
	Data operator [] ( const AString &szKey ) const
	{ 
		return (*this)[szKey.c_str()];
	}
	Data operator [] ( int nKey ) const
	{ 
		if (m_pBase!=nullptr) 
			return getPtr()->_getData(nKey); 
		//ERROR_LOG("\r\nERROR: Data object [%s] is nullptr, when get >[%d]\r\n", typeid(T).name(), nKey);
		return Data();
	}

public:
	~Auto()
	{
		setNull();
	}
	Auto()
		: m_pBase(nullptr)
	{

	}

	Auto(T *p)
		: m_pBase(p)
	{
		if (m_pBase!=nullptr)
		{
			//AssertEx(m_pBase->mUseCount>=0, "USE count less zero");
			m_pBase->mUseCount++;

			m_pBase->OnUseChanged(m_pBase->mUseCount);
		}
	}

	Auto(const Auto &other)
		: m_pBase(nullptr)
	{
		if (other.getPtr()!=nullptr)
		{
			m_pBase = (T*)(dynamic_cast<const T*>(other.getPtr()));

			if (m_pBase!=nullptr)
			{
				m_pBase->mUseCount++;
				m_pBase->OnUseChanged(m_pBase->mUseCount);
			}
		}
	}

	template<typename B>
	Auto(B *p)
		: m_pBase( dynamic_cast<T*>(p))
	{
		if (m_pBase!=nullptr)
		{
			//AssertEx(m_pBase->mUseCount>=0, "USE count less zero");
			m_pBase->mUseCount++;
			m_pBase->OnUseChanged(m_pBase->mUseCount);
		}
	}

	template<typename B>
	Auto(const Auto<B> &other)
		: m_pBase(nullptr)
	{
		if (other.getPtr())
		{
			m_pBase = (T*)(dynamic_cast<const T*>(other.getPtr()));

			if (m_pBase!=nullptr)
			{
				m_pBase->mUseCount++;
				m_pBase->OnUseChanged(m_pBase->mUseCount);
			}
		}
	}

	Auto& operator = (T *p)
	{
		setNull();
		m_pBase = p;
		if (m_pBase!=nullptr)
		{
			m_pBase->mUseCount++;
			m_pBase->OnUseChanged(m_pBase->mUseCount);
		}
		return *this;
	}

	Auto<T>& operator = (const Auto<T> &other)
	{
		setNull();
		m_pBase = (T*)dynamic_cast<const T*>(other.getPtr());

		if (m_pBase!=nullptr)
		{
			m_pBase->mUseCount++;
			m_pBase->OnUseChanged(m_pBase->mUseCount);
		}
		return *this;
	}

	template<typename B>
	Auto& operator = (B *p)
	{
		setNull();
		m_pBase = dynamic_cast<T*>(p);
		if (m_pBase!=nullptr)
		{
			m_pBase->mUseCount++;
			m_pBase->OnUseChanged(m_pBase->mUseCount);
		}
		return *this;
	}

	template<typename B>
	Auto<T>& operator = (const Auto<B> &other)
	{
		setNull();
		m_pBase = (T*)dynamic_cast<const T*>(other.getPtr());

		if (m_pBase!=nullptr)
		{
			m_pBase->mUseCount++;
			m_pBase->OnUseChanged(m_pBase->mUseCount);
		}
		return *this;
	}

	//----------------------------------------------------------------------------
	template<typename B>
	inline bool operator==( const Auto<B>& b) const
	{
		if (m_pBase==nullptr)
		{
			if (b.getPtr()==nullptr)
				return typeid(T)==typeid(B);
			return false;
		}
		return m_pBase==b.getPtr();
	}
	template<typename B>
	bool operator!=( const Auto<B>& b)  const
	{
		return !(*this==b);
	}
	//-------------------------------------------------------------------------*/
	void setNull()
	{
		if (m_pBase!=nullptr)
		{
			m_pBase->OnUseChanged(m_pBase->mUseCount-1);
			AssertEx(m_pBase->mUseCount>0, "USE count must more zero");
			if (--(m_pBase->mUseCount)==0)
			{
				T* p = m_pBase;
				m_pBase = nullptr;
				freeClass(p);
			}
		}
		m_pBase = nullptr;
	}

	T* getPtr(){ return m_pBase; }
	const T* getPtr() const { return m_pBase; }

	int getUseCount() const{ return m_pBase!=nullptr ? m_pBase->mUseCount:0; }

protected:
	void freeClass(T *ptr)
	{
		if (ptr !=nullptr) { ptr->Release(); }
	}

public:
	// 只用来方便替换Hand, 填充 Hand::_free()
	void _free() { setNull(); }


};


#endif //_INCLUDE_AUTOPTR_H_