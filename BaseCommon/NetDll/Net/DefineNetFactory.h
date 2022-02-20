#ifndef _INCLUDE_DEFINENETFACTORY_H_
#define _INCLUDE_DEFINENETFACTORY_H_

#pragma once

#include "DefinePacketFactory.h"
#include "AsyncProtocol.h"
#include "NetProcess.h"

//-------------------------------------------------------------------------
// 注册一般通知消息
template<int packetID, typename T, typename ProcessType>
class DefaultNetFactory : public tPacketFactory
{
public:
	DefaultNetFactory(tNetProcess *pProcess)
		: tPacketFactory(0)
	{
		mProcess = pProcess->mAutoThis;
	}

public:
	virtual Packet *_createPacket() { return MEM_NEW T(); }
	virtual PacketID_t	GetPacketID()const { return packetID; }

	virtual void ProcessPacket(tNetConnect *pConnect, Packet *pPacket) override
	{
		Auto<T> pak = pPacket;
		if (!pak)
		{
			ERROR_LOG("Is not %s packet", typeid(T).name());
			return;
		}
		Hand<ProcessType> process = mProcess->mpProcess;
		if (process)
			process->On(pConnect, *pak);
		else
			ERROR_LOG("%s run process is null", typeid(T).name());
	}

public:
	AProcess		mProcess;
};
//-------------------------------------------------------------------------
// 注册请求消息
template<int packetID, typename ReqMsg, typename RespMsg, typename ProcessType>
class DefaultNetReqeustFactory : public tPacketFactory
{
public:
	DefaultNetReqeustFactory(tNetProcess *pProcess)
		: tPacketFactory(0)
	{
		mProcess = pProcess->mAutoThis;
	}

public:
	virtual Packet *_createPacket() { return MEM_NEW ReqMsg(); }
	virtual PacketID_t	GetPacketID()const { return packetID; }

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
				pConnect->Send(&responseMsg, false);
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

#endif //_INCLUDE_DEFINENETFACTORY_H_
