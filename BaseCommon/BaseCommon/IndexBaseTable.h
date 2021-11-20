/********************************************************************
	created:	2014/07/24
	created:	24:7:2014   17:57
	filename: 	E:\RemoteCode\BaseCommon\BaseCommon\IndexBaseTable.h
	file path:	E:\RemoteCode\BaseCommon\BaseCommon
	file base:	IndexBaseTable
	file ext:	h
	author:		Yang Wenge
	
	purpose:	�����������ܱ��
				����DB Server
				֧����������KEY����, �Լ������������ݸ���ʱ�Զ�ά��
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
	// ��������
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
	AutoIndex						mMainKeyIndex;		// ��������

};
//-------------------------------------------------------------------------
// ��������е��ֶ�, ���ֶ�һ������, ��������ֶμ����ڵı��, ֻ��ͨ������ ClearAll() ���ͷ�
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

	// ����Ƿ�Ϊ��ͬ���ֶ�
	virtual bool needCheck() const { return true; }
	virtual int GetCheckCode() const 
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
	mutable CheckType		mCheckCode;				//�ֶ�����Ϊ�ַ����Ĺ�ϣ����, ���ڴ�������У��
	mutable bool			mbChanged;
};
//-------------------------------------------------------------------------
// ���б�������ָ���ֶεı��, һ������DBʹ�ö�
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
// �������Ƶı���ų��ֶ��ӱ�
class SkipBaseTable : public NormalBaseTable
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
#endif