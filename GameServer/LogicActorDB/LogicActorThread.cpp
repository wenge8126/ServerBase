#include "LogicActorThread.h"

#include "DBDefine.h"

#include <windows.h>
#include "CommonDefine.h"
#include <process.h>

#include "Vesion.h"
#include "BaseServer.h"
#include "SendDuanXin.h"
#include "ClientEvent.h"
#include "MsgData.h"
#include "GenerateCodeTool.h"
#include "t_player.h"
#include "AccountActor.h"
#include "PlayerActor.h"

#include "GenerateNoSQLUser.h"
#include "GenerateDBUser.h"

#include "Test2Component.h"
#include "ServerClientMsg.h"

#define CREATE_DB		0

using namespace NetCloud;

DEFINE_RUN_CONFIG(LogicActorDBConfig)
//-------------------------------------------------------------------------

class WorkerActor : public DBActor
{
public:
	// 创建玩家
	void On(RQ_CreatePlayerActor &msg, RS_CreatePlayerActor &resp, UnitID senderID, int)
	{
		Hand<PlayerActor> player = GetMgr()->CreateActor(Actor_Player, msg.mPlayerID);
		player->Init();
		resp.mResult = eNoneError;
	}

	void On(RQ_CreateDBTable &msg, RS_CreateDBTable &resp, UnitID senderID, int)
	{
		LOG("Rev %s msg %s :\r\n%s", senderID.dump().c_str(), msg.GetMsgName(), msg.dump().c_str());		

		for (auto it = msg.mDBConfigData->begin(); it; ++it)
		{
			AString info;
			AutoTable configTable;
			it.get().get(&configTable, typeid(AutoTable));
			if (configTable)
			{
#if CREATE_DB
				if (GetDBMgr()->CreateDBTable(it.key().c_str(), configTable, info))
				{
					info.Format("Succeed create table %s", it.key().c_str());
					AutoTable t = GetDBMgr()->GetTable(it.key().c_str());
					GenerateDBUser::generate(it.key(), t, "../GameServer/Common/", false);
				}			
#else
				GenerateNoSQLUser::generate(it.key(), configTable, "../GameServer/WebLogin/Code/", false);
				
#endif
			}
			else
			{
				info.Format("Restore config table %s fail", it.key().c_str());
			}
			resp.mResultInfo += info;
			resp.mResultInfo += "\r\n";
		}
#if CREATE_DB
		auto &list = GetDBMgr()->GetTableList();

		GenerateCodeTool::generateDBManager(list, msg.mExportCodePath, "DB"); 
#endif
	}		

	// 直通客户端消息
	void On(CS_RequestTest  &msg, MSG_Test &resp, UnitID, int)
	{
		NOTE_LOG("Test msg : \r\n%s", msg.dump().c_str());
		resp.mTest = "ok : 888";

		CoroutineTool::AsyncCall([=]()
		{
			tTimer::AWaitTime(5000);
			
			GN_NotifyNodeInfo testReq;
			testReq.mNodeKey = 991111;

			MSG_Test respTestMsg;

			AwaitClient(UnitID(1, 111), testReq, respTestMsg, 16000);

			NOTE_LOG(" -------@@@@ %s", respTestMsg.dump().c_str());

		});
	}

	template<typename RespMsg>
	bool AwaitClient(UnitID clientActorID, tBaseMsg &requestMsg, RespMsg &responseMsg, int overMilSecond)
	{
		SC_ActorRequestClientMsg msg;
		msg.mClientActorID = clientActorID;
		msg.mRequestMsgName = requestMsg.GetMsgName();
		msg.mRequestMsgData = MEM_NEW DataBuffer();
		requestMsg.serialize(msg.mRequestMsgData.getPtr());

		CS_ResponceServerActorMsg resp;

		if (Await(UnitID(Actor_Client, 1), msg, resp, overMilSecond) && resp.mResponseMsgData)
		{
			resp.mResponseMsgData->seek(0);
			responseMsg.restore(resp.mResponseMsgData.getPtr());

			return true;
		}
		else
			ERROR_LOG("AwaitClient fail");

		return false;
	}


	virtual void RegisterMsg(ActorManager *pActorMgr) override
	{
		//pActorMgr->RegisterActorMsg("RQ_FirstRmbReward", &Actor::OnMsg<TestActor, RQ_FirstRmbReward, RS_FirstRmbReward>);
		REG_ACTOR_MSG(WorkerActor, RQ_CreateDBTable, RS_CreateDBTable);
		REG_ACTOR_MSG(WorkerActor, RQ_CreatePlayerActor, RS_CreatePlayerActor);

		REG_ACTOR_MSG(WorkerActor, CS_RequestTest, MSG_Test);
	}

public:
	LogicActorThread* GetThread()
	{
		Auto<LogicActorThread::CDBMgr> mgr = GetMgr();
		return mgr->mpThread;
	}
};



//-------------------------------------------------------------------------
LogicActorThread::LogicActorThread() 
	: ServerThread("LogicActor")
{
	
}

AString LogicActorThread::GetTitle()
{

	AString runMode;
#if _DEBUG
	runMode = "开发";
#else
	runMode = "运行";
#endif

	char szDir[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH - 1, szDir);
	AString title;
	title.Format("%s_%s [127.0.0.1 : %d (%d)] v[%s] %s > ", config.title.c_str()
		, runMode.c_str()
		, config.share_server_port
		, config.share_server_code
		, SERVER_VERSION_FLAG
		, szDir
	);
	return title;
}

void LogicActorThread::SetTitle(const AString &title)
{
	//if (!mShareMemDBManager)
	{
		SetConsoleTitle(title.c_str());
		return;
	}
	//static UInt64 sLastSecondTime = TimeManager::Now();
	//static UInt64 sLastUpdateCount = 0;
	//static int sMaxUpdateOneSecond = 0;	

	//static float sRate = 0;
	//if (TimeManager::Now() - sLastSecondTime>0)
	//	sRate = (float)((mShareMemDBManager->mShareMemDBUpdate->mDebugCount - sLastUpdateCount) / (TimeManager::Now() - sLastSecondTime));	

	//if (TimeManager::Now() - sLastSecondTime >= 10)
	//{
	//	sLastSecondTime = TimeManager::Now();
	//	int count = (int)(mShareMemDBManager->mShareMemDBUpdate->mDebugCount - sLastUpdateCount);
	//	if (count > sMaxUpdateOneSecond)
	//		sMaxUpdateOneSecond = count;
	//	sLastUpdateCount = mShareMemDBManager->mShareMemDBUpdate->mDebugCount;
	//}

	//AString info;
	//info.Format("%s, %s %llu-%llu=%llu, max %.1f, R %.1f, Share %llu / %u"
	//	, title.c_str()
	//	, mShareMemDBManager->mShareMemDBUpdate->mbEmpty ? "Free" : "Update"
	//	, mShareMemDBManager->mShareMemDBUpdate->mUpdateCount
	//	, mShareMemDBManager->mShareMemDBUpdate->mDebugCount
	//	, mShareMemDBManager->mShareMemDBUpdate->mUpdateCount - mShareMemDBManager->mShareMemDBUpdate->mDebugCount
	//	, sMaxUpdateOneSecond * 0.1f
	//	, sRate
	//	, mShareMemDBManager->mShareMemDBUpdate->BufferSize()
	//	, mShareMemDBManager->mShareMemDBUpdate->BlockCount()
	//);

	//SetConsoleTitle(info.c_str());
}

void AsyncReadyDB(LogicActorThread *pThread)
{
	if (!pThread->mActorManager->mNetNode->AwaitConnectGate(config.data_db_node.gate.ip.c_str(), config.data_db_node.gate.port, 60000))
	{
		ERROR_LOG("Connect gate fail");
		return;
	}

	AutoNice param = MEM_NEW NiceData();
	param[DBBASE] = config.sql_db.mDBBASE;
	param[DBIP] = config.sql_db.mDBIP;
	param[DBPASSWORD] = config.sql_db.mDBPASSWORD;
	param[DBPORT] = config.sql_db.mDBPORT;
	param[DBUSER] = config.sql_db.mDBUSER;
	param[TABLE_LIST] = config.sql_db.mTABLE_LIST;
	param[DBPACKET_SIZE] = 16 * 1024 * 1024;
	
	// Update共享缓存key使用 Actor_进行ID_线程ID 组成的字符串, 生成哈希的整数
	AString keyString;
	int proID = _getpid();
	keyString.Format("Actor_%d_%llu", proID, pThread->GetThreadID());

	bool b = pThread->mActorManager->mShareDBManager->AsyncInitSQLUpdate(param, "127.0.0.1", config.share_server_port, config.share_server_code, MAKE_INDEX_ID(keyString.c_str()));

	NOTE_LOG("*** DB Init result > %s\r\n", b ? "succeed" : "fail");

	if (b)
	{
		//AutoTable t = pThread->mActorManager->mShareDBManager->GetTable("t_player");
		//ARecord re = t->GrowthNewRecord();
		//if (re)
		//	LOG("%s", re->dump().c_str());

		//t_player x = re;
		//LOG("====== %s", x.ID().c_str());
	}
}

void LogicActorThread::OnStart(void*)
{
	mActorManager = MEM_NEW CDBMgr(this, config.data_db_node.node.ip.c_str(), config.data_db_node.node.port, config.data_db_node.node.saft_code, 2);

	// 账号对象
	mActorManager->RegisterActor(Actor_Account, MEM_NEW DefineActorFactory< AccountActor>());
	mActorManager->RegisterComponect("DDCom", MEM_NEW EventFactory<DDCom>());
	mAccountActor = mActorManager->CreateActor(Actor_Account, 1);

	// 1区游戏对象
	mActorManager->RegisterActor(Actor_GameServer, MEM_NEW DefineActorFactory< GameServerActor>());
	mActorManager->CreateActor(Actor_GameServer, 1);

	// 工作线程对象
	mActorManager->RegisterActor(Actor_DBWorker, MEM_NEW DefineActorFactory<WorkerActor>());
	mWorkerActor = mActorManager->CreateActor(Actor_DBWorker, 1);

	// 玩家对象
	mActorManager->RegisterActor(Actor_Player, MEM_NEW DefineActorFactory<PlayerActor>());

	CoroutineTool::AsyncCall(AsyncReadyDB, this);

	ServerThread::OnStart(NULL);
}



bool LogicActorThread::NotifyThreadClose()
{

	// 等待所有的落地操作完成
	mActorManager->Close();



	const int waitSecondTime = 3;  // 10
	int nowPassSecond = 0;
	UInt64 now = TimeManager::NowTick();
	while (true)
	{
		Process(&now);
		TimeManager::Sleep(10);
		UInt64 nowPassTime = TimeManager::NowTick() - now;
		if ( nowPassTime> waitSecondTime * 1000)
			break;
		else
		{
			int x = (int)(nowPassTime * 0.001f);
			if (x != nowPassSecond)
			{
				NOTE_LOG("Wait close last time %d second", waitSecondTime - nowPassSecond);
				nowPassSecond = x;
			}
		}
	}
	mActorManager->Close();
	mActorManager._free();
	TableTool::green();
	//PRINT("\r\nDB清理完成, 请按回车正常退出程序\r\n");
	TableTool::white();
	return BaseThread::NotifyThreadClose();
}

void LogicActorThread::NotifyException()
{
	DuanXin::Send("share_excep", 0, 1);

	BaseThread::NotifyException();
	mbException = false;

	// 由于在协程内dump, 无法跳回主线, 因此, 直接重启进程
	tBaseServer::RestartProgram(this, true);
}

void LogicActorThread::DoCommand(const AString &commandString, StringArray &paramArray)
{
	if (commandString == "s")
	{
		AutoTable t = tBaseTable::NewBaseTable();
		t->AppendField("ID", FIELD_INT);
		t->AppendField("NAME", FIELD_CHAR_STRING);
		t->AppendField("AGE", FIELD_INT64);
		AString info;
		//mShareMemDBManager->CreateDBTable("t_test", t->GetField(), AutoNice(), "", info);
		NOTE_LOG( "------------------\r\n%s\r\n-------------------", info.c_str());
	}
	//else if (commandString == "r")
	//{
	//	Hand< DBShareMemSQL> memSql = mMainShareMemSQL;
	//	memSql->mbDebugStopReceive = false;
	//}
}

StateDataType LogicActorThread::GetRunStateInfo(tNiceData &info)
{
	info["VER"] = SERVER_VERSION_FLAG;

	//UInt64 alreadyUpdateCount = mShareDBTableManager->mShareMemDBUpdate->mDebugCount;
	//UInt64 updateCount = mShareDBTableManager->mShareMemDBUpdate->mUpdateCount;

	//AString strInfo;
	//strInfo.Format("%s %llu / %llu, ShareSQL %llu / %u, ShareNet [key %d, size %d]"
	//	, alreadyUpdateCount == updateCount ? "Free" : "Update"
	//	, alreadyUpdateCount
	//	, updateCount
	//	, mShareDBTableManager->mShareMemDBUpdate->BufferSize()
	//	, mShareDBTableManager->mShareMemDBUpdate->BlockCount()
	//	, config.share_key.key
	//	, config.share_key.size
	//);

	//info["INFO"] = strInfo;

	return ServerThread::GetRunStateInfo(info);
}



