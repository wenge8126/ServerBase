/********************************************************************
	created:	2014/07/27
	created:	27:7:2014   21:50
	filename: 	H:\RemoteGame\BaseCommon\ServerBase\DBWork\StringHashIndex.h
	file path:	H:\RemoteGame\BaseCommon\ServerBase\DBWork
	file base:	StringHashIndex
	file ext:	h
	author:		Yang Wenge
	
	purpose:	ʹ��STL��hashmap, Ŀǰֻ�����ڴ�����ַ���������

	˵��:		����Hash�ַ�����¼����, ��KEYֵ�ڴ��ƶ��ͷ��俽��ʱ���ʱ����
	ԭ��:		1 ֱ��ʹ�ö�KEY�������������ϣ�ַ�����Ӧ�ļ�¼
				2 ����ʱ, ���ҵ�KEY��ͬ��Ӧ�ĵ�һ����¼, Ȼ����������ٴ��ж��ַ���KEY��ȵļ�¼
*********************************************************************/
#ifndef _INCLUDE_STRINGHASHINDEX_H_
#define _INCLUDE_STRINGHASHINDEX_H_

#include "RecordIndex.h"
#include <unordered_map>

//using namespace std;

typedef std::unordered_map<AString, ARecord, AStringHash>		HashStringIndexRecordList;

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
class BaseCommon_Export StringHashIndex : public RecordIndex
{
	friend class HashStringIndexRecordIt;
	friend class Right_HashStringIndexRecordIt;

public:
	virtual ARecord GetRecord(Int64 nIndex) 
	{		
		return GetRecord(STRING(nIndex));
	}

	virtual ARecord GetRecord(float fIndex)
	{
		return GetRecord(STRING(fIndex));
	}
	//NOTE: ���ҵ���һ��, Ȼ��������ж��ַ���KEY
	virtual ARecord GetRecord(const char* szIndex)
	{		
		HashStringIndexRecordList::iterator it = mHashRecordList.find(szIndex);
		if (it!=mHashRecordList.end())
			return it->second;

		return ARecord();
	}	

	virtual ARecord GetRecord(const Data &nIndex)
	{
		return GetRecord(nIndex.string().c_str());
	}

	virtual bool ExistRecord(Int64 nIndex) { return ExistRecord(STRING(nIndex)); }
	virtual bool ExistRecord(float fIndex) { return ExistRecord(STRING(fIndex)); }
	virtual bool ExistRecord(const char* szIndex) { return GetRecord(szIndex); }

protected:
	virtual bool InsertRecord(ARecord scrRecord)
	{
		Data d = scrRecord->get(IndexFieldCol());
		if (d.empty())
			return false;

		AString key = d.string();
		
		std::pair<HashStringIndexRecordList::iterator, bool> p = mHashRecordList.insert(HashStringIndexRecordList::value_type(key, scrRecord));
		return p.second;
	}

	virtual bool RemoveRecord(Int64 nIndex)
	{
		return RemoveRecord(STRING(nIndex));
	}
	virtual bool RemoveRecord(float fIndex)
	{
		return RemoveRecord(STRING(fIndex));
	}
	virtual bool RemoveRecord(const char* szIndex)
	{		
		HashStringIndexRecordList::iterator it = mHashRecordList.find(szIndex);
		if (it!=mHashRecordList.end())
		{
			mHashRecordList.erase(it);
			return true;
		}
		return false;
	}
	virtual bool RemoveRecord(const AString &str) { return RemoveRecord(str.c_str()); }
	virtual bool RemoveRecord(ARecord record)
	{
		Data d = record->get(IndexFieldCol());
		if (d.empty())
			return false;

		return RemoveRecord(d.string());
	}

public:
	virtual ARecordIt GetRecordIt();

	virtual ARecordIt GetRecordIt(ARecord targetRecord);

	virtual ARecordIt GetLastRecordIt();

	virtual UInt64 GetCount(){ return mHashRecordList.size(); }
	virtual bool IsStringHashKey() { return true; }

	virtual Int64 GetNextGrowthKey()
	{
		AssertNote(0, "�ַ�����֧��������");
		return 0; 
	}

	virtual ARecord GetLastRecord()
	{
		ERROR_LOG("StringHashIndex GetLastRecord is begin record");
		auto it =  mHashRecordList.begin();
		if (it!=mHashRecordList.end())
			return it->second;

		return ARecord();
	}

	virtual ARecord GetRandRecord()
	{
		if (mHashRecordList.empty())
			return ARecord();
		size_t num = mHashRecordList.size();
		uint pos = CRand::RandUInt(num-1);

		uint i=0;
		for (HashStringIndexRecordList::iterator it=mHashRecordList.begin(); it!=mHashRecordList.end(); ++it, ++i)
		{
			if (i==pos)
				return it->second;
		}

		return ARecord();
	}

	virtual void ClearAll(tBaseTable *pOwnerTable)
	{		
		for (HashStringIndexRecordList::iterator it=mHashRecordList.begin(); it!=mHashRecordList.end(); ++it)
		{
			ARecord r = it->second;
			if (r && r->GetTable()==pOwnerTable)
				r._free();			
		}
		mHashRecordList.clear();
	}

protected:
	HashStringIndexRecordList		mHashRecordList;
};
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
class StringHashIndex;
class HashStringIndexRecordIt : public RecordIt
{
	Auto<StringHashIndex> mpOwnerIndex;

public:
	HashStringIndexRecordIt(HashStringIndexRecordList::iterator it, StringHashIndex  *pIndex);

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
# if 0
		if (mListIt==mpOwnerIndex->mHashRecordList.begin())
		{
			mListIt = mpOwnerIndex->mHashRecordList.end();
			return false;
		}
		else if (mListIt==mpOwnerIndex->mHashRecordList.end())
			return false;

		--mListIt;			
		return true;
#else
		AssertNote(0, "Cannot call --");
		return false;
#endif
	}
	Auto<RecordIt> Copy()
	{
		return MEM_NEW HashStringIndexRecordIt(mListIt, mpOwnerIndex.getPtr());
	}

	virtual operator bool () { return mListIt!=mEndIt; }
	virtual void erase();

	virtual bool GetKey(AString &key) override 
	{ 
		if (mListIt != mEndIt)
		{
			key = mListIt->first;
			return true;
		}
		return false;
	}

protected:
	HashStringIndexRecordList::iterator	mListIt;
	HashStringIndexRecordList::iterator	mEndIt;
};

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

#endif //_INCLUDE_STRINGHASHINDEX_H_