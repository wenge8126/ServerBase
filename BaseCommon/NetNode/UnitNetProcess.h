#pragma once


#ifndef _INCLUDE_UNITNETPROCESS_H_
#define _INCLUDE_UNITNETPROCESS_H_

namespace NetCloud
{
	//-------------------------------------------------------------------------
	// 网络事件处理回调函数, 用于网络组件挂接
	//-------------------------------------------------------------------------
#define  NetProcessParam   tNetUnit*
#define NetProcessParam2 NodePacket*

	class tNetUnit;
	class NodePacket;

	class NetProcessFunctor : public MemBase
	{
	public:
		virtual ~NetProcessFunctor() {};
		virtual void operator()(NetProcessParam paramData, NetProcessParam2) = 0;
		virtual NetProcessFunctor* Clone() { return NULL; }
	};

	template<typename T>
	class NetProcessFunction : public NetProcessFunctor
	{
	public:
		//! Member function slot type.
		typedef void(T::*CallBackFunction)(NetProcessParam, NetProcessParam2);

		NetProcessFunction(CallBackFunction func, T *obj)
			: d_function(func),
			d_object(NULL)
		{
			// NOTE: 如果编译错误在这里的话,说明你使用的类没有继承 public Base<T>
			// 目标是保存对象的指针对象, 防止对象被释放后, 错误执行
			d_object = obj;
		}


		virtual void operator()(NetProcessParam pUnit, NetProcessParam2 pPacket)
		{
			if (d_object)
				(d_object->*d_function)(pUnit, pPacket);
		}

		virtual NetProcessFunctor* Clone()
		{
			NetProcessFunction *p = MEM_NEW NetProcessFunction(d_function, d_object);
			return p;
		}

	private:
		CallBackFunction	d_function;
		T							*d_object;
	};

	class  NetProcess
	{
	public:
		/*!
		\brief
		Default constructor.  Creates a SubscriberSlot with no bound slot.
		*/
		NetProcess() { d_functor_impl = 0; };

		NetProcess(const NetProcess &other)
		{
			NetProcess *p = (NetProcess*)&other;
			d_functor_impl = p->d_functor_impl;
			p->d_functor_impl = NULL;
		}

		~NetProcess() { if (d_functor_impl) delete d_functor_impl; };


		NetProcess& operator = (const NetProcess &other)
		{
			cleanup();
			NetProcess *p = (NetProcess*)&other;
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
		void operator()(NetProcessParam param, NetProcessParam2 pPacket) const
		{
			if (d_functor_impl != NULL)
				(*d_functor_impl)(param, pPacket);
		}


		void run(NetProcessParam param, NetProcessParam2 pPacket)
		{
			if (d_functor_impl)
				(*d_functor_impl)(param, pPacket);
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
			if (d_functor_impl != NULL)
				delete d_functor_impl;

			d_functor_impl = 0;
		};

		void CopyFrom(NetProcess &other)
		{
			cleanup();
			d_functor_impl = other.d_functor_impl->Clone();
		}

		template<typename T>
		NetProcess(void(T::*function)(NetProcessParam, NetProcessParam2), T *obj) :
			d_functor_impl(MEM_NEW NetProcessFunction<T>(function, obj))
		{}


		template<typename T>
		void  setFunction(void(T::*function)(NetProcessParam, NetProcessParam2), T *obj)
		{
			cleanup();
			d_functor_impl = MEM_NEW NetProcessFunction<T>(function, obj);
		}


	private:
		//! Points to the internal functor object to which we are bound
		mutable NetProcessFunctor		*d_functor_impl;
	};
}

#endif //_INCLUDE_UNITNETPROCESS_H_
