#include "MemoryDBTable.h"
#include "MemoryTableFieldIndex.h"


NetCloud::MemoryDBTable::MemoryDBTable(TableActor *pActor) : SkipBaseTable(eInitNullField)
, mpActor(pActor)
{
	mOwnerPtr->mField = MEM_NEW  MemoryTableFieldIndex(this);
}
