#include "mShareMemTableData.h"

#include "mNewCreateShareMemFieldIndex.h"
#include "MySqlDBTool.h"
#include "TimeManager.h"
#include "mShareMemDBManager.h"
#include "DBDefine.h"

namespace NetCloud
{

	ShareMemTableData::~ShareMemTableData()
	{
		Auto<NewCreateShareMemFieldIndex> f = mField;
		if (f)
			f->mRecordMemPool.Close();
	}

	bool ShareMemTableData::InitField(const AString &strFieldInfo, AString &resultInfo)
	{
		if (IS_SHAREMEM_TABLE(mType))
			mField = MEM_NEW NewCreateShareMemFieldIndex(this);
		else
			mField = MEM_NEW BaseFieldIndex();

		bool b = mField->FullFromString(strFieldInfo);
		mField->_updateInfo();
		if (b && mField->check())
		{
			if(IS_SHAREMEM_TABLE(mType))
			{
				//检查是否全部允许使用共享内存
				bool bError = false;
				for (int i = 0; i < mField->getCount(); ++i)
				{
					if (!mField->getFieldInfo(i)->CanUseShareMemory())
					{
						AString error;
						error.Format("\r\n[%s] field %s : type [%s] can not use share memory", GetTableName(), mField->getFieldInfo(i)->getName().c_str(), mField->getFieldInfo(i)->getTypeString());
						resultInfo += error;
						ERROR_LOG(error.c_str());
						bError = true;
					}
				}
				if (bError)
					return false;
			}
			mTable = MEM_NEW SkipBaseTable(eInitNullField);
			mTable->SetTableName(mTableName.c_str());
			mTable->InitField(mField);
			mRecord = MEM_NEW TempSaveIndexRecord(mTable);

			mCreateRecord = MEM_NEW TempLoadRecord(mTable);

			FieldInfo info = mField->getFieldInfo(0);
			mKeyFieldName = info->getName();
			mbKeyIsString = info->getType() == FIELD_CHAR_STRING;
			return true;
		}
		resultInfo += "Field check fail";
		return false;
	}

	void ShareMemTableData::SetSlotData(const AString &slotData, EasyMap<short, bool> &resultSlotMap)
	{
		resultSlotMap.clear(false);
		if (slotData.length() > 0)
		{
			StringArray  strList;
			if (AString::Split(slotData.c_str(), strList, "~", 3) >= 2)
			{
				int begin = TOINT(strList[0].c_str());
				int end = TOINT(strList[1].c_str());
				if (begin <= end)
				{
					for (int i = begin; i <= end; ++i)
					{
						resultSlotMap.insert(i, true);
					}
					return;
				}
			}
			else if (slotData!="*")
				ERROR_LOG("Slot info format [0~99], now error > %s", slotData.c_str());
		}
	}

	AString ShareMemTableData::LoadAllRecord(tDBTool *pMySql)
	{
		// 内存表在DB进程加载, 不加载共享内存
		if (!IS_SHAREMEM_TABLE(mType))
		{
			AString info;
			info.Format("[%s]  is only memory table [COOL_MEMORY_TABLE], do not load record in share process", GetTableName());
			return info;
		}

		// 阻塞加载
		//先获取总的记录数量
		AString sql;
		sql.Format("select count(*) from `%s`", GetTableName());
		AutoNice resultData = pMySql->ExeSqlFunction(sql, true);
		if (!resultData)
		{
			AString info;
			info.Format("DB[%s]获取记录数量失败", GetTableName());
			return info;
		}
		int totalNum = resultData["count(*)"];
		int count = 0;
		int failCount = 0;
		if (totalNum > 0)
		{
			if (mType == SHARE_MEM_DATA_TABLE)
			{
				AString fieldList = mField->getFieldInfo(0)->getName();
				for (int i=1; i<mField->getCount(); ++i)
				{
					fieldList += ", ";
					fieldList += mField->getFieldInfo(i)->getName();
				}
				sql.Format("SELECT %s from `%s`", fieldList.c_str(), GetTableName());
			}
			else
				sql.Format("SELECT * from `%s`", GetTableName());

			pMySql->exeSql(sql, true);
			int showCount = 0;
			UInt64 now = TimeManager::NowTick();

			Auto< TempLoadRecord> tempLoadRecord = MEM_NEW TempLoadRecord(mTable);
			ARecord re = tempLoadRecord;

			Auto<NewCreateShareMemFieldIndex> field = mField;
			AString key;
			tempLoadRecord->_alloctData(0);

			while (failCount + count < totalNum)
			{
				if (!pMySql->LoadRecord(re))
				{
					++failCount;
					tempLoadRecord->_freeData();
					break;
				}				
				
				SHARE_PTR recordPtr = field->getShareMemoryPtr(tempLoadRecord->GetRecordData());				
				if (mbKeyIsString)
				{
					if (re->get(0, key))
					{
						mStringRecordHash.insert(key, recordPtr);
						tempLoadRecord->SetRecordData(NULL);
						tempLoadRecord->_alloctData(0);
					}
					else
						ERROR_LOG("Get record string key fail");
				}
				else
				{
					Int64 wID = 0;
					if (re->get(0, wID))
					{
						mRecordHash.insert(wID, recordPtr);
						tempLoadRecord->SetRecordData(NULL);
						tempLoadRecord->_alloctData(0);
						// 更新最大KEY值
						if (wID > mLastMaxKey)
							mLastMaxKey = wID;
					}
					else
						ERROR_LOG("Get record int64 key fail");
				}
				
				++count;
				if (++showCount >= 100000)
				{
					showCount = 0;
					PRINT("[%s] <%d%%> now load record [%d], use time %llu\r\n", GetTableName(), (int)(count * 100 / totalNum), count, TimeManager::NowTick() - now);
				}
			}
		}
		AString info;
		info.Format("[%s]  Succeed finish load record %d, exist fail %d, total %d", GetTableName(), count, failCount, totalNum);
		return info;
	}

	SHARE_PTR ShareMemTableData::NewCreateRecord(const AString &key)
	{
		Int64 nKey = 0;
		if (mbKeyIsString)
		{
			SHARE_PTR  *pPtr = mStringRecordHash.findPtr(key);
			if (pPtr)
			{
				WARN_LOG("%s Create record already exist > %s", mTableName.c_str(), key.c_str());
				return *pPtr;
			}
		}
		else
		{
			nKey = TOINT64(key.c_str());
			SHARE_PTR  *pPtr = mRecordHash.findPtr(nKey);
			if (pPtr)
			{
				WARN_LOG("%s Create record already exist > %s", mTableName.c_str(), key.c_str());
				return *pPtr;
			}
		}

		mCreateRecord->SetRecordData(NULL);
		mCreateRecord->_alloctData(0);

		mCreateRecord[0] = key;
		
		// 能够插入到数据库才认为成功，存在会插入失败
		AString sqlString;

		if (MySqlDBTool::_MakeSaveSqlData(sqlString, mTempSQLData, mCreateRecord.getPtr(), true, false, NULL, true))
		{
			int affectCount = 0;
			bool bError = !mpDBMgr->mMySQLTool->exeSql(sqlString, mTempSQLData.mDataArray, &affectCount);
			if (bError || affectCount <= 0)
			{
				mCreateRecord->_freeData();
				ERROR_LOG("[%s] Create record fail, may be already exist :\r\n%s", key.c_str(), mpDBMgr->mMySQLTool->getErrorMsg());
				return SHARE_PTR();
			}
			if (mpDBMgr->mBackDBMySQL)
			{
				if (!mpDBMgr->mBackDBMySQL->exeSql(sqlString, mTempSQLData.mDataArray, &affectCount))
					ERROR_LOG("[%s] Back DB create record fail, may be already exist :\r\n%s", key.c_str(), mpDBMgr->mBackDBMySQL->getErrorMsg());
			}
		}
		else
		{
			mCreateRecord->_freeData();
			ERROR_LOG("%s [%s] Make sql fail", GetTableName(), key.c_str());
			return SHARE_PTR();
		}
		mCreateRecord->FullAllUpdate(false);
		Auto<NewCreateShareMemFieldIndex> field = mField;
		SHARE_PTR recordPtr = field->getShareMemoryPtr(mCreateRecord->GetRecordData());
		mCreateRecord->SetRecordData(NULL);
		if (mbKeyIsString)
			mStringRecordHash.insert(key, recordPtr);
		else
			mRecordHash.insert(nKey, recordPtr);
		return recordPtr;
	}

	SHARE_PTR ShareMemTableData::GrowthNewRecord()
	{
		// NOTE:  使用分布表格, 产生的KEY, 必须是对应的哈希槽
		Int64 newKey = mLastMaxKey + 1;
		if (!mKeyHashSlotList.empty())
		{
			short hashSlot = HASH_SLOT(newKey);
			if (!mKeyHashSlotList.exist(hashSlot))
			{
				// 分配到SLOT范围内
				// 获取说明, 找到比当前新的KEY哈希大的哈希, 即下一个值, 不存在, 则分配到第一个最小的哈希+哈希总数
				bool bHave = false;
				for (int i = 0; i < mKeyHashSlotList.size(); ++i)
				{
					if (mKeyHashSlotList.getKey(i) > hashSlot)
					{
						bHave = true;
						newKey += mKeyHashSlotList.getKey(i) - hashSlot;
						break;
					}
				}
				if (!bHave)
				{
					newKey += mKeyHashSlotList.getKey(0) + DB_HASH_SLOT_COUNT - hashSlot;
				}
			}
		}
		mLastMaxKey = newKey;
		return NewCreateRecord(STRING(newKey));
	}

	bool ShareMemTableData::DeleteRecord(UInt64 recordDataPtr, const AString &recordKey)
	{
		SHARE_PTR  dataPtr;
		dataPtr.ptr = recordDataPtr;

		Auto<NewCreateShareMemFieldIndex> field = mField;
		void* pMem = field->getShareMemory(dataPtr);
		if (pMem == NULL)
		{
			ERROR_LOG("Get record data fail from SHARE_PTR  %d : %d", dataPtr.key, dataPtr.pos);
			return false;
		}
		mCreateRecord->SetRecordData( (char*)pMem );
		AString key = mCreateRecord[0];
		if (key == recordKey)
		{
			FieldInfo info = mField->getFieldInfo(0);
			AString sqlString;
			if (mbKeyIsString)
				sqlString.Format("Delete from `%s` where  `%s` = '%s'", mTableName.c_str(), info->getName().c_str(), key.c_str());
			else
				sqlString.Format("Delete from `%s` where `%s` = %s", mTableName.c_str(), info->getName().c_str(), key.c_str());
			int affectCount = 0;
			bool bResult = mpDBMgr->mMySQLTool->exeSql(sqlString, false, &affectCount);

			if (mpDBMgr->mBackDBMySQL)
			{
				int x = 0;
				if (!mpDBMgr->mBackDBMySQL->exeSql(sqlString, false, &x))
					ERROR_LOG("[%s] Back DB Delete record fail : %s", key.c_str(), mpDBMgr->mBackDBMySQL->getErrorMsg());
			}

			if (bResult && affectCount > 0)
			{
				if (mbKeyIsString)
					mStringRecordHash.erase(recordKey);
				else
					mRecordHash.erase(TOINT64(recordKey.c_str()));

				mCreateRecord->_freeData();
				DEBUG_LOG("Succeed delete record > %s  from table %s", key.c_str(), mTableName.c_str());
				return true;
			}
		}
		mCreateRecord->SetRecordData(NULL);
		DEBUG_LOG("Fail delete record > %s  from table %s", key.c_str(), mTableName.c_str());
		return false;
	}

	void ShareMemTableData::UpdateShareRecord(const  AString &recordKey, Buffer &recordStateData, uint nowUpdateCount)
	{
		SHARE_PTR *dataPtr = NULL;
		if (mbKeyIsString)
			dataPtr = mStringRecordHash.findPtr(recordKey.c_str());
		else
			dataPtr = mRecordHash.findPtr(TOINT64(recordKey.c_str()));

		if (dataPtr == NULL)
		{
			ERROR_LOG("No exist record %s", recordKey.c_str());
			return;
		}

		Auto<NewCreateShareMemFieldIndex> field = mField;
		void* pMem = field->getShareMemory(*dataPtr);
		if (pMem == NULL)
		{
			ERROR_LOG("Get record %s data fail from SHARE_PTR  %d : %d",  recordKey.c_str(), dataPtr->key, dataPtr->pos);
			return;
		}
		if (recordStateData.size() < mCreateRecord->_getStateDataSize() - sizeof(ShareUpdateInfo))
		{
			ERROR_LOG("Record %s sate data too small from SHARE_PTR  %d : %d, now %d < %d"
				, recordKey.c_str()
				, dataPtr->key
				, dataPtr->pos
				, recordStateData.size()
				, mCreateRecord->_getStateDataSize() - sizeof(ShareUpdateInfo)
			);
			return;
		}
		mCreateRecord->SetRecordData((char*)pMem);
		mCreateRecord->mpStateData = (char*)recordStateData.data();

		Auto<MySqlDBTool> tool = mpDBMgr->mMySQLTool;
		AString sql;

		if (tool->_MakeSaveSqlData(sql, mTempSQLData, mCreateRecord.getPtr(), false))
		{
			if (tool->exeSql(sql, mTempSQLData.mDataArray))
			{
				mCreateRecord->getUpdateInfo()->mAlreadyUpdateCount = nowUpdateCount;

				// 同步到备份库
				if (mpDBMgr->mBackDBMySQL)
				{
					Auto<MySqlDBTool> t = mpDBMgr->mBackDBMySQL;
					if (!t->exeSql(sql, mTempSQLData.mDataArray))
						ERROR_LOG("[%s] Back DB Update record fail : %s", mCreateRecord[0].string().c_str(), mpDBMgr->mBackDBMySQL->getErrorMsg());
				}
			}
			else
				ERROR_LOG("Run sql error : %s >\r\n%s", sql.c_str(), tool->getErrorMsg());
		}
		else
			ERROR_LOG("Make sql fail %s", mCreateRecord->ToNiceData()->dump().c_str());


		mCreateRecord->SetRecordData(NULL);
		mCreateRecord->mpStateData = NULL;

	}

	//bool ShareMemTableData::AppendShareMemory(int index, int recordDataSize)
	//{
	//	mField->_updateInfo();
	//	Auto<IndexDBRecord> r = mRecord;
	//	if (mField->getDataLength() + 1 + r->_getStateDataSize() != recordDataSize)
	//	{
	//		ERROR_LOG("Record data size check fail %d != field need %d", mField->getDataLength() + 1 + r->_getStateDataSize(), recordDataSize)
	//			return false;
	//	}
	//	AString keyName;
	//	keyName.Format("%s_%d", mTableName.c_str(), index);
	//	int key = MAKE_INDEX_ID(keyName.c_str());
	//	ShareMemAO *pMem = MEM_NEW ShareMemAO();
	//	UINT size = 0;
	//	if (pMem->Attach(key, size) == TRUE)
	//	{
	//		mPoolMemoryList.Append(index, pMem);
	//		return true;
	//	}
	//	else
	//	{
	//		delete pMem;
	//		ERROR_LOG("Attach share memory fail , may be no exist");
	//	}
	//	return false;
	//}



}