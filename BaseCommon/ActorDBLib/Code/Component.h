#pragma once

#ifndef _INCLUDE_COMPONENT_H_
#define _INCLUDE_COMPONENT_H_

#include "CEvent.h"

namespace NetCloud
{
	class Actor;

	class Component : public Logic::BaseEvent
	{
		friend class Actor;

	public:
		Actor* GetActor() { return mpActor; }
		virtual void Destory();

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

	protected:
		virtual bool _DoEvent() override
		{
			Start();
		}	

	public:
		virtual void Release() override;

	protected:
		Actor			*mpActor = NULL;
	};

	typedef Hand<Component> AComponent;
}


#endif //_INCLUDE_COMPONENT_H_
