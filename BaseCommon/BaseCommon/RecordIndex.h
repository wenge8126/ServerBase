/********************************************************************
	created:	2014/07/24
	created:	24:7:2014   10:59
	filename: 	E:\RemoteCode\BaseCommon\BaseCommon\RecordIndex.h
	file path:	E:\RemoteCode\BaseCommon\BaseCommon
	file base:	RecordIndex
	file ext:	h
	author:		Yang Wenge
	
	purpose:	��¼����, Ҳ������Ϊ��¼����, ���� �����Ͷ��
*********************************************************************/
#ifndef _INCLUDE_RECORDINDEX_H_
#define _INCLUDE_RECORDINDEX_H_

#include "BaseCommon.h"
#include "Auto.h"
#include "BaseRecord.h"

#include "RandTool.h"

#define		INT_INDEX_TYPE		Int64
//-------------------------------------------------------------------------
// ��¼����
// ������¼ for (ARecord r, ARecordIt it = t.GetRecordIt(); *it; r = ++it) 
// NOTE: r may be NULL
//-------------------------------------------------------------------------
class RecordIndex;
class tBaseTable;
class RecordIt : public AutoBase
{
protected:
    virtual ARecord GetRecord() = 0;

public:
	virtual ARecord Begin() = 0;
	virtual bool operator  ++() = 0;
	virtual bool operator  ++(int) { return ++(*this); }

	virtual bool operator  --() = 0;
	virtual bool operator  --(int) { return --(*this); }

	virtual operator bool () = 0;
	virtual operator ARecord () { return Record(); }

	virtual void erase() = 0;

	virtual Auto<RecordIt> Copy() = 0;

	virtual bool GetKey(AString &key) { return false; }

    virtual ARecord Record()
    {
        ARecord r = GetRecord();
        if (!r && r.getPtr()!=NULL)
        {
			r->ReloadByIterator(this);
        }
        return r;
    }

    // NOTE: ֻ����CoolTable �ڲ�ʹ��, ��������
    virtual ARecord _record() { return GetRecord(); }
};
typedef Auto<RecordIt>	ARecordIt;
//-------------------------------------------------------------------------
class BaseCommon_Export_H RecordIndex : public AutoBase // Base<RecordIndex>
{
	friend class tBaseTable;
	friend class BaseTable;
	friend class MemoryDBTable;
	friend class MemoryDBIndexTable;

public:
	virtual ARecord GetRecord(Int64 nIndex) = 0;
	virtual ARecord GetRecord(float fIndex) = 0;
	virtual ARecord GetRecord(const char* szIndex) = 0;
	virtual ARecord GetRecord(const AString &str) { return GetRecord(str.c_str()); }
	virtual ARecord GetRecord(const Data &nIndex) = 0;
	virtual ARecord GetRecord(int nIndex) { return GetRecord((Int64)nIndex); }

	virtual bool ExistRecord(Int64 nIndex) = 0;
	virtual bool ExistRecord(float fIndex) = 0;
	virtual bool ExistRecord(const char* szIndex) = 0;

	virtual bool InsertRecord(ARecord scrRecord) = 0;

public:
	virtual bool InsertLast(ARecord scrRecord)
	{
		ERROR_LOG("ERROR: ��ǰ����[%s]��֧������������¼", typeid(*this).name());
		return false; 
	}

	virtual bool RemoveRecord(float fIndex) = 0;
	virtual bool RemoveRecord(const char* szIndex) = 0;
	virtual bool RemoveRecord(Int64 nIndex) = 0;

	virtual bool RemoveRecord(ARecord record) = 0;	
	virtual bool RemoveRecord(const AString &str) { return RemoveRecord(str.c_str()); }

public:
	// ����ȷ��ɾ�����������ָ����¼
	virtual bool RemoveRecord(Int64 nIndex, ARecord record)
	{
		ARecord r = GetRecord(nIndex);
		if (r==record)
			return RemoveRecord(nIndex);
		return false;
	}
	virtual bool RemoveRecord(const char *nIndex, ARecord record)
	{
		ARecord r = GetRecord(nIndex);
		if (r==record)
			return RemoveRecord(nIndex);
		return false;
	}
	
public:
	// ������¼ for (ARecord r = t.Begin(); t; r = ++t) NOTE: r may be NULL
	virtual ARecordIt GetRecordIt() = 0;
	virtual ARecordIt GetRecordIt(ARecord targetRecord) = 0;
	virtual ARecordIt GetLastRecordIt() = 0;

	virtual bool MultipleKey() { return false; }
	virtual int IndexFieldCol() const { return mIndexCol; } //mIndexFieldName.c_str(); }
	virtual void SetIndexField(const char *szFieldName, int indexCol){ mIndexFieldName = szFieldName; mIndexCol = indexCol;}

	virtual UInt64 GetCount() = 0;

	// ���شӴ�С������λ��
	virtual UInt64 GetSortPos(ARecord record) 
	{
		AssertEx(0, "WARN: ʹ����δʵ�ֵĹ��� GetSqrtPos");
		return NULL_POS; 
	}

	virtual bool IsStringHashKey() { return false; }
	virtual bool IsMultiple() const { return false; }

	virtual Int64 GetNextGrowthKey(){ return 0; }
	virtual ARecord GetLastRecord() = 0;

	virtual ARecord GetRandRecord() = 0;

	virtual void ClearAll(tBaseTable *pOwnerTable) = 0;

public:
	RecordIndex():mIndexCol(-1){}
	virtual ~RecordIndex() {}

	Auto<RecordIndex> GetSelf() { return Auto<RecordIndex>(this); }

public:
	AString		mIndexFieldName;

protected:
	short				mIndexCol;
};

typedef Auto<RecordIndex>	AutoIndex;
//-------------------------------------------------------------------------
#endif //_INCLUDE_RECORDINDEX_H_