#include "WebLoginThread.h"


#include "CommonDefine.h"

#include "TaskManager.h"
//#include "NodeMsgEvent.h"
#include "BaseMsg.h"
#include "LoginMsgData.h"

#include "ConfigEnum.h"
#include "Vesion.h"

#include <io.h>
#include "Dump.h"
#include "BaseServer.h"
#include "SendDuanXin.h"
#include "SystemInfo.h"

#include <windows.h>

#include "LoginNetComponect.h"
#include "AsyncLoop.h"

#include "DBUserComponent.h"

using namespace uWS;

using namespace NetCloud;

using namespace std;

DEFINE_RUN_CONFIG(LoginConfig)

void Analysis(NiceData &msg, const AString &requestData)
{
	Array<AString> tempList;
	AString::Split(requestData.c_str(), tempList, "&", 100);
	for (int i = 0; i < tempList.size(); i++)
	{
		Array<AString> str;
		AString::Split(tempList[i].c_str(), str, "=", 2);

		if (str.size() == 2)
		{
			msg[str[0].c_str()] = str[1].c_str();
		}
	}
}

//-------------------------------------------------------------------------
int g_register_login::Run(ShareMemCloudDBNode *pNode, const AString &tableName, AutoNice &paramData, AutoNice &resultData)
{
	return eNoneError;
}


//-------------------------------------------------------------------------

WebLoginThread::WebLoginThread()
	: ServerThread(GetAppName())
{
	mServerListJson = "{}";
	mServerList = MEM_NEW r_server_list();

	mTaskSystem = MEM_NEW TaskSystem(32);
}

//-------------------------------------------------------------------------

AString WebLoginThread::GetTitle()
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

	bool bAccountLogin = IsAccountWeb(); // ((int)webWsAddrRe["VALUE"] <= 0);

	char szDir[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH - 1, szDir);
	AString title;

	{
		AString serverName = mServerInfo.SERVER_NAME;
		auto &config = CRunConfig<LoginConfig>::mConfig;
		title.Format("%s_%s <%d : %s>[%s://%s:%d]  v[%s] %s > "
			, GetAppName()
			, runMode.c_str()
			, mServerInfo.SERVER_ID
			, serverName.ANIS().c_str()
			, IsWss() ? "wss" : "ws"
			, config.ws_ip.c_str()
			, config.ws_port

			, SERVER_VERSION_FLAG
			, szDir
		);
	}
	return title;
}

void WebLoginThread::SetTitle(const AString &title)
{
	if (mbStartOk)
	{
		AString str;
		str.Format("%s Login: %d", title.c_str(), 1); // CL_LoginEvent::msTotalNowLogin);
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


void _ConnectGate(WebLoginThread *pThread)
{
	//LOG("=== Connect gate start");
	//bool re = Async::AwaitLoop(
	//	[=]()
	//{
	//	bool b = pThread->mActorManager->mNetNode->AwaitConnectGate(NetAddress(config.login_node.gate.ip.c_str(), config.login_node.gate.port));
	//	return b;
	//}
	//	, 10, 3000);

	//LOG("=== Connect gate : %s", re ? "ok" : "fail");

	//while (true)
	//{
	//	bool b = pThread->mActorManager->mNetNode->AwaitConnectGate(NetAddress(config.login_node.gate.ip.c_str(), config.login_node.gate.port));
	//	if (b)
	//		break;
	//	tTimer::AWaitTime(3000);
	//}
	pThread->mbStartOk = true;
}
//-------------------------------------------------------------------------


//-------------------------------------------------------------------------
// 实现客户端直接消息交互系统内所有Actor (中转Actor消息请求)
// 连接流程, 实现逻辑与逻辑层的Actor(LogicActor)
// 1 HTTP 验证, 是否新建, 存在准备连接验证数据
// 2 存在直接分配Actor
// 3 新建时, 根据数据库, 新建记录, 然后以记录ID, 为UnitID 新建Actor
// 4 Actor 内绑定 ConnectPtr, Connect 设置数据为对应 的Actor
//class ActorNetMsgComponent : public NetWorkerComponent
//{
//public:
//	virtual bool OnConnected(HandConnect connect)
//	{
//		NetWorkerComponent::OnConnected(connect);
//
//		CoroutineTool::AsyncCall(AsyncCreateConnectActor, connect);
//		return true;
//	}
//
//	static void AsyncCreateConnectActor(HandConnect connect)
//	{
//		Hand<NetWorkerComponent> comp = connect->GetUserData();
//		// 验证
//		// 获取数据
//		// 新建记录
//		// 新建
//	}
//
//	// 注册中转需要的事件
//	virtual void _RegisterMsg(Logic::tEventCenter *pCenter) override
//	{
//		REGISTER_EVENT(pCenter, CS_ActorMsg);
//		pCenter->RegisterMsg(200, MEM_NEW Logic::EventFactory<CS_ActorMsg>());
//	}
//};

//-------------------------------------------------------------------------
void WebLoginThread::OnStart(void*)
{
	NiceData lineParam;

	//mTaskSystem->RegisterTask(MEM_NEW DefineTaskFactory<BackZipMsgTask, eDBMsgTask_ZipData>());

	mActorManager = MEM_NEW LoginActorManager(this, config.login_node.node.ip.c_str(), config.login_node.node.port, config.login_node.node.saft_code, 2);

	// Login 启动
	{
		auto &config = CRunConfig<LoginConfig>::mConfig;

		SystemInfo::ReadCommandLineParam(GetCommandLine(), lineParam);
		int addPort = lineParam["port"];
		if (addPort != 0)
		{
			config.ws_port += addPort;

			AutoNice d = MEM_NEW NiceData();
			config.ToData(d);
			AString info;
			RunConfig::ShowConfig(d, "login_config", info);
			LOG("配置变更: %d ***************************************************************\r\n====================================\r\n%s====================================\r\n", addPort, info.ANIS().c_str());
		}
		// 连接LogicDB



		mActorManager->RegisterActor(Unit_Login, MEM_NEW DefineActorFactory<LoginActor>());
		mActorManager->RegisterComponect("HttpComponect", MEM_NEW EventFactory<HttpComponect>());
		mActorManager->RegisterComponect("TcpComponent", MEM_NEW EventFactory< TcpComponent>());

		mLoginActor = mActorManager->CreateActor(Unit_Login, config.login_id);


		mSdkMgr.InitThread();

		//CoroutineTool::AsyncCall(_ConnectGate, this);

		mActorManager->mNetNode->ConnectGate(config.login_node.gate.ip.c_str(), config.login_node.gate.port, 10000);


		//ServerThread::OnStart(NULL);
		//TraverseDeleteBackFiles("./");
		//TraverseDeleteBackFiles("./DBPlugin/");
	}
}

bool gbCanClose = false;

bool WebLoginThread::NotifyThreadClose()
{
	mTaskSystem->Close();

	mSdkMgr.Close();


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

void WebLoginThread::Process(void*)
{
	mSdkMgr.Process();

	mTaskSystem->Process();
	mActorManager->Process();
}
Array<int> coroList;
class TT : public AutoBase
{
public:
	void Test()
	{
		//AString response;
		//NiceData req;
		//req["cp_order_id"] = 1;
		//AString str = "cmd=1003&server_id=1&content=最新公告nnn";
		//mpThread->mSdkMgr.AwaitRequest("http://127.0.0.1:4000", response, str.UTF8().c_str());
		////mpThread->mSdkMgr.AwaitRequest("http://127.0.0.1:10001?pp=99&x=77", response, "package_id=5913&user_id=45685687&channel_userid=37481234&idfa=&idfv=&uuid=&imei=wy&mac=xx.xx.xx.xx&ip=106.122.227.55&platform=2&version=1.0.0&device_name=H5&device_user_name=H5&app_name=%E6%AC%A2%E5%A8%B1%E5%8F%91%E8%A1%8C-H5%E7%A7%BB%E5%8A%A8%E6%95%99%E5%AE%A4&app_version=1.0.0&app_build_version=1.0.0&juhesdk_version=1.0.0&channel_version=1.0.0&net_isp=null&net_status=null&cp_order_id=yfgame-2021%2F01%2F06+18%3A59%3A04-1&money=30&unix_name=%E9%92%BB%E7%9F%B3%E7%A4%BC%E5%8C%851&product_id=1&product_num=1&product_desc=%E9%92%BB%E7%9F%B3%E7%A4%BC%E5%8C%851&extends=&role_id=9&role_name=45685687&role_level=1&server_id=1&server_name=1&ext=undefined&juhe_order_id=jh278-8afde322befb59&sign=29429BCEFABB28CA9C1A1250ACB87C75&is_sandbox=1&real_money=30.00");
		//NOTE_LOG("Test http response > %s", response.c_str());
	}

	void TestPost()
	{
		coroList.push_back(CORO);
		YIELD;
		//AString response; 
		//mpThread->mSdkMgr.AwaitRequest("http://127.0.0.1:4000?cmd=1001&server_id=1", response);
		//NOTE_LOG("Test http response > %s", response.c_str());

		//mpThread->mSdkMgr.AwaitRequest("http://127.0.0.1:10001", response, "cmd=server_mail&server_id=1&player_id=1&title=test&content=11111&items=0|103036|10&type=2");
		//NOTE_LOG("Test http response > %s", response.c_str());

		//mpThread->mSdkMgr.AwaitRequest("http://127.0.0.1:10001", response, "cmd=code&server_id=1&code=hello&type=2");
		//NOTE_LOG("Test http response > %s", response.c_str());

		//mpThread->mSdkMgr.AwaitRequest("http://127.0.0.1:10001", response, "cmd=fenghao&server_id=1&type=1&player_id=1&device=1&reson=1&enddate=2017-12-02 00:00:00");
		//NOTE_LOG("Test http response > %s", response.c_str());
	}

public:
	WebLoginThread  *mpThread;
};

void WebLoginThread::NotifyException()
{
	DuanXin::Send("login_excep", 0, 1);

	BaseThread::NotifyException();
	mbException = false;

	// 由于在协程内dump, 无法跳回主线, 因此, 直接重启进程
	tBaseServer::RestartProgram(this, true);
}

CoroID gCoro = 0;

void TestCoro(WebLoginThread *)
{
	//LOG("TestCoro do ...");

	//gCoro = CORO;
	YIELD;
	//LOG("TestCoro finish");

}

void WebLoginThread::DoCommand(const AString &commandString, StringArray &paramArray)
{
	if (commandString == "d")
	{
		//Auto<TT> test = MEM_NEW TT();
		//test->mpThread = this;
		//ASYNCAUTO(&TT::Test, test.getPtr());
		if (gCoro != 0)
		{
			RESUME(gCoro);
			LOG("DoCommand d ok ...");
		}
	}
	else if (commandString == "d2")
	{
		CoroutineTool::AsyncCall(TestCoro, this);
	}

	else if (commandString == "i")
	{
		Auto<TT> test = MEM_NEW TT();
		test->mpThread = this;
		for (int i = 0; i < 10000; ++i)
		{
			ASYNCAUTO(&TT::TestPost, test.getPtr());
			CoroutineTool::CheckFinish();
		}
		LOG("Now count %d, %d", CoroutineTool::getCount(), coroList.size());
		UInt64 now = TimeManager::NowTick();

		for (int i = 0; i < coroList.size(); ++i)
		{
			RESUME(coroList[i]);
			CoroutineTool::CheckFinish();
		}

		LOG("Coro use time > %llu", TimeManager::NowTick() - now);

		//#define DEVELOP_MODE
		//		CoroutineTool::DumpDebug();
		//#endif
	}
	else if (commandString == "t")
	{
		if (paramArray.size() < 3)
		{
			ERROR_LOG("t : test no sql must 3 param, second is key, thrid is value(json)");
		}
		else
		{
			CoroutineTool::AsyncCall([=]()
			{
				AString json = paramArray[2];

				Hand<RecordNoSQLUserComponent> user = mLoginActor->GetComponent("TestNoSQLUserComponent");
				AString key = paramArray[1];
				AutoNice data = MEM_NEW NiceData();
				if (data->FullJSON(json))
				{
					AutoTable t = user->NewTable();
					AutoField field = t->GetField(); // MEM_NEW FieldIndex(NULL, data->count());
					//AutoTable t = tBaseTable::NewBaseTable();
					int col = 0;
					for (auto it = data->begin(); it; ++it)
					{
						AString name = it.key();
						AData &d = it.get();
						field->setField(name.c_str(), d.getType(), col++);
					}
					if (!field->check())
					{
						ERROR_LOG("Field init set fail");
						return;
					}
					
					user->InitRecord(t->CreateRecord(0, true));
					for (auto it = data->begin(); it; ++it)
					{
						AString name = it.key();
						AData &d = it.get();
						Data dd(&d);
						user->mDataRecord->set(name, dd);
					}

					if (user->Save(key))
						NOTE_LOG("Save succeed")
					else
						ERROR_LOG("Save fail");
				}
				else
					ERROR_LOG("Json 解析失败");
			});
		}
	}

	else if (commandString == "l")
	{
		if (paramArray.size() < 2)
		{
			ERROR_LOG("t : test no sql must 2 param, second is key");
		}
		else
		{
			CoroutineTool::AsyncCall([=]()
			{


				Hand<RecordNoSQLUserComponent> user = mLoginActor->GetComponent("RecordNoSQLUserComponent");
				AString key = paramArray[1];
				//user->mData.mNiceData = MEM_NEW NiceData();
				if (user->Load(key, true))
				{
					AutoNice dd = user->mDataRecord->ToNiceData();
					//AString json = user->mData.mNiceData->ToJSON();
					AString json = dd->ToJSON();
					if (json.length() > 0)
					{
						NOTE_LOG("Load succeed : %s", json.c_str());
					}
					else
						ERROR_LOG("Load fail");
				}
				else
					ERROR_LOG("Load reqeust fail");
			});
		}
	}
	else if (commandString == "t2")
	{
		if (paramArray.size() < 3)
		{
			ERROR_LOG("t : test no sql must 3 param, second is key, thrid is value(json)");
		}
		else
		{
			CoroutineTool::AsyncCall([=]()
			{
				Hand<TestNoSQLUserComponent > user = mLoginActor->GetComponent<TestNoSQLUserComponent>();
				//if (!user->mDataRecord)
				{
					AutoTable t = user->CreateTable();
					ARecord re = t->CreateRecord(paramArray[1], true);
					user->InitRecord(re);
					user->TYPE() = paramArray[2];
					user->Save(paramArray[1]);
				}
			}
			);
		}
	}
	else if (commandString == "t3")
	{
		if (paramArray.size() < 4)
		{
			ERROR_LOG("t : test no sql must 4 param, second is key, thrid is value(json)");
		}
		else
		{
			CoroutineTool::AsyncCall([=]()
			{
				Hand<Test2Component > user = mLoginActor->GetComponent<Test2Component>();
				//if (!user->mDataRecord)
				{
					AutoTable t = user->CreateTable();
					ARecord re = t->CreateRecord(paramArray[1], true);
					user->InitRecord(re);
					user->TYPE() = paramArray[2];
					user->YYY() = paramArray[3];
					user->Save(paramArray[1]);
				}
			}
			);
		}
	}
	else if (commandString == "i1")
	{
		if (paramArray.size() < 4)
		{
			ERROR_LOG("t : test no sql must 4 param, second is key, thrid is value(json)");
		}
		else
		{
			CoroutineTool::AsyncCall([=]()
			{
				Hand<PlayerItemComp > item = mLoginActor->GetComponent<PlayerItemComp>();
				//if (!user->mDataRecord)
				{								
					ARecord re = item->CreateItem();
					item->NAME() = paramArray[1];
					item->TYPE() = paramArray[2];
					item->COUNT() = paramArray[3];
					item->Save(item->mItemData.GetRecordKey(re));
				}
			}
			);
		}
	}
	else if (commandString == "l2")
	{
		//if (paramArray.size() < 4)
		//{
		//	ERROR_LOG("t : test no sql must 4 param, second is key, thrid is value(json)");
		//}

		CoroutineTool::AsyncCall([=]()
		{
			Hand<PlayerItemComp > item = mLoginActor->GetComponent<PlayerItemComp>();
			//if (!user->mDataRecord)
			{
				int i = 0;
				while (true)
				{
					AString key = "ptest_";
					key += i++;
					if (item->Load(key, true))
					{
						NOTE_LOG("Item : %s \r\n%s", key.c_str(), item->mDataRecord->ToNiceData()->dump().c_str());
					}
					else
						break;
				}
			}
		}
		);
	}
	else if (commandString == "l3")
	{
		if (paramArray.size() < 2)
		{
			ERROR_LOG("t : test no sql must 2 param, second is key, thrid is value(json)");
			return;
		}
		CoroutineTool::AsyncCall([=]()
		{
			Hand<PlayerItemComp > item = mLoginActor->GetComponent<PlayerItemComp>();
			item->mItemData.LoadAll(TOINT(paramArray[1].c_str()), item.getPtr());
		}
		);
	}
	else if (commandString == "r")
	{
		if (paramArray.size() < 2)
		{
			ERROR_LOG("t : test no sql must 2 param, second is key, thrid is value(json)");
			return;
		}
		CoroutineTool::AsyncCall([=]()
		{
			int id = TOINT(paramArray[1].c_str());
			Hand<PlayerItemComp > item = mLoginActor->GetComponent<PlayerItemComp>();
			ARecord re = item->mItemData.Find(id);
			if (re)
				item->mItemData.Remove(re, item.getPtr());
			else
				ERROR_LOG("No find item %d", id)
		}
		);
	}
}

template<bool bSSL>
void WebLoginThread::OnPlayerDisconnect(WebPlayer<bSSL> *pLayer)
{
	if (pLayer->mbForeClose)
		return;

	//AutoEvent evt = mLoginActor->StartEvent("LD_NotifyPlayerLeave", 0);
	//evt["ID"] = pLayer->mDBID;
	////evt->Start();
	//mLoginActor->SendEvent({ Unit_DBWorker, 0 }, evt.getPtr(), BROADCAST_MODE::eBroadcastByType);
}

void WebLoginThread::OnLoginDBStart(int dbCode)
{
	NOTE_LOG("DB %d start, now clear all player");

}


StateDataType WebLoginThread::GetRunStateInfo(tNiceData &info)
{
	AString strInfo;

	{
		info["SERVER_ID"] = mServerInfo.SERVER_ID;


		//strInfo.Format("Wss: %s, %s://%s:%d Login: %d", netInfo.c_str(), IsWss() ? "wss" : "ws", mServerInfo.WSS_IP.c_str(), mServerInfo.WSS_PORT, 1);
	}
	info["INFO"] = strInfo;
	info["VER"] = SERVER_VERSION_FLAG;

	ServerThread::GetRunStateInfo(info);

	//NOTE_LOG("%s > \r\n%s", info.dump().c_str(), info.ToJSON().c_str());
	return info.ToJSON();
}

AString WebLoginThread::GetServerList()
{
	NiceData info;
	if (!IsAccountWeb())
	{
		info["SERVER_ID"] = mServerInfo.SERVER_ID;

		info["WSS_IP"] = mServerInfo.WSS_IP;
		info["WSS_PORT"] = mServerInfo.WSS_PORT;
		//info["GATE_PORT"] = CRunConfig<LoginConfig>::mConfig.data_db_node.gate.port;
	}

	return info.ToJSON();
}



bool WebLoginThread::IsAccountWeb()
{
	return strstr(GetCommandLine(), " account_web_config") != NULL;
}

bool WebLoginThread::IsWss()
{


	return CRunConfig<LoginConfig>::mConfig.key_file.length() > 0;

}
