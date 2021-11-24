#include "NetWorkerComponent.h"

#include "TcpComponent.h"
#include "Actor.h"
#include "EventCenter.h"

AutoNet NetWorkerComponent::GetNet()
{
	if (!mNet)
	{
		Hand<SocketComponent> comp = mpActor->GetComponent<SocketComponent>();
		if (comp)
			mNet = comp->GetNet();
	}
	return mNet;
}

void NetWorkerComponent::RegisterMsg(const AString &msgName, AutoEventFactory msgFactory)
{
	AutoNet net = GetNet();
	if (net)
		net->GetEventCenter()->RegisterEvent(msgName, msgFactory);
}

//-------------------------------------------------------------------------
NetCloud::HandActor ComponectNetMsg::GetActor()
{
	if (!mNetConnect || mNetConnect->GetNetHandle() == NULL)
		return HandActor();
	Hand<SocketComponent> comp = mNetConnect->GetNetHandle()->GetAttachData();
	return comp;
}

Hand< NetWorkerComponent> ComponectNetMsg::GetNetWorker()
{
	if (!mNetConnect || mNetConnect->GetNetHandle() == NULL)
		return HandActor();
	Hand<SocketComponent> comp = mNetConnect->GetNetHandle()->GetAttachData();
	if (comp)
		return comp->GetNetWorkerComponent();

	return Hand<NetWorkerComponent>();
}
//-------------------------------------------------------------------------
NetCloud::HandActor ComponectResponseMsg::GetActor()
{
	if (!mNetConnect || mNetConnect->GetNetHandle() == NULL)
		return HandActor();
	Hand<SocketComponent> comp = mNetConnect->GetNetHandle()->GetAttachData();
	return comp;
}

Hand< NetWorkerComponent> ComponectResponseMsg::GetNetWorker()
{
	if (!mNetConnect || mNetConnect->GetNetHandle() == NULL)
		return HandActor();
	Hand<SocketComponent> comp = mNetConnect->GetNetHandle()->GetAttachData();
	if (comp)
		return comp->GetNetWorkerComponent();

	return Hand<NetWorkerComponent>();
}

//-------------------------------------------------------------------------

NetCloud::HandActor ComponectRequestMsg::GetActor()
{
	if (!mNetConnect || mNetConnect->GetNetHandle() == NULL)
		return HandActor();
	Hand<SocketComponent> comp = mNetConnect->GetNetHandle()->GetAttachData();
	return comp;
}




Hand< NetWorkerComponent> ComponectRequestMsg::GetNetWorker()
{
	if (!mNetConnect || mNetConnect->GetNetHandle() == NULL)
		return HandActor();
	Hand<SocketComponent> comp = mNetConnect->GetNetHandle()->GetAttachData();
	if (comp)
		return comp->GetNetWorkerComponent();

	return Hand<NetWorkerComponent>();
}
//-------------------------------------------------------------------------