#pragma once

#include "DBUser_t_data.h"
#include "ConfigEnum.h"
#include "DBUser_t_commodity.h"
#include "DBUser_t_commodity_data1.h"
#include "DBUser_t_commodity_data2.h"
#include "ServerToolMsg.h"
//-------------------------------------------------------------------------
#define SAFE_DATA_MAX_SIZE			(1*1024*1024)
#define WAIT_DESTORY_CACHE_OVERTIME		(30)	// 缓存等待消毁时间, 被使用后, 会从新开始计时

//-------------------------------------------------------------------------
// 大数据上传缓存组件
//-------------------------------------------------------------------------
class UploadCacheComponent : public Component
{
	class BigDataCache : public AutoBase
	{
	public:
		int mActiveSecond = TimeManager::Now();
		int mCacheID = 0;
		AutoData	mBigData;
	};
	typedef Auto<BigDataCache> ADataCache;

protected:
	EasyHash<MSG_ID, ADataCache>		mDataCacheList;

	EasyStack<int> mFreeIDList;

	int mMaxID = 0;

	int AlloctID()
	{
		if (!mFreeIDList.empty())
		{
			return mFreeIDList.pop();
		}
		return ++mMaxID;
	}

	void FreeID(int id)
	{
		mFreeIDList.push(id);
	}

public:
	// 取出数据, 返回数据, 并清除缓存
	AutoData TakeOutData(int nCacheID)
	{
		auto data = mDataCacheList.find(nCacheID);
		if (data)
		{
			OnCacheDestory(data);
			mDataCacheList.erase(nCacheID);
			return data->mBigData;
		}
		else
			ERROR_LOG("No exist cache data %d", nCacheID);
		
		return AutoData();
	}

	void OnCacheDestory(ADataCache d)
	{
		FreeID(d->mCacheID);
	}

	Hand<DBUserComponent> GetDBUserComponent(const AString &tableName)
	{
		return Hand<DBUserComponent>();
	}

	virtual void LowUpdate() override
	{
		auto now = TimeManager::Now();
		for (auto it = mDataCacheList.begin(); it; )
		{
			auto cache = *it;
			if (cache->mActiveSecond + WAIT_DESTORY_CACHE_OVERTIME < now)
			{
				OnCacheDestory(cache);
				it.erase();
				continue;
			}
			++it;
		}
	}

public:
	void On(DB_ReqeustBigDataUpload &req, DB_ResponseBigDataCacheID &response, UnitID, int)
	{
		if (req.mSize > SAFE_DATA_MAX_SIZE)
		{
			response.mCacheID = 0;
			response.mError = eError_Resource_TooLarge;
		}

		ADataCache cache = MEM_NEW BigDataCache();
		cache->mCacheID = AlloctID();
		cache->mBigData = MEM_NEW DataBuffer();
		mDataCacheList.insert(cache->mCacheID, cache);

		response.mCacheID = cache->mCacheID;
	}

	void On(DB_UploadPartData &req, DB_ResponseUploadPartDataResult &response, UnitID, int)
	{
		auto cache = mDataCacheList.find(req.mCacheID);
		if (cache)
		{
			DSIZE size = req.mPartData->dataSize();
			if (cache->mBigData->dataSize()+size > SAFE_DATA_MAX_SIZE)
			{
				// 安全检查, 资源太大, 会被移除
				OnCacheDestory(cache);
				mDataCacheList.erase(req.mCacheID);
				WARN_LOG("Upload %d cache data size too large %d > %d, then remove", req.mCacheID, cache->mBigData->dataSize()+size, SAFE_DATA_MAX_SIZE);				
				response.mError = eError_Resource_TooLarge;
				return;
			}

			cache->mActiveSecond = TimeManager::Now();
			cache->mBigData->_write(req.mPartData->data(), req.mPartData->dataSize());
			response.mError = 0;
		}
		else
			response.mError = eError_Resource_NoExist;
	}

public:
	virtual void RegisterMsg() override
	{
		REG_COMP_MSG(UploadCacheComponent, DB_ReqeustBigDataUpload, DB_ResponseBigDataCacheID);
		REG_COMP_MSG(UploadCacheComponent, DB_UploadPartData, DB_ResponseUploadPartDataResult);
	}
};

//-------------------------------------------------------------------------
// 可动态更新的数据组件, 如 图片, 用户数据等
class DataComponent : public DBUser_t_data
{
public:
	EasyMap<Int64, ARecord> mResourcesList;
	
	Hand<UploadCacheComponent> mBigDataCacheComponent;

public:
	bool CheckLoadRecord(Int64 key, const AString &checkMd5)
	{
		ARecord existRe = mResourcesList.find(key);
		if (existRe)
		{
			ResetRecord(existRe);
			if (checkMd5.length() == 32 && strMD5() != checkMd5)
			{
				if (ResetRecord(key))
					mResourcesList.insert(key, mDataRecord);
			}
		}
		else
			if (ResetRecord(key))
				mResourcesList.insert(key, mDataRecord);

		return mDataRecord;
	}

	virtual bool GetDataInfo(const char *szKey, const AString &checkMD5, Int64 &size, AString &md5) override
	{
		Int64 key = TOINT64(szKey);
		
		if (CheckLoadRecord(key, checkMD5))
		{
			AutoData d = dataDATA();
			size = d ? d->dataSize() : 0;
			md5 = strMD5();
			return true;
		}
		else
			WARN_LOG("No exist data : %s", szKey);

		return false;
	}

	virtual AutoData GetData(const char *szKey, const AString &checkMD5, Int64 position, int size) override
	{ 
		Int64 key = TOINT64(szKey);

		if (CheckLoadRecord(key, checkMD5))
		{
			AutoData d = dataDATA();
			if (d && d->dataSize() > position)
			{
				AutoData result = MEM_NEW DataBuffer();
				int s = size;
				if (d->dataSize() - position < size)
					s = d->dataSize() - position;
				result->_write(d->data() + position, s);

				return result;
			}			
		}
		else
			WARN_LOG("No exist data : %s", szKey);

		return AutoData();
	}

public:
	AutoData TakeOutCacheData(int cacheID)
	{
		if (!mBigDataCacheComponent)
		{
			mBigDataCacheComponent = GetActor()->GetComponent<UploadCacheComponent>();
			if (!mBigDataCacheComponent)
			{
				ERROR_LOG("UploadCacheComponent no exist");
				return AutoData();
			}
		}
		
		return mBigDataCacheComponent->TakeOutData(cacheID);		
	}

public:
	void On(DB_UpdateSaveData &req, DB_ReponseSaveResult &response, UnitID sender, int)
	{
		req.DumpLog();

		if (req.mData->dataSize() > SAFE_DATA_MAX_SIZE)
		{
			response.mErrorCode = eError_Resource_TooLarge;
			return;
		}
		ARecord existRe = mResourcesList.find(req.mIndexName.toInt64());
		if (existRe)
		{
			ResetRecord(existRe);
		}
		else
		{
			mDataRecord = LoadRecord(req.mIndexName.toInt64());
			if (!mDataRecord)
				mDataRecord = mDBTable->CreateRecord(req.mIndexName.toInt64(), true);

			if (mDataRecord)
				mResourcesList.insert(req.mIndexName.toInt64(), mDataRecord);
		}
		DATA() = req.mData.getPtr();
		SetMD5(req.mMD5.c_str());
		SetTIME((int)TimeManager::Now());

		LowUpdate();


		response.mInfo = u8"Save succeed";
	}

	void On(DB_LoadData &req, DB_ResponseData &response, UnitID, int)
	{
		ARecord existRe = mResourcesList.find(req.mIndexName.toInt64());
		if (existRe)
		{
			ResetRecord(existRe);
			// 如果提供正确的验证MD5, 则只有相符才直接返回缓存中的, 即提供的MD5长度不等于32, 也直接返回缓存数据
			if (req.mCheckMD5.length()!=32 || strMD5() == req.mCheckMD5)
			{
				NOTE_LOG("%s 缓存有效, 直接返回", req.mIndexName.c_str());
				response.mData = dataDATA();
				response.mMD5 = strMD5();
				return;
			}
		}

		if (ResetRecord(req.mIndexName.c_str()))
		{
			mResourcesList.insert(wDBID(), mDataRecord);

			response.mData = dataDATA();
			response.mMD5 = strMD5();
		}
		else
			response.mErrorCode = eError_Resource_NoExist;
	}

	// 通过分包缓存更新保存
	void On(DB_UpdateSaveCacheBigData &req, DB_ResponseUpdateSaveCacheDataResult &response, UnitID, int)
	{
		AutoData data = TakeOutCacheData(req.mCacheID);
		if (!data)
		{
			response.mError = eError_Resource_NoExist;
			return;
		}

		// 更新保存
		if (data->dataSize() > SAFE_DATA_MAX_SIZE)
		{
			response.mError = eError_Resource_TooLarge;
			return;
		}

		// 验证	
		AString stringMd5 = data->md5();
		if (req.mCheckMD5.length()==32 && stringMd5 != req.mCheckMD5)
		{
			response.mError = eError_ResourceCheckFail;
			return;
		}

		
		if (req.mKey.length() <= 0)
		{
			if (ResetRecord(mDBTable->GrowthNewRecord()))
			{
				mResourcesList.insert(mDataRecord[0], mDataRecord);
				response.mDBKey = mDataRecord[0];
			}
			else
			{
				response.mError = eErrorCode_DBQeuryFail;
				return;
			}
		}
		else
		{
			response.mDBKey = req.mKey;
			Int64 key = req.mKey.toInt64();

			ARecord existRe = mResourcesList.find(key);
			if (existRe)
			{
				ResetRecord(existRe);
			}
			else
			{
				mDataRecord = LoadRecord(key);
				if (!mDataRecord)
					mDataRecord = mDBTable->CreateRecord(key, true);

				if (mDataRecord)
					mResourcesList.insert(key, mDataRecord);
			}
		}
		DATA() = data.getPtr();
		SetMD5(stringMd5.c_str());
		SetTIME((int)TimeManager::Now());
		response.mError = eErrorCodeNone;

		LowUpdate();

	}


public:
	virtual void RegisterMsg() override
	{
		REG_COMP_MSG(DataComponent, DB_UpdateSaveData, DB_ReponseSaveResult);
		REG_COMP_MSG(DataComponent, DB_LoadData, DB_ResponseData);
		REG_COMP_MSG(DataComponent, DB_UpdateSaveCacheBigData, DB_ResponseUpdateSaveCacheDataResult);
	}
};



//-------------------------------------------------------------------------
// 大数据下载服务组件, 主要提供分段数据
//-------------------------------------------------------------------------
class DownloadComponent : public Component
{
public:
	void On(DB_RequestDownloadData &req, DB_ResponseDataInfo &resp, UnitID, int)
	{
		Hand<DBUserComponent> comp = GetActor()->GetDBUserComponent(req.mTableName.c_str());
		if (!comp->GetDataInfo(req.mKey.c_str(), req.mCheckMD5, resp.mSize, resp.mMD5))
		{
			resp.mMD5.setNull();
			resp.mError = eError_Resource_NoExist;
		}
	}

	void On(DB_RequestDownloadPartData &req, DB_ResponsePartData &resp, UnitID, int)
	{
		Hand<DBUserComponent> comp = GetActor()->GetDBUserComponent(req.mTableName.c_str());
		resp.mPartData = comp->GetData(req.mKey.c_str(), "", req.mPosition, req.mRequestSize);		
		if (!resp.mPartData)
			resp.mError = eError_Resource_NoExist;
	}

public:
	virtual void RegisterMsg() override
	{
		REG_COMP_MSG(DownloadComponent, DB_RequestDownloadData, DB_ResponseDataInfo);
		REG_COMP_MSG(DownloadComponent, DB_RequestDownloadPartData, DB_ResponsePartData);
	}
};

//-------------------------------------------------------------------------
// 直接更新或获取记录
class SaveRecordComponent : public Component
{
public:
	void On(DB_RequestSaveRecord &req, DB_ResponseSaveRecord &resp, UnitID, int)
	{
		// 需要增加安全检验码
		Hand<DBUserComponent> comp = GetActor()->GetDBUserComponent(req.mTableName.c_str());
		if (!comp->SaveRecordByData(req.mKey.c_str(), req.mRecordData))
		{			
			resp.mError = eError_Resource_NoExist;
		}
	}

public:
	virtual void RegisterMsg() override
	{
		REG_COMP_MSG(SaveRecordComponent, DB_RequestSaveRecord, DB_ResponseSaveRecord);
	}
};
//-------------------------------------------------------------------------
// 直接获取记录
class LoadRecordComponent : public Component
{
public:
	void On(DB_RequestLoadRecord &req, DB_ResponseRecord &resp, UnitID, int)
	{
		Hand<DBUserComponent> comp = GetActor()->GetDBUserComponent(req.mTableName.c_str());
		AutoData d = comp->GetRecordData(req.mKey.c_str());
		if (!d)
		{
			resp.mError = eError_Resource_NoExist;
		}
		else
		{
			resp.mRecordData = d;			
		}
		if (req.mbNeedField)
		{
			resp.mFieldData = comp->GetTableFieldData();
		}
	}

public:
	virtual void RegisterMsg() override
	{
		REG_COMP_MSG(LoadRecordComponent, DB_RequestLoadRecord, DB_ResponseRecord);
	}
};
//-------------------------------------------------------------------------
// 主要用来保存提供动态数据仓库
// 可手动配置此对象的ID, 用于分库 [ 设定共有ID 0~N, HASH = DBID % (N+1) ]
//-------------------------------------------------------------------------

class DataActor : public DBActor
{
	AComponent mDataComponent;
	AComponent mCommodity;

	AComponent mCommodityData1;
	AComponent mCommodityData2;

public:
	virtual void Init() override
	{
			mDataComponent = AddComponent("DataComponent");
			AddComponent("UploadCacheComponent");
			AddComponent("DownloadComponent");

			AddComponent("SaveRecordComponent");
			AddComponent("LoadRecordComponent");

			mCommodity = AddComponent("DBUser_t_commodity");
			mCommodityData1 = AddComponent("DBUser_t_commodity_data1");
			mCommodityData2 = AddComponent("DBUser_t_commodity_data2");
	}

	virtual AComponent GetDBUserComponent(const char *szTableIndex) override
	{
		if (strcmp(szTableIndex, "t_commodity")==0)
			return mCommodity;
		
		if (strcmp(szTableIndex, "t_commodity_data1") == 0)
			return mCommodityData1;

		if (strcmp(szTableIndex, "t_commodity_data2") == 0)
			return mCommodityData2;

		return mDataComponent;
	}

public:
	virtual void RegisterMsg() override
	{
		REG_COMPONENT(DataComponent);
		REG_COMPONENT(UploadCacheComponent);
		REG_COMPONENT(DownloadComponent);

		REG_COMPONENT(SaveRecordComponent);
		REG_COMPONENT(LoadRecordComponent);

		REG_COMPONENT(DBUser_t_commodity);
		REG_COMPONENT(DBUser_t_commodity_data1);
		REG_COMPONENT(DBUser_t_commodity_data2);
	}
};

//-------------------------------------------------------------------------