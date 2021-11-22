#include "ActorManager.h"
#include "MeshNetNode.h"


void NetCloud::ActorManager::LowProcess()
{
	mNetNode->LowProcess(1);
	mShareDBManager->LowProcess();
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
	mShareDBManager = MEM_NEW ActorDBMgr(this);
	mNetNode = MEM_NEW MeshNetNode(szCloudNodeIp, nCloudNodePort, nSafeCheck, threadNum);
	RegisterMsg();
}
