#pragma once

//#include "MeshNetGate.h"
//#include "MeshNetNode.h"
#include "ActorDBLib.h"

#include "AsyncGate.h"
#include "AsyncNode.h"

namespace NetCloud
{
	class ActorDBLib_Export ActorGate : public AsyncGate
	{
	public:
		ActorGate(); //int fixedGateCount, int gateID, const char *szCloudGateIp, int nCloudGatePort, int nGateMeshNetSafeCheck, int threadNum = 2);

	};


	class ActorDBLib_Export ActorNode : public AsyncNode
	{
	public:
		virtual void OnJumpGateReadyFinish(UInt64 unitID)
		{

		}
	};
}
