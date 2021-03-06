/********************************************************************
	created:	2014/07/24
	created:	24:7:2014   17:57
	filename: 	E:\RemoteCode\BaseCommon\BaseCommon\IndexBaseTable.h
	file path:	E:\RemoteCode\BaseCommon\BaseCommon
	file base:	IndexBaseTable
	file ext:	h
	author:		Yang Wenge
	
	purpose:	具有索引功能表格
				用于DB Server
				支持主键索引KEY更改, 以及其他索引数据更改时自动维护
*********************************************************************/
//-------------------------------------------------------------------------
#ifndef _INCLUDE_INDEXBASETABLE_H_
#define _INCLUDE_INDEXBASETABLE_H_

#include "BaseTable.h"


//-------------------------------------------------------------------------
class BaseCommon_Export BaseTable : public tBaseTable
{
public:
	virtual AutoIndex NewRecordIndex(FIELD_TYPE indexKeyType, bool bHash, bool bMultKey);
	virtual const char* GetTableType(){ return "Table"; }

public:
	BaseTable(INIT_FIELD_MODE initMode);
	~BaseTable();

public:
	// 设置索引
	virtual bool SetMainIndex(int indexCol, bool bHash, bool bMultiple = false) override;
	virtual bool SetMainIndex(const char *szFieldName, bool bHash, bool bMultiple = false) override;

	virtual void OnRecordDataChanged(ARecord re, int col, Int64 scrValue);
	virtual void OnRecordDataChanged(ARecord re, int col, const char *scrValue);

public:
	virtual ARecord GetRecord(Int64 nIndex)
	{
		if (mMainKeyIndex)
			return mMainKeyIndex->GetRecord(nIndex);
		return ARecord();
	}
	virtual ARecord GetRecord(float fIndex)
	{
		if (mMainKeyIndex)
			return mMainKeyIndex->GetRecord(fIndex);	

		return ARecord();
	}
	virtual ARecord GetRecord(const char* szIndex)
	{
		if (mMainKeyIndex)
			return mMainKeyIndex->GetRecord(szIndex);

		return ARecord();
	}

	virtual ARecord CreateRecord(Int64 nIndex, bool bReplace);
	virtual ARecord CreateRecord(float fIndex, bool bReplace);
	virtual ARecord CreateRecord(const char* szIndex, bool bReplace);

	virtual bool ExistRecord(Int64 nIndex) { return mMainKeyIndex->ExistRecord(nIndex); }
	virtual bool ExistRecord(float fIndex) { return mMainKeyIndex->ExistRecord(fIndex); }
	virtual bool ExistRecord(const char* szIndex) { return mMainKeyIndex->ExistRecord(szIndex); }
	virtual bool AppendRecord(ARecord scrRecord, bool bReplace, bool bRestore = false) override;

	virtual bool RemoveRecord(float fIndex);
	virtual bool RemoveRecord(const char* szIndex);
	virtual bool RemoveRecord(Int64 nIndex);
	virtual bool RemoveRecord(ARecord record);
	virtual bool DeleteRecord(ARecord record);

	virtual ARecord NewRecord();
	virtual ARecord _NewRecord() override;
	virtual ARecord GrowthNewRecord( DataStream *recordData );

	virtual void OnReplaceRecord(ARecord scrRe, ARecord destRe){}

public:
	virtual ARecordIt GetRecordIt();	
	virtual ARecordIt GetRecordIt(ARecord targetRecord) 
    { 
        if (!targetRecord)
            return ARecordIt();
        if (mMainKeyIndex)  return mMainKeyIndex->GetRecordIt(targetRecord); return ARecordIt(); 
    }

	virtual AutoIndex GetMainIndex()  override { return mMainKeyIndex; }
	virtual int GetMainIndexCol() override {  if (mMainKeyIndex) return mMainKeyIndex->IndexFieldCol();  return -1; }

	virtual void ClearAll();

	virtual void Init() override;

protected:
	AutoIndex						mMainKeyIndex;		// 主键索引

};
//-------------------------------------------------------------------------
// 索引表格中的字段, 此字段一量存在, 则会锁死字段及所在的表格, 只有通过表格的 ClearAll() 来释放
typedef int CheckType;
class BaseCommon_Export BaseFieldIndex : public FieldIndex
{
public:
	BaseFieldIndex()
		: FieldIndex(NULL, 0)
		, mCheckCode(0)
		, mbChanged(false)
	{

	}
public:
	virtual void _updateInfo()
	{
		FieldIndex::_updateInfo();
		mbChanged = true;
	}

	// 检查是否为相同的字段
	virtual bool needCheck() const { return true; }
	virtual int GetCheckCode() const override
	{
		if (mCheckCode==0 || mbChanged)
		{
			mCheckCode = _generateCode();
			mbChanged = false;
		}
		return mCheckCode; 
	}
	virtual bool CheckSame(int checkCode) const
	{ 
		if (mCheckCode==0 || mbChanged)
		{
			mCheckCode = _generateCode();
			mbChanged = false;
		}
		return mCheckCode==checkCode; 
	}

	int _generateCode() const;

public:
	mutable CheckType		mCheckCode;				//字段序列为字符串的哈希数字, 用于传送数据校验
	mutable bool			mbChanged;
};
//-------------------------------------------------------------------------
// 具有保留更新指定字段的表格, 一般用在DB使用端
//-------------------------------------------------------------------------
class BaseCommon_Export NormalBaseTable : public BaseTable
{
public:
	NormalBaseTable(INIT_FIELD_MODE initMode)
		: BaseTable(initMode){}

public:
	virtual const char* GetTableType(){ return "SkipTable"; }
	virtual bool SetSkipUpdateField(int nFieldCol, bool bSkip);

};
//-------------------------------------------------------------------------
// 具有名称的表格，排除字段子表
class BaseCommon_Export_H SkipBaseTable : public NormalBaseTable
{
public:
	SkipBaseTable(INIT_FIELD_MODE initMode)
		: NormalBaseTable(initMode)
	{
		mTableName = "DEFAULT"; 
	}

public:
	virtual const char* GetTableName() const override { return mTableName.c_str(); }
	virtual void SetTableName(const char *szTableName) override { mTableName = szTableName; }

protected:
	AString			mTableName;
};
//-------------------------------------------------------------------------*
// 只保存表结构, 不保存索引记录的表, 用于创建自由使用的记录
// 记录中保存此表格的AutoTable, 直到所有创建的记录都释放后,才会释放此表格
//-------------------------------------------------------------------------
class BaseCommon_Export StructBaseTable : public tBaseTable
{
public:
	StructBaseTable(INIT_FIELD_MODE initMode = eInitCreateField)
		: tBaseTable() 
	{
		if (initMode==eInitCreateField)
			mOwnerPtr->mField = MEM_NEW  BaseFieldIndex();
	}

public:

	virtual ARecord GetRecord(Int64 nIndex) { AssertEx(0, "Can not use StructBaseTable::GetRecord"); return ARecord(); }
	virtual ARecord GetRecord(float fIndex) { AssertEx(0, "Can not use StructBaseTable::GetRecord"); return ARecord(); }
	virtual ARecord GetRecord(const char* szIndex) { AssertEx(0, "Can not use StructBaseTable::GetRecord"); return ARecord(); }
	virtual ARecord GetRecord(int nIndex) { AssertEx(0, "Can not use StructBaseTable::GetRecord"); return ARecord(); }
	virtual ARecord GetRecord(UInt64 nIndex) { AssertEx(0, "Can not use StructBaseTable::GetRecord"); return ARecord(); }
	virtual ARecord GetRecord(const AString &strKey) { AssertEx(0, "Can not use StructBaseTable::GetRecord"); return ARecord(); }

	virtual ARecord CreateRecord(Int64 nIndex, bool bReplace)
	{
		ARecord re = _NewRecord();
		re->set(0, nIndex);
		return re;
	}
	virtual ARecord CreateRecord(float fIndex, bool bReplace) 
	{
		ARecord re = _NewRecord();
		re->set(0, fIndex);
		return re;
	}
	virtual ARecord CreateRecord(const char* szIndex, bool bReplace) 
	{
		ARecord re = _NewRecord();
		re->set(0, szIndex);
		return re;
	}
	virtual ARecord CreateRecord(const AString &strIndex, bool bReplace) { return CreateRecord(strIndex.c_str(), bReplace); }
	virtual ARecord CreateRecord(int nIndex, bool bReplace) { return CreateRecord((Int64)nIndex, bReplace); }

	// bRestore : true TableDataLoader:LoadTable 时, 恢复后, 同步更改到独立DB子表
	virtual bool AppendRecord(ARecord scrRecord, bool bReplace, bool bRestore = false) override { return true; }

	virtual bool RemoveRecord(Int64 nIndex) { AssertEx(0, "Can not use StructBaseTable::RemoveRecord"); return false; }
	virtual bool RemoveRecord(float fIndex) { AssertEx(0, "Can not use StructBaseTable::RemoveRecord"); return false; }
	virtual bool RemoveRecord(const char* szIndex) { AssertEx(0, "Can not use StructBaseTable::RemoveRecord"); return false; }
	virtual bool RemoveRecord(int nIndex) { AssertEx(0, "Can not use StructBaseTable::RemoveRecord"); return false; }

	virtual bool RemoveRecord(ARecord record) { AssertEx(0, "Can not use StructBaseTable::RemoveRecord"); return false; }

	// 删除DB数据库中的记录, 且只能通过此功能删除
	virtual bool DeleteRecord(ARecord record) { AssertEx(0, "Can not use StructBaseTable::DeleteRecord"); return false; }

	virtual ARecord NewRecord() { return _NewRecord(); }
	virtual ARecord _NewRecord() override;

public:
	virtual ARecordIt GetRecordIt() { AssertEx(0, "Can not use StructBaseTable::GetRecordIt"); return ARecordIt(); }

	virtual ARecordIt GetRecordIt(ARecord targetRecord) { AssertEx(0, "Can not use StructBaseTable::GetRecordIt"); return ARecordIt(); }
	virtual AutoIndex GetMainIndex() { AssertEx(0, "Can not use StructBaseTable::GetMainIndex"); return AutoIndex(); }

	virtual const char* GetTableType() override { return "StructTable"; }

	virtual void OnRecordDataChanged(ARecord re, int col, Int64 scrValue) {}
	virtual void OnRecordDataChanged(ARecord re, int col, const char *scrValue) {}

};
//-------------------------------------------------------------------------
#endif