using System;
using System.Collections;
using System.Collections.Generic;
using System.Net.Sockets;
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

public class GN_NotifyNodeInfo : NetPacket
{
    public UInt64 mNodeKey
    {
        set { mMsgData.set("mNodeKey", value); }
        get { return (UInt64)mMsgData.get("mNodeKey"); }
    }
}

public class ConnectFinishEvent : BaseEvent
{
    public override async void Do()
    {
        LOG.log("Connect ok, then start send test");
        NG_RequestGateInfo msg = new NG_RequestGateInfo();
        msg.mID = 30;
        msg.mRequestID = 77;
        msg.mNodeKey = 999;
        //MainStart.mNet.SendPacket(msg);
        var response = await msg.AsyncRequest(MainStart.mNet);
        if (response != null)
            response.mMsgData.dump("ok=========");
        else
        {
            LOG.log("Request fail, response null");
        }
    }
}

public class MainStart : MonoBehaviour
{
    public static Logic.TcpClientNet mNet = new TcpClientNet();
    // Start is called before the first frame update
    void Start()
    {
        EventCenter.Instance = new EventCenter();
        EventCenter.StaticRegister("ConnectFinishEvent", new DefineFactory<ConnectFinishEvent>());
        mNet.mNotifyConnectFinishEvent = EventCenter.Instance.StartEvent("ConnectFinishEvent");
        mNet.Connect("127.0.0.1", 4001);
        
        mNet.RegisterPacket((int)NET_PACKET_ID.eNotifyHeartBeat, new PingPacket(), null);
        mNet.RegisterPacket((int)NET_PACKET_ID.eNotifyNetEventID, new PingPacket(), null);
    }



    // Update is called once per frame
    void Update()
    {
        EventCenter.Instance.Process();
        mNet.Process(0.001f);
    }
}
