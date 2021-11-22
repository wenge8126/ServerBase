#pragma once

#ifndef _SHARESQLUPDATE_H_
#define _SHARESQLUPDATE_H_

#include "DynamicLoopBuffer.h"
#include "MySqlDBTool.h"
#include "WorkThread.h"
#include "Lock.h"
#include "TimeManager.h"
#include "ShareSQLUpdateLib.h"

namespace NetCloud
{
	enum DBMsgID
	{
		eMsgNone = 0,
		eMsgSQL = 1,
		eMsgUpdateRecord = 2,
	};
	struct  DBMsgHead
	{
		union
		{
			struct {
				uint mMsgID : 8;
				uint mSize : 24;
			};
			uint			value;
		};
	};

	// 发送更新端
	class ShareSQLUpdateLib_Export_H SendShareSQLUpdate : public AutoBase
	{

	public:
		// 128K*1M = 128G
		SendShareSQLUpdate()
			: mSendShareMemBuffer(1024 * 128, 1024 * 1024)
			, mpLock(NULL)
			, mDebugCount(0)
		{
			
		}
		~SendShareSQLUpdate()
		{
			delete mpLock;
		}

	public:
		void Close()
		{
			mSendShareMemBuffer.Close();
		}

	public:
		bool Init(int shareKey)
		{
			if (!mSendShareMemBuffer.Init(shareKey))
				return false;

			AString str;
			str.Format("_share_msg_%d", shareKey);
			if (mpLock != NULL)
				delete mpLock;
			mpLock = new MtLock(str.c_str());

			return true;
		}

		void Send(const AString &sqlString, DataStream *pData)
		{
			++mDebugCount;
			//NOTE_LOG("+++++++ > %llu", mDebugCount);
			mpLock->lock();

			mSendShareMemBuffer.UpdateDebugCount(mDebugCount);

			DBMsgHead  msgHead;
			msgHead.mMsgID = eMsgSQL;
			msgHead.mSize = sizeof(StrLenType) + sqlString.length() + pData->dataSize();

			mSendShareMemBuffer._write(&msgHead.value, sizeof(uint));
			mSendShareMemBuffer.writeString(sqlString);
			mSendShareMemBuffer._write(pData->data(), pData->dataSize());
			mpLock->unlock();
		}

		void SendRecordUpdate(const AString &sqlString, const Array<AutoData> &dataList, int dataCount)
		{
			++mDebugCount;
			//NOTE_LOG("+++++++ > %llu", mDebugCount);

			DBMsgHead  msgHead;
			msgHead.mMsgID = eMsgUpdateRecord;
			UInt64 checkTotal = sizeof(StrLenType) + sqlString.length() + sizeof(short);
			for (int i = 0; i < dataCount; ++i)
			{
				checkTotal += sizeof(DSIZE);
				AutoData data = dataList[i];
				if (data)
				{
					checkTotal += data->dataSize();
				}
			}
			msgHead.mSize = (uint)checkTotal;
			if (checkTotal != msgHead.mSize)
			{
				ERROR_LOG("Sql %s data too big, more %u > %lld", sqlString.c_str(), checkTotal, 0xFFFFFFF);
				return;
			}

			mpLock->lock();
			mSendShareMemBuffer.UpdateDebugCount(mDebugCount);

			mSendShareMemBuffer._write(&msgHead.value, sizeof(uint));
			mSendShareMemBuffer.writeString(sqlString);
			mSendShareMemBuffer.write((short)dataCount);
			for (int i = 0; i < dataCount; ++i)
			{
				AutoData data = dataList[i];
				if (data)
				{
					mSendShareMemBuffer.write((DSIZE)data->dataSize());
					mSendShareMemBuffer._write(data->data(), data->dataSize());
				}
				else
					mSendShareMemBuffer.write((DSIZE)0);
			}

			mpLock->unlock();
		}

		UInt64 BufferSize()
		{
			if (mSendShareMemBuffer.getArrayBuffer() == NULL)
				return 0;
			return mSendShareMemBuffer.getArrayInfo().mTotalMemorySize;
		}

		uint BlockCount()
		{
			if (mSendShareMemBuffer.getArrayBuffer() == NULL)
				return 0;
			return mSendShareMemBuffer.getArrayInfo().mArrayCount;
		}

	protected:
		ShareDynamicLoop						mSendShareMemBuffer;

		MtLock											*mpLock;
		UInt64											mDebugCount;
	};

	// 接收执行SQL更新
	class ShareSQLUpdateLib_Export_H ShareSQLUpdate : public WorkThread
	{
	public:
		ShareSQLUpdate()
			: mShareMemBuffer()
			, mpLock(NULL)
			, mbInit(false)
			, mDebugCount(0)
			, mUpdateCount(0)
			, mbEmpty(true)
		{}
		~ShareSQLUpdate()
		{
			delete mpLock;
		}


	public:
		bool Init(int shareKey, tNiceData &sqlParam)
		{
			if (mbInit)
			{
				ERROR_LOG("Already init, not need repeat init");
				return false;
			}

			if (!mSqlTool.InitStart(sqlParam))
			{
				ERROR_LOG("SQL connect fail >\r\n%s", sqlParam.dump().c_str());
				return false;
			}

			AutoNice	backSQLParam = (tNiceData*)sqlParam.get(BACK_SQL);
			if (backSQLParam)
			{
				mBackSQLTool = MEM_NEW MySqlDBTool();
				if (!mBackSQLTool->InitStart(*backSQLParam))
				{
					ERROR_LOG("Back SQL connect fail >\r\n%s", backSQLParam->dump().c_str());
					return false;
				}
			}

			AString str;
			str.Format("_share_msg_%d", shareKey);
			if (mpLock != NULL)
				delete mpLock;
			mpLock = new MtLock(str.c_str());

			mShareMemBuffer.Init(shareKey);
			InitThread();
			mbInit = true;
			return true;
		}



	public:
		virtual void backWorkThread(void) override
		{
			DataBuffer		dataBuffer(1024);
			Auto<TempDataBuffer>  mDBData = MEM_NEW TempDataBuffer();
			Array<AutoData> dataArray;
			dataArray.push_back(mDBData);

			try {
				while (IsActive())
				{
					// 防止MYSQL长时间无业务断开连接
					mSqlTool.Process();

					if (mpLock->trylock())
					{
						//mShareMemBuffer.Process();
						mUpdateCount = mShareMemBuffer.GetDebugCount();

						MemoryLock  memLock(&(mShareMemBuffer.getArrayInfo().mLock), tDynamicLoopBuffer::eWaitLock, tDynamicLoopBuffer::eWriteLock);
						uint msgInfoValue = 0;
						if (mShareMemBuffer.peek(&msgInfoValue, sizeof(uint)))
						{

							mbEmpty = false;
							DBMsgHead  head;
							head.value = msgInfoValue;

							if (head.mMsgID == eMsgSQL)
							{
								if (dataBuffer.size() < head.mSize + sizeof(uint))
									dataBuffer.resize(head.mSize + sizeof(uint));

								if (mShareMemBuffer.peek(dataBuffer.data(), head.mSize + sizeof(uint)))
								{
									++mDebugCount;
									//NOTE_LOG("------- > %llu", mDebugCount);

									mShareMemBuffer.skip(head.mSize + sizeof(uint));
									// 解锁, 不再操作缓存
									mpLock->unlock();

									dataBuffer.setDataSize(head.mSize + sizeof(uint));
									dataBuffer.seek(sizeof(uint));
									dataBuffer.readString(mSQLString);
									mDBData->InitSet(dataBuffer.data() + dataBuffer.tell(), dataBuffer.lastDataSize());

									//NiceData debugData;
									//mDBData->seek(0);
									//debugData.restore(mDBData.getPtr());
									//NOTE_LOG("Rev : %s > \r\n %s", mSQLString.c_str(), debugData.dump().c_str());

									if (!mSqlTool.exeSql(mSQLString, dataArray))
										ERROR_LOG("Update fail > %s\r\n%s", mSQLString.c_str(), mSqlTool._getErrorMsg());

									// 同步到备份库
									if (mBackSQLTool)
									{
										if (!mBackSQLTool->exeSql(mSQLString, dataArray))
											ERROR_LOG("Update back DB fail > %s\r\n%s", mSQLString.c_str(), mBackSQLTool->_getErrorMsg());
									}

									continue;
								}
							}
							else if (head.mMsgID == eMsgUpdateRecord)
							{
								uint msgSize = head.mSize + sizeof(uint);
								if ((uint)dataBuffer.size() < msgSize)
									dataBuffer.resize(msgSize);

								if (mShareMemBuffer.peek(dataBuffer.data(), msgSize))
								{
									mShareMemBuffer.skip(msgSize);
									// 解锁, 不再操作缓存
									mpLock->unlock();

									dataBuffer.setDataSize(msgSize);

									UpdateSQL(dataBuffer);

									continue;
								}
							}
							else
							{
								uint msgSize = head.mSize + sizeof(uint);
								if ((uint)dataBuffer.size() < msgSize)
									dataBuffer.resize(msgSize);

								if (mShareMemBuffer.peek(dataBuffer.data(), msgSize))
								{
									mShareMemBuffer.skip(msgSize);
									// 解锁, 不再操作缓存
									mpLock->unlock();
								
									ERROR_LOG("No process update msg %d : %u", head.mMsgID, head.mSize);
									continue;
								}
							}
						}
						mpLock->unlock();
					}
					mbEmpty = true;
					TimeManager::Sleep(1);
					mSqlTool.Process();
				}
			}
			catch (std::exception &e)
			{
				ERROR_LOG("Error exit update thread > %s", e.what());
			}
			catch (...)
			{
				ERROR_LOG("Error exit update thread");
			}
		}

		void UpdateSQL(DataBuffer &dataBuffer)
		{
			++mDebugCount;
			//NOTE_LOG("------- > %llu", mDebugCount);

			dataBuffer.seek(sizeof(uint));

			if (dataBuffer.readString(mSQLString))
			{
				short dataCount = 0;
				if (dataBuffer.read(dataCount))
				{
					mMulitDataArray.Reset();

					bool bError = false;
					for (int i = 0; i < dataCount; ++i)
					{
						DSIZE dataSize = 0;
						if (!dataBuffer.read(dataSize))
						{
							ERROR_LOG("Read data size fail");
							bError = true;
							break;
						}

						AutoData d = mMulitDataArray.NextData();

						if (d->size() < dataSize)
							d->resize(dataSize);

						if (dataBuffer._read(d->data(), dataSize))
							d->setDataSize(dataSize);
						else
						{
							ERROR_LOG("Read data fail");
							bError = true;
							break;
						}
					}
					if (bError)
					{
						ERROR_LOG("Data read fail, update fail > %s", mSQLString.c_str());
					}
					else
					{
						int affectCount = 0;
						if (!mSqlTool.exeSql(mSQLString, mMulitDataArray.mDataArray, &affectCount))
							ERROR_LOG("Update fail > %s\r\n%s", mSQLString.c_str(), mSqlTool._getErrorMsg())
						else
							DEBUG_LOG("Update succeed : %s > %d", mSQLString.c_str(), affectCount);

						if (mBackSQLTool)
						{
							if (!mBackSQLTool->exeSql(mSQLString, mMulitDataArray.mDataArray, &affectCount))
								ERROR_LOG("Update back DB fail > %s\r\n%s", mSQLString.c_str(), mBackSQLTool->_getErrorMsg())
							else
								DEBUG_LOG("Update back DB succeed : %s > %d", mSQLString.c_str(), affectCount);
						}
					}
				}
				else
					ERROR_LOG("Read data count fail");
			}
			else
				ERROR_LOG("Read sql  string fail");
		}

		// 其他线程调用判断是否全部完成
		void WaitAllUpdateOperateFinish()
		{
			UInt64 nowCount = mDebugCount;
			UInt64 beginTime = TimeManager::Now();
			while (!mbEmpty)
			{
				TimeManager::Sleep(3000);
				NOTE_LOG("!!! Wait share memory exist update over time %llu, Now update %llu, total %llu", TimeManager::Now() - beginTime, mDebugCount - nowCount, mDebugCount);
			}
		}

		// 管理线程等待全部完成后关闭
		void WaitFinishClose()
		{
			WaitAllUpdateOperateFinish();
			Close();
		}

		UInt64 BufferSize()
		{
			if (mShareMemBuffer.getArrayBuffer() == NULL)
				return 0;
			return mShareMemBuffer.getArrayInfo().mTotalMemorySize;
		}

		uint BlockCount()
		{
			if (mShareMemBuffer.getArrayBuffer() == NULL)
				return 0;
			return mShareMemBuffer.getArrayInfo().mArrayCount;
		}

	public:
		bool												mbEmpty;
		UInt64											mDebugCount;
		UInt64											mUpdateCount;

	protected:
		MySqlDBTool								mSqlTool;
		Auto<MySqlDBTool>					mBackSQLTool;
		ReceiveShareMemDynamicLoop	mShareMemBuffer;
		MtLock											*mpLock;

		AString											mSQLString;
		SQLDataArray								mMulitDataArray;
		bool												mbInit;
	};

	//-------------------------------------------------------------------------

}

#endif //_SHARESQLUPDATE_H_
