/********************************************************************
	created:	2013/09/07
	created:	7:9:2013   15:24
	filename: 	C:\Work\BaseCommon\BaseCommon\MySqlDBTool.h
	file path:	C:\Work\BaseCommon\BaseCommon
	file base:	MySqlDBTool
	file ext:	h
	author:		Yang WenGe
	
	purpose:	MySQL API ʵ��DB����

	NOTE:		1 �洢������, ��֧��BLOB���ݷ���
				2 ��ǰʹ�ü�¼���� BLOB�ֶ�֧��: 
				3 ��ȡ��DataRecord �ֶ�����Ϊ FIELD_DATA (AutoData)
				4 ȡ��ʱʹ�ý����ֱ��ȡ��, �ҷ�������
				5 ����ʱ, ���а󶨶���������Ϊ����
*********************************************************************/
#ifndef _INCLUDE_MYSQLDBTOOL_H_
#define _INCLUDE_MYSQLDBTOOL_H_

#include "ShareSQLUpdateLib.h"
#include "DBTool.h"
#include "BaseTable.h"

#define USE_MYSQL_API				1
#define MYSQL_TRY_RUN_TIME			(600)		// 10���ӽ���һ�μ������

struct  st_mysql_bind;
//-------------------------------------------------------------------------
// �������м�¼���ݵ��������黺��
struct SQLDataArray
{
public:
	Array<AutoData>	mDataArray;
	int mNowCount = 0;

public:
	void Reset()
	{
		mNowCount = 0;
	}

	AutoData NextData()
	{
		AutoData bufferData = mDataArray[mNowCount++];
		if (!bufferData)
		{
			bufferData = MEM_NEW DataBuffer(1024);
			mDataArray[mNowCount - 1] = bufferData;
		}
		else
			bufferData->clear(false);
		return bufferData;
	}
};
//-------------------------------------------------------------------------*/
#if USE_MYSQL_API
struct st_mysql;
struct st_mysql_res;
struct st_mysql_stmt;

class LoadDataCallBack;

class ShareSQLUpdateLib_Export  MySqlDBTool : public tDBTool
{
	struct MySQLBindData
	{
		AutoData  mData;
		unsigned long mLength;
	};

public:
	MySqlDBTool();
	~MySqlDBTool();

public:
	virtual bool InitStart(tNiceData &param);
	
	virtual void Stop();
	virtual void clearResult();

	bool SetCharacter();
public:
	// Load DB table, and all record
	AutoTable LoadDBTable(const char *szTableName, bool bLoadAllRecord = true);
	bool CreateDBTable(const char *szTableName, AutoField tableField, const char *szInfoData = "NONE");

	//��ֹ��ʱ���޲����Ͽ�, NOTE: ���뱣֤��MYSQLʹ���߳�Process, ���������쳣DUMP
	virtual void Process() override;

	void CheckConnect();
	void Reconnect(){ if (!InitStart(mInitParam)) NOTE_LOG("��������ʧ��"); }

public:
	virtual bool InitField(AutoField &hField, bool bMustAllField, Array<AString> *selectFieldList = NULL, Array<AString> *excludeFieldList = NULL);

	virtual AString GetFieldData();

	virtual bool LoadRecord(ARecord &hRecord);
	// ��ȡ�����ݵ�DataBuffer��, Ȼ���ٻָ�����¼, ���ں�̨�߳��첽����
	virtual int LoadRecordData(Array<DataBuffer> &resultDataList);

	virtual bool SaveRecord(BaseRecord *scrRecord, bool bUpdate, bool bTryInsert);

	virtual bool ExeByField(const char *szTable, AutoField &hRecord, const AString &whereString);

	// exe my sql string command, bNeedLoadData param is skip.
	virtual bool exeSql(const AString &sqlString, bool bNeedLoadData, int *affectCount = NULL, bool bNeedAutoCommit = true);

	virtual bool exeSql(const char *sqlString, bool bNeedLoadData, int *affectCount = NULL, bool bNeedAutoCommit = true);

	virtual bool exeSqlNeedLoad( const AString &sqlString )
	{
		return exeSql(sqlString, true);
	}

	virtual bool exeSql(const AString &sqlString, Array<AutoData> &paramDataList, int *pAffectCount = NULL);

	// ִ�ж������, ��Ҫ��λ�ȡ���
	bool exeMultiSql(const AString &sqlString, bool bNeedLoadData, int *affectCount /*= NULL*/);

	// �洢������, ��֧��BLOB���ݷ���
	virtual AutoNice ExeSqlFunction( const AString &sqlString, bool bAutoCommit);

	virtual AutoNice ExeSqlFunction( const AString &sqlString, Array<AutoData> &paramDataList, bool bAutoCommit = false );

	virtual SQL_RESULT LoadBlobData(const char *szTableName, const char *szFieldName, const char *szWhereCondition, DataStream *resultData);

	virtual bool SaveBlobData(const char *szTableName, const char *szFieldName, const char *szWhereCondition, DataStream *saveData, bool bTryInsert) override;

	virtual const char* _getErrorMsg();

	virtual const char* getErrorMsg(){ return mErrorInfo.c_str(); }

	virtual void Log(const char *szInfo, ...);

	virtual bool LoadData(int keyCol, int dataCol, LoadDataCallBack *loadCallBack, DataStream *pDestData = NULL);

public:
	bool _ExeStmt(const AString &sqlString, st_mysql_bind *pBindList, int &affectCount, bool bNeedAutoCommit, NiceData *pResultData = NULL);

	// To sql and need data, �����������ʱ, ��Ҫ�ѵ�һ��������������δ����״̬, bSaveDataField ��ָ����֧��ʹ�ù����ڴ��֮��������ֶ�, falseʱ, ���м��,��������֧�ֵ��ֶ�
    static bool _MakeSaveSqlData(AString &resultInsertSQL, SQLDataArray &resultData, BaseRecord *scrRecord, bool bInsert, bool bGrownInsert = false, const char *szTableName = NULL, bool bSaveOnlyShareMemField = false);

protected:
	bool _SaveBlobData( const AString &sqlString, DataStream *saveData );

	SQL_RESULT _LoadBlobData( const AString &sqlString, DataStream *resultData );


	void _LogSqlError(const AString &sqlString, st_mysql_stmt *pStamt);

	bool _getDataResult(st_mysql_stmt *mStmt, NiceData *resultNice);

	void _ClearMysqlResult(st_mysql_res *pResult, st_mysql_stmt *mStmt);

	void _SetAutoCommit(bool bAuto);

protected:
	st_mysql						*mMySql;
	st_mysql_res				*mResult;
	AString							mErrorInfo;

	NiceData						mInitParam;

	UInt64							mLastCheckTime;
	SQLDataArray			mTempBuffer;
};
//-------------------------------------------------------------------------*/
#endif
#endif //_INCLUDE_MYSQLDBTOOL_H_