
#ifndef _INCLUDE_BASEEVENTNET_H_
#define _INCLUDE_BASEEVENTNET_H_

#include "NetHandle.h"
#include "EventProtocol.h"
#include "NetHead.h"
//----------------------------------------------------------------------------------------------


class Net_Export  tBaseEventNet : public tNetHandle
{
public:
	tBaseEventNet(bool bCreateEventProtocol = true);
	virtual ~tBaseEventNet();

public:
	virtual void SetIndexName(const char* strIndexName) { mIndexName = strIndexName; }
	virtual const char*  GetIndexName(void) const { return mIndexName.c_str(); }

public:
	//virtual bool StartNet(void) = 0;
	virtual bool StartNet(const char *szIP, int port){ return false; }

	// Define over time 10 second.
	virtual int GetConnectOverTime(void) { return 10; } 
	virtual int GetTryConnectCount(void) { return 6; }
	virtual const char* GetConnectFinishEvent(void) { return NULL; }

	// ����������Ҫ��������������ӹ���
	virtual bool OnAddConnect(tNetConnect *pConnect){ return true; }
	virtual void OnCloseConnect(tNetConnect *pConnect){}
	virtual bool IsOk(void){return true;}
	virtual bool TryReady(void){ return true;}

public:	
	virtual bool OnReceiveEvent(tNetConnect *revConnect, Logic::tEvent *revEvent);	

public:
	virtual void RegisterEvent(AutoEventCenter hCenter){}
	virtual void BindEventCenter()
	{
		GetEventCenter()->_SetNetTool(0, GetSelf());
	}

public:
	virtual void SetNetProtocol(Auto<tNetProtocol> netProtocol)
	{
		if (mNetProtocol)
			netProtocol->AppendNetPacketFrom(mNetProtocol.getPtr(), false);
		mNetProtocol = netProtocol; 
	}
	virtual tNetProtocol* GetNetProtocol(void) { return mNetProtocol.getPtr(); }

	// ���ͺͽ���, Ĭ�ϲ������κ�ת��, ����Է��ַ�����һ��, ��������������ת��
	virtual AString OnSendStringConvertCode(const char *szSendString){ return AString(); }
	virtual AString OnReceiveStringConverCode(const char *szReceiveString){		return AString(); }	

	virtual void Log(const char *szInfo, ...);

protected:
	AString					mIndexName;
	Auto<tNetProtocol>		mNetProtocol;
};

class Net_Export BaseWebServer : public tBaseEventNet
{
public:
	virtual ~BaseWebServer(){}

public:
	virtual void StartWeb(int port, const AString &keyPem, const AString &certPem, const AString &pass) = 0;
	virtual bool StartNet(const AString &wssServerIP, int serverPort, const AString &keyPemFile = "", const AString &pfxFileName = "", const AString &pfxPassword = "") = 0;
	virtual bool AsyncStart(const AString &wssServerIP, int serverPort, const AString &keyPemFile = "", const AString &pfxFileName = "", const AString &pfxPassword = "") = 0;
	//virtual void OnResponse(const AString &requestData, AString &response, bool bPost, const AString &requestAddress) = 0;
	virtual void RegisterMsg(int eventIndex, Logic::AutoEventFactory factory) = 0;
	virtual void  ReadyConnect(Int64 oldid, Int64 id, tNetConnect *pConnect) = 0;
	virtual HandConnect GetConnect(Int64 playerID) = 0;

	virtual bool SendToClient(Int64 playerID, Logic::tEvent *pEvent) = 0;
	virtual void BroatcastMsg(Logic::tEvent *pMsg, Int64 excludeID) = 0;
	virtual void InitClearAllPlayer() = 0;
	virtual AString GetRunInfo() { return "None"; }
};

class Net_Export BaseWebConnect : public tNetConnect
{
public:
	virtual void InitNetUnit(Int64 dbID) { mDBID = dbID; }
	virtual void ForeClose() { ERROR_LOG("No write code"); }

public:
	Int64 mDBID = 0;
};

#endif //_INCLUDE_BASEEVENTNET_H_