#include "SQLComponect.h"
#include "TableActor.h"

void NetCloud::SQLComponect::Start()
{
	Hand<TableActor> actor = GetActor();
	actor->GetTable()->InitSQL(actor);
}
