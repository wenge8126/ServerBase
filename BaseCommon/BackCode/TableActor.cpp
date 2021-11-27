#include "TableActor.h"
#include "MemoryDBTable.h"
#include "ActorManager.h"


void NetCloud::TableActor::_AsyncInitDB(InitParam param)
{
	MySqlDBTool	mSql;
	if (!mSql.InitStart(*param.mSQLParam))
	{
		ERROR_LOG("Init sql fail : %s", mSql.getErrorMsg());
		return;
	}

	// ��ȡ����Ϣ: ����, hash Key
	AString tableName;
	int hashKey = 0;
	EActorTableType tableType = eATable_MYSQL;

	// ����
	TableActorID tID;
	tID.id = MAKE_INDEX_ID(tableName.c_str());
	tID.hash = hashKey;
	tID.type = 0;
	
	Hand<TableActor> actor = param.mgr->CreateActor(TableActorType, (Int64)tID.value);
	actor->StartDB(tableType, param.mSQLParam);
}

void NetCloud::TableActor::InitSQLTable(AutoNice param)
{
	mDBTable = MEM_NEW MemoryDBTable(this);
	Hand<SQLComponect> comp = AddComponent("MySQLComponect");
	mDBTable->InitSQL(comp);
}

void NetCloud::TableActor::InitShareSQL(AutoNice param)
{
	mDBTable = MEM_NEW MemoryDBTable(this);
	Hand<SQLComponect> comp = AddComponent("ShareSQLComponect");
	mDBTable->InitSQL(comp);
}

void NetCloud::TableActor::RegisterMsg(ActorManager *pActorMgr)
{	
	REG_COMPONENT(pActorMgr, MySQLComponect);
	REG_COMPONENT(pActorMgr, ShareSQLComponect);
}
