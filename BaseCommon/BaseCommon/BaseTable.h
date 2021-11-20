/********************************************************************
	created:	2014/07/24
	created:	24:7:2014   10:37
	filename: 	E:\RemoteCode\BaseCommon\BaseCommon\BaseTable.h
	file path:	E:\RemoteCode\BaseCommon\BaseCommon
	file base:	BaseTable
	file ext:	h
	author:		Yang Wenge
	
	purpose:	DB �������, ��Ҫ���ܹ����¼����¼����
*********************************************************************/
#ifndef _INCLUDE_BASETABLE_H_
#define _INCLUDE_BASETABLE_H_

#include "Auto.h"
#include "Hand.h"
#include "BaseRecord.h"
#include "RecordIndex.h"
#include "DBResultCallBack.h"
#include "LoadDBCallBack.h"
//-------------------------------------------------------------------------
// ����¼�ڴ�ؼ�¼������֧�� 32 * 0xFFFE, DBServerָ��Ϊ1000��Լ6500��
#define RECORD_BLOCK_INIT_NUM		(32)		
//-------------------------------------------------------------------------*/

//-------------------------------------------------------------------------
class tBaseTable;
// ��ȡ���ر�񷽷�,���ڿͻ��˴���Դ�������Դ���ж�ȡ��� �� �����ݿ��ж�ȡ
class TableLoader : public AutoBase
{
public:
	virtual ~TableLoader(){}
	virtual bool LoadTable(tBaseTable *pTable, DataStream *scrData) = 0;
	virtual bool SaveTable(tBaseTable *pTable, DataStream *resultData, const char *szIndexField = NULL) = 0;
};

typedef Auto<TableLoader>	AutoLoader;
//-------------------------------------------------------------------------*/
// ������¼�������м��, Ŀ���Ǳ��Ҳ����������󣬲����ü�¼�������������
class TablePtr : public AutoBase
{
public:
	AutoField	mField;
	tBaseTable	*mpTable;

	TablePtr() : mpTable(NULL) {}
};
typedef Auto<TablePtr> ATablePtr;
//-------------------------------------------------------------------------*/
struct TableCommonConfig
{
public:
	int		mRecordMemPoolBlockRecordCount;

	TableCommonConfig()
		: mRecordMemPoolBlockRecordCount(RECORD_BLOCK_INIT_NUM){	}
};
//-------------------------------------------------------------------------
// �ڴ���
//-------------------------------------------------------------------------
class BaseCommon_Export tBaseTable : public AutoBase
{
public:
	static Auto<tBaseTable> NewBaseTable(bool bUseRecordPool = false);
	static Auto<tBaseTable> NewFieldTable(bool bCreateField = true);
	static void SetCommonConfig(const TableCommonConfig &config){ msCommonConfig = config; }

	Data GetValue(const char* recordkey, const char *szFieldName);
	Data GetValue(int recordkey, const char *szFieldName);

	Data GetValue(const char* recordkey, int field);
	Data GetValue(int recordkey, int field);

public:
	virtual ARecord GetRecord(Int64 nIndex) = 0;
	virtual ARecord GetRecord(float fIndex) = 0;
	virtual ARecord GetRecord(const char* szIndex) = 0;
	virtual ARecord GetRecord(int nIndex) { return GetRecord((Int64)nIndex); }
	virtual ARecord GetRecord(UInt64 nIndex) { return GetRecord((Int64)nIndex); }
	virtual ARecord GetRecord(const AString &strKey){ return GetRecord(strKey.c_str()); } 

	virtual ARecord CreateRecord(Int64 nIndex, bool bReplace) = 0;
	virtual ARecord CreateRecord(float fIndex, bool bReplace) = 0;
	virtual ARecord CreateRecord(const char* szIndex, bool bReplace) = 0;
	virtual ARecord CreateRecord(const AString &strIndex, bool bReplace) { return CreateRecord(strIndex.c_str(), bReplace); }
	virtual ARecord CreateRecord(int nIndex, bool bReplace) { return CreateRecord((Int64)nIndex, bReplace); }

    // bRestore : true TableDataLoader:LoadTable ʱ, �ָ���, ͬ�����ĵ�����DB�ӱ�
	virtual bool AppendRecord(ARecord scrRecord, bool bReplace, bool bRestore = false) = 0;
	virtual void InsertDBNewRecord(ARecord newRecord)  {}

	virtual bool RemoveRecord(Int64 nIndex) = 0;
	virtual bool RemoveRecord(float fIndex) = 0;
	virtual bool RemoveRecord(const char* szIndex) = 0;
	virtual bool RemoveRecord(int nIndex) { return RemoveRecord((Int64)nIndex); }

	virtual bool RemoveRecord(ARecord record) = 0;	

	// ɾ��DB���ݿ��еļ�¼, ��ֻ��ͨ���˹���ɾ��
	virtual bool DeleteRecord(ARecord record) = 0;

	virtual ARecord NewRecord() = 0;
	virtual ARecord _NewRecord() = 0;

public:
	virtual ARecordIt GetRecordIt() = 0;
	virtual ARecordIt GetRecordIt(ARecord targetRecord) = 0;
	virtual AutoIndex GetMainIndex() = 0;

	virtual const char* GetTableType() = 0;

public:
	virtual bool Save(TableLoader *pLoader, DataStream *resultStream);

	virtual bool Load(TableLoader *pLoader, DataStream *scrDatao);
    
	virtual bool SaveCSV(const char *szCSVFilePathName, bool bTabDelimited = false);
	virtual bool LoadCSV(const char *szCSVFilePathName, bool bTabDelimited = false);

	virtual bool SaveData(DataStream *resultStream);
	virtual bool LoadFromData(DataStream *scrStream);

	// NOTE: Ϊ�˱�������ݺ�C#һ��, ͳһʹ���ַ������򷽷�����
	virtual bool SortSaveData(DataStream *destData, bool bSaveTableIndex = false);

	// �����ӱ�, ɾ����¼��, ������뵽ɾ���б�, ��ͬ�����л�����ʱ, ����ɾ����Ϣ
	virtual bool SaveDeleteInfo(DataStream *resultStream, bool bClearDeleteList){ return true; }
	virtual bool ApplyDeleteInfo(DataStream *scrData){ return true; }
	virtual bool HasDeleteRecord(){ return false; }
	virtual void ClearDeleteRecordList(){}

	virtual void SaveRecordData(const char *szKey, AutoNice recordData, bool bNewInsert, DBResultCallBack callBack){ ERROR_LOG("δʵ��[SaveRecordData]"); callBack(NULL, false); }
	virtual void SaveRecordData(KEY nKey, AutoNice recordData, bool bNewInsert, DBResultCallBack callBack){  SaveRecordData(STRING(nKey), recordData, bNewInsert, callBack); }		
	virtual void LoadRecordData(const char *szKey, AString fieldInfo, DBResultCallBack callBack){ ERROR_LOG("δʵ��[LoadRecordData]"); callBack(NULL, false); }
	virtual void LoadRecordData(KEY nKey, AString fieldInfo, DBResultCallBack callBack){  LoadRecordData(STRING(nKey), fieldInfo, callBack); }		

	virtual bool NeedTrySaveRecord() const { return false; }
    // NOTE: ����DB�ӱ�ʱ���ټ��ÿ����¼�ĸ���״̬
    virtual bool NeedCheckRecordUpdateState() const { return true; }

	// �����첽���µ�ȡDB��¼
	virtual bool CheckRecord(const char *szKey, ARecord &targetRe){ WARN_LOG("һ������Ҫִ��CheckRecord"); targetRe = GetRecord(szKey); return targetRe; } 
	virtual void ReloadRecord(const char *szKey, ARecord targetRe, LoadDBCallBack callBack){}

public:
	virtual bool InitField(AutoField scrField);
	virtual void Init();
	virtual AutoField GetField() const { return mOwnerPtr->mField; }
	virtual void ReadyField(){ if (mOwnerPtr->mField) mOwnerPtr->mField->_updateInfo(); }
	virtual FieldInfo AppendField(const char *szFieldName, FIELD_TYPE type){ return SetField(szFieldName, type, GetField()->getCount());}
	virtual FieldInfo AppendField(const char *szFieldName, const char *szType){ return SetField(szFieldName, szType, GetField()->getCount());}
	virtual FieldInfo SetField(const char *szFieldName, FIELD_TYPE type, int col);

	virtual FieldInfo SetField(const char *szFieldName, const char *szType, int col)
	{
		return SetField(szFieldName, (FIELD_TYPE)FieldInfoManager::GetMe().toFieldType(szType), col);
	}
	bool SetFieldTable( const char *szField, tBaseTable *pSubTable );

	virtual void OnFieldChangeBefore(){	}
	virtual void OnFieldChanged() { }
    virtual bool OnCreateDBBefore(){ return true; }
    virtual void OnCreateDBFinish(){ }
    virtual bool UseBackups() const { return false; }
    virtual void  SetUseBackups(bool bUse){  }

	virtual bool SetMainIndex(int indexCol, bool bHash, bool bMultiple = false){ return false; }
	virtual bool SetMainIndex(const char *szFieldName, bool bHash, bool bMultiple = false){ return false; }
	virtual int  GetMainIndexCol();

	virtual bool SetIndexField(const char *szFieldName, bool bHash){ return false; }
	virtual AutoIndex GetIndex(const char *szFieldName){ return AutoIndex(); }
	virtual AString GetIndexFieldsInfo(){ return AString(); }

	virtual void OnRecordDataChanged(ARecord re, int col, Int64 scrValue) = 0;
	virtual void OnRecordDataChanged(ARecord re, int col, const char *scrValue) = 0;
    virtual void OnAppendRecord(BaseRecord *scrRecord, bool bNew){}

	virtual bool SetSkipUpdateField(const char *szFieldName, bool bSkip);
	virtual bool SetSkipUpdateField(int nFieldCol, bool bSkip){ return false; }	

	virtual void CheckUpdate(){}

public:
	tBaseTable();		
	virtual ~tBaseTable();

public:
	virtual bool Empty(){ return !GetMainIndex() || GetMainIndex()->GetCount()<=0; }
	virtual int GetRecordCount(){ if (!GetMainIndex()) return 0; return (int)GetMainIndex()->GetCount(); }
	virtual void ClearAll(){ /*mField.setNull();*/ }
	virtual const char* GetTableName() const { return "None"; }
	virtual void SetTableName(const char *szTableName){ WARN_LOG("���ܱ�������������,����Ϊ�˽�ʡ�ռ�"); }

	virtual bool InKeyRange(const char *key){ return false;}

	virtual ARecord GrowthNewRecord(DataStream *recordData=NULL){ return ARecord(); }

	virtual Int64 MaxRecordIndexID() { if (GetMainIndex()) return GetMainIndex()->GetNextGrowthKey()-1; return 0; }

public:
	virtual void Process(){}
	ATablePtr GetTablePtr() { return mOwnerPtr; }
	Auto<tBaseTable> GetSelf() { return Auto<tBaseTable>(this); }

	virtual void OnGenerateCode(AString &codeString){}

	virtual void Close() {}

protected:
	ATablePtr			mOwnerPtr;
	
#if SAVE_DEBUG_INFO
	static __declspec(thread) int msCount;
#endif

public:
	static TableCommonConfig msCommonConfig;

};

//-------------------------------------------------------------------------
typedef Auto<tBaseTable>	AutoTable;
//-------------------------------------------------------------------------

struct TableIt
{
public:
	ARecordIt	mRecordIt;
	AutoTable	mTable;

public:
	operator bool () { return _Have(); }
	bool operator  ++() { return _Next(); }
	bool operator  ++(int) { return ++(*this); }
	ARecord operator *() { return getCurrRecord(); }

public:
	TableIt(AutoTable table)
	{
		mTable = table;
		if (table)
			mRecordIt = table->GetRecordIt();
	}

	bool _Have()
	{
		if (mRecordIt)
			return *mRecordIt;

		return false;
	}

	bool _Next()
	{
		if (mRecordIt)
		{
			return ++(*mRecordIt);
		}
		return false;
	}

	ARecord getCurrRecord()
	{
		if (mRecordIt)
			return *mRecordIt;
		return ARecord();
	}

	bool RemoveRecord()
	{
		if (mRecordIt)
		{
			mRecordIt->erase();
			return true;
		}
		return false;
	}
};
//-------------------------------------------------------------------------*/
#endif //_INCLUDE_BASETABLE_H_