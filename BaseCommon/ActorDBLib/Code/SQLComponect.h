#pragma once

#include "DBTableLoadSQL.h"
#include "Component.h"
#include "ActorDBLib.h"

namespace NetCloud
{
	class ActorDBLib_Export SQLComponect : public DBTableLoadSQL
	{
	public:
		void Start() override;

	};

	class ActorDBLib_Export MySQLComponect : public SQLComponect
	{
	public:
		SaveSQLTask	mSaveTask;

	public:
		virtual bool SaveRecord(DBTableManager *pDBMgr, BaseRecord *targetRecord, bool bInsert);
	};


	class ActorDBLib_Export ShareSQLComponect : public SQLComponect
	{
	public:
		AString mSQLString;

	public:
		bool SaveRecord(DBTableManager *pDBMgr, BaseRecord *record, bool bInsert);
	};
}
