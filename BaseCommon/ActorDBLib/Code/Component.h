#pragma once

#ifndef _INCLUDE_COMPONENT_H_
#define _INCLUDE_COMPONENT_H_

#include "CEvent.h"
#include "ActorDBLib.h"

namespace NetCloud
{
	class Actor;
	class ActorManager;

	class ActorDBLib_Export Component : public Logic::BaseEvent
	{
		friend class Actor;
		friend class ActorManager;

	public:
		Actor* GetActor() { return mpActor; }
		virtual void Destory();

		ActorManager* GetMgr();

	protected:
		// 实例后, 加入到Actor之后即执行Awake
		virtual void Awake() {}

		// 实例后, 在下次循环中执行一次Start()
		virtual void Start() override;

		virtual void AsyncStart() {}

		// StartUpdate 开启后执行 Update, StopUpdate() 结束Update
		virtual void Update(float onceTime) override {  }

		// 低速Update
		virtual void LowUpdate() {}

		// 释放之前执行 OnDestory
		virtual void OnDestory() {}

		virtual void RegisterMsg() {}

	protected:
		virtual bool _DoEvent() override
		{
			Start();
			return true;
		}	

		virtual void SetData(AutoNice &scrData) {}
		virtual size_t AppendData(const tNiceData &scrData, bool bReplace) { return 0; }
		virtual tNiceData& GetData(void) { static NiceData s; return s; }
		virtual const tNiceData& GetData(void) const { static NiceData s; return s;  }

	public:
		virtual void Release() override;

		const char* Type() { return typeid(*this).name(); }

	protected:
		Actor			*mpActor = NULL;
	};

	typedef Hand<Component> AComponent;

	//-------------------------------------------------------------------------
	// 需要高速 Update(与逻辑线程循环一至), 必须继承ProcessComponect
	class ActorDBLib_Export ProcessComponent : public Component
	{
	public:
		virtual void Process() { WARN_LOG("No override Process, then check need ProcessComponect type by %s", typeid(*this).name()); };

	protected:
		virtual bool _DoEvent() override;

	public:
		void Release() override
		{
			RemoveProcess();
			Component::Release();
		}

		void OnForceFreeBefore() override
		{
			RemoveProcess();
			Component::OnForceFreeBefore();
		}

		void RemoveProcess();
	};
	typedef Hand< ProcessComponent> AProcessComponect;
	//-------------------------------------------------------------------------
}



#endif //_INCLUDE_COMPONENT_H_
