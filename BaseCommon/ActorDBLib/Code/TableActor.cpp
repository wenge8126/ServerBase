#include "TableActor.h"
#include "MemoryDBTable.h"
#include "ActorManager.h"


void NetCloud::TableActor::InitSQLTable()
{
	mDBTable = MEM_NEW MemoryDBTable(this);
	Hand<SQLComponect> comp = AddComponent("MySQLComponect");
	mDBTable->InitSQL(comp);
}

void NetCloud::TableActor::InitShareSQL()
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
