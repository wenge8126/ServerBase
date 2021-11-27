#include "SQLComponect.h"

#include "DBTableManager.h"

namespace NetCloud
{

	void NetCloud::SQLComponect::Start()
	{

	}

	bool NetCloud::MySQLComponect::SaveRecord(DBTableManager *pDBMgr, BaseRecord *targetRecord, bool bInsert)
	{
		if (!MySqlDBTool::_MakeSaveSqlData(mSaveTask.mSQLString, mSaveTask.mArrayData, targetRecord, bInsert))
		{
			ERROR_LOG("Make sql fail");
			return false;
		}
		
		if (mSaveTask.WriteToThread(mLoadSQLThread.mWriteData, eSQL_SaveRecord))
			return true;
		else
			ERROR_LOG("Save SQL run fail > %s \r\n", mSaveTask.mSQLString.c_str());
		AssertNote(0, "No override SaveRecord");
		return false;
	}

	bool ShareSQLComponect::SaveRecord(DBTableManager *pDBMgr, BaseRecord *record, bool bInsert)
	{
		bool bRe = MySqlDBTool::_MakeSaveSqlData(mSQLString, pDBMgr->mTempDataArray, record, bInsert);
		if (bRe)
		{
			//t->mUpdateCount++;
			pDBMgr->mSQLUpdate.SendRecordUpdate(mSQLString, pDBMgr->mTempDataArray.mDataArray, pDBMgr->mTempDataArray.mNowCount);

			//mLastUpdateTimeMilSec = TimeManager::NowTick();
			return true;
		}
		return false;
	}

}