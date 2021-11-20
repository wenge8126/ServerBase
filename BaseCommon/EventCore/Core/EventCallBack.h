
#ifndef _INCLUDE_EVENTCALLBACK_H_
#define _INCLUDE_EVENTCALLBACK_H_

#include "Hand.h"


//-------------------------------------------------------------------------
// �¼��ص�����
//-------------------------------------------------------------------------
namespace Logic
{
	class tEvent;
}

#define  EventParamType   Logic::tEvent*


class EventBaseFunctor : public MemBase
{
public:
	virtual ~EventBaseFunctor() {};
	virtual bool operator()(EventParamType paramData) = 0;
	virtual bool Equal(const EventBaseFunctor*other) const = 0;
	virtual EventBaseFunctor* Clone() { return NULL; }

	virtual bool valid() const { return true; }
};

template<typename T>
class EventCallBackFunction : public EventBaseFunctor
{
public:
	//! Member function slot type.
	typedef bool(T::*CallBackFunction)(EventParamType);

	EventCallBackFunction(CallBackFunction func, T* obj) 
		: d_function(func),
		d_object(NULL)
	{
		// NOTE: ����������������Ļ�,˵����ʹ�õ���û�м̳� public Base<T>
		// Ŀ���Ǳ�������ָ�����, ��ֹ�����ͷź�, ����ִ��
		d_object = obj->GetSelf();
	}


	virtual bool operator()(EventParamType pDBOperate)
	{
		if (d_object)
			return (d_object.getPtr()->*d_function)(pDBOperate);

		return false;
	}

	virtual EventBaseFunctor* Clone() override
	{
		EventBaseFunctor *p = new EventCallBackFunction(d_function, d_object.getPtr());
		return p;
	}

	virtual bool Equal(const EventBaseFunctor*other)  const override 
	{ 
		const EventCallBackFunction<T> *fun = dynamic_cast<const EventCallBackFunction<T>*>(other);
		if (fun == NULL)
			return false;
		return d_object == fun->d_object && d_function == fun->d_function;
	}

	virtual bool valid() const { return d_object; }

private:
	CallBackFunction d_function;
	Hand<T>    d_object;
};

class  EventCallBack
{
public:
	/*!
	\brief
	Default constructor.  Creates a SubscriberSlot with no bound slot.
	*/
	EventCallBack() {  d_functor_impl = 0; };

	EventCallBack(const EventCallBack &other)
	{
		EventCallBack *p = (EventCallBack*)&other;
		d_functor_impl = p->d_functor_impl;
		p->d_functor_impl = NULL;
	}

	~EventCallBack() { if ( d_functor_impl ) delete d_functor_impl; };


	EventCallBack& operator = (const EventCallBack &other)
	{
		cleanup();
		EventCallBack *p = (EventCallBack*)&other;
		d_functor_impl = p->d_functor_impl;
		p->d_functor_impl = NULL;
		return *this;
	}

	bool Nothing(){ return d_functor_impl == NULL; }

	/*!
	\brief
	Invokes the slot functor that is bound to this Subscriber.  Returns
	whatever the slot returns, unless there is not slot bound when false is
	always returned.
	*/
	bool operator()(EventParamType param) const
	{
		if (d_functor_impl)
			return (*d_functor_impl)(param);
		return false;
	}


	bool run(EventParamType param)
	{
		if (d_functor_impl)
			return (*d_functor_impl)(param);
		return false;
	}
	/*!
	\brief
	Returns whether the SubscriberSlot is internally connected (bound).
	*/
	bool connected() const
	{
		return d_functor_impl != 0;
	}

	bool valid() const
	{
		return d_functor_impl != NULL && d_functor_impl->valid();
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

	void CopyFrom(EventCallBack &other)
	{
		cleanup();
		d_functor_impl = other.d_functor_impl->Clone();
	}

	bool operator == (const EventCallBack &other) const
	{
		return d_functor_impl != NULL && other.d_functor_impl!=NULL && d_functor_impl->Equal(other.d_functor_impl);
	}

	template<typename T>
	EventCallBack( bool(T::*function)(EventParamType), T* obj) :
	d_functor_impl(MEM_NEW EventCallBackFunction<T>(function, obj))
	{}


	template<typename T>
	void  setFunction(bool(T::*function)(EventParamType), T* obj)
	{
		cleanup();
		d_functor_impl = MEM_NEW EventCallBackFunction<T>(function, obj);
	}

private:
	//! Points to the internal functor object to which we are bound
	mutable EventBaseFunctor		*d_functor_impl;
};


#endif //_INCLUDE_EVENTCALLBACK_H_