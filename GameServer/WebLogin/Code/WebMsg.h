#pragma once
#ifndef _INCLDUE_WEBMSG_H_
#define _INCLDUE_WEBMSG_H_

#include "ClientEvent.h"
#include "Task.h"

namespace NetCloud
{
	class RequestDBOperatePak;
}
class BaseWebServer;

class WebMsg
{
public:
	static void Register(BaseWebServer *pNet);


};

class CL_LoginEvent : public Logic::tClientEvent
{
public:
	static int msTotalNowLogin;

protected:
	AString mAccount;

public:
	virtual bool _DoEvent() override;

	void TestAccountLogin();

	void CheckLoginResult(AutoNice resultData, int  nResult);

	virtual void InitData() override
	{
		Logic::tClientEvent::InitData();
		mAccount.setNull();
	}

	virtual void _OnFinish() override
	{
		--msTotalNowLogin;
	}
};

class CL_DBOpreate : public Logic::tClientEvent
{
public:
	static EasyHash<AString, bool> mAllowOperateList;

	static void InitAllowList();

public:
	virtual void AsyncDo() override
	{
		DoEvent(true);
	}

	virtual bool _DoEvent() override;

	void OnLoad(NetCloud::RequestDBOperatePak *pDB, AutoNice resultData, int  nResult);

	void OnZipFinish(AutoData scrData, AutoData zipData, bool bSucceed)
	{
		if (bSucceed)
		{
			GetResponseEvent()->GetData()["DATA"] = zipData.getPtr();
			GetResponseEvent()->GetData()["_ZIP_"] = true;
			//NOTE_LOG("%s zip response msg %d,  zip %d, rate %.2f", GetEventName(), scrData->dataSize(), zipData->dataSize(), zipData->dataSize() * 100.0f/ scrData->dataSize());
		}
		else
		{
			LOG("%s zip msg data fail %d, Now use scr data", GetEventName(), scrData->dataSize());
		}
		Finish();
	}
};

class CL_EventMsgEvent : public Logic::tBaseNetEvent
{
public:
	virtual bool _DoEvent() override;

};

class CL_RequestResourceEvent : public Logic::tClientEvent
{
	virtual bool _DoEvent() override;

	void OnLoadResource(NetCloud::RequestDBOperatePak *pDB, AutoNice resultData, int  nResult);

	//virtual void SendResponce(AutoEvent hRespEvent) override;

public:
	// 缓存资源数据, 根据MD5确定是否需要重新从DB调取
	struct CacheData 
	{
		AString mMD5;
		AutoData mData;
	};
	static EasyHash<AString, CacheData>		msCacheList;

};

class CL_CodeActiveEvent : public Logic::tClientEvent
{

public:
	virtual bool _DoEvent() override;

protected:
	
};

//-------------------------------------------------------------------------
// 后台压缩任务, 分担主线时间
class BackZipMsgTask : public Task
{
public:
	Auto<DataBuffer>		mMsgData;
	AutoData						mZipData;
	bool								mbSucceed = false;
	Hand< CL_DBOpreate>					mCallEvent;

public:
	BackZipMsgTask()
	{
		mZipData = MEM_NEW DataBuffer();
	}

	virtual void InitFree() override
	{
		mMsgData.setNull();
		mZipData->clear(false);
		mbSucceed = false;
	}

	virtual void Execute(TaskManager *pTaskMgr) override
	{
		if (mMsgData->ZipData(mZipData, 0, 0, 0, MAX_SPEED) > 0)
			mbSucceed = true;
		else
		{
			mbSucceed = false;
			ERROR_LOG("Zip msg data fail : %d", mMsgData->dataSize());
		}
	}

	virtual void OnFinished(TaskManager *pTaskMgr) 
	{ 
		if (mCallEvent)
			mCallEvent->OnZipFinish(mMsgData, mZipData, mbSucceed);

		InitFree(); 
	}
};

//-------------------------------------------------------------------------

#endif //_INCLDUE_WEBMSG_H_