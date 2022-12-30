#pragma once

#include "DBUser_t_data.h"
#include "ConfigEnum.h"
#include "DBUser_t_commodity.h"
#include "DBUser_t_commodity_data1.h"
#include "DBUser_t_commodity_data2.h"
#include "ServerToolMsg.h"
#include "DBUser_t_template.h"
#include "UploadCacheComponent.h"
#include "DownloadComponent.h"
#include "RecordComponent.h"

//-------------------------------------------------------------------------
// 可动态更新的数据组件, 如 图片, 用户数据等
class DataComponent : public DBUser_t_data
{
public:
	EasyMap<Int64, ARecord> mResourcesList;
	
	Hand<UploadCacheComponent> mBigDataCacheComponent;

public:
	void Start()
	{
		DBUser_t_data::Start();

		AString key;
		bool b = LoadMaxKey(key);
		NOTE_LOG("%s load : %s Now max key : %s", b?"OK":"Fail", GetTableName(), key.c_str());
	}

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
// 主要用来保存提供动态数据仓库
// 可手动配置此对象的ID, 用于分库 [ 设定共有ID 0~N, HASH = DBID % (N+1) ]
//-------------------------------------------------------------------------

class DataActor : public Actor
{
	AComponent mDataComponent;
	AComponent mCommodity;

	AComponent mCommodityData1;
	AComponent mCommodityData2;
	AComponent mTemplate;

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
			mTemplate = AddComponent("DBUser_t_template");
	}

	virtual AComponent GetDBUserComponent(const AString &tableIndex) override
	{
		if (tableIndex == "t_commodity")
			return mCommodity;
		
		if (tableIndex == "t_commodity_data1")
			return mCommodityData1;

		if (tableIndex == "t_commodity_data2")
			return mCommodityData2;

		if (tableIndex=="t_data")
			return mDataComponent;

		if (tableIndex=="t_template")
			return mTemplate;

		ERROR_LOG("No exist table %s component", tableIndex.c_str());
		return AComponent();
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
		REG_COMPONENT(DBUser_t_template);
		
	}
};

//-------------------------------------------------------------------------