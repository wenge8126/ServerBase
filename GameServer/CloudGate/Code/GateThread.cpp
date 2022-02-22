#include "GateThread.h"

#include <windows.h>
#include "CommonDefine.h"
#include "Vesion.h"
#include "BaseServer.h"
#include "SendDuanXin.h"
#include "ActorGate.h"

GateServerConfig CRunConfig<GateServerConfig>::mConfig;

auto &config = CRunConfig<GateServerConfig>::mConfig;

//-------------------------------------------------------------------------
class CGate : public NetCloud::ActorGate
{
public:
	GateThread *mpThread;


	CGate(GateThread *pThread) //, int fixedGateCount, int gateID, const char *szCloudGateIp, int nCloudGatePort, int nGateMeshNetSafeCheck, int threadNum = 2)
		//: ActorGate(fixedGateCount, gateID, szCloudGateIp, nCloudGatePort, nGateMeshNetSafeCheck, threadNum)
		: mpThread(pThread)
	{

	}


};

class NG_SyncRunState : public Logic::tBaseNetEvent
{
public:
	virtual bool _DoEvent() override
	{
		Hand<MeshGate::ViaductNodeServerNet> net = mNetConnect->GetNetHandle();
		Hand<CGate> gate = net->mpMeshGate;
		return true;
	}
};
class NG_CloseServer : public Logic::tBaseNetEvent
{
public:
	virtual bool _DoEvent() override
	{
		Hand<MeshGate::ViaductNodeServerNet> net = mNetConnect->GetNetHandle();
		Hand<CGate> gate = net->mpMeshGate;

		NOTE_LOG("Node reqeust close gate > %s", GetData().dump().c_str());
		gate->mpThread->mbNeedRestart = get("RESTART");
		gate->mpThread->RequestClose();

		return true;
	}
};
//-------------------------------------------------------------------------
// 获取当前运行状态
class NG_GetRunState : public Logic::tClientEvent
{
public:
	virtual bool _DoEvent() override
	{
		Hand<MeshGate::ViaductNodeServerNet> net = mNetConnect->GetNetHandle();
		Hand<CGate> gate = net->mpMeshGate;
	
		gate->mpThread->GetRunStateInfo(GetResponseEvent()->GetData());
		Finish();
		return true;
	}
};
//-------------------------------------------------------------------------

AString GateThread::GetTitle()
{
	int gateCode = 0;

	AString runMode;
#if _DEBUG
	runMode = "开发";
#else
	runMode = "运行";
#endif
	 
	char szDir[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH - 1, szDir);
	AString title;
	title.Format("%s_%s <%d>[%s:%d] Gate[%s:%d] v[%s] %s > ", config.title.c_str()
		, runMode.c_str()
		, gateCode
		, config.gate_server.ip.c_str()
		, config.gate_server.port
		, config.gate_node.address.ip.c_str()
		, config.gate_node.address.port
		, SERVER_VERSION_FLAG
		, szDir
	);
	return title;
}

void GateThread::SetTitle(const AString &title)
{
	SetConsoleTitle(title.c_str());
}


void GateThread::OnStart(void*)
{
	RunConfig::InitLoad();


	ServerThread::OnStart(NULL);

	mGate = MEM_NEW CGate(this); // , config.gate_node.fix_count, config.gate_node.gate_code, config.gate_node.address.ip.c_str(), config.gate_node.address.port, config.gate_node.address.saft_code);

	//mGate->Init(NetCloud::Address(config.gate_server.ip.c_str(), config.gate_server.port), NetCloud::Address(config.main_gate.ip.c_str(), config.main_gate.port));

	mGate->StartGate(config.gate_node.fix_count
		, config.gate_node.gate_code
		, config.gate_server.ip.c_str()
		, config.gate_server.port
		, config.main_gate.ip.c_str()
		, config.main_gate.port
		, config.gate_node.address.saft_code
	);

	//Hand<MeshGate> gate = mGate;
	//REGISTER_EVENT(gate->mNodeServerNet->GetEventCenter(), NG_SyncRunState);
	//REGISTER_EVENT(gate->mNodeServerNet->GetEventCenter(), NG_CloseServer);
	//REGISTER_EVENT(gate->mNodeServerNet->GetEventCenter(), NG_GetRunState);

	AutoTable whiteListTable = TableManager::getSingleton().CreateNewTable(CLOUD_WHITE_LIST_NAME);
	if (!whiteListTable->LoadCSV("RunConfig/GateWhiteList.csv"))
		TableManager::getSingleton().ReleaseTable(CLOUD_WHITE_LIST_NAME);
	
}

void GateThread::NotifyException()
{
	DuanXin::Send("gate_excep", 0, 1);

	BaseThread::NotifyException();
	mbException = false;

	// 由于在协程内dump, 无法跳回主线, 因此, 直接重启进程
	tBaseServer::RestartProgram(this, true);
}

void GateThread::DoCommand(const AString &commandString, StringArray &paramArray)
{

}

StateDataType GateThread::GetRunStateInfo(tNiceData &info)
{
	info["VER"] = SERVER_VERSION_FLAG;

	Hand<MeshGate> gate = mGate;
	// Gate连接数量, 节点连接数量, Unit数量
	AString strInfo;
	strInfo.Format("Unit %d, Node %d, GConnect %d, Gate %d", gate->TotalUnitCount(), gate->GetCloudNodeCount(), gate->GetNowGateConnect(), gate->GetGateCount());
	info["INFO"] = strInfo;

	return ServerThread::GetRunStateInfo(info);
}
