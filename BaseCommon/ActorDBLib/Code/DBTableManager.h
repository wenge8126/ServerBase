#pragma once

#ifndef _INCLUDE_DBTABLEMANAGER_H_
#define _INCLUDE_DBTABLEMANAGER_H_

#include "Auto.h"
#include "BaseTable.h"

#include "ShareSQLUpdate.h"
#include "FastHash.h"
#include "LogicDBTable.h"
//#include "NetNode.h"
#include "ActorDBLib.h"
#include "IOCPServerNet.h"
#include "CoroutineTool.h"

namespace NetCloud
{
	/*/-------------------------------------------------------------------------
	使用Actor 缓存用到的记录, 及进入逻辑统一
	Actor分布部署, 所以只需要一个落地库即可完成并发
	//-------------------------------------------------------------------------*/
	class ActorDBLib_Export DBTableManager : public AutoBase
	{
	public:
		// 返回激活检查时间(秒), >0 使用缓存表
		virtual int NeedCacheTable(const AString &tableName) { return 0; }

	public:
		AutoTable GetTable(const AString &tableName)
		{
			return mTableList.find(tableName);
		}
		FastHash<AString, AutoTable>& GetTableList() { return	mTableList; }

		void InitTableList(AutoTable listTable);

		AutoTable InitCreateTable(const AString &tableName, const AString &strTableInfoData, AString &resultInfo);

		AString LoadAllDBTable();


		AString InitLoadDB();

		/*/ 根据配置表格创建DB表
		INT,STRING,STRING,INT,BOOL,STRING,STRING,BOOL
		ID,NAME,TYPE,MAX_LENGTH,IS_INDEX,INFO,SLOT,IS_ORDER
		-1,,,,,,0~99,1
		0,ID,INT64,,,主键,,
		1,NAME,FIELD_CHAR_STRING,127,1,,,
		*/
		virtual bool CreateDBTable(const char *szTableName, AutoTable structConfigTable, AString &errorInfo);
		virtual bool CreateDBTable(const AString &tableName, AutoTable table, AutoNice  extParam);

		virtual AutoTable ModifyDBTable(const char *szTableName, AutoTable structConfigTable, AString &errorInfo);

		// 根据DB配置结构表, 生成DB表格
		virtual AutoTable GenerateDBTable(const char *szTableName, AutoTable structConfigTable, AutoNice &extParam, AString &errorInfo);

	public:
		DBTableManager();
		~DBTableManager()
		{
			Close();			
		}

	public:
		void Close()
		{
			mbReadyClose = true;
		}


		void Process();

		void LowProcess();

		virtual void OnShareSQLDBClose()
		{

		}

		// 异步初始连接SQL更新进程
		bool AsyncInitSQLUpdate(AutoNice sqlParam, const char *szShareDBServerIP, int nShareDBServerPort, int nShareNetSaftCode, int updateShareKey, bool bUseShareSQL);

		static AutoEvent AwaitRequestMemDB(AutoNet mShareSQLNet, const char *szRequestEventName, tNiceData *pParamData, int waitMilSecond);

	public:
		SendShareSQLUpdate					mSQLUpdate;
		bool												mbUseShareSQL = false;
		SQLDataArray								mTempDataArray;

	protected:
		AutoNet										mShareDBClientNet;
		
		AutoNice										mInitParam;
		FastHash<AString, AutoTable>		mTableList;
		AutoTable										mTableListDBTable;
		bool												mbReadyClose = false;
		
	};
	typedef Auto<DBTableManager>	AutoDBManager;
	//-------------------------------------------------------------------------
	class ShareDBClientNet : public IOCPClientNet
	{
	public:
		ShareDBClientNet(DBTableManager *pDB);

	public:
		bool AsyncConnect(const char *szIP, int port)
		{
			if (mConnectCoroID != 0)
			{
				ERROR_LOG("Wait connect ...");
				return false;
			}
			if (CORO == 0)
			{
				ERROR_LOG("AsyncConnect must in coro");
				return false;
			}
			mConnectCoroID = CORO;
			if (StartNet(szIP, port))
			{
				YIELD
					return true;
			}
			return false;
		}

		virtual void OnConnected() override {
			RESUME(mConnectCoroID);
			mConnectCoroID = 0;
		}

		bool IsConnected()
		{
			return GetClientConnect() && !GetClientConnect()->IsDisconnect();
		}

	public:
		virtual void OnCloseConnect(tNetConnect *pConnect) override
		{
			if (GetClientConnect().getPtr() == pConnect)
			{
				mpDBManager->OnShareSQLDBClose();
			}
		}

		virtual Logic::tEventCenter* GetEventCenter(void) const override { return ((ShareDBClientNet*)this)->mEventCenter.getPtr(); }

		virtual void Process() override
		{
			mEventCenter->ProcessEvent();
			IOCPClientNet::Process();
		}

		virtual int GetSafeCode() override { return mSafeCode; }

	public:
		DBTableManager			*mpDBManager = NULL;
		AutoEventCenter			mEventCenter;
		CoroID							mConnectCoroID = 0;
		int								mSafeCode = 0;
	};

	//-------------------------------------------------------------------------
}


#endif //_INCLUDE_DBTABLEMANAGER_H_