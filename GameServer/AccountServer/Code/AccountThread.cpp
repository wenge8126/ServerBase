#include "AccountThread.h"


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

#include "AccountActor.h"

using namespace uWS;

using namespace NetCloud;

using namespace std;

DEFINE_RUN_CONFIG(AccountServerConfig)


//-------------------------------------------------------------------------



//-------------------------------------------------------------------------

AccountThread::AccountThread()
	: ServerThread(GetAppName())
{
	mServerListJson = "{}";
	//mServerList = MEM_NEW r_server_list();

	mTaskSystem = MEM_NEW TaskSystem(32);
}

//-------------------------------------------------------------------------

AString AccountThread::GetTitle()
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
		//auto &config = CRunConfig<AccountServerConfig>::mConfig;
		title.Format("%s_%s <%d : %s>[%s://%s:%d]  v[%s] %s > "
			, GetAppName()
			, runMode.c_str()
			, 0
			, serverName.ANIS().c_str()
			,"http"
			, config.web_net.http_address.c_str()
			, config.web_net.port

			, SERVER_VERSION_FLAG
			, szDir
		);
	}
	return title;
}

void AccountThread::SetTitle(const AString &title)
{
	if (mbStartOk)
	{
		//AString str;
		//str.Format("%s Account: %d", title.c_str(), 1); // CL_LoginEvent::msTotalNowLogin);
		SetConsoleTitle(title.c_str());
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


//-------------------------------------------------------------------------
// 实现客户端直接消息交互系统内所有Actor (中转Actor消息请求)
// 连接流程, 实现逻辑与逻辑层的Actor(LogicActor)
// 1 HTTP 验证, 是否新建, 存在准备连接验证数据
// 2 存在直接分配Actor
// 3 新建时, 根据数据库, 新建记录, 然后以记录ID, 为UnitID 新建Actor
// 4 Actor 内绑定 ConnectPtr, Connect 设置数据为对应 的Actor

//-------------------------------------------------------------------------
void AccountThread::OnStart(void*)
{
	
	ServerThread::OnStart(NULL);


	mActorManager = MEM_NEW AccountActorManager(this, config.actor_node.node.ip.c_str(), config.actor_node.node.port, config.actor_node.node.saft_code, 2);

	// Login 启动
	{
		//auto &config = CRunConfig<LoginConfig>::mConfig;

		//SystemInfo::ReadCommandLineParam(GetCommandLine(), lineParam);
		//int addPort = lineParam["port"];
		//if (addPort != 0)
		//{
		//	config.ws_port += addPort;

		//	AutoNice d = MEM_NEW NiceData();
		//	config.ToData(d);
		//	AString info;
		//	RunConfig::ShowConfig(d, "login_config", info);
		//	LOG("配置变更: %d ***************************************************************\r\n====================================\r\n%s====================================\r\n", addPort, info.ANIS().c_str());
		//}
		// 连接LogicDB


		mActorManager->RegisterActor(Actor_Account, MEM_NEW DefineActorFactory<AccountActor>());
		mActorManager->RegisterComponect("HttpComponect", MEM_NEW EventFactory<HttpComponect>());


		mLoginActor = mActorManager->CreateActor(Actor_Account, config.account_id);


		

		CoroutineTool::AsyncCall([=]()
		{
			mbStartOk = mActorManager->mNetNode->AwaitConnectGate(config.actor_node.gate.ip.c_str(), config.actor_node.gate.port, 10000);
		});


		//ServerThread::OnStart(NULL);
		//TraverseDeleteBackFiles("./");
		//TraverseDeleteBackFiles("./DBPlugin/");
	}
}

bool gbCanClose = false;

bool AccountThread::NotifyThreadClose()
{
	mTaskSystem->Close();

	


	for (int i = 0; i < 100; ++i)
	{
		Process(NULL);
		LowProcess(10);
		TimeManager::Sleep(10);
	}

	mLoginActor._free();
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

void AccountThread::Process(void*)
{
	

	mTaskSystem->Process();
	mActorManager->Process();
}
Array<int> coroList;


void AccountThread::NotifyException()
{
	DuanXin::Send("login_excep", 0, 1);

	BaseThread::NotifyException();
	mbException = false;

	// 由于在协程内dump, 无法跳回主线, 因此, 直接重启进程
	tBaseServer::RestartProgram(this, true);
}

CoroID gCoro = 0;

void TestCoro(AccountThread *)
{
	//LOG("TestCoro do ...");

	//gCoro = CORO;
	YIELD;
	//LOG("TestCoro finish");

}

void AccountThread::DoCommand(const AString &commandString, StringArray &paramArray)
{

}

void AccountThread::OnLoginDBStart(int dbCode)
{
	NOTE_LOG("DB %d start, now clear all player");

}


StateDataType AccountThread::GetRunStateInfo(tNiceData &info)
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

AString AccountThread::GetServerList()
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



