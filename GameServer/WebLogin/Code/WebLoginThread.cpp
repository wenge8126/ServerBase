#include "WebLoginThread.h"

#include "CWssServerNet.h"
#include "CommonDefine.h"

#include "TaskManager.h"
#include "NodeMsgEvent.h"
#include "BaseMsg.h"
#include "LoginMsgData.h"

#include "ConfigEnum.h"
#include "Vesion.h"
//#include "WebPlayer.inl"
#include "CWssServerNet.inl"
#include <io.h>
#include "Dump.h"
#include "BaseServer.h"
#include "SendDuanXin.h"
#include "SystemInfo.h"

#include <windows.h>

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
	runMode = "����";
#else
	runMode = "����";
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
		str.Format("%s Login: %d", title.c_str(), CL_LoginEvent::msTotalNowLogin);
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
    string current_path=path+"*.*"; //���Զ������ĺ�׺Ϊ*.exe��*.txt���������ض���׺���ļ���*.*��ͨ�����ƥ����������,·�����ӷ��������б��/���ɿ�ƽ̨
    //���ļ����Ҿ��
    intptr_t handle=_findfirst(current_path.c_str(),&file_info);
    //����ֵΪ-1�����ʧ��
    if(-1==handle)
        return false;
    do
    {
        //�ж��Ƿ���Ŀ¼
        string attribute;
        if(file_info.attrib==_A_SUBDIR) //��Ŀ¼
            attribute="dir";
		else
		{
			attribute = "file";
			//NOTE_LOG("%s", file_info.name);
			if (FileDataStream::GetFileExtName(file_info.name) == ".bak")
			{
				::DeleteFile((path+file_info.name).c_str());
			}
		}
        //����ļ���Ϣ������,�ļ���(����׺)���ļ�����޸�ʱ�䡢�ļ��ֽ���(�ļ�����ʾ0)���ļ��Ƿ�Ŀ¼
        //cout<<file_info.name<<' '<<file_info.time_write<<' '<<file_info.size<<' '<<attribute<<endl; //��õ�����޸�ʱ����time_t��ʽ�ĳ����ͣ���Ҫ����������ת������ʱ����ʾ
        //file_num++;
 
    }while(!_findnext(handle,&file_info));  //����0�������
    //�ر��ļ����
    _findclose(handle);
    return true;
}
 
void _ConnectGate(WebLoginThread *pThread)
{
	while (true)
	{
		bool b = pThread->mActorManager->mNetNode->AwaitConnectGate(NetAddress(config.login_node.gate.ip.c_str(), config.login_node.gate.port));
		if (b)
			break;
		tTimer::AWaitTime(3000);
	}
	pThread->mbStartOk = true;
}
//-------------------------------------------------------------------------
class CS_ActorMsg : public ComponectResponseMsg
{
public:
	virtual bool _DoEvent() override
	{
		ASYNC(&CS_ActorMsg::Async, this);
		return true;
	}

	void Async()
	{
		AString msgName = get("MSG_NAME");
		int type = get("UNIT_TYPE");
		Int64 id = get("UNIT_ID");

		GetResponseEvent()["RESP"] = GetActor()->Await(msgName, GetData(), { type, id }, 6000).getPtr();
		Finish();
	}

protected:
private:
};

//-------------------------------------------------------------------------
// ʵ�ֿͻ���ֱ����Ϣ����ϵͳ������Actor (��תActor��Ϣ����)
class ActorNetMsgComponent : public NetWorkerComponent
{
public:
	// ע����ת��Ҫ���¼�
	virtual void _RegisterMsg(Logic::tEventCenter *pCenter) override
	{
		REGISTER_EVENT(pCenter, CS_ActorMsg);
	}
};

//-------------------------------------------------------------------------
void WebLoginThread::OnStart(void*)
{
	NiceData lineParam;

	mTaskSystem->RegisterTask(MEM_NEW DefineTaskFactory<BackZipMsgTask, eDBMsgTask_ZipData>());

	mActorManager = MEM_NEW LoginActorManager(this, config.login_node.node.ip.c_str(), config.login_node.node.port, config.login_node.node.saft_code, 2);

	// Login ����
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
			LOG("���ñ��: %d ***************************************************************\r\n====================================\r\n%s====================================\r\n", addPort, info.ANIS().c_str());
		}
		// ����LogicDB

		BaseWebServer *pNet = NULL;
		if (IsWss())
			pNet = MEM_NEW CWssServerNet<true>(this);
		else
			pNet = MEM_NEW CWssServerNet<false>(this);
		mWebWsNet = pNet;
		
		pNet->StartNet(config.ws_ip.c_str(), config.ws_port, config.key_file, config.cert_file, config.password);

		mActorManager->RegisterActor(Unit_Login, MEM_NEW DefineActorFactory<LoginActor>());
		mActorManager->RegisterComponect("HttpComponect", MEM_NEW EventFactory<HttpComponect>());
		mActorManager->RegisterComponect("TcpComponent", MEM_NEW EventFactory< TcpComponent>());
		mActorManager->RegisterComponect("ActorNetMsgComponent", MEM_NEW EventFactory< ActorNetMsgComponent>());

		mLoginActor = mActorManager->CreateActor(Unit_Login, config.login_id);
		

		mSdkMgr.InitThread();

		CoroutineTool::AsyncCall(_ConnectGate, this);
	
		//ServerThread::OnStart(NULL);
		//TraverseDeleteBackFiles("./");
		//TraverseDeleteBackFiles("./DBPlugin/");
	}
}


bool WebLoginThread::NotifyThreadClose()
{
	mTaskSystem->Close();

	mSdkMgr.Close();

	if (mWebWsNet)
	{
		mWebWsNet->StopNet();
		mWebWsNet->Process();
		mWebWsNet._free();
	}

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

	return ServerThread::NotifyThreadClose();
}

void WebLoginThread::Process(void*)
{
	mSdkMgr.Process();

	if (mWebWsNet)
		mWebWsNet->Process();

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
		//AString str = "cmd=1003&server_id=1&content=���¹���nnn";
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

	// ������Э����dump, �޷���������, ���, ֱ����������
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
		for (int i=0; i<10000; ++i)
		{
			ASYNCAUTO(&TT::TestPost, test.getPtr());
			CoroutineTool::CheckFinish();
		}
		LOG("Now count %d, %d", CoroutineTool::getCount(), coroList.size());
		UInt64 now = TimeManager::NowTick();

		for (int i=0; i<coroList.size(); ++i)
		{
			RESUME(coroList[i]);
			CoroutineTool::CheckFinish();
		}

		LOG("Coro use time > %llu", TimeManager::NowTick() - now);
		
//#define DEVELOP_MODE
//		CoroutineTool::DumpDebug();
//#endif
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

	Hand<BaseWebServer> net = mWebWsNet;
	net->InitClearAllPlayer();
}


StateDataType WebLoginThread::GetRunStateInfo(tNiceData &info)
{	
	AString strInfo;

	{
		info["SERVER_ID"] = mServerInfo.SERVER_ID;

		//info["WSS_IP"] = mServerInfo.WSS_IP;
		//info["WSS_PORT"] = mServerInfo.WSS_PORT;
		//info["GATE_PORT"] = CRunConfig<LoginConfig>::mConfig.data_db_node.gate.port;
		Hand< BaseWebServer> net = mWebWsNet;
		AString netInfo = net ? net->GetRunInfo() : "Null net";
		strInfo.Format("Wss: %s, %s://%s:%d Login: %d", netInfo.c_str(), IsWss() ? "wss" : "ws", mServerInfo.WSS_IP.c_str(), mServerInfo.WSS_PORT, CL_LoginEvent::msTotalNowLogin);
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
	return strstr(GetCommandLine(), " account_web_config")!=NULL;
}

bool WebLoginThread::IsWss()
{

	
	return CRunConfig<LoginConfig>::mConfig.key_file.length() > 0;
	
}