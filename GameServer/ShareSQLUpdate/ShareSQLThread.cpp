#include "ShareSQLThread.h"

#include "DBDefine.h"

#include <windows.h>
#include "CommonDefine.h"
#include "Vesion.h"
#include "BaseServer.h"
//#include "SendDuanXin.h"
#include "IOCPServerNet.h"

using namespace NetCloud;

DEFINE_RUN_CONFIG(ShareDBConfig)
//-------------------------------------------------------------------------
class DM_CloseServer : public NetCloud::tShareMemResponseEvent
{
public:
	virtual bool _DoEvent() override
	{
		GetResponseEvent()["RESULT"] = true;
		Finish();
		NOTE_LOG("Account do close server > %s", GetData().dump().c_str());
		((ShareSQLThread::CShareMemMgr*)GetDBMgr())->mpThread->mbNeedRestart = get("RESTART");
		((ShareSQLThread::CShareMemMgr*)GetDBMgr())->mpThread->mbRequestClose = true;
		((ShareSQLThread::CShareMemMgr*)GetDBMgr())->mpThread->RequestClose();		

		return true;
	}
};

class DM_GetRunInfo : public NetCloud::tShareMemResponseEvent
{
public:
	virtual bool _DoEvent() override
	{
		((ShareSQLThread::CShareMemMgr*)GetDBMgr())->mpThread->GetRunStateInfo(GetResponseEvent()->GetData());
		Finish();
		return true;
	}
};

//-------------------------------------------------------------------------
ShareSQLThread::ShareSQLThread() 
	: ServerThread("DBShareMemory")
{
	mShareMemDBManager = MEM_NEW CShareMemMgr(this, config.server_address.saft_code);
}

AString ShareSQLThread::GetTitle()
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
	title.Format("%s_%s [%s : %d] v[%s] %s > ", config.title.c_str()
		, runMode.c_str()
		, config.server_address.ip.c_str()
		, config.server_address.port
		, SERVER_VERSION_FLAG
		, szDir
	);
	return title;
}

void ShareSQLThread::SetTitle(const AString &title)
{
	if (!mShareMemDBManager)
	{
		SetConsoleTitle(title.c_str());
		return;
	}
	static UInt64 sLastSecondTime = TimeManager::Now();
	static UInt64 sLastUpdateCount = 0;
	static int sMaxUpdateOneSecond = 0;	

	static float sRate = 0;
	//if (TimeManager::Now() - sLastSecondTime>0)
		//sRate = (float)((mShareMemDBManager->mShareMemDBUpdate->mDebugCount - sLastUpdateCount) / (TimeManager::Now() - sLastSecondTime));	

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
	SetConsoleTitle(title.c_str());
}

void ShareSQLThread::OnStart(void*)
{

	mShareMemDBManager->StartNet(config.server_address.ip.c_str(), config.server_address.port);

	AString info; // = mShareMemDBManager->InitLoad(*(NiceData*)param.getPtr());
	REGISTER_EVENT(mShareMemDBManager->GetEventCenter(), DM_CloseServer);
	REGISTER_EVENT(mShareMemDBManager->GetEventCenter(), DM_GetRunInfo);

	NOTE_LOG("*** DB Init result > %s\r\n", info.length()>0 ? info.c_str() : "succeed");


	ServerThread::OnStart(NULL);
}

bool ShareSQLThread::NotifyThreadClose()
{
	//Hand<IOCPServerNet> net = mShareMemDBManager->mShareMemNet;
	//if (!mbRequestClose && !net->GetClientConnect()->IsDisconnect())
	//{
	//	NOTE_LOG("===> Logic DB connected, share memory now can not close!");
	//	return false;
	//}

	// 等待所有的落地操作完成
	//mShareMemDBManager->mShareMemDBUpdate->WaitAllUpdateOperateFinish();
	mShareMemDBManager->StopNet();


	///??? Refuse close  If no finish save sql data task
	//Hand< ShareMemMsgHander> dbShareSQL = mShareMemDBManager.mShareMemNet;
	//LoopData &threadData = dbShareSQL->GetSendBuffer();
	//if (dbShareSQL->GetReceiveBuffer().dataSize() > 0 || threadData.dataSize() > 0 || threadData.mWaitSendBuffer.dataSize() > 0)
	//{
	//	NOTE_LOG("严重警告:当前DBSQL存在需要处理的操作,不允许退出");
	//	return false;
	//}

	// 通知到DB即将关闭
	//DBShareSQLInitMsg *p = dynamic_cast<DBShareSQLInitMsg *>(mMainShareMemSQL->GetMsg(eShareMsg_ShareSQLNotifyState));
	//p->SetResult(eStateShareSQL_Close);
	//mMainShareMemSQL->SendSQLMsg(p);

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
	mShareMemDBManager->StopNet();
	mShareMemDBManager._free();
	TableTool::green();
	//PRINT("\r\nDB清理完成, 请按回车正常退出程序\r\n");
	TableTool::white();
	return BaseThread::NotifyThreadClose();
}

void ShareSQLThread::NotifyException()
{
	//DuanXin::Send("share_excep", 0, 1);

	BaseThread::NotifyException();
	mbException = false;

	// 由于在协程内dump, 无法跳回主线, 因此, 直接重启进程
	tBaseServer::RestartProgram(this, true);
}

void ShareSQLThread::DoCommand(const AString &commandString, StringArray &paramArray)
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

StateDataType ShareSQLThread::GetRunStateInfo(tNiceData &info)
{
	info["VER"] = SERVER_VERSION_FLAG;

	UInt64 alreadyUpdateCount = 0; // = mShareMemDBManager->mShareMemDBUpdate->mDebugCount;
	UInt64 updateCount = 0; // = mShareMemDBManager->mShareMemDBUpdate->mUpdateCount;

	AString strInfo;
	strInfo.Format("%s %llu / %llu, ShareSQL %llu / %u, ShareNet [%s : %d]"
		, alreadyUpdateCount == updateCount ? "Free" : "Update"
		, alreadyUpdateCount
		, updateCount
		, 0 //mShareMemDBManager->mShareMemDBUpdate->BufferSize()
		, 0 //mShareMemDBManager->mShareMemDBUpdate->BlockCount()
		, config.server_address.ip.c_str()
		, config.server_address.port
	);

	info["INFO"] = strInfo;

	return ServerThread::GetRunStateInfo(info);
}

