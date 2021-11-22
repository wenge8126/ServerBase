#pragma once

#ifndef _INCLUDE_SQLUPDATEMANAGER_H_
#define _INCLUDE_SQLUPDATEMANAGER_H_

#include "ShareSQLUpdate.h"
#include "NetHandle.h"
#include "ClientEvent.h"
#include "IOCPServerNet.h"
#include "IOCPConnect.h"

namespace NetCloud
{

	class ShareSQLUpdateLib_Export SQLUpdateManager : public IOCPServerNet
	{
	public:
		SQLUpdateManager();

		~SQLUpdateManager()
		{
			StopNet();
			mEventCenter._free();
		}

	public:
		virtual HandConnect CreateConnect() override;

		virtual Logic::tEventCenter* GetEventCenter() const override
		{
			return ((SQLUpdateManager*)this)->mEventCenter.getPtr();
		}

		virtual void Process() override
		{
			mEventCenter->ProcessEvent();
			IOCPServerNet::Process();
		}

	public:
		AutoEventCenter					mEventCenter;
	};

	//-------------------------------------------------------------------------
	// Logic使用连接
	class LogicConnect : public IOCPServerConnect
	{
	public:
		LogicConnect() {}
		~LogicConnect()
		{
			mSqlUpdate.WaitFinishClose();
		}

	public:
		bool ReadySQLThread(int sqlKey, AutoNice sqlParam)
		{
			return mSqlUpdate.Init(sqlKey, *sqlParam);
		}

	public:
		virtual void OnDisconnect() override
		{
			// 断开关闭落地线程
			mSqlUpdate.WaitFinishClose();
		}

	public:
		ShareSQLUpdate			mSqlUpdate;
	};


	//-------------------------------------------------------------------------
	class ShareSQLUpdateLib_Export tShareMemResponseEvent : public Logic::tClientEvent
	{
	public:
		SQLUpdateManager*  GetDBMgr();
	};
}

#endif //_INCLUDE_SQLUPDATEMANAGER_H_
