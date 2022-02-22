#pragma once
#ifndef _INCLUDE_CLOUDDBTHREAD_H_
#define _INCLUDE_CLOUDDBTHREAD_H_

#include "BaseThread.h"
//#include "Gate.h"
#include "CRunConfig.h"
#include "Vesion.h"
#include "AsyncGate.h"

class GateThread : public ServerThread
{
public:
	GateThread()
		: ServerThread("CloudGate")
		, mbNeedRestart(false)
	{

	}

	virtual AString GetTitle() override;
	virtual void SetTitle(const AString &title);

public:
	// Ã¿Ãë1Íò´ÎLoop
	virtual int OnceTime(void) const { return 10; }
	virtual int OnceLowTime() const { return 100000; }

	virtual void OnStart(void*);
	virtual void OnStop(void*) {}

	virtual bool NotifyThreadClose() override
	{
		mGate->CloseGate();
		return ServerThread::NotifyThreadClose();
	}

	virtual void NotifyException() override;

	virtual void Process(void*) override
	{
		mGate->Process();
	}

	virtual void LowProcess(int spaceTime) override
	{
		ServerThread::LowProcess(spaceTime);
		mGate->LowProcess(spaceTime);
	}

	virtual void DoCommand(const AString &commandString, StringArray &paramArray) override;

	virtual StateDataType GetRunStateInfo(tNiceData &info) override;

public:
	Hand<AsyncGate>		mGate;
	bool							mbNeedRestart;
};


#endif //_INCLUDE_CLOUDDBTHREAD_H_