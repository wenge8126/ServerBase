#ifndef _INCLUDE_WEBGAMENET_H_
#define _INCLUDE_WEBGAMENET_H_

#include "EventCenter.h"
#include "EventFactory.h"
#include "IOCPServerNet.h"
#include "../WebSocket/WebServerProtocol.h"
#include "NetHead.h"
//#include "DdzRoom.h"

//using namespace Logic;

//-------------------------------------------------------------------------
// IOCP�Ż���, δ���ݴ�WS ���� , ֹͣʹ�� 2020.8.16
//-------------------------------------------------------------------------

class Net_Export WebGameNet : public IOCPServerNet
{
public:
    void RegisterMsg(int eventIndex, Logic::AutoEventFactory factory);

	//??? �ر�ͳһ��������, ���ڴ��Զ�������Ϣ
	virtual void LowProcess(int spaceTime){   }

public:
    WebGameNet();

public:
    virtual void _OnConnectStart(tNetConnect *pConnect) override{}
    virtual void OnMsgRegistered(int eventNameIndex){}
    virtual int GetSafeCode() override { return 0; }

    virtual HandConnect CreateConnect() override;

    Logic::tEventCenter* GetEventCenter( void ) const
    {
        return ((WebGameNet*)(this))->mEventCenter.getPtr();
    }

    // �����Զ�����ID
    virtual bool NeedUpdateMsgIndex() const override { return false; }
    virtual bool NeedMsgEventIndex(const char *msgName, int msgNameIndex) const override { return false; }

public:
    AutoEventCenter mEventCenter;
};

class Net_Export WebPlayerConnect : public WebConnect
{
public:
	WebPlayerConnect(int revBufferLen = 0, int sendBufferLen = 0)
		: WebConnect(revBufferLen, sendBufferLen){}

public:
    virtual void OnReceiveFrame(DataStream *revFrameData, int frameType, bool bLastFrame) override;

    virtual bool SendEvent(Logic::tEvent *pEvent) override;

public:
    DataBuffer  mSendMsgBuffer;
};


#endif //_INCLUDE_WEBGAMENET_H_