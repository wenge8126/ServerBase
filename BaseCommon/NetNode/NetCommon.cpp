#include "NetCommon.h"
#include "NetUnit.h"
#include "AsyncNode.h"

namespace NetCloud
{
	void NodePacket::OnStartSend(tNetUnit *pSendUnit)
	{
#if DEBUG_CLOUD_NET
		AString nodeInfo = "None node";
		if (pSendUnit->mNode && pSendUnit->mNode->mpProcess!=NULL)
		{
			Hand<AsyncNode> node = pSendUnit->mNode->mpProcess;
			nodeInfo = NetAddress(node->mNodeNet->GetKey()).dump();
		}

		 mSendInfo.Format("*** %s Start %s  %s > %s >>>\r\n", nodeInfo.c_str(), GetMsgName(), pSendUnit->mID.dump().c_str(), mTargetID.dump().c_str()); 
		 DEBUG_LOG("\r\n-------------------------------------------------------\r\n\r\n%s\r\n\r\n------------------------------------------------------\r\n", mSendInfo.c_str());
#endif
	}

	void NodePacket::OnBeforeSend(tNodeConnect *pNode)
	{
		++mSendCount;
#if DEBUG_CLOUD_NET
		AString info;
		info.Format("%d N : %s \r\n>", mSendCount, pNode->GetAddress().dump().c_str());
		mSendInfo += info;
		DEBUG_LOG("\r\n-------------------------------------------------------\r\n\r\n%s\r\n\r\n------------------------------------------------------\r\n", mSendInfo.c_str());
#endif
	}

	void NodePacket::OnBeforeSend(tGateConnect *pNode)
	{
		++mSendCount;
#if DEBUG_CLOUD_NET
		AString info;
		info.Format("%d G : %s \r\n>", mSendCount, pNode->GetAddress().dump().c_str());
		mSendInfo += info;
		DEBUG_LOG("\r\n==================================\r\n\r\n%s\r\n\r\n==================================\r\n", mSendInfo.c_str());
#endif
	}

	bool NetCloud::NodePacket::OnBeforeRun(tNetUnit *pUnit)
	{
#if DEBUG_CLOUD_NET
		AString info;
		info.Format("Run at target %s = this %s END\r\n", pUnit->mID.dump().c_str(), mTargetID.dump().c_str());
		mSendInfo += info;
		DEBUG_LOG("\r\n-------------------------------------------------------\r\n\r\n%s\r\n\r\n------------------------------------------------------\r\n", mSendInfo.c_str());
#endif
		return false;
	}
}
