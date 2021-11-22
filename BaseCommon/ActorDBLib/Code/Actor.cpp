#include "Actor.h"
#include "ActorManager.h"


NetCloud::AUnit NetCloud::ActorFactory::_NewActor()
{
	Hand<Actor> actor = NewActor();
	actor->mActorFactory = this;

	return actor;
}

NetCloud::AutoDBManager NetCloud::Actor::GetDBMgr()
{
	ActorManager *pMgr = GetMgr();
	if (pMgr != NULL)
		return pMgr->mShareDBManager;
	return AutoDBManager();
}

ARecord NetCloud::Actor::LoadRecord(const char *szTableName, const char *szKey)
{
	AutoTable t = GetMgr()->mShareDBManager->GetTable(szTableName);
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

ARecord NetCloud::Actor::LoadRecord(const char *szTableName, Int64 nKey)
{
	AutoTable t = GetMgr()->mShareDBManager->GetTable(szTableName);
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
