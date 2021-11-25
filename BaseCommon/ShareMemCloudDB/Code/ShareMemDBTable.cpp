#include "ShareMemDBTable.h"

#include "ShareMemAO.h"
#include "CoroutineTool.h"
#include "Event.h"
#include "ShareMemCloudDBNode.h"

#include "StringHashIndex.h"
#include "Int64MapIndex.h"
#include "MultipleInt64MapIndex.h"
#include "MultipleStringIndex.h"
#include "IntEasyHashIndex.h"
#include "Int64HashIndex.h"
#include "StringEasyHashIndex.h"
#include "DBDefine.h"
#include "mShareMemTableData.h"
#include "MultipleUInt64MapIndex.h"

namespace NetCloud
{

	NetCloud::ShareMemDBTable::ShareMemDBTable(ShareMemCloudDBNode *pDB) 
		: CloudDBTable(pDB, eInitNullField)
		, mUpdateRecordList(1024*1024)
		, mUpdateCount(0)
		, mAlreadyUpdateCount(0)
	{
		if (mpDB!=NULL)
			mOwnerPtr->mField = MEM_NEW  ShareMemFieldIndex();
	}

	void NetCloud::ShareMemDBTable::ClearAll()
	{
		// 直接设置所有记录数据空间为NULL
		ARecordIt it = GetRecordIt();
		if (it)
		for (; *it; ++(*it) )
		{
			Auto<ShareMemRecord> re = it->_record();
			if (re)
				re->_freeData();
		}
		SkipBaseTable::ClearAll();
		for (int i = 0; i < mShareMemList.size(); ++i)
		{
			MEM_DELETE mShareMemList[i];
		}
		mShareMemList.clear(false);
	}

	bool NetCloud::ShareMemDBTable::RestoreFromShareMem(const Array<int> &sharePtrList)
	{
		ClearAll();

		Auto<ShareMemFieldIndex> field = GetField();
		DSIZE  recordDataSize = field->_getRecordMemSize();

		for (int i = 0; i < sharePtrList.size(); ++i)
		{
			SM_KEY key = (SM_KEY)sharePtrList[i];

			ShareMemAO *pShareMem = MEM_NEW ShareMemAO();

			UINT size = 0;
			if (pShareMem->Attach(key, size) == FALSE)
			{
				MEM_DELETE pShareMem;
				ERROR_LOG("[%u] Share memory key attach fail", key);
				continue;
			}

			int count = pShareMem->GetSize() / recordDataSize;
			if (recordDataSize*count != pShareMem->GetSize())
			{
				ERROR_LOG("Check memory size error, last size %d != %d * %d", pShareMem->GetSize(), recordDataSize, count);
				continue;
			}
			for (int n = 0; n < count; ++n)
			{
				char *szRecordData = pShareMem->GetDataPtr() + recordDataSize * n;
				if (*szRecordData == eShareMemory_Used)
				{
					// 状态数据是 记录数据之后
					_RestoreRecord(szRecordData + 1);
				}
			}
			mShareMemList.push_back(pShareMem);
		}
		return true;
	}

	bool NetCloud::ShareMemDBTable::_RestoreRecord(char *pRecordMemPtr)
	{
		Auto< ShareMemRecord> record = NewRecord();
		record->SetShareMem( pRecordMemPtr );
		return AppendRecord(record, true);
	}

	ARecord NetCloud::ShareMemDBTable::CreateRecord(Int64 nIndex, bool bReplace)
	{
		ARecord existRe = GetRecord((Int64)nIndex);
		if (existRe)
		{
			if (bReplace)
			{
				existRe->InitData();
				existRe->InitNewData();
				existRe->_set(0, (Int64)nIndex);
				existRe->FullAllUpdate(true);
				return existRe;
			}
			return ARecord();
		}

		return _ReqeustMemDBCreateRecord(STRING(nIndex));
	}

	ARecord NetCloud::ShareMemDBTable::CreateRecord(const char* szIndex, bool bReplace)
	{
		ARecord existRe = GetRecord(szIndex);
		if (existRe)
		{
			if (bReplace)
			{
				existRe->InitData();
				existRe->InitNewData();
				existRe->_set(0, szIndex);
				existRe->FullAllUpdate(true);
				return existRe;
			}
			return ARecord();
		}

		return _ReqeustMemDBCreateRecord(szIndex);

	}

	void* NetCloud::ShareMemDBTable::GetShareRecordMem(SHARE_PTR recordPtr)
	{
		if (recordPtr.key >= 0 && recordPtr.key < mShareMemList.size())
		{
			ShareMemAO  *pMem = mShareMemList[recordPtr.key];
			if (pMem != NULL && recordPtr.pos<(int)pMem->GetSize())
				return pMem->GetDataPtr() + recordPtr.pos;
			else
				ERROR_LOG("%d:%d Memory ptr is error", recordPtr.key, recordPtr.pos);
		}
		else
			ERROR_LOG("No exist share memory %d:%d", recordPtr.key, recordPtr.pos);

		return NULL;
	}

	void ShareMemDBTable::LowProcess()
	{
		UInt64 now = TimeManager::NowTick();

		int count = 0;
		for (Auto<ShareMemRecord> record = mUpdateRecordList.value(); !mUpdateRecordList.empty(); record = mUpdateRecordList.value())
		{
			mUpdateRecordList._remove(mUpdateRecordList.__getRoot());		
			
			if (++count >= 100)
			{
				if (TimeManager::NowTick() - now > 1)
				{
					DEBUG_LOG("More then 1 milsecond %s", GetTableName());
					break;
				}
				count = 0;
			}

			if (record)
			{
				
				++mAlreadyUpdateCount;
			}
		}

	
	}

	ARecord ShareMemDBTable::GrowthNewRecord(DataStream *recordData /*= NULL*/)
	{
		return _ReqeustMemDBCreateRecord("", true);

		//if (mMainKeyIndex)
		//{
		//	// NOTE:  使用分布表格, 产生的KEY, 必须是对应的哈希槽
		//	Int64 newKey = mLastMaxKey + 1;
		//	if (!mKeyHashSlotList.empty())
		//	{
		//		short hashSlot = HASH_SLOT(newKey);
		//		if (!mKeyHashSlotList.exist(hashSlot))
		//		{
		//			// 分配到SLOT范围内
		//			// 获取说明, 找到比当前新的KEY哈希大的哈希, 即下一个值, 不存在, 则分配到第一个最小的哈希+哈希总数
		//			bool bHave = false;
		//			for (int i = 0; i < mKeyHashSlotList.size(); ++i)
		//			{
		//				if (mKeyHashSlotList.getKey(i) > hashSlot)
		//				{
		//					bHave = true;
		//					newKey += mKeyHashSlotList.getKey(i) - hashSlot;
		//					break;
		//				}
		//			}
		//			if (!bHave)
		//			{
		//				newKey += mKeyHashSlotList.getKey(0) + DB_HASH_SLOT_COUNT - hashSlot;
		//			}
		//		}
		//	}

		//	return CreateRecord(newKey, false);
		//}
		//ERROR_LOG("%s Main record index not init create", GetTableName());
		//return ARecord();
	}

	ARecord ShareMemDBTable::NewRecord()
	{
		return MEM_NEW ShareMemRecord(this);
	}

	ARecord ShareMemDBTable::_NewRecord()
	{
		return MEM_NEW ShareMemRecord(this);
	}

	bool ShareMemDBTable::DeleteRecord(ARecord record)
	{
		Auto<ShareMemRecord> re = record;
		if (re)
		{
			AString recordKey = re[0];
			void *pData = re->_getData();
			if (RemoveRecord(record))
			{
				for (int i = 0; i < mShareMemList.size(); ++i)
				{
					ShareMemAO *pMem = mShareMemList[i];
					if (pMem != NULL)
					{
						recordKey = re[0];
						char *pSharePtr = pMem->GetDataPtr();
						if (pData >= pSharePtr && pData < (pSharePtr + pMem->GetSize()) )
						{
							int key = pMem->GetKey();
							SHARE_PTR  ptr;
							ptr.key = i;
							ptr.pos = (int)((char*)pData - pSharePtr);

							AutoEvent notifyEvent = mpDB->StartShareMemMsg("DM_NotifyDeleteRecord");
							notifyEvent["TABLE_NAME"] = GetTableName();
							notifyEvent["RECORD_KEY"] = recordKey;
							notifyEvent["RECORD_PTR"] = ptr.ptr;
							bool bSend = notifyEvent->Send();
							if (!bSend)
								ERROR_LOG("Send DM_NotifyDeleteRecord fail");
							return bSend;
						}
					}
				}

				ERROR_LOG("Remove record fail %s, from table %s, find not share memory PTR", record[0].string().c_str(), GetTableName());
			}
			else
				ERROR_LOG("Remove record fail %s, from table %s", record[0].string().c_str(), GetTableName());
		}
		return false;
	}

	ARecord ShareMemDBTable::_ReqeustMemDBCreateRecord(const AString &key, bool bGrowth)
	{
		if (CORO == 0)
		{
			ERROR_LOG("CreateRecord Must call in coroutine");
			return ARecord();
		}

		// 通过异步请求共享进程进行分配记录内存, 然后再通过恢复新建追加
		NiceData requestData;
		requestData["TABLE_NAME"] = mTableName;
		requestData["RECORD_KEY"] = key;
		requestData["IS_GROWTH"] = bGrowth;
		AutoEvent respCreateRecordData = mpDB->AwaitRequestMemDB("DM_CreateRecord", &requestData, 10000);
		if (!respCreateRecordData)
		{
			ERROR_LOG("Request mem create record fail");
			return ARecord();
		}

		UInt64 recordPtrValue = respCreateRecordData["RECORD_PTR"];
		if (recordPtrValue == 0)
		{
			ERROR_LOG("Mem create record response record ptr is NULL");
			return ARecord();
		}
		SHARE_PTR recordPtr;
		recordPtr.ptr = recordPtrValue;
		void *pRecordData = GetShareRecordMem(recordPtr);
		if (pRecordData == NULL)
		{
			ERROR_LOG("Get record share memory ptr fail >%d:%d", recordPtr.key, recordPtr.pos);
			return ARecord();
		}

		Auto<ShareMemRecord> r = NewRecord();
		r->SetShareMem((char*)pRecordData);

		if (AppendRecord(r, true))
			return r;

		ERROR_LOG("Append new record fail > %s", key.c_str());
		return ARecord();
	}

	void ShareMemDBTable::AppendShareMem(int key, int arrayIndex)
	{
		if (mShareMemList.size() != arrayIndex)
		{
			ERROR_LOG("Share mem index error, now %d != %d", (int)mShareMemList.size(), arrayIndex);
			return;
		}
		ShareMemAO  *pMem = MEM_NEW ShareMemAO();
		UINT size = 0;
		if (pMem->Attach(key, size))
		{
			mShareMemList.push_back(pMem);
		}
		else
		{
			delete pMem;
			ERROR_LOG("Share attack fail > key %d", key);
		}
	}


	void ShareMemRecord::OnUseChanged(int useCount)
	{
		if (_getData() == NULL)
			return;

	}


}