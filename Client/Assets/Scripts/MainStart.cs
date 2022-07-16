using System;
using System.Collections;
using System.Collections.Generic;
using System.Net.Sockets;
using System.Threading.Tasks;
using Logic;
using UnityEngine;

using Logic;

public class NG_RequestGateInfo : RequestPacket
{
    public UInt64 mNodeKey
    {
        set { mMsgData.set("mNodeKey", value); }
        get { return (UInt64)mMsgData.get("mNodeKey"); }
    }
}

public class GN_NotifyNodeInfo : BasePacket
{
    public UInt64 mNodeKey
    {
        set { mMsgData.set("mNodeKey", value); }
        get { return (UInt64)mMsgData.get("mNodeKey"); }
    }
}

public class CS_RequestTest : RequestPacket
{
    public string mInfo
    {
        set { mMsgData.set("mInfo", value); }
        get { return (string)mMsgData.get("mInfo"); }
    }
}
public class MSG_Test : BasePacket
{
    public string mInfo
    {
        set { mMsgData.set("mInfo", value); }
        get { return (string)mMsgData.get("mInfo"); }
    }
}


public class ConnectFinishEvent : BaseEvent
{
    public override async void Do()
    {
        LOG.log("Connect ok, then start send test");
        // NG_RequestGateInfo msg = new NG_RequestGateInfo();
        // msg.mID = 30;
        // msg.mRequestID = 77;
        // msg.mNodeKey = 999;
        // //MainStart.mNet.SendPacket(msg);
        // var response = await msg.AsyncRequest(MainStart.mNet);
        // if (response != null)
        //     response.dump("ok=========");
        // else
        // {
        //     LOG.log("Request fail, response null");
        // }

        CS_RequestTest testMsg = new CS_RequestTest();
        testMsg.mInfo = "&&&&& test info *********";
        testMsg.mRequestID = 1111;

        var uid = new UnitID(104, 1);

        UInt64 x = uid;

        var mm = new UnitID(x);
        
        LOG.log($"{uid.dump()}  === {mm.dump()} *** {x.ToString()}");
        
        
        // CS_ClientRequest requestPacket = new CS_ClientRequest();
        // requestPacket.mTargetActorID = 999;
        // requestPacket.mRequestMsgName = "CS_RequestTest";
        // requestPacket.mActorMsg = testMsg.mMsgData;
        // var response = await requestPacket.AsyncRequest(MainStart.mNet);
        var response = await MainStart.mNet.AsyncRequest(new UnitID(104, 1), "CS_RequestTest", testMsg, 6000);
        if (response != null)
            response.dump("ok=========");
        else
        {
            LOG.log("Request fail, response null");
        }
    }
}

public class MainStart : MonoBehaviour
{
    public static ActorManager mActorMgr;
    public static Logic.TcpClientNet mNet;

    public Actor mActor;
    // Start is called before the first frame update
    void Start()
    {
        EventCenter.Instance = new EventCenter();
        mActorMgr = new ActorManager();
        EventCenter.StaticRegister("ConnectFinishEvent", new DefineFactory<ConnectFinishEvent>());
        
        mNet = new TcpClientNet();
        
        mNet.mNotifyConnectFinishEvent = EventCenter.Instance.StartEvent("ConnectFinishEvent");
        //mNet.Connect("127.0.0.1", 4001);
        
        mNet.RegisterPacket((int)NET_PACKET_ID.eNotifyHeartBeat, new PingPacket(), null);
        mNet.RegisterPacket((int)NET_PACKET_ID.eNotifyNetEventID, new PingPacket(), null);
        
        mNet.RegisterPacket((int)41, new NG_RequestGateInfo(), RequestFunction);

        //await Task.Delay(3);
        //mNet.mNotifyConnectFinishEvent.Do();

        
        mActorMgr.RegisterActor(new DefineActorFactory<TestActor>(1));

        mActor = mActorMgr.CreateActor(1, 111);

        EventCenter.WaitAction(TestDestoryActor, 6);
    }

    void TestDestoryActor()
    {
        mActor.Destory();
    }

    NiceData RequestFunction(tNetTool netTool, RequestPacket packet)
    {
        packet.mMsgData.dump("kkk: RequestFunction *********");
        GN_NotifyNodeInfo resp = new GN_NotifyNodeInfo();
        resp.mNodeKey = 5555;
        return resp.mMsgData;
    }

    // Update is called once per frame
    void Update()
    {
        EventCenter.Instance.Process();
        mNet.Process(0.001f);
        mActorMgr.Process();
    }
}

public class TestActor : Actor
{

    public override void  Awake()
    {
        Log("TestActor : Awake");

    }

    public override void Start()
    {
        Log("TestActor : Start");
    }
    
    public override void OnDestory()
    {
        Log("TestActor : OnDestory");
    }
}