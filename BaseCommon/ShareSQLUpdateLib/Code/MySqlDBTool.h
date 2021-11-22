/********************************************************************
	created:	2013/09/07
	created:	7:9:2013   15:24
	filename: 	C:\Work\BaseCommon\BaseCommon\MySqlDBTool.h
	file path:	C:\Work\BaseCommon\BaseCommon
	file base:	MySqlDBTool
	file ext:	h
	author:		Yang WenGe
	
	purpose:	MySQL API 实现DB操作

	NOTE:		1 存储过程中, 已支持BLOB数据返回
				2 当前使用记录处理 BLOB字段支持: 
				3 调取到DataRecord 字段类型为 FIELD_DATA (AutoData)
				4 取出时使用结果集直接取出, 且分析长度
				5 保存时, 进行绑定二进制数据为参数
*********************************************************************/
#ifndef _INCLUDE_MYSQLDBTOOL_H_
#define _INCLUDE_MYSQLDBTOOL_H_

#include "ShareSQLUpdateLib.h"
#include "DBTool.h"
#include "BaseTable.h"

#define USE_MYSQL_API				1
#define MYSQL_TRY_RUN_TIME			(600)		// 10分钟进行一次检查连接

struct  st_mysql_bind;
//-------------------------------------------------------------------------
// 用于序列记录数据的数据数组缓存
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

	//防止长时间无操作断开, NOTE: 必须保证在MYSQL使用线程Process, 否则会出现异常DUMP
	virtual void Process() override;

	void CheckConnect();
	void Reconnect(){ if (!InitStart(mInitParam)) NOTE_LOG("重新连接失败"); }

public:
	virtual bool InitField(AutoField &hField, bool bMustAllField, Array<AString> *selectFieldList = NULL, Array<AString> *excludeFieldList = NULL);

	virtual AString GetFieldData();

	virtual bool LoadRecord(ARecord &hRecord);
	// 获取列数据到DataBuffer中, 然后再恢复到记录, 用于后台线程异步加载
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

	// 执行多条语句, 需要多次获取结果
	bool exeMultiSql(const AString &sqlString, bool bNeedLoadData, int *affectCount /*= NULL*/);

	// 存储过程中, 已支持BLOB数据返回
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

	// To sql and need data, 如果是自增长时, 需要把第一列索引数据设置未更新状态, bSaveDataField 是指保存支持使用共享内存池之外的类型字段, false时, 进行检查,并放弃不支持的字段
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