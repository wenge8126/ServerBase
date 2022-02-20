
#ifndef _INCLUDE_NETPROCESS_H_
#define _INCLUDE_NETPROCESS_H_

#pragma once

#include "NetHead.h"
#include "BaseMsg.h"
#include "CoroutineTool.h"

//-------------------------------------------------------------------------
class tNetProcess;
class NetProcessPtr : public AutoBase
{
public:
	tNetProcess		*mpProcess = NULL;
};

typedef Auto<NetProcessPtr> AProcess;

class Net_Export tNetProcess : public Base<tNetProcess>
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

public:
	AProcess		mAutoThis;
};

//-------------------------------------------------------------------------

#endif //_INCLUDE_NETPROCESS_H_