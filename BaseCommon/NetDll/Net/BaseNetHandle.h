#ifndef _INCLUDE_BASENETHANDLE_H_
#define _INCLUDE_BASENETHANDLE_H_

#include "NetHead.h"
#include "NetHandle.h"

class Net_Export tBaseNetHandle : public tNetHandle
{
public:
	tBaseNetHandle(tNetProtocol *pProtocol)
		: mpNetProtocol(pProtocol)
	{
		AssertEx(mpNetProtocol!=NULL, "Must ready net protocol");
	}

	virtual ~tBaseNetHandle()
	{
		if (mpNetProtocol==NULL)
			delete mpNetProtocol;
		mpNetProtocol = NULL;
	}

public:
public:
	virtual void SetIndexName(const char* strIndexName) { mIndexName = strIndexName; }
	virtual const char*  GetIndexName(void) const { return mIndexName.c_str(); }

public:
	//virtual bool StartNet(void) = 0;

	// Define over time 10 second.
	virtual int GetConnectOverTime(void) { return 10; } 
	virtual int GetTryConnectCount(void) { return 6; }
	virtual const char* GetConnectFinishEvent(void) { return NULL; }

public:	
	virtual void ProcessReceivePacket(tNetConnect *pConnect, Packet *pPacket) override;
	virtual bool OnReceiveEvent(tNetConnect *revConnect, Logic::tEvent *revEvent);

public:
	//virtual bool Send(Packet *pMsgPacket, tNetConnect *pConnect, ){ return pConnect->Send(pMsgPacket); }
	//virtual bool Send(Logic::tEvent *sendEvent, int target);
	virtual bool Send(Logic::tEvent *sendEvent, tNetConnect *pConnect);

public:
	virtual tNetProtocol* GetNetProtocol(void) { return mpNetProtocol; }


	// ���ͺͽ���, Ĭ�ϲ������κ�ת��, ����Է��ַ�����һ��, ��������������ת��
	virtual AString OnSendStringConvertCode(const char *szSendString){ return AString(); }
	virtual AString OnReceiveStringConverCode(const char *szReceiveString){		return AString(); }

	// ����������Ҫ��������������ӹ���
	virtual bool OnAddConnect(tNetConnect *pConnect){ return true; }

	virtual void Log(const char *szInfo, ...);

protected:
	AString			mIndexName;
	tNetProtocol		*mpNetProtocol;
};


#endif //_INCLUDE_BASENETHANDLE_H_