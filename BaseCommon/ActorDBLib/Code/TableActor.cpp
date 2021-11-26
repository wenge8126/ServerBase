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

	// 获取表信息: 名称, hash Key
	AString tableName;
	int hashKey = 0;
	EActorTableType tableType = eATable_MYSQL;

	// 创建
	TableActorID tID;
	tID.id = MAKE_INDEX_ID(tableName.c_str());
	tID.hash = hashKey;
	
	Hand<TableActor> actor = param.mgr->CreateActor(TableActorType, tID.value);
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
