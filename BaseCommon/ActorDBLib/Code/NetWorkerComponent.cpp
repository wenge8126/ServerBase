#include "NetWorkerComponent.h"

#include "TcpComponent.h"
#include "Actor.h"
#include "EventCenter.h"

//AutoNet NetWorkerComponent::GetNet()
//{
//	if (!mNet)
//	{
//		Hand<SocketComponent> comp = mpActor->GetComponent<SocketComponent>();
//		if (comp)
//		{
//			mNet = comp->GetNet();
//			if (mNet)
//			{
//				_RegisterMsg(mNet->GetEventCenter());
//			}
//		}
//	}
//	return mNet;
//}

//void NetWorkerComponent::RegisterMsg(const AString &msgName, AutoEventFactory msgFactory)
//{
//	AutoNet net = GetNet();
//	if (net)
//		net->GetEventCenter()->RegisterEvent(msgName, msgFactory);
//}

//-------------------------------------------------------------------------

