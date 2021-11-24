#include "ActorManager.h"
#include "MeshNetNode.h"
#include "Component.h"


void NetCloud::ActorManager::RegisterComponect(const char *szName, Hand<tEventFactory> factory)
{
	AComponent comp = factory->NewEvent();
	comp->RegisterMsg(this);
	mNetNode->GetEventCenter()->RegisterEvent(szName, factory);
}

void NetCloud::ActorManager::LowProcess()
{
	mNetNode->LowProcess(1);
	
	Hand<MeshNetNode> node = mNetNode;
	for (auto it = node->mUnitListHash.begin(); it; ++it)
	{
		HandActor actor = it.get();
		if (actor)
			actor->LowProcess();
	}
}

NetCloud::ActorManager::ActorManager(const char *szCloudNodeIp, int nCloudNodePort, int nSafeCheck, int threadNum)
{
	mSelfPtr = MEM_NEW ActorMgrPtr();
	mSelfPtr->mpMgr = this;
	
	mNetNode = MEM_NEW MeshNetNode(szCloudNodeIp, nCloudNodePort, nSafeCheck, threadNum);
	RegisterMsg();
}
