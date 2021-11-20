
#include "StringHashIndex.h"
#include "BaseTable.h"

#if TABLE_USE_STL_INDEX

ARecord HashStringIndexRecordIt::Begin( )
{
	mListIt = mpOwnerIndex->mHashRecordList.begin();
	if (mListIt!=mEndIt) 
		return mListIt->second;

	return ARecord();
}

void HashStringIndexRecordIt::erase()
{
	if (mListIt!=mEndIt)
	{
		mListIt = mpOwnerIndex->mHashRecordList.erase(mListIt);
	}
}

HashStringIndexRecordIt::HashStringIndexRecordIt( HashStringIndexRecordList::iterator it, StringHashIndex *pIndex )
{
	mpOwnerIndex = pIndex;
	mListIt = it;
	mEndIt = mpOwnerIndex->mHashRecordList.end();
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------


ARecordIt StringHashIndex::GetRecordIt()
{
	return MEM_NEW HashStringIndexRecordIt(mHashRecordList.begin(), this);
}

ARecordIt StringHashIndex::GetRecordIt( ARecord targetRecord )
{
    if (!targetRecord)
        return ARecordIt();
	AString key = targetRecord->get(IndexFieldCol()).string();
	HashStringIndexRecordList::iterator it = mHashRecordList.find(key);
	if (it!=mHashRecordList.end())	
	{
		return MEM_NEW HashStringIndexRecordIt(it, this);			
	}
	return ARecordIt();
}

ARecordIt StringHashIndex::GetLastRecordIt()
{
#if 0
	if (mHashRecordList.empty())
		return ARecordIt();

	return MEM_NEW HashStringIndexRecordIt(--mHashRecordList.end(), this);
#else
	WARN_LOG("std::unordered_map not exist last value, and last value like begin value, so return begin");
	if (mHashRecordList.empty())
		return ARecordIt();

	return MEM_NEW HashStringIndexRecordIt(mHashRecordList.begin(), this);
#endif
}


#endif