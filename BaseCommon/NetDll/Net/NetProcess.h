
#ifndef _INCLUDE_NETPROCESS_H_
#define _INCLUDE_NETPROCESS_H_

#pragma once

#include "NetHead.h"
#include "BaseMsg.h"
#include "CoroutineTool.h"
#include "AnyData.h"

//-------------------------------------------------------------------------
class tNetProcess;
class NetProcessPtr : public AutoBase
{
public:
	tNetProcess		*mpProcess = NULL;
};

typedef Auto<NetProcessPtr> AProcess;

class Net_Export tNetProcess : public AnyData
{
public:
	tNetProcess()
	{
		mAutoThis = MEM_NEW NetProcessPtr();
		mAutoThis->mpProcess = this;
	}

	~tNetProcess()
	{
		mAutoThis->mpProcess = NULL;
	}

public:
	static AutoNice Await(tNetConnect *pConnect, int msgID, tRequestMsg &req, int overMilSecond);

	template<typename RespMsg>
	static bool Await(tNetConnect *pConnect, int msgID, tRequestMsg &req, RespMsg &respMsg, int overMilSecond)
	{
		AutoNice respData = Await(pConnect, msgID, req, overMilSecond);
		if (!respData)
			return false;
		respMsg.Full(respData);
		return true;
	}

	virtual bool ProcessPacket(tNetConnect* pConnect, Packet *pPacket) { return false; }


public:
	AProcess		mAutoThis;
};

typedef Hand<tNetProcess> HandProcess;

//-------------------------------------------------------------------------

#endif //_INCLUDE_NETPROCESS_H_