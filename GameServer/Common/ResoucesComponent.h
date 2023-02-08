#pragma once

#ifndef _INCLUDE_RESOURCESCOMPONENT_H_
#define _INCLUDE_RESOURCESCOMPONENT_H_
#include "DBUser_t_resources.h"
#include "ServerToolMsg.h"

// 可动态更新的资源组件, 如 配置, UI图片等
class ResourcesComponent : public DBUser_t_resources
{
public:
	EasyMap<AString, ARecord> mResourcesList;
	ARecord mResourceListRecord;

public:
	void On(MS_UpdateSaveResource &req, MS_ReponseSaveResult &response, UnitID sender, int)
	{
		req.DumpLog();
		NOTE_LOG(" DATA size %d", req.mResourceData->dataSize());

		ARecord existRe = mResourcesList.find(req.mIndexName);
		if (existRe)
		{
			ResetRecord(existRe);
		}
		else
		{
			mDataRecord = LoadRecord(req.mIndexName.c_str());
			if (!mDataRecord)
				mDataRecord = mDBTable->CreateRecord(req.mIndexName.c_str(), true);
		}
		DATA() = req.mResourceData.getPtr();
		SetMD5(req.mMD5.c_str());
		SetTIME((int)TimeManager::Now());

		LowUpdate();

		if (req.mIndexName == CONFIG_LIST_INDEX)
		{
			mResourceListRecord = mDataRecord;
		}


		response.mInfo = u8"保存资源成功: ";
		response.mInfo += req.mIndexName;
	}

	void On(CR_LoadResource &req, RC_ResponseResource &response, UnitID, int)
	{
		if (mResourceListRecord && req.mIndexName == CONFIG_LIST_INDEX)
		{
			ResetRecord(mResourceListRecord);
			response.mResourceData = dataDATA();
			response.mMD5 = strMD5();
			return;
		}

		ARecord existRe = mResourcesList.find(req.mIndexName);
		if (existRe)
		{
			ResetRecord(existRe);
			if (strMD5() == req.mMD5)
			{
				NOTE_LOG("%s 缓存有效, 直接返回", req.mIndexName.c_str());
				response.mResourceData = dataDATA();
				response.mMD5 = strMD5();
				return;
			}
		}

		if (ResetRecord(req.mIndexName.c_str()))
		{
			mResourcesList.insert(strNAME(), mDataRecord);

			response.mResourceData = dataDATA();
			response.mMD5 = strMD5();

			if (req.mIndexName == CONFIG_LIST_INDEX)
			{
				mResourceListRecord = mDataRecord;
			}
		}
		else
			response.mErrorCode = 1;
	}

public:
	virtual void RegisterMsg() override
	{
		REG_COMP_MSG(ResourcesComponent, MS_UpdateSaveResource, MS_ReponseSaveResult);
		REG_COMP_MSG(ResourcesComponent, CR_LoadResource, RC_ResponseResource);
	}
};

#endif