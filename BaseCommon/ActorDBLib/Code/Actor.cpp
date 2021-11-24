#include "Actor.h"
#include "ActorManager.h"


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


AutoNice NetCloud::Actor::Await(const AString &requestMsgName, tNiceData &reqestMsg, UnitID targetID, int waitMilSecond)
{
	Auto< ActorRequestPacket> requestPak = GetMgr()->mNetNode->CreatePacket(eMsgRequest);
	requestPak->mNetUnit = this;
	Auto<ActorResponResultPacket> resultPak = requestPak->Await(requestMsgName, reqestMsg, targetID, waitMilSecond);
	if (resultPak)
	{
		AutoNice msg = MEM_NEW NiceData();
		resultPak->mResultData->seek(0);
		if (!msg->restore(resultPak->mResultData.getPtr()))
		{
			ERROR_LOG("Restore %s fail", requestMsgName.c_str());
		}
		return msg;
	}
	return AutoNice();
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
