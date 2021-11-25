#include "mDBRecordShareMemoryPool.h"
#include "mShareMemTableData.h"
#include "BaseTable.h"
#include "MySqlDBTool.h"
#include "mShareMemTableData.h"
#include "mShareMemDBManager.h"


#ifdef __LINUX__
#include <unistd.h>
#else
#include <process.h>
#endif

namespace NetCloud
{

	DBRecordShareMemoryPool::DBRecordShareMemoryPool(ShareMemTableData *pMemSql) : mpMemTableData(pMemSql)
		, mpNewPointer(NULL)
	{
		mNowAllocCount = tBaseTable::msCommonConfig.mRecordMemPoolBlockRecordCount;

		mObjectSize = 9;
		mNowShareMemoryIndex = 0;

#if RECORD_POOL_DEBUG
		mNewCount = 0;

		mFreeCount = 0;
		mAllotCount = 0;
		mSysCount = 0;
#endif
		//Restore();
	}

	void DBRecordShareMemoryPool::MyAlloc()
	{
		//AssertEx(m_NewPointer==NULL, "Must is null when alloc");
		mNowAllocCount *= 2;
		
		size_t newSize = mObjectSize * mNowAllocCount;

		if (newSize > 256 * 1024 * 1024)
		{
			mNowAllocCount = (int)(256 * 1024 * 1024 / mObjectSize);
			newSize = mObjectSize * mNowAllocCount;
		}

		int processID = (int)getpid();
		AString keyName;
		keyName.Format("%s_%d_%d", mpMemTableData->GetTableName(), processID, mNowShareMemoryIndex++);

		int key = MAKE_INDEX_ID(keyName.c_str());

		ShareMemAO *pMem = MEM_NEW ShareMemAO();
		if (pMem->Create(key, newSize) == TRUE)
		{
			DEBUG_LOG("Succeed create share memory > key %d = size %llu", key, newSize);
			memset(pMem->GetDataPtr(), 0, newSize);
			
			mpNewPointer = (unsigned char*)pMem->GetDataPtr();
			mpNewPointer += 1;

			//强制转型为双指针，这将改变每个内存块头4个字节的含义。 
			unsigned char **cur = reinterpret_cast<unsigned char**>(mpNewPointer);
			unsigned char *next = mpNewPointer;
			for (size_t i = 0; i < mNowAllocCount - 1; i++)
			{
				next += mObjectSize;
				*cur = next;
				//这样，所分配的每个内存块的头4个字节就被“强行“解释为指向下一个内存块的指针， 即形成了内存块的链表结构。 
				cur = reinterpret_cast<unsigned char**>(next);
			}
			*cur = 0;

			//ShareMemReocrdData *pRecordMem = MEM_NEW ShareMemReocrdData(pMem, mpMemTableData->GetTableName(), mpMemTableData->mField);
			ShareMemAO *pExist = mAllocList.find(mNowShareMemoryIndex - 1);
			if (pExist != NULL)
				delete pExist;
			mAllocList.erase(mNowShareMemoryIndex - 1);
			mAllocList.insert(mNowShareMemoryIndex - 1, pMem);

			//if (mpMemTableData->mField->getCount() > 1)
			//	pRecordMem->InitStart(mpMemTableData->mpDBMgr->mInitParam);
			//else
			//	NOTE_LOG("!!! %s field count is %d, then stop thread update", mpMemTableData->GetTableName(), mpMemTableData->mField->getCount());

			mpMemTableData->mpDBMgr->NotifyNewAppendShareMem(mpMemTableData->mTableName, key, mNowShareMemoryIndex - 1);
		}
		else
		{
			delete pMem;
			DEBUG_LOG("Create share memory fail> key %d = size %llu", key, newSize);
			AssertNote(0, "Share memory create fail, may be already exist key"); 
		}
	}



	void DBRecordShareMemoryPool::Close()
	{
		Auto<MySqlDBTool> tool = mpMemTableData->mpDBMgr->mMySQLTool;
		// 释放前, 将所有内存停止后台更新, 然后检查全部落地
		//for (int i = 0; i < mAllocList.size(); ++i)
		//{
		//	mAllocList[i]->Close(tool.getPtr());
		//}
		// 最后检查落地, 无需再加锁, 此时, 落地线程关闭, DB进程也已经关闭
		ProcessRecordDataMustSave(tool.getPtr());
	}

	void DBRecordShareMemoryPool::clear()
	{
		for (size_t i = 0; i < mAllocList.size(); ++i)
		{
			mAllocList.get(i)->Destory();
			delete mAllocList.get(i);
		}
		mAllocList.clear(false);
		mNowShareMemoryIndex = 0;
	}

	SHARE_PTR DBRecordShareMemoryPool::getShareMemoryPtr(const char *szRecordDataPtr)
	{
		for (size_t i = 0; i < mAllocList.size(); ++i)
		{
			ShareMemAO *pData = mAllocList.get(i);
			const char *szPtr = pData->GetDataPtr();
			if (szRecordDataPtr >= szPtr && szRecordDataPtr < szPtr + pData->GetSize())
			{
				SHARE_PTR ptr;
				ptr.key = i;
				ptr.pos = (int)(szRecordDataPtr - szPtr);
				return ptr;
			}
		}
		return SHARE_PTR();
	}

	void* DBRecordShareMemoryPool::getShareMemory(SHARE_PTR memPtr)
	{
		if (memPtr.key >= 0 && memPtr.key < mAllocList.size())
		{
			ShareMemAO *p = mAllocList[memPtr.key];
			if (p != NULL)
			{
				int memKey = p->GetKey();
				char *pSharePtr = p->GetDataPtr();
				return pSharePtr + memPtr.pos;
			}
		}
		return NULL;
	}

	//void DBRecordShareMemoryPool::ProcessRecordDataSave(MySqlDBTool *pDBTool)
	//{
	//	AString     sqlString;

	//	//int stateDataSize = mRecord->_getStateDataSize();
	//	int recordDataSize = mObjectSize; // mField->getDataLength() + 1 + stateDataSize;
	//	int stateDataPos = mpMemTableData->mField->getDataLength();

	//	for (int i = 0; i < mAllocList.size(); ++i)
	//	{
	//		ShareMemAO *pMem = mAllocList[i];
	//		if (pMem == NULL)
	//			continue;
	//		int count = pMem->GetSize() / recordDataSize;
	//		if (recordDataSize*count != pMem->GetSize())
	//		{
	//			ERROR_LOG("Check memory size error, last size %d != %d * %d", pMem->GetSize(), recordDataSize, count);
	//			continue;
	//		}


	//		for (int n = 0; n < count; ++n)
	//		{
	//			char *szRecordData = pMem->GetDataPtr() + recordDataSize * n;

	//			// 状态数据是 记录数据之后
	//			_UpdateShareMemoryRecord(pDBTool, szRecordData + 1, sqlString, mTempSQLData, szRecordData + 1 + stateDataPos, false);				
	//		}

	//	}
	//}

	//!!! NOTE: 此函数仅用于关闭时, DB进程也已经关闭, 所以不需要再加锁, 直接检查落地
	void DBRecordShareMemoryPool::ProcessRecordDataMustSave(MySqlDBTool *pDBTool)
	{
		AString     sqlString;

		//int stateDataSize = mRecord->_getStateDataSize();
		int recordDataSize = mObjectSize; // mField->getDataLength() + 1 + stateDataSize;
		int stateDataPos = mpMemTableData->mField->getDataLength();

		for (int i = 0; i < mAllocList.size(); ++i)
		{
			ShareMemAO *pMem = mAllocList[i];
			if (pMem == NULL)
				continue;
			int count = pMem->GetSize() / recordDataSize;
			if (recordDataSize*count != pMem->GetSize())
			{
				ERROR_LOG("Check memory size error, last size %d != %d * %d", pMem->GetSize(), recordDataSize, count);
				continue;
			}

			for (int n = 0; n < count; ++n)
			{
				char *szRecordData = pMem->GetDataPtr() + recordDataSize * n;
				if (*szRecordData != eShareMemory_Free)
				{					
					//!!! 这个函数用于关闭时, DB进程也已经关闭, 所以不需要再加锁, 直接检查落地
						// 状态数据是 记录数据之后
						_UpdateShareMemoryRecord(pDBTool, szRecordData + 1, sqlString, mTempSQLData, szRecordData + 1 + stateDataPos, false);
				}
			}

		}
	}

	void DBRecordShareMemoryPool::_UpdateShareMemoryRecord(MySqlDBTool *pDBTool, char *szRecordData, AString &sqlString, SQLDataArray &fieldRecordData, char *szStateData, bool bInsert)
	{
		Auto<TempSaveIndexRecord> &tempRecord = mpMemTableData->mRecord;
		tempRecord->SetRecordData(szRecordData);
		tempRecord->mpStateData = szStateData;

		if (tempRecord->getUpdateInfo()->mShareUpdateCount == tempRecord->getUpdateInfo()->mAlreadyUpdateCount)
			return;

		if (pDBTool->_MakeSaveSqlData(sqlString, fieldRecordData, tempRecord.getPtr(), bInsert, false, NULL, true))
		{
			int affectCount = 0;
			bool bError = !pDBTool->exeSql(sqlString, fieldRecordData.mDataArray, &affectCount);
			//if (bError && !fieldRecordData.empty())
			//{
			//	for (int i = 0; i < fieldRecordData.size(); ++i)
			//	{
			//		if (fieldRecordData[i]->dataSize() > 16 * 1024 * 1024) // 64000)
			//		{
			//			ERROR_LOG("严重错误: Data %d size %d >64000\r\n >%s", i, fieldRecordData[i]->dataSize(), sqlString.c_str());
			//		}
			//	}
			//}

			if (affectCount <= 0)
			{
				if (pDBTool->_MakeSaveSqlData(sqlString, fieldRecordData, tempRecord.getPtr(), !bInsert, false, NULL, true))
				{
					bError = !pDBTool->exeSql(sqlString, fieldRecordData.mDataArray, &affectCount);

					if (affectCount <= 0 && bError)
					{
						WARN_LOG("更新或保存记录失败 [%s] at table [%s], SQL>%s", tempRecord->get(0).c_str(), mpMemTableData->GetTableName(), sqlString.c_str());
						NOTE_LOG("MYSQL ERROR: %s", pDBTool->getErrorMsg());
					}
				}
			}
			if (affectCount > 0)
			{
				DEBUG_LOG("Succeed save %s at table %s", tempRecord->get(0).c_str(), mpMemTableData->GetTableName());
			}
			tempRecord->FullAllUpdate(false);
		}
		tempRecord->SetRecordData(NULL);
	}

	void DBRecordShareMemoryPool::UpdateShareMemoryRecord(MySqlDBTool *pDBTool, int shareMemIndex, int offsetPos, char *szStateData, byte stateDataSize, bool bInsert)
	{
		Auto<TempSaveIndexRecord> &tempRecord = mpMemTableData->mRecord;
		if (stateDataSize != tempRecord->_getStateDataSize())
		{
			ERROR_LOG("Record state size error %d != state size %d", stateDataSize, tempRecord->_getStateDataSize());
			return;
		}
		ShareMemAO *pMem = mAllocList.find(shareMemIndex);
		if (pMem != NULL)
		{
			if ((pMem->GetDataPtr() + offsetPos + mObjectSize) // mpMemTableData->mField->getDataLength())
				<= (pMem->GetDataPtr() + pMem->GetSize())
				)
			{
				AString sqlString;
				
				_UpdateShareMemoryRecord(pDBTool, pMem->GetDataPtr() + offsetPos, sqlString, mTempSQLData, szStateData, bInsert);
			}
			else
				ERROR_LOG("Over share memory %d , offset pos %d", shareMemIndex, offsetPos);
		}
		else
			ERROR_LOG("No exist share memory %d", shareMemIndex);
	}

	void DBRecordShareMemoryPool::SaveMemPtrData(DataStream *pDestData)
	{
		Array<int> tempArray;
		tempArray.resize((int)mAllocList.size());
		for (int i = 0; i < mAllocList.size(); ++i)
		{
			tempArray[i] = (int)mAllocList[i]->GetKey();
		}
		if (!pDestData->writeArray(tempArray))
			ERROR_LOG("Save mem key fail");
	}

}