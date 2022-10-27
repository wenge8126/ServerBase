#include "Component.h"
#include "Actor.h"
#include "CoroutineTool.h"
#include "ActorManager.h"

void NetCloud::Component::Destory()
{
	Free();
}

NetCloud::ActorManager* NetCloud::Component::GetMgr()
{
	Hand<ActorEventCenter> center = GetEventCenter();
	if (center)
		return center->mMgrPtr->mpMgr;
}

void NetCloud::Component::Start()
{
	ASYNC(&Component::AsyncStart, this);
}

void NetCloud::Component::Release()
{
	if (mpActor != NULL)
	{
		OnDestory();

		AComponent comp = mpActor->GetComponent(GetEventName());
		if (comp.getPtr() == this)
			mpActor->mComponentList.erase(GetEventName());

		mpActor = NULL;
	}
	BaseEvent::Release();
}
//-------------------------------------------------------------------------
bool NetCloud::ProcessComponent::_DoEvent()
{
	Component::_DoEvent();
	// Start 之后, 才会加入到高速循环列表内, 才会执行Process
	if (mpActor != NULL)
	{
		for (int i = 0; i < mpActor->GetMgr()->mProcessComponectList.size(); ++i)
		{
			if (mpActor->GetMgr()->mProcessComponectList[i].getPtr() == this)
				return true;
		}
		mpActor->GetMgr()->mProcessComponectList.push_back(GetSelf());
	}
	return true;
}

void NetCloud::ProcessComponent::RemoveProcess()
{
	if (mpActor != NULL && mpActor->GetMgr()!=NULL)
	{
		for (int i = 0; i < mpActor->GetMgr()->mProcessComponectList.size(); ++i)
		{
			if (mpActor->GetMgr()->mProcessComponectList[i].getPtr() == this)
			{
				mpActor->GetMgr()->mProcessComponectList.removeAt(i);
				break;
			}
		}
	}
}

//-------------------------------------------------------------------------