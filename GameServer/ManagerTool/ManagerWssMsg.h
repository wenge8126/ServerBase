#pragma once

#include "NodeMsgEvent.h"

extern CManagerToolDlg  *gpManagerToolDlg;

enum ManagerWssMsgType
{
	//WEB_REQUST_DB = 6,						// DBÆÀÏûÏ¢
	eMsgTypeGetRunState = 100,

};


class TM_RequestRunState : public Logic::tClientEvent
{
public:
	virtual void _AsyncDo() override
	{
		gpManagerToolDlg->AsyncTotalRunState(GetSelf());
		//GetResponseEvent()["INFO"] = "OK";
	}

	virtual void SendResponce(AutoEvent hRespEvent) override
	{
		static Auto<DataBuffer> d = MEM_NEW DataBuffer();
		static AutoData zipData = MEM_NEW DataBuffer();
		d->clear(false);
		zipData->clear(false);

		hRespEvent->GetData().serialize(d.getPtr());
		d->ZipData(zipData);

		hRespEvent->GetData().clear(false);
		hRespEvent["DATA"] = zipData.getPtr();

		Logic::tClientEvent::SendResponce(hRespEvent);
	}
};

class TM_PingEvent : public Logic::tClientEvent
{
public:
	virtual bool _DoEvent() override
	{
		Finish();
		return true;
	}
};

class ManagerWssMsg
{
public:
	static void Register(BaseWebServer *pNet)
	{
		pNet->RegisterMsg(eMsgTypeGetRunState, MEM_NEW Logic::EventFactory<TM_RequestRunState>());
	}
};
