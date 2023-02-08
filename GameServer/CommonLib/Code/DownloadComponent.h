#pragma once

//-------------------------------------------------------------------------
// 大数据下载服务组件, 主要提供分段数据
//-------------------------------------------------------------------------
class DownloadComponent : public Component
{
public:
	void On(DB_RequestDownloadData &req, DB_ResponseDataInfo &resp, UnitID, int)
	{
		Hand<DBUserComponent> comp = GetActor()->GetDBUserComponent(req.mTableName);
		if (!comp || !comp->GetDataInfo(req.mKey.c_str(), req.mCheckMD5, resp.mSize, resp.mMD5))
		{
			resp.mMD5.setNull();
			resp.mError = eError_Resource_NoExist;
		}
	}

	void On(DB_RequestDownloadPartData &req, DB_ResponsePartData &resp, UnitID, int)
	{
		Hand<DBUserComponent> comp = GetActor()->GetDBUserComponent(req.mTableName);

		if (!comp)
		{
			resp.mError = eError_Resource_NoExist;
			return;
		}

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
