
#include "MultipleUInt64MapIndex.h"
#include "BaseTable.h"

#	if TABLE_USE_STL_INDEX

ARecord HashMultiUInt64IndexRecordIt::Begin()
{
	mListIt = mpOwnerIndex->mHashRecordList.begin();
	if (mListIt!=mEndIt) 
		return mListIt->second;

	return ARecord();
}

void HashMultiUInt64IndexRecordIt::erase()
{
	if (mListIt!=mEndIt)
	{
		mListIt = mpOwnerIndex->mHashRecordList.erase(mListIt);
	}
}



//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

ARecordIt MultipleUInt64MapIndex::GetRecordIt()
{
	return MEM_NEW HashMultiUInt64IndexRecordIt(mHashRecordList.begin(), this);
}

ARecordIt MultipleUInt64MapIndex::GetRecordIt( ARecord targetRecord )
{
    if (!targetRecord)
        return ARecordIt();
        INDEX_TYPE key = targetRecord->get(IndexFieldCol());
	MultiIntUInt64IndexRecordList::iterator it = mHashRecordList.find(key);
	while (it!=mHashRecordList.end() && it->first==key)	
	{		
		if (it->second==targetRecord)
			return MEM_NEW HashMultiUInt64IndexRecordIt(it, this);		
		++it;					
	}
	return ARecordIt();
}

ARecordIt MultipleUInt64MapIndex::GetLastRecordIt()
{
	if (mHashRecordList.empty())
		return ARecordIt();

	return MEM_NEW HashMultiUInt64IndexRecordIt(--mHashRecordList.end(), this);
}

#endif //TABLE_USE_STL_INDEX