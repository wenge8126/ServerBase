#include "Actor.h"
#include "ActorManager.h"
#include "AsyncNode.h"

NetCloud::AUnit NetCloud::ActorFactory::_NewActor()
{
	Hand<Actor> actor = NewActor();
	actor->mActorFactory = this;

	return actor;
}

NetCloud::AComponent NetCloud::Actor::AddComponent(const AString &compName)
{
	AComponent existComp = mComponentList.find(compName);

	AComponent comp = GetEventCenter()->StartEvent(compName);
	if (comp)
	{
		if (existComp)
		{
			// 不会立即释放, 支持循环执行此处
			WARN_LOG("Now already exist componect <%s>", compName);
			existComp->Free();
		}

		comp->mpActor = this;
		mComponentList.insert(compName, comp);
		comp->Awake();
		comp->DoEvent(false);
	}
	else
		ERROR_LOG("No register componect <%s>", compName);

	return comp;
}

bool NetCloud::Actor::RemoveComponent(const AString &compName)
{
	AComponent existComp = mComponentList.find(compName);
	if (existComp)
	{
		// 不会立即释放, 支持循环执行此处
		LOG("Now remove componect <%s>", compName);
		existComp->Free();
		return true;
	}
	return false;
}


AutoNice NetCloud::Actor::Await(Auto<TransferPacket> tranPak, UnitID targetID, int overMilSecond)
{
	AsyncNode *pNetNode = GetNetNode();
	if (pNetNode == NULL)
	{
		ERROR_LOG("Actor %s Node is NULL, May be not append node", GetID().dump().c_str());
		return AutoNice();
	}
	
	Auto< AsyncProtocol> protocol = pNetNode->mNodeNet->GetNetProtocol();
	AutoNice resp;
	AWaitResponse pWait = protocol->AllotWaitID();

	tranPak->mSenderID = GetID();
	tranPak->mTargetID = targetID;
	tranPak->mRequestID = pWait->mRequestMsgID;
	tranPak->mMsgType = eActorMsg_Reqeust;

	if (!SendTo(tranPak.getPtr()))
	{
		protocol->FreeWaitID(pWait.getPtr());
		ERROR_LOG("Actor %s send fail > %s", GetID().dump().c_str(), targetID.dump().c_str());
		return AutoNice();
	}

	pWait->mWaitCoroID = CORO;
	pWait->Wait(overMilSecond);

	YIELD;
	pWait->StopWait();
	if (pWait->mResponsePacket)
	{
		Auto<TransferPacket> pResp = pWait->mResponsePacket;

		resp = MEM_NEW NiceData();
		pResp->mData.seek(0);

		if (!resp->restore(&pResp->mData))
		{
			ERROR_LOG("Restore response msg data fail  : msg ID %u", pWait->mRequestMsgID);
			protocol->FreeWaitID(pWait.getPtr());
			return AutoNice();
		}
	}
	protocol->FreeWaitID(pWait.getPtr());
	return resp;
}

//{
//	Auto< ActorRequestPacket> requestPak = GetMgr()->mNetNode->CreatePacket(eMsgRequest);
//	requestPak->mNetUnit = this;
//	Auto<ActorResponResultPacket> resultPak = requestPak->Await(requestMsgName, reqestMsg, targetID, waitMilSecond);
//	if (resultPak)
//	{
//		AutoNice msg = MEM_NEW NiceData();
//		resultPak->mResultData->seek(0);
//		if (!msg->restore(resultPak->mResultData.getPtr()))
//		{
//			ERROR_LOG("Restore %s fail", requestMsgName.c_str());
//		}
//		return msg;
//	}
//	return AutoNice();
//}

bool NetCloud::Actor::SendMsg(const AString &msgName, tBaseMsg &msg, UnitID targetID, BROADCAST_MODE eMode /*= eBroadcastNone*/)
{
	AsyncNode *pNetNode = GetNetNode();
	if (pNetNode == NULL)
	{
		ERROR_LOG("Actor %s Node is NULL, May be not append node", GetID().dump().c_str());
		return AutoNice();
	}

	Auto< AsyncProtocol> protocol = pNetNode->mNodeNet->GetNetProtocol();
	Auto<TransferPacket> pak = protocol->CreatePacket(eNGN_TransferMsg);
	pak->mMsgType = (byte)eActorMsg_Notify;
	pak->mbBroadcast = (byte)eMode;
	pak->mTargetID = targetID;
	pak->mSenderID = GetID();

	pak->mData.clear(false);
	pak->mData.writeString(msgName);
	msg.serialize(&pak->mData);

	return SendTo(pak.getPtr());

	//Auto<ActorNotifyPacket> msgPak = GetMgr()->mNetNode->CreatePacket(eMsgNotify);
	//msgPak->mNotifyType = eBroadcastNone;
	//return msgPak->SendMsg(this, targetID, msgName, msg);
}

bool NetCloud::Actor::OnReceiveProcess(NodePacket *pNodePacket)
{
	Auto<TransferPacket> pak = pNodePacket;
	if (pak)
	{
		Hand<AsyncNode> pNetNode = GetNetNode();
		if (pNetNode == NULL)
		{
			ERROR_LOG("Actor %s Node is NULL, May be not append node", GetID().dump().c_str());
			return AutoNice();
		}
		AString msgName;
		pak->mData.seek(0);
		if (!pak->mData.readString(msgName))
		{
			ERROR_LOG("Read msg name fail");
			return true;
		}
		switch (pak->mMsgType)
		{
		case eActorMsg_response:
		{
			Auto< AsyncProtocol> protocol = pNetNode->mNodeNet->GetNetProtocol();
			auto waitResp = protocol->FindWaitResponse(pak->mRequestID);
			if (waitResp)
			{
				waitResp->mResponsePacket = pak;
				RESUME(waitResp->mWaitCoroID);
			}
			else
				ERROR_LOG("No find wait request %u", pak->mRequestID);
			return true;
			break;
		}
		case eActorMsg_Reqeust:
		{	
			auto fun = GetMgr()->mOnMsgFunctionList.find(msgName);
			if (fun != NULL)
			{
				CoroutineTool::AsyncCall([=]()
				{
					Auto< AsyncProtocol> protocol = pNetNode->GetNet()->GetNetProtocol();
					Auto<TransferPacket> respPacket = protocol->CreatePacket(eNGN_TransferMsg);
					respPacket->mSenderID = pak->mSenderID;
					(*fun)(this, (DataStream*)&pak->mData, respPacket.getPtr());
					respPacket->mSenderID = GetID();
					respPacket->mTargetID = pak->mSenderID;
					respPacket->mRequestID = pak->mRequestID;
					respPacket->mMsgType = eActorMsg_response;
					if (!SendTo(respPacket.getPtr()))
						ERROR_LOG("Response send fail");
				});
			}
			else
				ERROR_LOG("No register process request function : %s", msgName.c_str());
		}
		break;

		case eActorMsg_Notify:
		{			
			auto fun = GetMgr()->mOnNotifyMsgFunctionList.find(msgName);
			if (fun != NULL)
			{
				CoroutineTool::AsyncCall([&]()
				{
					(*fun)(this, &pak->mData, pak->mSenderID);
				});
			}
			else
				ERROR_LOG("No register process request function : %s", msgName.c_str());
		}
		break;

		default:
			ERROR_LOG("Can not process %d", pak->mMsgType);
		}

		return true;
	}

	return pNodePacket->Execute(NULL) == 0;				

	///return BigMsgUnit::OnReceiveProcess(pNodePacket);
}

AsyncNode* NetCloud::Actor::GetNetNode()
{
	if (mNode)
		return dynamic_cast<AsyncNode*>(mNode->mpProcess);
	return NULL;
}

Logic::tEventCenter* NetCloud::Actor::GetEventCenter()
{
	return GetMgr()->mEventCenter.getPtr();
}

//-------------------------------------------------------------------------
ARecord NetCloud::DBActor::LoadRecord(const char *szTableName, const char *szKey)
{
	AutoTable t = GetDBMgr()->GetTable(szTableName);
	if (t)
	{
		ARecord re = t->GetRecord(szKey);
		if (re)
		{
			mDataRecordList.push_back(re);
			return re;
		}
		ERROR_LOG("No exist %s in table %s", szKey, szTableName);
	}
	else
		ERROR_LOG("No exist table %s", szTableName);
	return ARecord();
}

NetCloud::AutoDBManager NetCloud::DBActor::GetDBMgr()
{
	ActorManager *pMgr = GetMgr();
	if (pMgr != NULL)
		return pMgr->GetDBMgr();
	return AutoDBManager();
}

ARecord NetCloud::DBActor::LoadRecord(const char *szTableName, Int64 nKey)
{
	AutoTable t = GetDBMgr()->GetTable(szTableName);
	if (t)
	{
		ARecord re = t->GetRecord(nKey);
		if (re)
		{
			mDataRecordList.push_back(re);
			return re;
		}
		ERROR_LOG("No exist %s in table %s", STRING(nKey), szTableName);
	}
	else
		ERROR_LOG("No exist table %s", szTableName);
	return ARecord();
}
