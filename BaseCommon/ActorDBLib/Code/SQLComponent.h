#pragma once

#include "DBTableLoadSQL.h"
#include "Component.h"
#include "ActorDBLib.h"

namespace NetCloud
{
	class ActorDBLib_Export SQLComponent : public DBTableLoadSQL
	{
	public:
		void Start() override;

	};

	class ActorDBLib_Export MySQLComponent : public SQLComponent
	{
	public:
		SaveSQLTask	mSaveTask;

	public:
		virtual bool SaveRecord(DBTableManager *pDBMgr, BaseRecord *targetRecord, bool bInsert);
	};


	class ActorDBLib_Export ShareSQLComponent : public SQLComponent
	{
	public:
		AString mSQLString;

	public:
		bool SaveRecord(DBTableManager *pDBMgr, BaseRecord *record, bool bInsert);
	};
}
