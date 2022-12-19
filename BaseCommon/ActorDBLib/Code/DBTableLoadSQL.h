#pragma once

#ifndef _INCLUDE_DBDATABUFFER_H_
#define _INCLUDE_DBDATABUFFER_H_

#include "EasyHash.h"
#include "MySqlDBTool.h"
#include "IndexDBRecord.h"
#include "IndexBaseTable.h"
#include "SQLThread.h"
#include "ActorDBLib.h"
#include "Component.h"

namespace NetCloud
{
	class DBTableManager;
	class DBTableLoadSQL : public Component
	{
	public:
		DBTableLoadSQL()
			: mbStringKey(false)
		{}

	public:
		virtual bool SaveRecord(DBTableManager *pDBMgr, BaseRecord *record, bool bInsert) { AssertNote(0, "No override SaveRecord");  return false; }

	public:
		// 如果在协程内自动启动异步加载, 否则同步加载
		virtual bool AwaitLoadRecord(const char *szKey, ARecord targetRecord)
		{
			//if (IN_CORO)
			//{
			//	ALoadTask	task = MEM_NEW LoadSQLTask();

			//	if (mbStringKey)
			//		task->mSQLString.Format("Select * from `%s` where `%s`='%s'", mTableName.c_str(), mKeyFieldName.c_str(), szKey);
			//	else
			//		task->mSQLString.Format("Select * from `%s` where `%s`=%s", mTableName.c_str(), mKeyFieldName.c_str(), szKey);
			//	task->mTargetRecord = targetRecord;
			//	task->mLoadSQLType = eSQL_LoadRecord;
			//	return task->AWait(&mLoadSQLThread);
			//}
			//else
			{
				AString sqlString;
				if (mbStringKey)
					sqlString.Format("Select * from `%s` where `%s`='%s'", mTableName.c_str(), mKeyFieldName.c_str(), szKey);
				else
					sqlString.Format("Select * from `%s` where `%s`=%s", mTableName.c_str(), mKeyFieldName.c_str(), szKey);

				if (mMainThreadSQLTool.exeSql(sqlString, true))
					return mMainThreadSQLTool.LoadRecord(targetRecord);
				else
					ERROR_LOG("SQL run fail > %s \r\n %s", sqlString.c_str(), mMainThreadSQLTool._getErrorMsg());
			}
			return false;
		}

		bool AwaitGrowRecondID(ARecord targetRecord)
		{
			//if (IN_CORO)
			//{
			//	ALoadTask	task = MEM_NEW LoadSQLTask();
			//	task->mSQLString.Format("insert into `%s` set `%s`=DEFAULT; select max(`%s`) as LAST from `%s`;"
			//		, mTableName.c_str()
			//		, mKeyFieldName.c_str()				
			//		, mKeyFieldName.c_str()					
			//		, mTableName
			//	);
			//	task->mTargetRecord = targetRecord;
			//	task->mLoadSQLType = eSQL_GrowRecond;
			//	return task->AWait(&mLoadSQLThread);
			//}
			//else
			{
				AString sqlString;
				sqlString.Format("insert into `%s` set `%s`=DEFAULT; Select * from `%s` where `%s`=(select max(`%s`) from `%s`);"
					, mTableName.c_str()
					, mKeyFieldName.c_str()
					, mTableName.c_str()
					, mKeyFieldName.c_str()
					, mKeyFieldName.c_str()
					, mTableName.c_str()
				);

				if (mMainThreadSQLTool.exeMultiSql(sqlString, true, NULL))
					return mMainThreadSQLTool.LoadRecord(targetRecord);
				else
					ERROR_LOG("SQL run fail > %s \r\n %s", sqlString.c_str(), mMainThreadSQLTool._getErrorMsg());
			}
			return false;
		}

	public:

		//virtual void Process()
		//{
		//	mLoadSQLThread.Process();
		//}

		virtual void LowProcess()
		{
			mMainThreadSQLTool.Process();
		}

	public:
		virtual bool InitStart(tBaseTable *pTable, tNiceData *pSQLParam)
		{
			mSQLParam.append(*pSQLParam, true);

			if (!mMainThreadSQLTool.InitStart(mSQLParam))
			{
				ERROR_LOG("Main Mysql ready fail > %s\r\n", mSQLParam.dump().c_str());
				return false;
			}


			//if (!mLoadSQLThread.InitReady(pTable, pSQLParam))
			//{
			//	ERROR_LOG("SQL load thread ready fail");
			//	return false;
			//}

			mTableName = pTable->GetTableName();
			FieldInfo info = pTable->GetField()->getFieldInfo(0);
			mbStringKey = info->getType() == FIELD_CHAR_STRING || info->getType() == FIELD_STRING;
			mKeyFieldName = info->getName();

			return true;
		}

	public:
		NiceData				mSQLParam;
		MySqlDBTool		mMainThreadSQLTool;			// 主线的SQL, 用于不在协和函数中阻塞加载

		AString					mTableName;
		AString					mKeyFieldName;
		bool						mbStringKey;

		//SQLThread			mLoadSQLThread;
	};
	//-------------------------------------------------------------------------

};

#endif //_INCLUDE_DBDATABUFFER_H_