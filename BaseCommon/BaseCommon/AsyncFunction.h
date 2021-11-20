
#ifndef _INCLUDE_ASYNCFUNCTION_H_
#define _INCLUDE_ASYNCFUNCTION_H_

#include "Hand.h"

//-------------------------------------------------------------------------
// 异步过程函数, 此函数为 继承AutoBase 对象的成员函数(保证了在异步执行期间不会释放)
//-------------------------------------------------------------------------
#define  AsyncFunParamType   void

class tASyncBaseFunctor : public MemBase
{
public:
	virtual ~tASyncBaseFunctor() {};
	virtual void operator()(AsyncFunParamType ) = 0;
	virtual tASyncBaseFunctor* Clone() { return NULL; }
};

template<typename T>
class AsyncProcessFun : public tASyncBaseFunctor
{
public:
	//! Member function slot type.
	typedef void(T::*CallBackFunction)(AsyncFunParamType);

	AsyncProcessFun(CallBackFunction func, T *obj)
		: d_function(func),
		d_object(NULL)
	{
		// NOTE: 如果编译错误在这里的话,说明你使用的类没有继承 public Base<T>
		// 目标是保存对象的指针对象, 防止对象被释放后, 错误执行
		d_object = obj->GetSelf();
	}


	virtual void operator()(AsyncFunParamType )
	{
		if (d_object)
			(d_object.getPtr()->*d_function)();
	}

	virtual tASyncBaseFunctor* Clone()
	{
		AsyncProcessFun *p = MEM_NEW AsyncProcessFun(d_function, d_object.getPtr());
		return p;
	}

private:
	CallBackFunction d_function;
	Hand<T>    d_object;
};

class  AsyncFunction
{
public:
	/*!
	\brief
	Default constructor.  Creates a SubscriberSlot with no bound slot.
	*/
	AsyncFunction() { d_functor_impl = 0; };

	AsyncFunction(const AsyncFunction &other)
	{
		AsyncFunction *p = (AsyncFunction*)&other;
		d_functor_impl = p->d_functor_impl;
		p->d_functor_impl = NULL;
	}

	~AsyncFunction() { if (d_functor_impl) delete d_functor_impl; };


	AsyncFunction& operator = (const AsyncFunction &other)
	{
		cleanup();
		AsyncFunction *p = (AsyncFunction*)&other;
		d_functor_impl = p->d_functor_impl;
		p->d_functor_impl = NULL;
		return *this;
	}

	bool empty() { return d_functor_impl == 0; }
	/*!
	\brief
	Invokes the slot functor that is bound to this Subscriber.  Returns
	whatever the slot returns, unless there is not slot bound when false is
	always returned.
	*/
	void operator()(AsyncFunParamType ) const
	{
		if (d_functor_impl)
			(*d_functor_impl)();
	}


	void run(AsyncFunParamType )
	{
		if (d_functor_impl)
			(*d_functor_impl)();
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

	void CopyFrom(AsyncFunction &other)
	{
		cleanup();
		d_functor_impl = other.d_functor_impl->Clone();
	}

	template<typename T>
	AsyncFunction(void(T::*function)(AsyncFunParamType ), T *obj) :
		d_functor_impl(MEM_NEW AsyncProcessFun<T>(function, obj) )
	{}


	template<typename T>
	void  setFunction(void(T::*function)(AsyncFunParamType ), T *obj)
	{
		cleanup();
		d_functor_impl = MEM_NEW AsyncProcessFun<T>(function, obj);
	}


private:
	//! Points to the internal functor object to which we are bound
	mutable tASyncBaseFunctor		*d_functor_impl;
};


#endif //_INCLUDE_ASYNCFUNCTION_H_