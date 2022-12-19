//#pragma once
//#ifndef _INCLDUE_SQLTHREAD_H_
//#define _INCLDUE_SQLTHREAD_H_
//
////-------------------------------------------------------------------------
//// 使用无锁循环缓存进行异步保存读取SQL记录
//// *** NOTE: 大数据记录时, 保存与读取测试失败, 停止使用 2022/12/17
////-------------------------------------------------------------------------
//
//#include "WorkThread.h"
//#include "Auto.h"
//#include "BaseRecord.h"
//#include "CoroutineTool.h"
//#include "LoopDataBuffer.h"
//#include "MySqlDBTool.h"
//#include "EasyStack.h"
//#include "TimeManager.h"
//
////-------------------------------------------------------------------------
//#define SQL_STRING_MAX		(1024)
//
////-------------------------------------------------------------------------
//
//enum SQLTaskType
//{
//	eSQL_LoadRecord = 0,
//	eSQL_GrowRecond = 1,					// 自增加记录, 返回ID
//	eSQL_SaveRecord = 2,
//};
//
//struct TaskDataHead
//{
//	StrLenType	mIndex = 0;
//	StrLenType	mDataLength = 0;
//	SQLTaskType	mType = eSQL_LoadRecord;
//};
//
//class SQLThread;
//
//class LoadSQLTask : public AutoBase
//{
//public:
//	ARecord			mTargetRecord;
//	AString				mSQLString;
//	CoroID				mCoroID = 0;
//	bool					mbSucceed = false;
//	SQLTaskType	mLoadSQLType = eSQL_LoadRecord;
//
//public:
//	LoadSQLTask()
//	{
//
//	}
//
//public:
//	bool WriteToThread(ThreadLoopData &targetData, StrLenType index);
//
//	bool AWait(SQLThread *pSQLThread);
//
//	void Finish(bool bLoadSucceed)
//	{
//		mbSucceed = bLoadSucceed;
//		RESUME(mCoroID);
//	}
//};
//
//typedef Auto<LoadSQLTask> ALoadTask;
////-------------------------------------------------------------------------
//class SaveSQLTask
//{
//public:
//	AString					mSQLString;
//	SQLDataArray		mArrayData;
//
//public:
//	bool WriteToThread(ThreadLoopData &targetData, StrLenType index)
//	{
//		DSIZE len = mSQLString.length()+1;
//		
//
//		int totalSize = sizeof(short);
//		short dataCount = mArrayData.mNowCount;
//		for (int i=0; i<dataCount; ++i)
//		{
//			totalSize += mArrayData.mDataArray[i]->dataSize();
//		}
//
//		TaskDataHead head;
//		head.mIndex = len;			// 特别注意, 保存时, mIndex 为查询字符串的长度
//		head.mDataLength = len + totalSize;
//		head.mType = eSQL_SaveRecord;
//
//		if (len  > SQL_STRING_MAX)
//		{
//			ERROR_LOG("SQL string %d > %d", len, SQL_STRING_MAX);
//			return false;
//		}
//
//		targetData.Write((const char*)&head, sizeof(head));
//		targetData.Write(mSQLString.c_str(), mSQLString.length()+1);
//		targetData.write(dataCount);
//		for (int i = 0; i < dataCount; ++i)
//		{
//			DSIZE size = mArrayData.mDataArray[i]->dataSize();
//			targetData.writeData(mArrayData.mDataArray[i].getPtr(), size);
//		}
//
//		return true;
//	}
//};
////-------------------------------------------------------------------------
//class SQLThread : protected WorkThread
//{
//
//public:
//	~SQLThread()
//	{
//		Close();
//	}
//
//public:
//	bool InitReady(tBaseTable *pTable, tNiceData *pSQLParam)
//	{
//		if (!mSQLTool.InitStart(*pSQLParam))
//		{
//			ERROR_LOG("SQL init fail: %s", mSQLTool.getErrorMsg());
//			return false;
//		}
//
//		mBackRecord = pTable->_NewRecord();
//		mBackRecord->_alloctData(0);
//
//		mBackGrowTable = tBaseTable::NewBaseTable();
//		mBackGrowTable->AppendField("LAST", FIELD_INT64);
//		mBackGrowRecond = mBackGrowTable->NewRecord();
//
//		InitThread();
//		return true;
//	}
//
//public:
//	virtual bool AppendSQL(ALoadTask  task)
//	{
//		StrLenType index = 0;
//		if (mFreeIndexCode.empty())
//			index = ++mLastIndex;
//		else
//			index = mFreeIndexCode.pop();
//
//		mTastList.insert(index, task);
//		return task->WriteToThread(mWriteData, index);		
//	}
//
//	virtual void Process()
//	{
//		mWriteData.ProcessSendData();
//		mSQLTool.Process();
//
//		if (mReadData.peek(&mReadHead, sizeof(mReadHead)))
//		{
//			if (mReadBuffer.size() < mReadHead.mDataLength + sizeof(mReadHead))
//				mReadBuffer.resize(mReadHead.mDataLength + sizeof(mReadHead));
//
//			if (mReadHead.mDataLength <= 0)
//			{
//				mFreeIndexCode.push(mReadHead.mIndex);
//				// SQL执行失败
//				mReadData.skip(mReadHead.mDataLength + sizeof(mReadHead));
//
//				ALoadTask task;
//				mTastList.erase(mReadHead.mIndex, task);
//				if (task)
//					task->Finish(false);
//			}
//			else if (mReadData.peek(mReadBuffer.data(), mReadHead.mDataLength + sizeof(mReadHead)))
//			{
//				mFreeIndexCode.push(mReadHead.mIndex);
//				mReadBuffer.setDataSize(mReadHead.mDataLength + sizeof(mReadHead));
//				mReadData.skip(mReadHead.mDataLength + sizeof(mReadHead));
//
//				ALoadTask task;
//				mTastList.erase(mReadHead.mIndex, task);
//				if (task)
//				{
//					mReadBuffer.seek(sizeof(mReadHead));
//					if (mReadHead.mType == eSQL_LoadRecord)
//					{
//						if (task->mTargetRecord->restoreData(&mReadBuffer))
//							task->Finish(true);
//						else
//						{
//							ERROR_LOG("Restore record fail");
//							task->Finish(false);
//						}
//					}
//					else if (mReadHead.mType == eSQL_GrowRecond)
//					{
//						Int64 lastID = 0;
//						if (mReadBuffer.read(lastID))
//						{
//							task->mTargetRecord->set(0, lastID);
//							task->Finish(true);
//						}
//						else
//						{
//							ERROR_LOG("Read last id fail");
//							task->Finish(false);
//						}
//					}
//					else
//						ERROR_LOG("No define sql load type %d", mReadHead.mType);
//				}
//			}
//		}
//
//	}
//
//public:
//	virtual void backWorkThread(void)
//	{
//		TaskDataHead head;
//		int lenSize = sizeof(TaskDataHead);
//		char  szSQLString[SQL_STRING_MAX + 1+ sizeof(TaskDataHead)];
//		DataBuffer	mRecordData;
//		Array<AutoData> sqlDataList;
//
//		while (IsActive())
//		{
//			mReadData.ProcessSendData();
//			if (!mWriteData.peek(&head, lenSize))
//			{
//				TimeManager::Sleep(1);
//				continue;
//			}			
//
//			if (head.mType == eSQL_SaveRecord)
//			{
//				if (mWriteData.dataSize()<lenSize+head.mDataLength)
//					continue;				
//
//				if (!mWriteData.peek(szSQLString, lenSize + head.mIndex))
//				{
//					AssertNote(0, "Must read sql string");
//					continue;
//				}
//				szSQLString[lenSize + head.mIndex] = '\0';
//				
//				mWriteData.skip(lenSize+head.mIndex);
//				short dataSize = 0;
//				mWriteData.read(dataSize);
//				if (sqlDataList.size() < dataSize)
//					sqlDataList.resize(dataSize);
//				for (int i = 0; i < dataSize; ++i)
//				{
//					if (!sqlDataList[i])
//						sqlDataList[i] = MEM_NEW DataBuffer();
//
//					if (!mWriteData.readData(sqlDataList[i].getPtr()))
//					{
//						ERROR_LOG("Code error");
//						AssertNote(0, "Code error");
//						return;
//					}
//				}
//				if (!mSQLTool.exeSql(szSQLString + lenSize, sqlDataList))
//				{
//					ERROR_LOG("SQL fail : %s\r\n%s", szSQLString + lenSize, mSQLTool.getErrorMsg());
//				}
//				continue;
//			}
//
//			if (mWriteData.peek(szSQLString, lenSize + head.mDataLength))
//			{
//				mWriteData.skip(lenSize + head.mDataLength);
//				
//				szSQLString[lenSize + head.mDataLength] = '\0';
//				if (mSQLTool.exeMultiSql(szSQLString + lenSize, true, NULL))
//				{
//					if (head.mType == SQLTaskType::eSQL_LoadRecord)
//					{
//						if (mSQLTool.LoadRecord(mBackRecord))
//						{
//							mRecordData.clear(false);
//							if (mBackRecord->saveData(&mRecordData))
//							{
//								head.mDataLength = mRecordData.dataSize();
//								mReadData.Write((const char*)&head, lenSize);
//								mReadData.Write(mRecordData.data(), mRecordData.dataSize());
//								continue;
//							}
//							else
//								ERROR_LOG("Record date save fail : %s", szSQLString + lenSize);
//						}
//						else
//							ERROR_LOG("SQL load record fail : %s", szSQLString + lenSize);
//					}
//					else if (head.mType == SQLTaskType::eSQL_GrowRecond)
//					{
//						if (mSQLTool.LoadRecord(mBackGrowRecond))
//						{
//							Int64 id = mBackGrowRecond[0];
//
//							head.mDataLength = sizeof(id);
//							mReadData.Write((const char*)&head, lenSize);
//							mReadData.Write((const char*)&id, sizeof(id));
//							continue;
//						}
//						else
//							ERROR_LOG("SQL load record fail");
//					}
//				}
//				else				
//					ERROR_LOG("SQL error: %s\r\n%s", mSQLTool.getErrorMsg(), szSQLString + lenSize);
//
//					head.mDataLength = 0;
//					mReadData.Write((const char*)&head, lenSize);
//			}
//		}
//	}
//
//public:
//	SQLThread()
//		: mWriteData(1024*1024*8)
//		, mReadData(1024*1024*8)
//	{}
//
//public:
//	ThreadLoopData				mWriteData;				// No lock, NOTE: 由于不可变化大小, 所以, 最大包不可大于此空间值(可采用临时大包处理方案)
//
//protected:
//	AutoTable							mBackGrowTable;
//	ARecord							mBackGrowRecond;		// 用于只获取ID的
//	ARecord							mBackRecord;
//	ThreadLoopData				mReadData;
//	EasyHash<int, ALoadTask>	mTastList;
//	MySqlDBTool					mSQLTool;
//	TaskDataHead					mReadHead;
//	DataBuffer						mReadBuffer;
//
//	EasyStack<StrLenType>	mFreeIndexCode;
//	StrLenType						mLastIndex = 0;
//};
//
//
//
//
//
//#endif //_INCLDUE_SQLTHREAD_H_
//
//
