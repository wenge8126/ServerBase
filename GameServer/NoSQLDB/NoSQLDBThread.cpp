#include "NoSQLDBThread.h"

#include "DBDefine.h"

#include <windows.h>
#include "CommonDefine.h"
#include <process.h>

#include "Vesion.h"
#include "BaseServer.h"
//#include "SendDuanXin.h"

#include "NoSQLActor.h"
#include "MsgData.h"

#include "GenerateNoSQLUser.h"

using namespace NetCloud;

DEFINE_RUN_CONFIG(LogicActorDBConfig)
//-------------------------------------------------------------------------

class WorkerActor : public Actor
{
public:
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

	virtual void RegisterMsg() override
	{
		//pActorMgr->RegisterActorMsg("RQ_FirstRmbReward", &Actor::OnMsg<TestActor, RQ_FirstRmbReward, RS_FirstRmbReward>);
		REG_ACTOR_MSG(WorkerActor, RQ_CreateDBTable, RS_CreateDBTable);
	}

public:
	NoSQLDBThread* GetThread()
	{
		Auto<NoSQLDBThread::CDBMgr> mgr = GetMgr();
		return mgr->mpThread;
	}
};



//-------------------------------------------------------------------------
NoSQLDBThread::NoSQLDBThread() 
	: ServerThread("NoSQLDB")
{
	
}

AString NoSQLDBThread::GetTitle()
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

void NoSQLDBThread::SetTitle(const AString &title)
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

void AsyncReadyDB(NoSQLDBThread *pThread)
{
	if (!pThread->mActorManager->mNetNode->AwaitConnectGate("127.0.0.1", 10000, 60000))
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

	bool b = pThread->mActorManager->mShareDBManager->AsyncInitSQLUpdate(param, "127.0.0.1", config.share_server_port, config.share_server_code, MAKE_INDEX_ID(keyString.c_str()), false);

	NOTE_LOG("*** DB Init result > %s\r\n", b ? "succeed" : "fail");

	if (b)
	{
		pThread->mNoSQLDBActor = pThread->mActorManager->CreateActor(NOSQL_DB_TYPE, 0);


		//AutoTable t = pThread->mActorManager->mShareDBManager->GetTable("t_player");
		//ARecord re = t->GrowthNewRecord();
		//if (re)
		//	LOG("%s", re->dump().c_str());

		//t_player x = re;
		//LOG("====== %s", x.ID().c_str());
	}
}

void NoSQLDBThread::OnStart(void*)
{
	mActorManager = MEM_NEW CDBMgr(this, "127.0.0.1", 10001, 10, 2);

	// 账号对象
	mActorManager->RegisterActor(NOSQL_DB_TYPE, MEM_NEW DefineActorFactory< NoSQLActor>());
	//mActorManager->RegisterComponect("DDCom", MEM_NEW EventFactory<DDCom>());

	// 工作线程对象
	mActorManager->RegisterActor(Actor_DBWorker, MEM_NEW DefineActorFactory<WorkerActor>());
	mWorkerActor = mActorManager->CreateActor(Actor_DBWorker, 1);


	CoroutineTool::AsyncCall(AsyncReadyDB, this);

	ServerThread::OnStart(NULL);
}



bool NoSQLDBThread::NotifyThreadClose()
{
	mWorkerActor._free();
	mNoSQLDBActor._free();
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

void NoSQLDBThread::NotifyException()
{
	//DuanXin::Send("share_excep", 0, 1);

	BaseThread::NotifyException();
	mbException = false;

	// 由于在协程内dump, 无法跳回主线, 因此, 直接重启进程
	tBaseServer::RestartProgram(this, true);
}

void NoSQLDBThread::DoCommand(const AString &commandString, StringArray &paramArray)
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

StateDataType NoSQLDBThread::GetRunStateInfo(tNiceData &info)
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



