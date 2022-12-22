#include "LogicDBTable.h"
#include "DBTableManager.h"
#include "SQLComponent.h"
#include "DBUserComponent.h"

namespace NetCloud
{

	NetCloud::LogicDBTable::LogicDBTable(bool bShareSQL) : SkipBaseTable(eInitPoolField)
	{
		if (bShareSQL)
			mDBDataLoadSQL = MEM_NEW ShareSQLComponent();
		else
			mDBDataLoadSQL = MEM_NEW MySQLComponent();
	}

	ARecord NetCloud::LogicDBTable::_NewRecord()
	{
		return MEM_NEW LogicDBRecord(this);
	}

	ARecord NetCloud::LogicDBTable::NewRecord()
	{
		ARecord re = MEM_NEW LogicDBRecord(this);
		return re;
	}

	ARecord NetCloud::LogicDBTable::GrowthNewRecord(DataStream *recordData /*= NULL*/)
	{
		ARecord r = _NewRecord();
		r->_alloctData(0);
		
		if (mDBDataLoadSQL->AwaitGrowRecondID(r))
		{
			AString key = r[0].string();
			if (recordData != NULL)
			{
				if (!r->restoreData(recordData))
				{
					ERROR_LOG("Restor recond fail");
					r[0] = key;
					return r;
				}
				r[0] = key;
				r->FullAllUpdate(true);
				r->SaveUpdate();
			}
			return r;
		}
		
		return ARecord();
	}

	void NetCloud::LogicDBTable::InsertDBNewRecord(ARecord newRecord)
	{
		newRecord->FullAllUpdate(true);
		
		if (mDBDataLoadSQL->SaveRecord(mpDB, newRecord.getPtr(), true) )
			newRecord->FullAllUpdate(false);
	}

	void NetCloud::LogicDBTable::ApplyExt(AutoNice extParam)
	{
		FieldInfo info = GetField()->getFieldInfo(0);
		mbStringKey = info->getType() == FIELD_STRING || info->getType() == FIELD_CHAR_STRING;
		mKeyFieldName = info->getName();

		if (!mDBDataLoadSQL->InitStart(this, mSQLParam.getPtr()))
		{
			ERROR_LOG("%s init load SQL fail", GetTableName());
		}
	}

	//-------------------------------------------------------------------------
	void NetCloud::LogicDBRecord::SaveUpdate()
	{
		if (NeedUpdate())
		{
			Auto<LogicDBTable> t = GetTable();
			if (t->mDBDataLoadSQL->SaveRecord(t->mpDB, this, false))
				FullAllUpdate(false);
		}
	}
	//-------------------------------------------------------------------------

	bool LogicDBTable::LoadAllRecord(DBUserComponent *pComponent)
	{
		AString sql;

		sql.Format("SELECT * from `%s`", GetTableName());

		if (!mDBDataLoadSQL->mMainThreadSQLTool.exeSql(sql, true))
		{
			ERROR_LOG("Exe sql %s error : %s", sql.c_str(), mDBDataLoadSQL->mMainThreadSQLTool.getErrorMsg());
			return false;
		}

		MySqlDBTool &tool = mDBDataLoadSQL->mMainThreadSQLTool;

		ARecord r = NewRecord();
		r->_alloctData(0);

		while (true)
		{
			if (tool.LoadRecord(r))
			{
				pComponent->OnLoadRecord(r);
			}
			else
				break;
		}
		return true;
	}

}