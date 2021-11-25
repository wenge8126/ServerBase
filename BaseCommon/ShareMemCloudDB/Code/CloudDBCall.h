
#ifndef _INCLUDE_CLOUDDBCALL_H_
#define _INCLUDE_CLOUDDBCALL_H_

#include "Hand.h"

//-------------------------------------------------------------------------
// DB操作回调函数
//-------------------------------------------------------------------------
#define  ResultDatraType2   AutoNice
#define  ResultDatraType  NetCloud::RequestDBOperatePak

namespace NetCloud
{
	class RequestDBOperatePak;
}
class CloudDBBaseFunctor : public MemBase
{
public:
	virtual ~CloudDBBaseFunctor() {};
	virtual void operator()(ResultDatraType *paramData, ResultDatraType2, int nResult) = 0;
	virtual CloudDBBaseFunctor* Clone() { return NULL; }
};

template<typename T>
class CloudDBCallBackFunction : public CloudDBBaseFunctor
{
public:
	//! Member function slot type.
	typedef void(T::*CallBackFunction)(ResultDatraType *, ResultDatraType2, int);

	CloudDBCallBackFunction(CallBackFunction func, T* obj)
		: d_function(func),
		d_object(NULL)
	{
		// NOTE: 如果编译错误在这里的话,说明你使用的类没有继承 public Base<T>
		// 目标是保存对象的指针对象, 防止对象被释放后, 错误执行
		d_object = obj->GetSelf();
	}


	virtual void operator()(ResultDatraType *pDBOperate, ResultDatraType2 resultData, int nResult)
	{
		if (d_object)
			(d_object.getPtr()->*d_function)(pDBOperate, resultData, nResult);
	}

	virtual CloudDBBaseFunctor* Clone()
	{
		CloudDBCallBackFunction *p = MEM_NEW CloudDBCallBackFunction(d_function, d_object.getPtr());
		return p;
	}

private:
	CallBackFunction d_function;
	Hand<T>    d_object;
};

class  CloudDBCall
{
public:
	/*!
	\brief
	Default constructor.  Creates a SubscriberSlot with no bound slot.
	*/
	CloudDBCall() { d_functor_impl = 0; };

	CloudDBCall(const CloudDBCall &other)
	{
		CloudDBCall *p = (CloudDBCall*)&other;
		d_functor_impl = p->d_functor_impl;
		p->d_functor_impl = NULL;
	}

	~CloudDBCall() { if (d_functor_impl) delete d_functor_impl; };


	CloudDBCall& operator = (const CloudDBCall &other)
	{
		cleanup();
		CloudDBCall *p = (CloudDBCall*)&other;
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
	void operator()(ResultDatraType *param, ResultDatraType2 resultData, int nResult) const
	{
		if (d_functor_impl)
			(*d_functor_impl)(param, resultData, nResult);
	}


	void run(ResultDatraType *param, ResultDatraType2 resultData, int nResult)
	{
		if (d_functor_impl)
			(*d_functor_impl)(param, resultData, nResult);
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

	void CopyFrom(CloudDBCall &other)
	{
		cleanup();
		d_functor_impl = other.d_functor_impl->Clone();
	}

	template<typename T>
	CloudDBCall(void(T::*function)(ResultDatraType*, ResultDatraType2, int), T* obj) :
		d_functor_impl(MEM_NEW CloudDBCallBackFunction<T>(function, obj))
	{}


	template<typename T>
	void  setFunction(void(T::*function)(ResultDatraType*, ResultDatraType2, int), T* obj)
	{
		cleanup();
		d_functor_impl = MEM_NEW CloudDBCallBackFunction<T>(function, obj);
	}

private:
	//! Points to the internal functor object to which we are bound
	mutable CloudDBBaseFunctor		*d_functor_impl;
};


#endif //_INCLUDE_CLOUDDBCALL_H_
