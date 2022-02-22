#ifndef _INCLUDE_MESHEDNODENET_H_
#define _INCLUDE_MESHEDNODENET_H_

#include "IOCPServerNet.h"
#include "IOCPConnect.h"
#include "EventCenter.h"
#include "Common.h"
//-------------------------------------------------------------------------
#define DEFALUT_MESH_NODE_SAFE_CODE		(-200)

//using namespace Logic;

typedef EasyMap<GSKEY, AutoEvent>		NetNodeList;
/*/-------------------------------------------------------------------------
// ��״����ڵ�Net
// ʵ��ÿ���ڵ㻥��, ������ GameServer ֮�佨��
// ��������жϺ�, ֧����������, ��ͨ�� World�е��б�, ������������(���ӻ��Ƴ�)

// NOTE: 1 һ���¼�������, ֻ������һ����������
//		 2 �¼�����, ʹ�� _getTarget() �õ�����ID����
//		 3 ��������¼����Ķ������������¼�������ʹ�� SR_RequestEventFactory ע��
//		   Ŀ��֧��ͬһ�¼����ͼ����� ͬ�������¼����� ��� _R �� CS_NetTestEvent_R
		 4 ����NodeNet ��һ�Զ�������磬������ϢID�����ܵõ�ͳһ����ǰ�ڵ��ڣ���֧��ID����������Ϣ�Ż�
   ʵ�֣�
		1 ���������ڵ������ AutoNet				mServerNet
		2 �����߳� ConnectNetThread
		3 �������ݽṹ NetNodeConnectData���������ӳɹ�ʧ��, �ұ������Ӷ���
		4 ���������ڵ�������� MeshedNodeNet ������󴴽����൱��һ��������������
		5 ����Э��ͬʱ���������Ӽ��������ӵķ�������mServerNet
		
//-------------------------------------------------------------------------*/
class NetNodeConnectData;

class Net_Export MeshedNodeNet : public IOCPBaseNet
{
public:
	MeshedNodeNet(const char *szServerIp, int port, int nSafeCheck, int threadNum = 2);
	~MeshedNodeNet();

	virtual const char* NodeInfo(){ return "THIS"; }

public:
	// ���������ӶԷ�Node���ɹ���, ֪ͨ�Է��¼�����֮ǰ����Я����������
	virtual void OnRequestNode(NetNodeConnectData *nodeData, AutoEvent &requestEvt) { DEBUG_LOG("======= %s [%s:%d] start request", NodeInfo(), GetIp(), GetPort()); }

	// ��Ϊ�������ߣ����յ����������Ӻ�������¼�, ��Ҫ���������ߵ�ǰ�����ڵ�Ⱥ����Ϣ
	virtual void OnResponseNode(GSKEY otherPartyGSKey, AutoEvent &requstEvt, AutoEvent &respEvt) {
		respEvt["INFO"] = NodeInfo();  DEBUG_LOG("****** %s [%s:%d] response", NodeInfo(), GetIp(), GetPort());
	}

	// �Է��ظ�����
	virtual void OnReceiveResponse(NetNodeConnectData *nodeData, AutoEvent &requstEvt, AutoEvent &respEvt);

	virtual void OnOtherNodeRequestConnect(HandConnect connect, GSKEY otherServerNetKey);
	virtual NetNodeConnectData* RequestConnectOtherNode(GSKEY netKey);

	virtual void OnAppendNetNode(NetNodeConnectData *nodeData){}
	NetNodeList& GetNetNodeList(){ return mRequestConnectList; }

public:
	//virtual bool StartNet()
	//{
	//	return mServerNet->StartNet(mServerIp.c_str(), mServerPort);
	//}
	// NOTE: ���ڵ�����������ڵ��ַ, ���������ڵ��쳣�Ͽ���,����������ͬһ���������Ľڵ�
	void SetMainNode(const char *szIP, int port);
	virtual bool StartNet(const char *szIP, int port);
	virtual void StopNet();
	virtual void NotifyClose();

	virtual Logic::tEventCenter* GetServerCenter(){ return mServerNet->GetEventCenter(); }
	virtual Logic::tEventCenter* GetEventCenter(void) const override
	{
		return (Logic::tEventCenter*)mEventCenter.getPtr();
	}

	virtual void SetEventCenter(AutoEventCenter serverCenter, AutoEventCenter connectCenter);

	virtual void RegisterNodeEvent(AutoEventCenter serverCenter, AutoEventCenter connectCenter){}

	virtual void SetSafeCode(int safeCode) { mSafeCheckCode = safeCode; }
	virtual int GetSafeCode() { return mSafeCheckCode; }
	GSKEY GetNetKey() const { return mServerNetKey; }

	// �쳣�Ͽ��󣬳����������Ӵ�����0 �������������ӣ�ֱ���Ƴ� -1 һֱ��������
	virtual int GetTryConnectCount(void) override { return 3; }

	// �����������ʱ��
	virtual int GetConnectOverTime(void) override { return 6; }

public:
	virtual bool Connect(const char *szIp, int nPort, int overmilSecond);

	virtual void OnConnected(tNetConnect *pConnect);
	virtual void OnConnectFail(AutoEvent waitEvent);
	virtual void OnCloseConnect(tNetConnect *pConnect);
	virtual void OnRemoveNode(NetNodeConnectData *pNode){}

	virtual void _OnConnectNodeFault(AutoEvent hConnect);

	virtual bool OnAddConnect(tNetConnect *pConnect) override { return true; }

	virtual void Process();
	// ��Ƶѭ��(���1��һ��)
	virtual void LowProcess(int spaceTime) override
	{
		IOCPBaseNet::LowProcess(spaceTime);
		mServerNet->LowProcess(spaceTime);
	}

	HandConnect GetRequestConnect(GSKEY otherNetKey);
	AutoEvent StartRequestEvent(const char *szEvent, GSKEY targetNetkey);

	NetNodeConnectData* GetNodeConnectData(HandConnect connect);

	virtual void RemoveNode(NetNodeConnectData *pNode);

	virtual AString DumpAllConnect(bool bPrint = false);

public:
	virtual HandConnect CreateConnect();

	virtual void _SendSafeCode( IOCPConnect *pRecConnect );

	virtual const char* GetIp(void) const{ return mServerIp.c_str(); AssertEx(0, "More client"); return "MORE IP"; }
	virtual int	GetPort(void) const { return mServerPort; AssertEx(0, "More client"); return -1; }

	virtual void SetNetProtocol(Auto<tNetProtocol> netProtocol)
	{ 
		IOCPBaseNet::SetNetProtocol(netProtocol);
		Hand<tBaseEventNet> net = mServerNet;
		net->SetNetProtocol(netProtocol);
	}

	virtual void OnReceivePacket(Packet *p) override { ERROR_LOG("No process packet %s", p->GetMsgName()); }

public:
	AutoEventCenter		mEventCenter;
	GSKEY				mServerNetKey;
	GSKEY				mMainNodeKey;

protected:
	AutoNet				mServerNet;			// ���������ڵ��������

	AString				mServerIp;
	int					mServerPort;
	int					mSafeCheckCode;
	bool				mbStop;

	NetNodeList			mRequestConnectList;
};
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
class NodeServerConnect : public IOCPServerConnect
{
public:
	NodeServerConnect()
		: mServerNetKey(0)
		, mNetNodeConnectData(NULL)
	{

	}

	virtual void OnConnectSafeCodeError(const char *szIp, int port, int safeCode) override
	{
		ERROR_LOG(" [%s : %d] connect safe code error [%d], now net code [%d]", szIp, port, safeCode, GetNetHandle()->GetSafeCode());
		TableTool::red();
		PRINT("ERROR: [%s : %d] connect safe code error [%d], now net code [%d]\r\n", szIp, port, safeCode, GetNetHandle()->GetSafeCode());
		TableTool::white();
	}

public:
	GSKEY				mServerNetKey;
	NetNodeConnectData	*mNetNodeConnectData;
};
//-------------------------------------------------------------------------
class NodeServerNet : public IOCPServerNet
{
public:
	NodeServerNet(MeshedNodeNet *pMeshNet, int threadNum)
		: IOCPServerNet(NET_CONNECT_MAX, threadNum)
		, mOwnerMeshNet(pMeshNet)
	{

	}

	~NodeServerNet()
	{
		if (mEventCenter)
			mEventCenter->RemoveAllFactory();
		mEventCenter._free();
	}

public:
	virtual Logic::tEventCenter* GetEventCenter(void) const { return (Logic::tEventCenter*)mEventCenter.getPtr(); }
	virtual int GetSafeCode(){ return mOwnerMeshNet->GetSafeCode(); }

	virtual bool OnAddConnect(tNetConnect *pConnect)
	{
		Hand<IOCPConnect> conn = pConnect;
		int port = 0;
		DEBUG_LOG("[%s: %d]  by connect [%s:%d]", GetIp(), GetPort(), conn->GetRemoteAddr(port).c_str(), port);
		return mOwnerMeshNet->OnAddConnect(pConnect);
	}

	virtual void OnCloseConnect(tNetConnect *pConnect)
	{
		mOwnerMeshNet->OnCloseConnect(pConnect);
	}

	virtual HandConnect CreateConnect(){ return MEM_NEW NodeServerConnect(); }

	virtual void RemoveNode(NetNodeConnectData *pNode);

	virtual void Process() override
	{
		IOCPServerNet::Process();
		if (mEventCenter)
			mEventCenter->ProcessEvent();
	}

	// NodeNet һ�Զ�������磬��֧���Ż�ID����������Ϣ, ����ֱ�ӷ���true
	virtual bool NeedUpdateMsgIndex() const override { return true; }

	virtual void OnReceivePacket(Packet *p) override { mOwnerMeshNet->OnReceivePacket(p); }

public:
	MeshedNodeNet			*mOwnerMeshNet;
	AutoEventCenter			mEventCenter;
};
//-------------------------------------------------------------------------
class Net_Export NodeRequestConnect : public IOCPConnect
{
public:
	NodeRequestConnect()
		: mNetNodeConnectData(NULL)
	{

	}

	GSKEY GetTargetNetKey();

public:
	NetNodeConnectData	*mNetNodeConnectData;
};
//-------------------------------------------------------------------------

#endif //_INCLUDE_MESHEDNODENET_H_