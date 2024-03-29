using HybridCLR;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Threading.Tasks;
using Logic;
using UnityEngine;

using Logic;
using UnityEngine.Networking;

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

// public class CS_RequestTest : RequestPacket
// {
//     public override string MsgName()
//     {
//         return "CS_RequestTest";
//     }
//     public string mInfo
//     {
//         set { mMsgData.set("mInfo", value); }
//         get { return (string)mMsgData.get("mInfo"); }
//     }
// }
// public class MSG_Test : BasePacket
// {
//     public string mTest
//     {
//         set { mMsgData.set("mTest", value); }
//         get { return (string)mMsgData.get("mTest"); }
//     }
// }


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
        

        
    }
}

public class MainStart : MonoBehaviour
{
    public static ActorManager mActorMgr;
    public static Logic.TcpClientNet mNet;
    public static Actor mCurrentActor;
    
    public Actor mActor;
    // Start is called before the first frame update
    void Start()
    {
        DontDestroyOnLoad(gameObject);
        
        EventCenter.Instance = new EventCenter();
        mActorMgr = new ActorManager();
        EventCenter.StaticRegister("ConnectFinishEvent", new DefineFactory<ConnectFinishEvent>());
        
        mNet = new TcpClientNet();
        
        //LuaMain.StartLua();
        
        mNet.mNotifyConnectFinishEvent = EventCenter.Instance.StartEvent("ConnectFinishEvent");
        //mNet.Connect("127.0.0.1", 4001);
        
        mNet.RegisterPacket((int)NET_PACKET_ID.eNotifyHeartBeat, new PingPacket(), null);
        mNet.RegisterPacket((int)NET_PACKET_ID.eNotifyNetEventID, new PingPacket(), null);
        
        mNet.RegisterPacket((int)41, new NG_RequestGateInfo(), RequestFunction);


        mActorMgr.RegisterActor(new DefineActorFactory<TestActor>(1));
        mActor = mActorMgr.CreateActor(1, 111);
        
        //mActor.mFactory.RegisterActorMsg(new ComponentProcessServerRequest<TestComponent, GN_NotifyNodeInfo>(TestComponent.On));
        //EventCenter.WaitAction(TestDestoryActor, 6);
        mActorMgr.RegisterComponent<TestComponent>();
        //EventCenter.StaticRegister("TestComponent", new DefineFactory<TestComponent>());

        mActor.AddComponent("TestComponent");
        
        //await Task.Delay(3);
        //mNet.mNotifyConnectFinishEvent.Do();

        // bool bOk = await mNet.AsyncConnect("127.0.0.1", 4001, 3000);
        // LOG.log($"Connect {bOk.ToString()}");
        // if (bOk)
        //     TestRequestMsg();
        
        mActorMgr.RegisterActor(new DefineActorFactory<LoginActor>((int)EActorType.ClientActor_Login));
        mActor = mActorMgr.CreateActor((int)EActorType.ClientActor_Login, 1);
        
        mActorMgr.RegisterActor(new DefineActorFactory<ClientActor>((int)EActorType.Actor_Client));
    }

    async void TestRequestMsg()
    {
        
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

    void TestNotifyMsg()
    {
        var msg = new SCS_NotifyMsg();
        msg.mID = (byte)CS_MSG_ID.eMsg_ServerClientNotify;
        msg.mMsgName = "MSG_Test";
        var notifyData = new DataBuffer();
        msg.mNotifyMsgData = notifyData;
        msg.mActorID = new UnitID(104, 1);

        var notifyMsg = new MSG_Test();
        notifyMsg.mF = 0.999f;
        notifyMsg.mTest = "**jjjjjjjjjjkkkkkkkk**";
        notifyMsg.mXX = 999999994444444;

        notifyMsg.Write(ref notifyData);

        MainStart.mNet.SendPacket(msg);
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
        if (EventCenter.Instance!=null)
            EventCenter.Instance.Process();
        if (mNet!=null)
            mNet.Process(0.001f);
        if (mActorMgr!=null)
         mActorMgr.Process();
    }

    public static async Task WaitTest()
    {
        await Task.Delay(4000);
    }
    
    //--------------------------------------------------
        
}

public class TestActor : Actor
{

    public override void  Awake()
    {
        Log("TestActor : Awake");

    }

    public override async void Start()
    {
        Log("TestActor : Start");
        
        // var msg = new GN_NotifyNodeInfo();
        // msg.mNodeKey = 8877;
        // var d = await OnRequestMsg("GN_NotifyNodeInfo", msg.mMsgData);
        // d.dump("^^^^^^^^^^^^");
    }
    
    public override void OnDestory()
    {
        Log("TestActor : OnDestory");
    }

    static public async Task<NiceData> On(TestActor actor, GN_NotifyNodeInfo req) 
    {
        //var req = new GN_NotifyNodeInfo();
        //req.mMsgData = reqData;
        LOG.log("888888@@@@@@@@@@@@ " + req.GetType());
        //(actor as TestActor).On(req);
        req.mMsgData.dump("===========");
        MSG_Test t = new MSG_Test();
        t.mTest = "pppp+++9999";
        return t.mMsgData;
    }

    static public void Notify(TestActor actor, SC_ResponseTest req)
    {
        req.mMsgData.dump("---------------**********");
    }

    public override void RegisterMsg()
    {
        RegisterMsg<TestActor, GN_NotifyNodeInfo>(On);
        RegisterMsg<TestActor, SC_ResponseTest>(Notify);
    }

    // public override async Task<NiceData> On<T>(T reqMsg)
    //     //where T:GN_NotifyNodeInfo
    // {
    //     var req = reqMsg as GN_NotifyNodeInfo;
    //     LOG.log("888888@@@@@@@@@@@@ " + req.GetType());
    //     //(actor as TestActor).On(req);
    //     req.mMsgData.dump("===========");
    //     MSG_Test t = new MSG_Test();
    //     t.mInfo = "pppp+++9999";
    //     return t.mMsgData;
    // }
}

public class TestComponent : tComponent
{
    public override void  Awake()
    {
        Log("TestComponent : Awake");

    }

    public override void Start()
    {
        Log("TestComponent : Start");
    }
    
    public override void OnRemove()
    {
        Log("TestComponent : OnRemove");
    }
    
    static public async Task<NiceData> On(TestComponent comp, GN_NotifyNodeInfo req) 
    {
        //var req = new GN_NotifyNodeInfo();
        //req.mMsgData = reqData;
        LOG.log( comp.GetType().Name+ " 6o0O666888888@@@@@@@@@@@@ " + req.GetType());
        //(actor as TestActor).On(req);
        req.mMsgData.dump("===========");
        MSG_Test t = new MSG_Test();
        t.mTest = "~~~~~~~~~~~~~~kkkkkpppp+++9999";
        return t.mMsgData;
    }
    
    static public void Notify(TestComponent actor, SC_ResponseTest req)
    {
        req.mMsgData.dump("444444444&&&&&&&&&&**********");
    }

    public override void RegisterMsg(ActorManager mgr)
    {
        mgr.RegisterMsg<TestComponent, GN_NotifyNodeInfo>(On);
        mgr.RegisterMsg<TestComponent, SC_ResponseTest>(Notify);
    }
    
    

}