

#ifndef _INCLUDE_CALLBACK_H_
#define _INCLUDE_CALLBACK_H_

#include "Hand.h"
//-------------------------------------------------------------------------
// 异步回调函数, 支持Auto 和 Hand对象, 支持不同的函数参数
//-------------------------------------------------------------------------
class DBOperate;

template<typename ParamType>
class tBaseFunctor : public MemBase
{
public:
	virtual ~tBaseFunctor() {};
	virtual void operator()(ParamType paramData, bool bSucceed) = 0;
	virtual tBaseFunctor* Clone() { return NULL; }
};

template<typename T, typename ParamType>
class CBackFunction : public tBaseFunctor<ParamType>
{
public:
	//! Member function slot type.
	typedef void(T::*CallBackFunction)(ParamType , bool);

	CBackFunction(CallBackFunction func, T* obj) 
		: d_function(func),
		d_object(NULL)
	{
		// NOTE: 如果编译错误在这里的话,说明你使用的类没有继承 public Base<T>
		// 目标是保存对象的指针对象, 防止对象被释放后, 错误执行
		d_object = obj->GetSelf();
	}


	virtual void operator()(ParamType pDBOperate, bool bSucceed)
	{
		if (d_object)
			(d_object.getPtr()->*d_function)(pDBOperate, bSucceed);
	}

	virtual tBaseFunctor<ParamType>* Clone() override
	{
		CBackFunction *p = MEM_NEW CBackFunction(d_function, d_object.getPtr());
		return p;
	}

private:
	CallBackFunction d_function;
	Hand<T>    d_object;
};

template<typename ParamType>
class CallBack
{
public:
	/*!
	\brief
	Default constructor.  Creates a SubscriberSlot with no bound slot.
	*/
	CallBack() {  d_functor_impl = 0; };

	CallBack(const CallBack &other)
	{
		CallBack *p = (CallBack*)&other;
		d_functor_impl = p->d_functor_impl;
		p->d_functor_impl = NULL;
	}

	~CallBack() { if ( d_functor_impl ) delete d_functor_impl; };


	CallBack& operator = (const CallBack &other)
	{
		cleanup();
		CallBack *p = (CallBack*)&other;
		d_functor_impl = p->d_functor_impl;
		p->d_functor_impl = NULL;
		return *this;
	}

	bool empty(){ return d_functor_impl==0; }
	/*!
	\brief
	Invokes the slot functor that is bound to this Subscriber.  Returns
	whatever the slot returns, unless there is not slot bound when false is
	always returned.
	*/
	void operator()(ParamType param, bool bSucceed) const
	{
		if (d_functor_impl)
			(*d_functor_impl)(param, bSucceed);
	}


	void run(ParamType param, bool bSucceed)
	{
		if (d_functor_impl)
			(*d_functor_impl)(param, bSucceed);
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

	void CopyFrom(CallBack &other)
	{
		cleanup();
		d_functor_impl = other.d_functor_impl->Clone();
	}

	template<typename T>
	CallBack( void(T::*function)(ParamType, bool bSucceed), T* obj) :
	d_functor_impl(MEM_NEW CBackFunction<T, ParamType>(function, obj))
	{}


	template<typename T>
	void  setFunction(void(T::*function)(ParamType, bool bSucceed), T* obj)
	{
		cleanup();
		d_functor_impl = MEM_NEW CBackFunction<T, ParamType>(function, obj);
	}

private:
	//! Points to the internal functor object to which we are bound
	mutable tBaseFunctor<ParamType>		*d_functor_impl;
};



#endif //_INCLUDE_CALLBACK_H_