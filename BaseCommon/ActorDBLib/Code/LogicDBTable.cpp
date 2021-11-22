#include "LogicDBTable.h"
#include "DBTableManager.h"


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

	if (mDBDataLoadSQL.AwaitGrowRecondID(r))
		return r;

	return ARecord();
}

void NetCloud::LogicDBTable::InsertDBNewRecord(ARecord newRecord)
{
	newRecord->FullAllUpdate(true);
	AString sql;

	bool bRe = MySqlDBTool::_MakeSaveSqlData(sql, mTempDataArray, newRecord.getPtr(), true);
	if (bRe)
	{
		mpDB->mSQLUpdate.SendRecordUpdate(sql, mTempDataArray.mDataArray, mTempDataArray.mNowCount);
		newRecord->FullAllUpdate(false);
	}
}

//-------------------------------------------------------------------------
void NetCloud::LogicDBRecord::SaveUpdate()
{
	if (NeedUpdate())
	{
		Auto<LogicDBTable> t = GetTable();
		AString sql;
		bool bRe = MySqlDBTool::_MakeSaveSqlData(sql, t->mTempDataArray, this, false);
		if (bRe)
		{
			//t->mUpdateCount++;
			t->mpDB->mSQLUpdate.SendRecordUpdate(sql, t->mTempDataArray.mDataArray, t->mTempDataArray.mNowCount);
			FullAllUpdate(false);
			//mLastUpdateTimeMilSec = TimeManager::NowTick();
		}
	}
}
//-------------------------------------------------------------------------