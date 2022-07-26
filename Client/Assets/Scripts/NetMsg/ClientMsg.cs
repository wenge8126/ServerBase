//Auto genereate msg data code		
using System;
using System.Collections;
using System.Collections.Generic;
using Logic;


public class CS_RequestLogin : RequestPacket
{
    public Int64 mDBID		
    {
        set { mMsgData.set("mDBID",  value); }
        get { return (Int64)mMsgData.getObject("mDBID"); }
    }

    public int mRequestID		
    {
        set { mMsgData.set("mRequestID",  value); }
        get { return (int)mMsgData.getObject("mRequestID"); }
    }

    public string mToken		
    {
        set { mMsgData.set("mToken",  value); }
        get { return (string)mMsgData.getObject("mToken"); }
    }



    public CS_RequestLogin() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mDBID", scrData.getObject("mDBID"));
        mMsgData.set("mRequestID", scrData.getObject("mRequestID"));
        mMsgData.set("mToken", scrData.getObject("mToken"));
    }

    public override void InitData() 
    {
        mDBID = 0;
        mRequestID = 0;
        mToken = "";
    }

    public override string MsgName()   { return "CS_RequestLogin"; }

};

public class SC_ResponseLogin : BasePacket
{
    public int error		
    {
        set { mMsgData.set("error",  value); }
        get { return (int)mMsgData.getObject("error"); }
    }



    public SC_ResponseLogin() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("error", scrData.getObject("error"));
    }

    public override void InitData() 
    {
        error = 0;
    }

    public override string MsgName()   { return "SC_ResponseLogin"; }

};

//  请求游戏数据
public class CG_RequestPlayerData : BasePacket
{


    public CG_RequestPlayerData() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
    }

    public override void InitData() 
    {
    }

    public override string MsgName()   { return "CG_RequestPlayerData"; }

};

//  回复玩家数据
public class GC_ResponsePlayerData : BasePacket
{
    public NiceData mData		
    {
        set { mMsgData.set("mData",  value); }
        get { return mMsgData.getObject("mData") as NiceData; }
    }

    public string mName		
    {
        set { mMsgData.set("mName",  value); }
        get { return (string)mMsgData.getObject("mName"); }
    }



    public GC_ResponsePlayerData() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mData", scrData.getObject("mData"));
        mMsgData.set("mName", scrData.getObject("mName"));
    }

    public override void InitData() 
    {
        if (mData!=null) mData.clear();
        mName = "";
    }

    public override string MsgName()   { return "GC_ResponsePlayerData"; }

};

