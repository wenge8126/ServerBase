
#ifndef _INCLUDE_MULTIPLEUINT64MAPINDEX_H_
#define _INCLUDE_MULTIPLEUINT64MAPINDEX_H_

#include "RecordIndex.h"

#	if TABLE_USE_STL_INDEX

#	include <map>
//-------------------------------------------------------------------------
//using namespace std;

#define INDEX_TYPE		UInt64

// 用于排行排序时, 使用hash_multimap 不可以得到顺序
typedef std::multimap<INDEX_TYPE, ARecord>		MultiIntUInt64IndexRecordList;
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
class BaseCommon_Export MultipleUInt64MapIndex : public RecordIndex
{
	friend class HashMultiUInt64IndexRecordIt;

public:
	virtual ARecord GetRecord(Int64 nIndex) 
	{		
		MultiIntUInt64IndexRecordList::iterator it = mHashRecordList.find((INDEX_TYPE)nIndex);
		if (it!=mHashRecordList.end())
			return it->second;

		return ARecord();
	}

	virtual ARecord GetRecord(float fIndex)
	{
		return GetRecord((Int64)fIndex);
	}
	//NOTE: 先找到第一个, 然后再逐个判断字符串KEY
	virtual ARecord GetRecord(const char* szIndex)
	{		
		return GetRecord(TOINT64(szIndex));
	}

	virtual ARecord GetRecord(const Data &nIndex)
	{
		return GetRecord((Int64)nIndex);
	}

	virtual bool ExistRecord(Int64 nIndex) {  return GetRecord(nIndex); }
	virtual bool ExistRecord(float fIndex) { return ExistRecord((Int64)(fIndex)); }
	virtual bool ExistRecord(const char* szIndex) { return ExistRecord(TOINT64(szIndex)); }

protected:
	virtual bool InsertRecord(ARecord scrRecord)
	{
		Data d = scrRecord->get(IndexFieldCol());
		if (d.empty())
			return false;

		INDEX_TYPE key = d;

		auto p = mHashRecordList.insert(MultiIntUInt64IndexRecordList::value_type(key, scrRecord));
		return p != mHashRecordList.end();
	}

	virtual bool RemoveRecord(Int64 nIndex)
	{
		MultiIntUInt64IndexRecordList::iterator it = mHashRecordList.find((INDEX_TYPE)nIndex);
		if (it!=mHashRecordList.end())
		{
			mHashRecordList.erase(it);
			return true;
		}
		return false;
	}
	virtual bool RemoveRecord(float fIndex)
	{
		return RemoveRecord((Int64)(fIndex));
	}
	virtual bool RemoveRecord(const char* szIndex)
	{		
		return RemoveRecord(TOINT64(szIndex));
	}

	virtual bool RemoveRecord(const char *nIndex, ARecord record){ return RemoveRecord(TOINT64(nIndex)); }
	virtual bool RemoveRecord(Int64 nIndex, ARecord record)
	{
		MultiIntUInt64IndexRecordList::iterator it = mHashRecordList.find((INDEX_TYPE)nIndex);
		while (it!=mHashRecordList.end() && it->first==nIndex)	
		{			
			if (it->second==record)
			{
				mHashRecordList.erase(it);
				return true;
			}
			++it;				
		}
		return false;
	}

public:
	virtual ARecordIt GetRecordIt();

	virtual ARecordIt GetLastRecordIt();

	virtual ARecordIt GetRecordIt(ARecord targetRecord);

	virtual UInt64 GetCount(){ return mHashRecordList.size(); }
	virtual bool IsStringHashKey() { return false; }
	virtual bool IsMultiple() const override { return true; }

	virtual Int64 GetNextGrowthKey()
	{
		AssertNote(0, "字符串不支持自增长");
		return 0; 
	}

	virtual UInt64 GetSortPos(ARecord record) 
	{
		Data d = record->get(IndexFieldCol());
		if (d.empty())
			return false;

		Int64 key = d;

		UInt64 i = 0;
		MultiIntUInt64IndexRecordList::iterator it = mHashRecordList.find((INDEX_TYPE)key);
		for (; it!=mHashRecordList.end(); ++it)
		{
			if (it->second==record)
			{
				i = 1;				
			}
			else
				++i;
		}
		return i;
	}

	virtual ARecord GetLastRecord() override
	{
		ARecordIt last = GetLastRecordIt();
		if (last)
			last->Record();
		//ERROR_LOG("MultipleUInt64MapIndex GetLastRecord is begin record");
		//auto it =  mHashRecordList.begin();
		//if (it!=mHashRecordList.end())
		//	return it->second;

		return ARecord();
	}

	virtual ARecord GetRandRecord()
	{
		if (mHashRecordList.empty())
			return ARecord();
		size_t num = mHashRecordList.size();
		uint pos = CRand::RandUInt(num-1);

		uint i=0;
		for (MultiIntUInt64IndexRecordList::iterator it=mHashRecordList.begin(); it!=mHashRecordList.end(); ++it, ++i)
		{
			if (i==pos)
				return it->second;
		}

		return ARecord();
	}

	virtual void ClearAll(tBaseTable *pOwnerTable)
	{		
		for (MultiIntUInt64IndexRecordList::iterator it=mHashRecordList.begin(); it!=mHashRecordList.end(); ++it)
		{
			ARecord r = it->second;
			if (r && r->GetTable()==pOwnerTable)
				r._free();			
		}
		mHashRecordList.clear();
	}

	public:
		virtual bool AppendRecord(ARecord scrRecord, bool bReplace)
		{
			Data d = scrRecord->get(IndexFieldCol());
			if (d.empty())
				return false;

			Int64 key = (Int64)(UInt64)d;
			if (bReplace)
			{
				while (RemoveRecord((Int64)key)){}
			}
			mHashRecordList.insert(MultiIntUInt64IndexRecordList::value_type((INDEX_TYPE)key, scrRecord));
			return true;
		}

		virtual bool RemoveRecord(ARecord record)
		{
			Data d = record->get(IndexFieldCol());
			if (d.empty())
				return false;

			INDEX_TYPE key = d;

			MultiIntUInt64IndexRecordList::iterator it = mHashRecordList.find(key);
			for (; it!=mHashRecordList.end(); ++it)
			{
				if (it->first!=key)
					break;

				if (it->second==record)
				{
					mHashRecordList.erase(it);
					return true;
				}
			}

			return false;
		}

		virtual bool MultipleKey() { return true; }

protected:
	MultiIntUInt64IndexRecordList		mHashRecordList;
};
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
class HashMultiUInt64IndexRecordIt : public RecordIt
{
protected:
	Auto<MultipleUInt64MapIndex>	mpOwnerIndex;

public:
	HashMultiUInt64IndexRecordIt(MultiIntUInt64IndexRecordList::iterator it, MultipleUInt64MapIndex *index)
	{
		mpOwnerIndex = index;
		
		mListIt = it;
		mEndIt = mpOwnerIndex->mHashRecordList.end();
	}

public:
	virtual ARecord GetRecord() 
	{
		if (mListIt!=mEndIt)
			return mListIt->second; 

		return ARecord();
	}
	virtual ARecord Begin();
	virtual bool operator  ++() 
	{ 
		if (mListIt!=mEndIt)
		{
			++mListIt; 
			return mListIt!=mEndIt;
		}
		return false; 
	}	
	virtual bool operator  --()
	{
		if (mListIt==mpOwnerIndex->mHashRecordList.begin())
		{
			mListIt = mpOwnerIndex->mHashRecordList.end();
			return false;
		}
		else if (mListIt==mpOwnerIndex->mHashRecordList.end())
			return false;

		--mListIt;			
		return true;
	}
	Auto<RecordIt> Copy()
	{
		return MEM_NEW HashMultiUInt64IndexRecordIt(mListIt, mpOwnerIndex.getPtr());
	}

	virtual operator bool () { return mListIt!=mEndIt; }
	virtual void erase();


protected:
	MultiIntUInt64IndexRecordList::iterator	mListIt;
	MultiIntUInt64IndexRecordList::iterator	mEndIt;
};
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

#	endif

#endif //_INCLUDE_MULTIPLEINT64MAPINDEX_H_