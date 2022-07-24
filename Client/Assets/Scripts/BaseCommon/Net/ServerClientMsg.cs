//Auto genereate msg data code		
using System;
using System.Collections;
using System.Collections.Generic;
using Logic;


public class CS_ClientRequest : RequestPacket
{
    public int mRequestID		
    {
        set { mMsgData.set("mRequestID",  value); }
        get { return (int)mMsgData.getObject("mRequestID"); }
    }

    public DataBuffer mRequestMsgData		
    {
        set { mMsgData.set("mRequestMsgData",  value); }
        get { return mMsgData.getObject("mRequestMsgData") as DataBuffer; }
    }

    public string mRequestMsgName		
    {
        set { mMsgData.set("mRequestMsgName",  value); }
        get { return (string)mMsgData.getObject("mRequestMsgName"); }
    }

    public UInt64 mTargetActorID		
    {
        set { mMsgData.set("mTargetActorID",  value); }
        get { return (UInt64)mMsgData.getObject("mTargetActorID"); }
    }



    public CS_ClientRequest() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mRequestID", scrData.getObject("mRequestID"));
        mMsgData.set("mRequestMsgData", scrData.getObject("mRequestMsgData"));
        mMsgData.set("mRequestMsgName", scrData.getObject("mRequestMsgName"));
        mMsgData.set("mTargetActorID", scrData.getObject("mTargetActorID"));
    }

    public override void InitData() 
    {
        mRequestID = 0;
        if (mRequestMsgData!=null) mRequestMsgData.clear(true);
        mRequestMsgName = "";
        mTargetActorID = 0;
    }

    public override string MsgName()   { return "CS_ClientRequest"; }

};

//  服务器内部Actor请求客户端消息发向ClientActor, 再通过ClientActor 请求客户端SC_ServerRequestClientMsg
public class SC_ActorRequestClientMsg : BasePacket
{
    public UInt64 mClientActorID		
    {
        set { mMsgData.set("mClientActorID",  value); }
        get { return (UInt64)mMsgData.getObject("mClientActorID"); }
    }

    public DataBuffer mRequestMsgData		
    {
        set { mMsgData.set("mRequestMsgData",  value); }
        get { return mMsgData.getObject("mRequestMsgData") as DataBuffer; }
    }

    public string mRequestMsgName		
    {
        set { mMsgData.set("mRequestMsgName",  value); }
        get { return (string)mMsgData.getObject("mRequestMsgName"); }
    }



    public SC_ActorRequestClientMsg() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mClientActorID", scrData.getObject("mClientActorID"));
        mMsgData.set("mRequestMsgData", scrData.getObject("mRequestMsgData"));
        mMsgData.set("mRequestMsgName", scrData.getObject("mRequestMsgName"));
    }

    public override void InitData() 
    {
        mClientActorID = 0;
        if (mRequestMsgData!=null) mRequestMsgData.clear(true);
        mRequestMsgName = "";
    }

    public override string MsgName()   { return "SC_ActorRequestClientMsg"; }

};

//  服务器与客户端Actor间互传通知消息
public class SCS_NotifyMsg : BasePacket
{
    public UInt64 mActorID		//  服务器或客户端ActorID
    {
        set { mMsgData.set("mActorID",  value); }
        get { return (UInt64)mMsgData.getObject("mActorID"); }
    }

    public string mMsgName		
    {
        set { mMsgData.set("mMsgName",  value); }
        get { return (string)mMsgData.getObject("mMsgName"); }
    }

    public DataBuffer mNotifyMsgData		
    {
        set { mMsgData.set("mNotifyMsgData",  value); }
        get { return mMsgData.getObject("mNotifyMsgData") as DataBuffer; }
    }



    public SCS_NotifyMsg() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mActorID", scrData.getObject("mActorID"));
        mMsgData.set("mMsgName", scrData.getObject("mMsgName"));
        mMsgData.set("mNotifyMsgData", scrData.getObject("mNotifyMsgData"));
    }

    public override void InitData() 
    {
        mActorID = 0;
        mMsgName = "";
        if (mNotifyMsgData!=null) mNotifyMsgData.clear(true);
    }

    public override string MsgName()   { return "SCS_NotifyMsg"; }

};

public class CS_ResponceServerActorMsg : BasePacket
{
    public DataBuffer mResponseMsgData		
    {
        set { mMsgData.set("mResponseMsgData",  value); }
        get { return mMsgData.getObject("mResponseMsgData") as DataBuffer; }
    }



    public CS_ResponceServerActorMsg() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mResponseMsgData", scrData.getObject("mResponseMsgData"));
    }

    public override void InitData() 
    {
        if (mResponseMsgData!=null) mResponseMsgData.clear(true);
    }

    public override string MsgName()   { return "CS_ResponceServerActorMsg"; }

};

public class SC_ResponseMsg : BasePacket
{
    public NiceData mResponseData		
    {
        set { mMsgData.set("mResponseData",  value); }
        get { return mMsgData.getObject("mResponseData") as NiceData; }
    }



    public SC_ResponseMsg() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mResponseData", scrData.getObject("mResponseData"));
    }

    public override void InitData() 
    {
        if (mResponseData!=null) mResponseData.clear();
    }

    public override string MsgName()   { return "SC_ResponseMsg"; }

};

public class SC_ResponseTest : BasePacket
{
    public string mInfo		
    {
        set { mMsgData.set("mInfo",  value); }
        get { return (string)mMsgData.getObject("mInfo"); }
    }



    public SC_ResponseTest() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mInfo", scrData.getObject("mInfo"));
    }

    public override void InitData() 
    {
        mInfo = "";
    }

    public override string MsgName()   { return "SC_ResponseTest"; }

};

public class CS_RequestTest : RequestPacket
{
    public string mInfo		
    {
        set { mMsgData.set("mInfo",  value); }
        get { return (string)mMsgData.getObject("mInfo"); }
    }

    public int mRequestID		
    {
        set { mMsgData.set("mRequestID",  value); }
        get { return (int)mMsgData.getObject("mRequestID"); }
    }



    public CS_RequestTest() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mInfo", scrData.getObject("mInfo"));
        mMsgData.set("mRequestID", scrData.getObject("mRequestID"));
    }

    public override void InitData() 
    {
        mInfo = "";
        mRequestID = 0;
    }

    public override string MsgName()   { return "CS_RequestTest"; }

};

public class MSG_Test : BasePacket
{
    public float mF		
    {
        set { mMsgData.set("mF",  value); }
        get { return (float)mMsgData.getObject("mF"); }
    }

    public string mTest		
    {
        set { mMsgData.set("mTest",  value); }
        get { return (string)mMsgData.getObject("mTest"); }
    }

    public int mX		
    {
        set { mMsgData.set("mX",  value); }
        get { return (int)mMsgData.getObject("mX"); }
    }

    public Int64 mXX		
    {
        set { mMsgData.set("mXX",  value); }
        get { return (Int64)mMsgData.getObject("mXX"); }
    }



    public MSG_Test() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mF", scrData.getObject("mF"));
        mMsgData.set("mTest", scrData.getObject("mTest"));
        mMsgData.set("mX", scrData.getObject("mX"));
        mMsgData.set("mXX", scrData.getObject("mXX"));
    }

    public override void InitData() 
    {
        mF = 0;
        mTest = "";
        mX = 0;
        mXX = 0;
    }

    public override string MsgName()   { return "MSG_Test"; }

};

