#include "SQLComponent.h"
#include "CoroutineTool.h"
#include "DBTableManager.h"
#include "TimeManager.h"

namespace NetCloud
{

	void NetCloud::SQLComponent::Start()
	{

	}

	//bool NetCloud::MySQLComponent::SaveRecord(DBTableManager *pDBMgr, BaseRecord *targetRecord, bool bInsert)
	//{
	//	if (!MySqlDBTool::_MakeSaveSqlData(mSaveTask.mSQLString, mSaveTask.mArrayData, targetRecord, bInsert))
	//	{
	//		ERROR_LOG("Make sql fail");
	//		return true;
	//	}

	//	//mMainThreadSQLTool.exeSql(mSaveTask.mSQLString, mSaveTask.mArrayData.mDataArray);
	//	//return true;

	//	// 之下的测试失败, 停止使用
	//	
	//	if (mSaveTask.WriteToThread(mLoadSQLThread.mWriteData, eSQL_SaveRecord))
	//		return true;
	//	else
	//		ERROR_LOG("Save SQL run fail > %s \r\n", mSaveTask.mSQLString.c_str());
	//	AssertNote(0, "No override SaveRecord");
	//	return false;
	//}

	bool ShareSQLComponent::SaveRecord(DBTableManager *pDBMgr, BaseRecord *record, bool bInsert)
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

	void BackSaveUpdateSQLThread::backWorkThread()
	{
		while (IsActive())
		{
			if (mListLock.trylock())
			{
				if (mSaveUpdateList.empty())
				{
					mListLock.unlock();
					TimeManager::Sleep(1);
					continue;
				}
				ASaveData data;
				mSaveUpdateList.pop_front(data);
				mListLock.unlock();

				if (data)
				{
					int affectCol = 0;
					if (!mSaveSQLTool.exeSql(data->mSQLString, data->mArrayData.mDataArray, &affectCol))
					{
						ERROR_LOG("Save update sql data fail : %s", mSaveSQLTool.getErrorMsg());
					}
					else if (affectCol <= 0)
					{
						WARN_LOG("Save update sql no affect line : %s", data->mSQLString.c_str());
					}
				}
			}
			else
				TimeManager::Sleep(1);
		}
	}

}