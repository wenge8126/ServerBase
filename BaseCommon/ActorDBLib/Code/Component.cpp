#include "Component.h"
#include "Actor.h"
#include "CoroutineTool.h"

void NetCloud::Component::Destory()
{
	Free();
}

void NetCloud::Component::Start()
{
	ASYNC(&Component::AsyncStart, this);
}

void NetCloud::Component::Release()
{
	OnDestory();

	AComponent comp = mpActor->GetComponent(GetEventName());
	if (comp.getPtr() == this)
		mpActor->mComponentList.erase(GetEventName());

	mpActor = NULL;

	BaseEvent::Release();
}
