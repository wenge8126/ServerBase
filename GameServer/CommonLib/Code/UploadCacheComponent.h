#pragma once

#include "Component.h"

//-------------------------------------------------------------------------
#define SAFE_DATA_MAX_SIZE			(1*1024*1024+64 * 1024*1024)  // 65M, 为了上传客户端
#define WAIT_DESTORY_CACHE_OVERTIME		(30)	// 缓存等待消毁时间, 被使用后, 会从新开始计时

//-------------------------------------------------------------------------
// 大数据上传缓存组件
//-------------------------------------------------------------------------
class UploadCacheComponent : public NetCloud::Component
{
	class BigDataCache : public AutoBase
	{
	public:
		UInt64 mActiveSecond = TimeManager::Now();
		int mCacheID = 0;
		AutoData	mBigData;
	};
	typedef Auto<BigDataCache> ADataCache;

protected:
	int mMaxDataSize = SAFE_DATA_MAX_SIZE;
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
	virtual void SetDataMaxSize(int limitSize) { mMaxDataSize = limitSize; }
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

	//Hand<DBUserComponent> GetDBUserComponent(const AString &tableName)
	//{
	//	return Hand<DBUserComponent>();
	//}

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
		if (req.mSize > mMaxDataSize)
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
		auto cache = mDataCacheList.find((MSG_ID)req.mCacheID);
		if (cache)
		{
			DSIZE size = req.mPartData->dataSize();
			if (cache->mBigData->dataSize() + size > mMaxDataSize)
			{
				// 安全检查, 资源太大, 会被移除
				OnCacheDestory(cache);
				mDataCacheList.erase((MSG_ID)req.mCacheID);
				WARN_LOG("Upload %d cache data size too large %d > %d, then remove", req.mCacheID, cache->mBigData->dataSize() + size, SAFE_DATA_MAX_SIZE);
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
