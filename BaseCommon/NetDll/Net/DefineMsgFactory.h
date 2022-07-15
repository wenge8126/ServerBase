#ifndef _INCLUDE_DEFINEMSGFACTORY_H_
#define _INCLUDE_DEFINEMSGFACTORY_H_

#pragma once

#include "DefinePacketFactory.h"
#include "AsyncProtocol.h"
#include "NetProcess.h"

//-------------------------------------------------------------------------
// ע��һ��֪ͨ��Ϣ
template<int packetID, typename Msg, typename ProcessType>
class DefaultMsgFactory : public tPacketFactory
{
public:
	DefaultMsgFactory(tNetProcess *pProcess)
		: tPacketFactory(0)
	{
		mProcess = pProcess->mAutoThis;
	}

public:
	virtual Packet *_createPacket() { return MEM_NEW Msg(); }
	virtual PacketID_t	GetPacketID()const { return packetID; }

	virtual const char* GetPacketName() override
	{
		return typeid(Msg).name();
	}

	virtual void ProcessPacket(tNetConnect *pConnect, Packet *pPacket) override
	{
		Auto<Msg> pak = pPacket;
		if (!pak)
		{
			ERROR_LOG("Is not %s packet", typeid(Msg).name());
			return;
		}
		Hand<ProcessType> process = mProcess->mpProcess;
		if (process)
			process->On(pConnect, *pak);
		else
			ERROR_LOG("%s run process is null", typeid(Msg).name());
	}

public:
	AProcess		mProcess;
};
//-------------------------------------------------------------------------
// ע��������Ϣ
template<int packetID, typename ReqMsg, typename RespMsg, typename ProcessType>
class DefaultReqeustMsgFactory : public tPacketFactory
{
public:
	DefaultReqeustMsgFactory(tNetProcess *pProcess)
		: tPacketFactory(0)
	{
		mProcess = pProcess->mAutoThis;
	}

public:
	virtual Packet *_createPacket() { return MEM_NEW ReqMsg(); }
	virtual PacketID_t	GetPacketID()const { return packetID; }

	virtual const char* GetPacketName() override
	{
		return typeid(ReqMsg).name();
	}

	virtual void ProcessPacket(tNetConnect *pConnect, Packet *pPacket) override
	{
		Auto<ReqMsg> pak = pPacket;
		if (!pak)
		{
			ERROR_LOG("Is not %s packet", typeid(ReqMsg).name());
			return;
		}
		Hand<ProcessType> process = mProcess->mpProcess;
		if (process)
		{
			RespMsg  resp;
			process->On(pConnect, *pak, resp);
			ResponseMsgPacket  responseMsg;
			responseMsg.mRequestID = pak->mRequestID;
			responseMsg.mData.clear(false);
			if (resp.Write(responseMsg.mData) == TRUE)
				pConnect->Send(responseMsg.GetPacketID(), &responseMsg);
			else
				ERROR_LOG("Write packet %s fail", typeid(RespMsg).name());
		}
		else
			ERROR_LOG("%s run process is null", typeid(ReqMsg).name());
	}

public:
	AProcess		mProcess;
};
//-------------------------------------------------------------------------
// ע���첽�����������Ϣ
// ִ��NetProcess : OnAsyncRequest
template<int packetID, typename ReqMsg, typename RespMsg, typename ProcessType>
class AsyncProcessReqeustMsgFactory : public tPacketFactory
{
public:
	AsyncProcessReqeustMsgFactory(tNetProcess *pProcess)
		: tPacketFactory(0)
	{
		mProcess = pProcess->mAutoThis;
	}

public:
	virtual Packet *_createPacket() { return MEM_NEW ReqMsg(); }
	virtual PacketID_t	GetPacketID()const { return packetID; }

	virtual const char* GetPacketName() override
	{
		return typeid(ReqMsg).name();
	}

	virtual void ProcessPacket(tNetConnect *pConnect, Packet *pPacket) override
	{
		Auto<ReqMsg> pak = pPacket;
		HandConnect conn = pConnect;

		CoroutineTool::AsyncCall([=]()
		{
			if (!pak)
			{
				ERROR_LOG("Is not %s packet", typeid(ReqMsg).name());
				return;
			}
			Hand<ProcessType> process = mProcess->mpProcess;
			if (process)
			{
				RespMsg  resp;
				process->OnAsyncRequest(conn, *pak, resp);
				ResponseMsgPacket  responseMsg;
				responseMsg.mRequestID = pak->mRequestID;
				responseMsg.mData.clear(false);
				if (conn && resp.Write(responseMsg.mData) == TRUE)
					((tNetConnect*)conn.getPtr())->Send(responseMsg.GetPacketID(), &responseMsg);
				else
					ERROR_LOG("Write packet %s fail", typeid(RespMsg).name());
			}
			else
				ERROR_LOG("%s run process is null", typeid(ReqMsg).name());
		});
	}

public:
	AProcess		mProcess;
};
//-------------------------------------------------------------------------

#endif //_INCLUDE_DEFINEMSGFACTORY_H_
