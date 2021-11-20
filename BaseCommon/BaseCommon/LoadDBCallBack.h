
#ifndef _INCLUDE_LOADDBCALLBACK_H_
#define _INCLUDE_LOADDBCALLBACK_H_

#include "Hand.h"

//-------------------------------------------------------------------------
// DB操作回调函数
//-------------------------------------------------------------------------

class DBOperate;


class LoadBaseFunctor : public MemBase
{
public:
	virtual ~LoadBaseFunctor() {};
	virtual void operator()(ARecord paramData, AString key) = 0;
	virtual LoadBaseFunctor* Clone() { return NULL; }
};

template<typename T>
class LoadDBCallBackFunction : public LoadBaseFunctor
{
public:
	//! Member function slot type.
	typedef void(T::*CallBackFunction)(ARecord , AString);

	LoadDBCallBackFunction(CallBackFunction func, T* obj) 
		: d_function(func),
		d_object(NULL)
	{
		// NOTE: 如果编译错误在这里的话,说明你使用的类没有继承 public Base<T>
		// 目标是保存对象的指针对象, 防止对象被释放后, 错误执行
		d_object = obj->GetSelf();
	}


	virtual void operator()(ARecord pDBOperate, AString bSucceed)
	{
		if (d_object)
			(d_object.getPtr()->*d_function)(pDBOperate, bSucceed);
	}

	virtual LoadBaseFunctor* Clone()
	{
		LoadDBCallBackFunction *p = MEM_NEW LoadDBCallBackFunction(d_function, d_object.getPtr());
		return p;
	}

private:
	CallBackFunction d_function;
	Hand<T>    d_object;
};

class  LoadDBCallBack
{
public:
	/*!
	\brief
	Default constructor.  Creates a SubscriberSlot with no bound slot.
	*/
	LoadDBCallBack() {  d_functor_impl = 0; };

	LoadDBCallBack(const LoadDBCallBack &other)
	{
		LoadDBCallBack *p = (LoadDBCallBack*)&other;
		d_functor_impl = p->d_functor_impl;
		p->d_functor_impl = NULL;
	}

	~LoadDBCallBack() { if ( d_functor_impl ) delete d_functor_impl; };


	LoadDBCallBack& operator = (const LoadDBCallBack &other)
	{
		cleanup();
		LoadDBCallBack *p = (LoadDBCallBack*)&other;
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
	void operator()(ARecord param, AString bSucceed) const
	{
		if (d_functor_impl)
			(*d_functor_impl)(param, bSucceed);
	}


	void run(ARecord param, AString bSucceed)
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

	void CopyFrom(LoadDBCallBack &other)
	{
		cleanup();
		d_functor_impl = other.d_functor_impl->Clone();
	}

	template<typename T>
	LoadDBCallBack( void(T::*function)(ARecord, AString bSucceed), T* obj) :
	d_functor_impl(MEM_NEW LoadDBCallBackFunction<T>(function, obj))
	{}


	template<typename T>
	void  setFunction(void(T::*function)(ARecord, AString bSucceed), T* obj)
	{
		cleanup();
		d_functor_impl = MEM_NEW LoadDBCallBackFunction<T>(function, obj);
	}

private:
	//! Points to the internal functor object to which we are bound
	mutable LoadBaseFunctor		*d_functor_impl;
};


#endif //_INCLUDE_LOADDBCALLBACK_H_