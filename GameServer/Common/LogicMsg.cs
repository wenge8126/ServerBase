//Auto genereate msg data code		
using System;
using System.Collections;
using System.Collections.Generic;
using Logic;


//  请求加入到购物车
public class CG_RequestAddCart : BasePacket
{
    public int mCount		
    {
        set { mMsgData.set("mCount",  value); }
        get { return (int)mMsgData.getObject("mCount"); }
    }

    public Int64 mGoodsID		
    {
        set { mMsgData.set("mGoodsID",  value); }
        get { return (Int64)mMsgData.getObject("mGoodsID"); }
    }

    public NiceData mParam		
    {
        set { mMsgData.set("mParam",  value); }
        get { return mMsgData.getObject("mParam") as NiceData; }
    }



    public CG_RequestAddCart() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mCount", scrData.getObject("mCount"));
        mMsgData.set("mGoodsID", scrData.getObject("mGoodsID"));
        mMsgData.set("mParam", scrData.getObject("mParam"));
    }

    public override void InitData() 
    {
        mCount = 0;
        mGoodsID = 0;
        if (mParam!=null) mParam.clear();
    }

    public override string MsgName()   { return "CG_RequestAddCart"; }

};

public class CG_ReponseAddCart : BasePacket
{
    public Int64 mBuyID		
    {
        set { mMsgData.set("mBuyID",  value); }
        get { return (Int64)mMsgData.getObject("mBuyID"); }
    }

    public int mError		
    {
        set { mMsgData.set("mError",  value); }
        get { return (int)mMsgData.getObject("mError"); }
    }



    public CG_ReponseAddCart() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mBuyID", scrData.getObject("mBuyID"));
        mMsgData.set("mError", scrData.getObject("mError"));
    }

    public override void InitData() 
    {
        mBuyID = 0;
        mError = 0;
    }

    public override string MsgName()   { return "CG_ReponseAddCart"; }

};

