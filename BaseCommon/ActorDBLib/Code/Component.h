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
		// ʵ����, ���뵽Actor֮��ִ��Awake
		virtual void Awake() {}

		// ʵ����, ���´�ѭ����ִ��һ��Start()
		virtual void Start() override;

		virtual void AsyncStart() {}

		// StartUpdate ������ִ�� Update, StopUpdate() ����Update
		virtual void Update(float onceTime) override {  }

		// ����Update
		virtual void LowUpdate() {}

		// �ͷ�֮ǰִ�� OnDestory
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
	// ��Ҫ���� Update(���߼��߳�ѭ��һ��), ����̳�ProcessComponect
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
