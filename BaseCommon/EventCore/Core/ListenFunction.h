#pragma once


#ifndef _INCLUDE_LISTENFUNCTION_H_
#define _INCLUDE_LISTENFUNCTION_H_

#include "Hand.h"

//-------------------------------------------------------------------------
// 异步过程函数, 此函数为 继承AutoBase 对象的成员函数(保证了在异步执行期间不会释放)
//-------------------------------------------------------------------------
#define  ListenParamType   tNiceData*

class tFunBaseFunctor : public MemBase
{
public:
	virtual ~tFunBaseFunctor() {};
	virtual void operator()(ListenParamType) = 0;
	virtual tFunBaseFunctor* Clone() { return NULL; }

	virtual bool Equal(const tFunBaseFunctor *other) const { return false; }
	virtual bool Valid() const { return true; }
};

template<typename T>
class ListenCallFun : public tFunBaseFunctor
{
public:
	//! Member function slot type.
	typedef void(T::*CallBackFunction)(ListenParamType);

	ListenCallFun(CallBackFunction func, T *obj)
		: d_function(func),
		d_object(NULL)
	{
		// NOTE: 如果编译错误在这里的话,说明你使用的类没有继承 public Base<T>
		// 目标是保存对象的指针对象, 防止对象被释放后, 错误执行
		d_object = obj->GetSelf();
	}


	virtual void operator()(ListenParamType data)
	{
		if (d_object)
			(d_object.getPtr()->*d_function)(data);
	}

	virtual tFunBaseFunctor* Clone()
	{
		ListenCallFun *p = MEM_NEW ListenCallFun(d_function, d_object.getPtr());
		return p;
	}

	virtual bool Equal(const tFunBaseFunctor *other) const override 
	{ 
		const ListenCallFun *pOther = dynamic_cast<const ListenCallFun*>(other);
		if (pOther != NULL)
			return d_object == pOther->d_object && d_function == pOther->d_function;
		return false; 
	}

	virtual bool Valid() const override { return d_object; }

private:
	CallBackFunction d_function;
	Hand<T>    d_object;
};

class  ListenFunction
{
public:
	/*!
	\brief
	Default constructor.  Creates a SubscriberSlot with no bound slot.
	*/
	ListenFunction() { d_functor_impl = 0; };

	ListenFunction(const ListenFunction &other)
	{
		ListenFunction *p = (ListenFunction*)&other;
		d_functor_impl = p->d_functor_impl;
		p->d_functor_impl = NULL;
	}

	~ListenFunction() { if (d_functor_impl) delete d_functor_impl; };


	ListenFunction& operator = (const ListenFunction &other)
	{
		cleanup();
		ListenFunction *p = (ListenFunction*)&other;
		d_functor_impl = p->d_functor_impl;
		p->d_functor_impl = NULL;
		return *this;
	}

	bool empty() { return d_functor_impl == 0 || !d_functor_impl->Valid(); }

	bool operator == (const ListenFunction &other) const
	{
		return d_functor_impl != NULL && other.d_functor_impl && d_functor_impl->Equal(other.d_functor_impl);		
	}

	/*!
	\brief
	Invokes the slot functor that is bound to this Subscriber.  Returns
	whatever the slot returns, unless there is not slot bound when false is
	always returned.
	*/
	void operator()(ListenParamType pData) const
	{
		if (d_functor_impl)
			(*d_functor_impl)(pData);
	}


	void run(ListenParamType pData)
	{
		if (d_functor_impl)
			(*d_functor_impl)(pData);
	}
	/*!
	\brief
	Returns whether the SubscriberSlot is internally connected (bound).
	*/
	bool connected() const
	{
		return d_functor_impl != 0;
	}


	/*!
	\brief
	Disconnects the slot internally and performs any required cleanup
	operations.
	*/
	void cleanup()
	{
		if (d_functor_impl)
			delete d_functor_impl;

		d_functor_impl = 0;
	};

	void CopyFrom(ListenFunction &other)
	{
		cleanup();
		d_functor_impl = other.d_functor_impl->Clone();
	}

	template<typename T>
	ListenFunction(void(T::*function)(ListenParamType), T *obj) :
		d_functor_impl(MEM_NEW ListenCallFun<T>(function, obj))
	{}


	template<typename T>
	void  setFunction(void(T::*function)(ListenParamType), T *obj)
	{
		cleanup();
		d_functor_impl = MEM_NEW ListenCallFun<T>(function, obj);
	}


private:
	//! Points to the internal functor object to which we are bound
	mutable tFunBaseFunctor		*d_functor_impl;
};


#endif //_INCLUDE_LISTENFUNCTION_H_