#pragma once

#include "DBTableLoadSQL.h"
#include "Component.h"
#include "ActorDBLib.h"
#include "WorkThread.h"

namespace NetCloud
{
	class ActorDBLib_Export SQLComponent : public DBTableLoadSQL
	{
	public:
		void Start() override;

	};

	//-------------------------------------------------------------------------
	//class ActorDBLib_Export MySQLComponent : public SQLComponent
	//{
	//public:
	//	SaveSQLTask	mSaveTask;

	//public:
	//	virtual bool SaveRecord(DBTableManager *pDBMgr, BaseRecord *targetRecord, bool bInsert);
	//};

	//-------------------------------------------------------------------------
	// 使用共享进程方式, 将SQL任务转移到共享进程保存, 测试下来, 比较好用
	class ActorDBLib_Export ShareSQLComponent : public SQLComponent
	{
	public:
		AString mSQLString;

	public:
		bool SaveRecord(DBTableManager *pDBMgr, BaseRecord *record, bool bInsert);
	};

	//-------------------------------------------------------------------------
	//后台线程方式保存
	//-------------------------------------------------------------------------
	struct SaveUpdateData : public AutoBase
	{
		AString mSQLString;
		SQLDataArray		mArrayData;
	};
	//-------------------------------------------------------------------------
	//后台线程方式保存
	class BackSaveUpdateSQLThread : public WorkThread
	{
		typedef Auto<SaveUpdateData> ASaveData;

	public:
		void AppendSave( ASaveData data)
		{
			mListLock.lock();
			mSaveUpdateList.push_back(data);
			mListLock.unlock();
		}

		virtual void Close(void) override
		{
			WorkThread::Close();

			// 关闭后, 如果还存在未保存的, 继续保存
			while (!mSaveUpdateList.empty())
			{
				ASaveData data;
				mSaveUpdateList.pop_front(data);				

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
		}

	public:
		virtual void backWorkThread() override;

	public:
		MySqlDBTool	mSaveSQLTool;

		
		EasyList<ASaveData>	mSaveUpdateList;

		CsLock	mListLock;
	};

	// 实现直接使用后台线程方式保存, 不再返回保存结果
	class ActorDBLib_Export MySQLComponent : public SQLComponent
	{
		BackSaveUpdateSQLThread		mBackSaveThread;

	public:
		bool InitStart(tBaseTable *pTable, tNiceData *pSQLParam) override
		{
			if (SQLComponent::InitStart(pTable, pSQLParam))
			{
				if (mBackSaveThread.mSaveSQLTool.InitStart(mSQLParam))
				{
					mBackSaveThread.InitThread();
					return true;
				}
				else
					ERROR_LOG("Save update sql tool init fail : %s", mBackSaveThread.mSaveSQLTool.getErrorMsg());
			}
			return false;
		}

		virtual void OnDestory() override { mBackSaveThread.Close(); }

	public:
		virtual bool SaveRecord(DBTableManager *pDBMgr, BaseRecord *targetRecord, bool bInsert) override
		{
			Auto<SaveUpdateData>	pData = MEM_NEW SaveUpdateData();
			if (!MySqlDBTool::_MakeSaveSqlData(pData->mSQLString, pData->mArrayData, targetRecord, bInsert))
			{
				ERROR_LOG("Make sql fail");

				return false;
			}
			else
				DEBUG_LOG("Save update sql data ok : \r\n <%s>", pData->mSQLString.c_str());

			mBackSaveThread.AppendSave(pData);
			return true;
		}
	};
	//-------------------------------------------------------------------------
	
}
