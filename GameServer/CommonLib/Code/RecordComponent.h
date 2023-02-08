#pragma once

#include "ServerToolMsg.h"
#include "Component.h"
#include "ConfigEnum.h"
#include "ActorManager.h"

//-------------------------------------------------------------------------
// 直接更新或获取记录
class SaveRecordComponent : public NetCloud::Component
{
public:
	//void On(DB_RequestSaveRecord &req, DB_ResponseSaveRecord &resp, UnitID, int)
	//{
	//	// 需要增加安全检验码
	//	Hand<DBUserComponent> comp = GetActor()->GetDBUserComponent(req.mTableName);
	//	if (!comp || !comp->SaveRecordByData(req.mKey.c_str(), req.mRecordData, req.mbGrowthKey, resp.mDBKey))
	//	{
	//		resp.mError = eError_Resource_NoExist;
	//	}
	//}

	void On(DB_RequestSaveRecordByNice &req, DB_ResponseSaveRecord &resp, UnitID, int)
	{
		if (req.mRecordData)
			NOTE_LOG("Record data : \r\n%s", req.mRecordData->dump().c_str());
		Hand<DBUserComponent> comp = GetActor()->GetDBUserComponent(req.mTableName);
		
		if (!comp || !comp->SaveRecordByNiceData(req.mKey.c_str(), req.mRecordData, req.mbGrowthKey, resp.mDBKey))
		{
			resp.mError = eError_Resource_NoExist;
		}
		
	}

public:
	virtual void RegisterMsg() override
	{
		//REG_COMP_MSG(SaveRecordComponent, DB_RequestSaveRecord, DB_ResponseSaveRecord);
		REG_COMP_MSG(SaveRecordComponent, DB_RequestSaveRecordByNice, DB_ResponseSaveRecord);
	}
};
//-------------------------------------------------------------------------
// 直接获取记录
class LoadRecordComponent : public NetCloud::Component
{
public:
	void On(DB_RequestLoadRecord &req, DB_ResponseRecord &resp, UnitID, int)
	{
		Hand<DBUserComponent> comp = GetActor()->GetDBUserComponent(req.mTableName);

		if (!comp)
		{
			resp.mError = eError_Resource_NoExist;
			return;
		}

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

	void On(DB_LoadMaxKey &req, DB_ResponseMaxkey &resp, UnitID, int)
	{
		Hand<DBUserComponent> comp = GetActor()->GetDBUserComponent(req.mTableName);
		if (!comp || !comp->LoadMaxKey(resp.mMaxKey))
		{
			resp.mError = eError_Resource_NoExist;
		}
	}

	void On(DB_RequestLoadRecordData &req, DB_ResponseRecordData &response, UnitID, int)
	{
		Hand<DBUserComponent> comp = GetActor()->GetDBUserComponent(req.mTableName);
		if (!comp)
		{
			response.mError = eError_TableNoExist;
			return;
		}

		ARecord re = comp->LoadRecord(req.mKey.c_str());
		if (!re)
		{
			response.mError = eError_RecordNoExist;
			return;
		}
		response.mData = MEM_NEW NiceData();
		for (int i=0; i<req.mFieldList.size(); ++i)
		{
			AString key = req.mFieldList[i];
			Data d = re->get(key.c_str());
			if (!d.empty())
				response.mData->set(key.c_str(), d);
		}
	}

public:
	virtual void RegisterMsg() override
	{
		REG_COMP_MSG(LoadRecordComponent, DB_RequestLoadRecord, DB_ResponseRecord);
		REG_COMP_MSG(LoadRecordComponent, DB_LoadMaxKey, DB_ResponseMaxkey);
		REG_COMP_MSG(LoadRecordComponent, DB_RequestLoadRecordData, DB_ResponseRecordData);
	}
};

