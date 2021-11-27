#include "LogicDBTable.h"
#include "DBTableManager.h"
#include "SQLComponect.h"

namespace NetCloud
{

	NetCloud::LogicDBTable::LogicDBTable(bool bShareSQL) : SkipBaseTable(eInitPoolField)
	{
		if (bShareSQL)
			mDBDataLoadSQL = MEM_NEW ShareSQLComponect();
		else
			mDBDataLoadSQL = MEM_NEW MySQLComponect();
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
			return r;

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
			if (t->mDBDataLoadSQL->SaveRecord(t->mpDB, this, true))
				FullAllUpdate(false);
		}
	}
	//-------------------------------------------------------------------------

}