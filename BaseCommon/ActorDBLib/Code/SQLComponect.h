#pragma once

#include "DBTableLoadSQL.h"
#include "Component.h"
#include "ActorDBLib.h"

namespace NetCloud
{
	class ActorDBLib_Export SQLComponect : public Component
	{
	public:
		void Start() override;

	public:
		virtual Auto<DBTableLoadSQL> GetSQL() = 0;
	};

	class ActorDBLib_Export MySQLTableTool : public DBTableLoadSQL
	{
	public:
		virtual bool SaveRecord(ARecord targetRecord)
		{ 
			//???
			AString sqlString;			
			//if (!mMainThreadSQLTool._MakeSaveSqlData())
			if (mMainThreadSQLTool.exeSql(sqlString, true))
				return mMainThreadSQLTool.LoadRecord(targetRecord);
			else
				ERROR_LOG("SQL run fail > %s \r\n %s", sqlString.c_str(), mMainThreadSQLTool._getErrorMsg());
			AssertNote(0, "No override SaveRecord");  return false; 
		}
	};

	class ActorDBLib_Export MySQLComponect : public SQLComponect
	{
	public:
		virtual void Awake()
		{
			mSQL = MEM_NEW MySQLTableTool();
		}

		virtual Auto<DBTableLoadSQL> GetSQL() override { return mSQL; }

	protected:
		Auto<DBTableLoadSQL> mSQL;
	};

	class ActorDBLib_Export ShareSQLComponect : public SQLComponect
	{
	public:
		virtual Auto<DBTableLoadSQL> GetSQL() override { return mSQL; }

	protected:
		Auto<DBTableLoadSQL> mSQL;
	};
}
