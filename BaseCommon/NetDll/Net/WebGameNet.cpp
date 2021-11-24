#include "WebGameNet.h"
#include "CEvent.h"
#include "ClientEvent.h"


//#include "GameDefine.h"
//----------------------------------------------------------------------
class TestWebMsg : public Logic::tClientEvent
{
public:
    virtual bool _DoEvent() override
    {
        //GetResponseEvent()["KKK"] = "ok";
        //GetResponseEvent()["AAA"] = 8765321;
        //GetResponseEvent()["AAZ"] = 8.12345678f;
        AutoArrayData d = MEM_NEW ArrayNiceData();
        d[0] = 99;
        d[2] = 101;
        d[3] = 220;
        GetResponseEvent()["TEST_ARR"] = d;
        Finish();
        Hand<Logic::tServerEvent> evt = GetEventCenter()->StartEvent(SC_TEST_REQUEST_MSG);
        evt->mNetConnect = mNetConnect;
        evt->Start();
        return false;
    }
};

class TestResponseMsg : public Logic::tServerEvent
{
public:
    virtual bool _DoEvent()
    {
        set("GGG", 9883);
        set("INFO", "TestEgretRequest");
        WaitTime(10);
        return true;
    }

    virtual void _OnResp(AutoEvent &respEvent) override
    {
        NOTE_LOG("Client response >\r\n%s", respEvent->GetData().dump().c_str());
    }
};
//----------------------------------------------------------------------

//-------------------------------------------------------------------------
WebGameNet::WebGameNet()
{    
	AssertNote(0, "IOCP优化后, 未兼容此WS 网络, 存在连接失败, 目前停止使用 2020.8.16");

	SetNetProtocol(MEM_NEW WebNetProtocal());
    mEventCenter = MEM_NEW WebNetEventCenter();
    mEventCenter->_SetNetTool(0, GetSelf());

    //RegisterMsg(CS_TEST_WEB_MSG, MEM_NEW EventFactory<TestWebMsg>());
    //RegisterMsg(SC_TEST_REQUEST_MSG, MEM_NEW EventFactory<TestResponseMsg>());
}

void WebGameNet::RegisterMsg(int eventIndex, Logic::AutoEventFactory factory)
{
    factory->SetID(eventIndex);
    AString name;
    name.Format("WebMsgEvent_%d", eventIndex);
    factory->SetEventName(name.c_str());
    Hand<WebNetEventCenter> webCenter = mEventCenter;
    webCenter->RegisterMsg(factory);
}

HandConnect WebGameNet::CreateConnect()
{
    return MEM_NEW WebPlayerConnect();
}
//----------------------------------------------------------------------


void WebPlayerConnect::OnReceiveFrame(DataStream *revFrameData, int frameType, bool bLastFrame)
{
    if (frameType==WS_TEXT_FRAME)
    {                
        NOTE_LOG("Rev WEB >%s", AString::getANIS(revFrameData->data()).c_str());            
        //SendMsgData(revFrameData, WS_TEXT_FRAME);
    }
    else if (frameType==WS_BINARY_FRAME)
    {
        //static int x = 0;
        //NOTE_LOG(">>> Rev %d %d", ++x, (int)revFrameData->dataSize());
        revFrameData->seek(0);
        AutoEvent evt = GetNetHandle()->GetEventCenter()->RestoreMsg(revFrameData);
        if (evt)
        {
            NOTE_LOG("Rev msg event >%s", evt->GetEventName());
            NOTE_LOG("%s", evt->GetData().dump().c_str());
            evt->_OnBindNet(this);
            evt->DoEvent(true);
        }
        else
            ERROR_LOG("Restore msg event fail >%d", revFrameData->dataSize());

        //AString info = revFrameData->readString();
        ////info.toANIS();
        //NiceData d;
        //d.restore(revFrameData);
        //NOTE_LOG("INFO >%s", info.toANIS());
        //NOTE_LOG("%s", d.dump().c_str());
        //NOTE_LOG("------------------------");
    }
}

bool WebPlayerConnect::SendEvent(Logic::tEvent *pEvent)
{
    mSendMsgBuffer.clear(false);
	AutoEvent evt = pEvent->GetSelf();
    bool b = pEvent->GetEventCenter()->SerializeMsg(evt, &mSendMsgBuffer);
    if (b)
        return SendMsgData(&mSendMsgBuffer, WS_BINARY_FRAME);
    return false;
}
//----------------------------------------------------------------------