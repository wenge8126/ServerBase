#include "GameThread.h"


#include "CommonDefine.h"

#include "TaskManager.h"
//#include "NodeMsgEvent.h"
#include "BaseMsg.h"


#include "ConfigEnum.h"
#include "Vesion.h"

#include <io.h>
#include "Dump.h"
#include "BaseServer.h"
#include "SendDuanXin.h"
#include "SystemInfo.h"

#include <windows.h>


#include "AsyncLoop.h"

#include "PlayerActor.h"
#include "GameServerActor.h"

using namespace uWS;

using namespace NetCloud;

using namespace std;

DEFINE_RUN_CONFIG(GameServerConfig)





//-------------------------------------------------------------------------

GameThread::GameThread()
	: ServerThread(GetAppName())
{
	mServerListJson = "{}";
	//mServerList = MEM_NEW r_server_list();

	mTaskSystem = MEM_NEW TaskSystem(32);
}

//-------------------------------------------------------------------------

AString GameThread::GetTitle()
{
	AString runMode;
#if _DEBUG
	runMode = "开发";
#else
	runMode = "运行";
#endif

	//AutoTable table = TableManager::getSingleton().GetTable("Config");
	//ARecord webWsAddrRe = table->GetRecord("WebWs");
	//ARecord serverRe = table->GetRecord("ServerArea");
	//AString serverName = serverRe["STRING"];



	char szDir[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH - 1, szDir);
	AString title;

	{
		AString serverName = "AccountServer";
		auto &config = CRunConfig<GameServerConfig>::mConfig;
		title.Format("%s_%s <%d : %s>[%s:%d]  v[%s] %s > "
			, GetAppName()
			, runMode.c_str()
			, 0
			, serverName.ANIS().c_str()			
			, config.server_node.node.ip.c_str()
			, config.server_node.node.port

			, SERVER_VERSION_FLAG
			, szDir
		);
	}
	return title;
}

void GameThread::SetTitle(const AString &title)
{
	if (mbStartOk)
	{
		AString str;
		str.Format("%s Server: %d", title.c_str(), 1); // CL_LoginEvent::msTotalNowLogin);
		SetConsoleTitle(str.c_str());
	}
	else
	{
		AString info;
		info.Format("X %s starting ...", GetAppName());
		SetConsoleTitle(info.c_str());
	}
}


bool TraverseDeleteBackFiles(const std::string &path)
{
	_finddata_t file_info;
	string current_path = path + "*.*"; //可以定义后面的后缀为*.exe，*.txt等来查找特定后缀的文件，*.*是通配符，匹配所有类型,路径连接符最好是左斜杠/，可跨平台
	//打开文件查找句柄
	intptr_t handle = _findfirst(current_path.c_str(), &file_info);
	//返回值为-1则查找失败
	if (-1 == handle)
		return false;
	do
	{
		//判断是否子目录
		string attribute;
		if (file_info.attrib == _A_SUBDIR) //是目录
			attribute = "dir";
		else
		{
			attribute = "file";
			//NOTE_LOG("%s", file_info.name);
			if (FileDataStream::GetFileExtName(file_info.name) == ".bak")
			{
				::DeleteFile((path + file_info.name).c_str());
			}
		}
		//输出文件信息并计数,文件名(带后缀)、文件最后修改时间、文件字节数(文件夹显示0)、文件是否目录
		//cout<<file_info.name<<' '<<file_info.time_write<<' '<<file_info.size<<' '<<attribute<<endl; //获得的最后修改时间是time_t格式的长整型，需要用其他方法转成正常时间显示
		//file_num++;

	} while (!_findnext(handle, &file_info));  //返回0则遍历完
	//关闭文件句柄
	_findclose(handle);
	return true;
}


//-------------------------------------------------------------------------
// 实现客户端直接消息交互系统内所有Actor (中转Actor消息请求)
// 连接流程, 实现逻辑与逻辑层的Actor(LogicActor)
// 1 HTTP 验证, 是否新建, 存在准备连接验证数据
// 2 存在直接分配Actor
// 3 新建时, 根据数据库, 新建记录, 然后以记录ID, 为UnitID 新建Actor
// 4 Actor 内绑定 ConnectPtr, Connect 设置数据为对应 的Actor

//-------------------------------------------------------------------------
void GameThread::OnStart(void*)
{
	NiceData lineParam;


	mActorManager = MEM_NEW GameActorManager(this, config.server_node.node.ip.c_str(), config.server_node.node.port, config.server_node.node.saft_code, 2);

	// Login 启动
	{

		mActorManager->RegisterActor(Actor_GameServer, MEM_NEW DefineActorFactory<GameServerActor>());
		
		mActorManager->RegisterActor(Actor_Player, MEM_NEW DefineActorFactory<PlayerActor>());
	
		mGameServerActor = mActorManager->CreateActor(Actor_GameServer, config.SERVER_ID);
		

		//CoroutineTool::AsyncCall(_ConnectGate, this);

	
		CoroutineTool::AsyncCall([&]()
		{
			mbStartOk = mActorManager->mNetNode->AwaitConnectGate(config.server_node.gate.ip.c_str(), config.server_node.gate.port, 10000);

			//AutoNice param = MEM_NEW NiceData();
			//param[DBBASE] = config.db_config.mDBBASE;
			//param[DBIP] = config.db_config.mDBIP;
			//param[DBPASSWORD] = config.db_config.mDBPASSWORD;
			//param[DBPORT] = config.db_config.mDBPORT;
			//param[DBUSER] = config.db_config.mDBUSER;
			//param[TABLE_LIST] = config.db_config.mTABLE_LIST;
			//param[DBPACKET_SIZE] = 16 * 1024 * 1024;

			//// Update共享缓存key使用 Actor_进行ID_线程ID 组成的字符串, 生成哈希的整数
			//AString keyString;
			//int proID = _getpid();
			//keyString.Format("Actor_%d_%llu", proID, GetThreadID());

			//bool b = mActorManager->AsyncInitSQLUpdate(param, "127.0.0.1", 0, 0, MAKE_INDEX_ID(keyString.c_str()));

			//NOTE_LOG("*** DB Init result > %s\r\n", b ? "succeed" : "fail");
			//if (b)
				mGameServerActor = mActorManager->CreateActor(Actor_GameServer, 1);
		}
		);

		//ServerThread::OnStart(NULL);
		//TraverseDeleteBackFiles("./");
		//TraverseDeleteBackFiles("./DBPlugin/");
	}
}

bool gbCanClose = false;

bool GameThread::NotifyThreadClose()
{
	mTaskSystem->Close();

	


	for (int i = 0; i < 100; ++i)
	{
		Process(NULL);
		LowProcess(10);
		TimeManager::Sleep(10);
	}

	mGameServerActor._free();
	for (int i = 0; i < 10; ++i)
	{
		Process(NULL);
		LowProcess(100);
		TimeManager::Sleep(100);
	}

	CoroutineTool::AsyncCall([=]()
	{
		mActorManager->Close();
		tTimer::AWaitTime(6000);
		mActorManager._free();
		gbCanClose = true;
		setActive(false);
	}
	);

	return false;
	//return ServerThread::NotifyThreadClose();
}

void GameThread::Process(void*)
{
	

	mTaskSystem->Process();
	mActorManager->Process();
}
Array<int> coroList;


void GameThread::NotifyException()
{
	DuanXin::Send("login_excep", 0, 1);

	BaseThread::NotifyException();
	mbException = false;

	// 由于在协程内dump, 无法跳回主线, 因此, 直接重启进程
	tBaseServer::RestartProgram(this, true);
}

CoroID gCoro = 0;

void TestCoro(GameThread *)
{
	//LOG("TestCoro do ...");

	//gCoro = CORO;
	YIELD;
	//LOG("TestCoro finish");

}

void GameThread::DoCommand(const AString &commandString, StringArray &paramArray)
{

}

void GameThread::OnLoginDBStart(int dbCode)
{
	NOTE_LOG("DB %d start, now clear all player");

}


StateDataType GameThread::GetRunStateInfo(tNiceData &info)
{
	AString strInfo;

	{
		//info["SERVER_ID"] = mServerInfo.SERVER_ID;


		//strInfo.Format("Wss: %s, %s://%s:%d Login: %d", netInfo.c_str(), IsWss() ? "wss" : "ws", mServerInfo.WSS_IP.c_str(), mServerInfo.WSS_PORT, 1);
	}
	info["INFO"] = strInfo;
	info["VER"] = SERVER_VERSION_FLAG;

	ServerThread::GetRunStateInfo(info);

	//NOTE_LOG("%s > \r\n%s", info.dump().c_str(), info.ToJSON().c_str());
	return info.ToJSON();
}

AString GameThread::GetServerList()
{
	NiceData info;
	//if (!IsAccountWeb())
	//{
	//	info["SERVER_ID"] = mServerInfo.SERVER_ID;

	//	info["WSS_IP"] = mServerInfo.WSS_IP;
	//	info["WSS_PORT"] = mServerInfo.WSS_PORT;
	//	//info["GATE_PORT"] = CRunConfig<LoginConfig>::mConfig.data_db_node.gate.port;
	//}

	return info.ToJSON();
}



bool GameThread::IsAccountWeb()
{
	return true;
}

