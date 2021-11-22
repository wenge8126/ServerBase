#include "SQLUpdateManager.h"
#include "IOCPServerNet.h"
#include "ClientEvent.h"

namespace NetCloud
{


	//-------------------------------------------------------------------------
	class DM_ReqeustReadySQLThread : public tShareMemResponseEvent
	{
	public:
		virtual bool _DoEvent() override
		{
			int sqlKey = get(SHARE_SQL_KEY);

			if (sqlKey == 0)
			{
				GetResponseEvent()["ERROR"] = "SHARE_SQL_KEY param no set";
				ERROR_LOG("* SHARE_SQL_KEY param no set, now can not init agin %d", sqlKey);
				return true;
			}

			AutoNice	sqlParam = (tNiceData*)get("SQL");
			if (!sqlParam)
			{
				GetResponseEvent()["ERROR"] = "SQL param no set";
				ERROR_LOG("* DB SQL : SQL param no set, now can not init agin %d", sqlKey);
				return true;
			}

			Hand< LogicConnect> conn = mNetConnect;

			if (!conn->ReadySQLThread(sqlKey, sqlParam))
			{
				GetResponseEvent()["ERROR"] = "SQL update already init or init fail";
				ERROR_LOG("* DB SQL : Already init %s, now can not init agin %d", SHARE_SQL_KEY, sqlKey);
				Finish();
				return true;
			}		

			NOTE_LOG("* DB SQL : %s = %d", SHARE_SQL_KEY, sqlKey);

			GetResponseEvent()["RESULT"] = true;

			Finish();
			return true;
		}
	};

	//-------------------------------------------------------------------------

	SQLUpdateManager::SQLUpdateManager()
	{
		mEventCenter = MEM_NEW Logic::EventCenter();

		REGISTER_EVENT(mEventCenter, DM_ReqeustReadySQLThread);
	}

	HandConnect SQLUpdateManager::CreateConnect()
	{
		return MEM_NEW LogicConnect();
	}

	NetCloud::SQLUpdateManager* tShareMemResponseEvent::GetDBMgr()
	{
		if (!mNetConnect)
		{
			return NULL;
		}
		Hand<SQLUpdateManager> net = mNetConnect->GetNetHandle();
		return net.getPtr();
	}

}

