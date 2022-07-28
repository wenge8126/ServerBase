#pragma once
#ifndef _INCLUDE_SCACTOR_H_
#define _INCLUDE_SCACTOR_H_

#include "Actor.h"
#include "ServerClientMsg.h"
#include "CommonLib.h"
#include "CommonDefine.h"


//-------------------------------------------------------------------------
// ����ֱ����ͻ���ActorͨѶ����
class CommonLib_Export_H SCActor : public NetCloud::Actor
{
public:
	template<typename RespMsg>
	bool AwaitClient(Int64 clientID, UnitID clientActorID, tBaseMsg &requestMsg, RespMsg &responseMsg, int overMilSecond)
	{
		//�����Ż�, ����ʱ, ֱ�ӱ��浽��ת��Ϣ��, �ȴ��󲻿���ʹ��
		static thread_local SC_ActorRequestClientMsg msg;
		msg.mClientActorID = clientActorID;
		msg.mRequestMsgName = requestMsg.GetMsgName();
		if (!msg.mRequestMsgData)
			msg.mRequestMsgData = MEM_NEW DataBuffer();
		else
			msg.mRequestMsgData->clear(false);
		requestMsg.serialize(msg.mRequestMsgData.getPtr());

		// ��Ϊ�첽ִ��, ��������
		CS_ResponceServerActorMsg resp;

		if (Await(UnitID(Actor_LoginClient, clientID), msg, resp, overMilSecond) && resp.mResponseMsgData)
		{
			resp.mResponseMsgData->seek(0);
			responseMsg.restore(resp.mResponseMsgData.getPtr());

			return true;
		}
		else
			ERROR_LOG("AwaitClient fail");

		return false;
	}

	bool SendClientMsg(Int64 clientID, UnitID clientActorID, tBaseMsg &notifyMsg )
	{
		static thread_local SCS_NotifyMsg msg;

		msg.mActorID = clientActorID;
		msg.mMsgName = notifyMsg.GetMsgName();
		if (!msg.mNotifyMsgData)
			msg.mNotifyMsgData = MEM_NEW DataBuffer();
		else
			msg.mNotifyMsgData->clear(false);
		if (!notifyMsg.serialize(msg.mNotifyMsgData.getPtr()))
		{
			ERROR_LOG("Save notify msg fail : %s", notifyMsg.GetMsgName());
			return false;
		}
		return SendMsg(msg, UnitID(Actor_LoginClient, clientID));
	}

	void Notify(SCS_NotifyMsg &notifyMsg, UnitID sender, int nCompValue)
	{
		auto fun = mActorFactory->mOnNotifyMsgFunctionList.find(notifyMsg.mMsgName);
		if (fun == NULL)
			fun = GetMgr()->mOnNotifyMsgFunctionList.find(notifyMsg.mMsgName);
		if (fun != NULL && notifyMsg.mNotifyMsgData)
		{
			notifyMsg.mNotifyMsgData->seek(0);
			// �Ѿ���Э����
			(*fun)(this, (DataStream*)notifyMsg.mNotifyMsgData.getPtr(), sender, nCompValue);			
		}
		else
			ERROR_LOG("No register process request function : %s", notifyMsg.mMsgName.c_str());
	}

	void RegisterMsg(ActorManager *pActorMgr) override
	{
		REG_NOTIFY_MSG(SCActor, SCS_NotifyMsg);
	}
};

#endif // _INCLUDE_SCACTOR_H_