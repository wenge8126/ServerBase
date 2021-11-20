
#ifndef _INCLUDE_ASYNCAUTOFUNCTION_H_
#define _INCLUDE_ASYNCAUTOFUNCTION_H_

#include "Auto.h"
#include "AsyncFunction.h"
//-------------------------------------------------------------------------
// 异步过程函数, 此函数为 继承AutoBase 对象的成员函数(保证了在异步执行期间不会释放)
//-------------------------------------------------------------------------

class aASyncBaseFunctor : public MemBase
{
public:
	virtual ~aASyncBaseFunctor() {};
	virtual void operator()(AsyncFunParamType ) = 0;
	virtual aASyncBaseFunctor* Clone() { return NULL; }
};

template<typename T>
class aAsyncProcessFun : public aASyncBaseFunctor
{
public:
	//! Member function slot type.
	typedef void(T::*CallBackFunction)(AsyncFunParamType);

	aAsyncProcessFun(CallBackFunction func, T *obj)
		: d_function(func),
		d_object(NULL)
	{
		// NOTE: 如果编译错误在这里的话,说明你使用的类没有继承 public Base<T>
		// 目标是保存对象的指针对象, 防止对象被释放后, 错误执行
		d_object = obj;
	}


	virtual void operator()(AsyncFunParamType )
	{
		if (d_object)
			(d_object.getPtr()->*d_function)();
	}

	virtual aASyncBaseFunctor* Clone()
	{
		aAsyncProcessFun *p = MEM_NEW aAsyncProcessFun(d_function, d_object.getPtr());
		return p;
	}

private:
	CallBackFunction d_function;
	Auto<T>    d_object;
};

class  AsyncAutoFunction
{
public:
	/*!
	\brief
	Default constructor.  Creates a SubscriberSlot with no bound slot.
	*/
	AsyncAutoFunction() { d_functor_impl = 0; };

	AsyncAutoFunction(const AsyncAutoFunction &other)
	{
		AsyncAutoFunction *p = (AsyncAutoFunction*)&other;
		d_functor_impl = p->d_functor_impl;
		p->d_functor_impl = NULL;
	}

	~AsyncAutoFunction() { if (d_functor_impl) delete d_functor_impl; };


	AsyncAutoFunction& operator = (const AsyncAutoFunction &other)
	{
		cleanup();
		AsyncAutoFunction *p = (AsyncAutoFunction*)&other;
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

	void CopyFrom(AsyncAutoFunction &other)
	{
		cleanup();
		d_functor_impl = other.d_functor_impl->Clone();
	}

	template<typename T>
	AsyncAutoFunction(void(T::*function)(AsyncFunParamType ), T *obj) :
		d_functor_impl(MEM_NEW aAsyncProcessFun<T>(function, obj) )
	{}


	template<typename T>
	void  setFunction(void(T::*function)(AsyncFunParamType ), T *obj)
	{
		cleanup();
		d_functor_impl = MEM_NEW aAsyncProcessFun<T>(function, obj);
	}


private:
	//! Points to the internal functor object to which we are bound
	mutable aASyncBaseFunctor		*d_functor_impl;
};


#endif //_INCLUDE_ASYNCAUTOFUNCTION_H_