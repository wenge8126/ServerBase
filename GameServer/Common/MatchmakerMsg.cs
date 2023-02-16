//Auto genereate msg data code		
using System;
using System.Collections;
using System.Collections.Generic;
using Logic;


//  请求一个公用TOKEN,用于开放数据
public class CS_GetPublicToken : BasePacket
{


    public CS_GetPublicToken() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
    }

    public override void InitData() 
    {
    }

    public override string MsgName()   { return "CS_GetPublicToken"; }

};

public class CS_ResponsePublicToken : BasePacket
{
    public int mError		
    {
        set { mMsgData.set("mError",  value); }
        get { return (int)mMsgData.getObject("mError"); }
    }

    public DataBuffer mToken		
    {
        set { mMsgData.set("mToken",  value); }
        get { return mMsgData.getObject("mToken") as DataBuffer; }
    }



    public CS_ResponsePublicToken() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mError", scrData.getObject("mError"));
        mMsgData.set("mToken", scrData.getObject("mToken"));
    }

    public override void InitData() 
    {
        mError = 0;
        if (mToken!=null) mToken.clear(true);
    }

    public override string MsgName()   { return "CS_ResponsePublicToken"; }

};

//  请求发送验证码到手机
public class CS_RequestSendCheckCode : BasePacket
{
    public Int64 mPhone		
    {
        set { mMsgData.set("mPhone",  value); }
        get { return (Int64)mMsgData.getObject("mPhone"); }
    }



    public CS_RequestSendCheckCode() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mPhone", scrData.getObject("mPhone"));
    }

    public override void InitData() 
    {
        mPhone = 0;
    }

    public override string MsgName()   { return "CS_RequestSendCheckCode"; }

};

public class CS_ResponseSendCheckCode : BasePacket
{
    public int mError		
    {
        set { mMsgData.set("mError",  value); }
        get { return (int)mMsgData.getObject("mError"); }
    }



    public CS_ResponseSendCheckCode() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mError", scrData.getObject("mError"));
    }

    public override void InitData() 
    {
        mError = 0;
    }

    public override string MsgName()   { return "CS_ResponseSendCheckCode"; }

};

//  请求创建用户
public class CS_ReqCreateUser : BasePacket
{
    public string mCheckCode		
    {
        set { mMsgData.set("mCheckCode",  value); }
        get { return (string)mMsgData.getObject("mCheckCode"); }
    }

    public Int64 mPhone		
    {
        set { mMsgData.set("mPhone",  value); }
        get { return (Int64)mMsgData.getObject("mPhone"); }
    }

    public bool mbCheckManager		//  是否检查为管理员
    {
        set { mMsgData.set("mbCheckManager",  value); }
        get { return (bool)mMsgData.getObject("mbCheckManager"); }
    }



    public CS_ReqCreateUser() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mCheckCode", scrData.getObject("mCheckCode"));
        mMsgData.set("mPhone", scrData.getObject("mPhone"));
        mMsgData.set("mbCheckManager", scrData.getObject("mbCheckManager"));
    }

    public override void InitData() 
    {
        mCheckCode = "";
        mPhone = 0;
        mbCheckManager = false;
    }

    public override string MsgName()   { return "CS_ReqCreateUser"; }

};

public class CS_RespCreateUserResult : BasePacket
{
    public int mError		
    {
        set { mMsgData.set("mError",  value); }
        get { return (int)mMsgData.getObject("mError"); }
    }

    public DataBuffer mToken		//  Token数据
    {
        set { mMsgData.set("mToken",  value); }
        get { return mMsgData.getObject("mToken") as DataBuffer; }
    }

    public Int64 mUserID		
    {
        set { mMsgData.set("mUserID",  value); }
        get { return (Int64)mMsgData.getObject("mUserID"); }
    }

    public bool mbNewUser		//  是否为新用户
    {
        set { mMsgData.set("mbNewUser",  value); }
        get { return (bool)mMsgData.getObject("mbNewUser"); }
    }



    public CS_RespCreateUserResult() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mError", scrData.getObject("mError"));
        mMsgData.set("mToken", scrData.getObject("mToken"));
        mMsgData.set("mUserID", scrData.getObject("mUserID"));
        mMsgData.set("mbNewUser", scrData.getObject("mbNewUser"));
    }

    public override void InitData() 
    {
        mError = 0;
        if (mToken!=null) mToken.clear(true);
        mUserID = 0;
        mbNewUser = false;
    }

    public override string MsgName()   { return "CS_RespCreateUserResult"; }

};

//  应用保存推荐人手机号
public class CS_ApplySaveTJPhone : BasePacket
{
    public Int64 mDBID		//  自己的ID
    {
        set { mMsgData.set("mDBID",  value); }
        get { return (Int64)mMsgData.getObject("mDBID"); }
    }

    public Int64 mPhone		//  推荐人的手机号码
    {
        set { mMsgData.set("mPhone",  value); }
        get { return (Int64)mMsgData.getObject("mPhone"); }
    }



    public CS_ApplySaveTJPhone() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mDBID", scrData.getObject("mDBID"));
        mMsgData.set("mPhone", scrData.getObject("mPhone"));
    }

    public override void InitData() 
    {
        mDBID = 0;
        mPhone = 0;
    }

    public override string MsgName()   { return "CS_ApplySaveTJPhone"; }

};

public class CS_ResponseApplyTJPhone : BasePacket
{
    public int mError		
    {
        set { mMsgData.set("mError",  value); }
        get { return (int)mMsgData.getObject("mError"); }
    }

    public int mRewardRMB		//  奖奖励金额分
    {
        set { mMsgData.set("mRewardRMB",  value); }
        get { return (int)mMsgData.getObject("mRewardRMB"); }
    }



    public CS_ResponseApplyTJPhone() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mError", scrData.getObject("mError"));
        mMsgData.set("mRewardRMB", scrData.getObject("mRewardRMB"));
    }

    public override void InitData() 
    {
        mError = 0;
        mRewardRMB = 0;
    }

    public override string MsgName()   { return "CS_ResponseApplyTJPhone"; }

};

//  请求获取用户ID
public class CS_ReqUserID : BasePacket
{
    public string mCheckCode		
    {
        set { mMsgData.set("mCheckCode",  value); }
        get { return (string)mMsgData.getObject("mCheckCode"); }
    }

    public Int64 mPhone		
    {
        set { mMsgData.set("mPhone",  value); }
        get { return (Int64)mMsgData.getObject("mPhone"); }
    }



    public CS_ReqUserID() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mCheckCode", scrData.getObject("mCheckCode"));
        mMsgData.set("mPhone", scrData.getObject("mPhone"));
    }

    public override void InitData() 
    {
        mCheckCode = "";
        mPhone = 0;
    }

    public override string MsgName()   { return "CS_ReqUserID"; }

};

public class CS_RespUserID : BasePacket
{
    public int mError		
    {
        set { mMsgData.set("mError",  value); }
        get { return (int)mMsgData.getObject("mError"); }
    }

    public Int64 mUserID		
    {
        set { mMsgData.set("mUserID",  value); }
        get { return (Int64)mMsgData.getObject("mUserID"); }
    }



    public CS_RespUserID() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mError", scrData.getObject("mError"));
        mMsgData.set("mUserID", scrData.getObject("mUserID"));
    }

    public override void InitData() 
    {
        mError = 0;
        mUserID = 0;
    }

    public override string MsgName()   { return "CS_RespUserID"; }

};

//  统一回复结果
public class CS_ResponseResult : BasePacket
{
    public int mError		
    {
        set { mMsgData.set("mError",  value); }
        get { return (int)mMsgData.getObject("mError"); }
    }



    public CS_ResponseResult() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mError", scrData.getObject("mError"));
    }

    public override void InitData() 
    {
        mError = 0;
    }

    public override string MsgName()   { return "CS_ResponseResult"; }

};

//  请求加入到喜欢关注列表
public class CS_RequestAddLike : BasePacket
{
    public Int64 mTargetID		
    {
        set { mMsgData.set("mTargetID",  value); }
        get { return (Int64)mMsgData.getObject("mTargetID"); }
    }



    public CS_RequestAddLike() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mTargetID", scrData.getObject("mTargetID"));
    }

    public override void InitData() 
    {
        mTargetID = 0;
    }

    public override string MsgName()   { return "CS_RequestAddLike"; }

};

//  移除喜欢
public class CS_ReqRemoveLike : BasePacket
{
    public Int64 mTargetID		
    {
        set { mMsgData.set("mTargetID",  value); }
        get { return (Int64)mMsgData.getObject("mTargetID"); }
    }



    public CS_ReqRemoveLike() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mTargetID", scrData.getObject("mTargetID"));
    }

    public override void InitData() 
    {
        mTargetID = 0;
    }

    public override string MsgName()   { return "CS_ReqRemoveLike"; }

};

public class CS_RequestGiveGift : BasePacket
{
    public Int64 mDBID		
    {
        set { mMsgData.set("mDBID",  value); }
        get { return (Int64)mMsgData.getObject("mDBID"); }
    }

    public int mGiftCount		
    {
        set { mMsgData.set("mGiftCount",  value); }
        get { return (int)mMsgData.getObject("mGiftCount"); }
    }

    public int mGiftType		
    {
        set { mMsgData.set("mGiftType",  value); }
        get { return (int)mMsgData.getObject("mGiftType"); }
    }

    public Int64 mTargetID		
    {
        set { mMsgData.set("mTargetID",  value); }
        get { return (Int64)mMsgData.getObject("mTargetID"); }
    }



    public CS_RequestGiveGift() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mDBID", scrData.getObject("mDBID"));
        mMsgData.set("mGiftCount", scrData.getObject("mGiftCount"));
        mMsgData.set("mGiftType", scrData.getObject("mGiftType"));
        mMsgData.set("mTargetID", scrData.getObject("mTargetID"));
    }

    public override void InitData() 
    {
        mDBID = 0;
        mGiftCount = 0;
        mGiftType = 0;
        mTargetID = 0;
    }

    public override string MsgName()   { return "CS_RequestGiveGift"; }

};

//  请求更新保存展示数据
public class CS_ReqSaveShowData : BasePacket
{
    public Int64 mDBID		
    {
        set { mMsgData.set("mDBID",  value); }
        get { return (Int64)mMsgData.getObject("mDBID"); }
    }

    public NiceData mShowData		
    {
        set { mMsgData.set("mShowData",  value); }
        get { return mMsgData.getObject("mShowData") as NiceData; }
    }

    public bool mbNews		
    {
        set { mMsgData.set("mbNews",  value); }
        get { return (bool)mMsgData.getObject("mbNews"); }
    }



    public CS_ReqSaveShowData() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mDBID", scrData.getObject("mDBID"));
        mMsgData.set("mShowData", scrData.getObject("mShowData"));
        mMsgData.set("mbNews", scrData.getObject("mbNews"));
    }

    public override void InitData() 
    {
        mDBID = 0;
        if (mShowData!=null) mShowData.clear();
        mbNews = false;
    }

    public override string MsgName()   { return "CS_ReqSaveShowData"; }

};

//  请求获取展示数据
public class CS_ReqLoadShowData : BasePacket
{
    public Int64 mDBID		
    {
        set { mMsgData.set("mDBID",  value); }
        get { return (Int64)mMsgData.getObject("mDBID"); }
    }



    public CS_ReqLoadShowData() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mDBID", scrData.getObject("mDBID"));
    }

    public override void InitData() 
    {
        mDBID = 0;
    }

    public override string MsgName()   { return "CS_ReqLoadShowData"; }

};

//  统一回复获取数据
public class CS_RespLoadData : BasePacket
{
    public NiceData mData		
    {
        set { mMsgData.set("mData",  value); }
        get { return mMsgData.getObject("mData") as NiceData; }
    }

    public int mError		
    {
        set { mMsgData.set("mError",  value); }
        get { return (int)mMsgData.getObject("mError"); }
    }



    public CS_RespLoadData() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mData", scrData.getObject("mData"));
        mMsgData.set("mError", scrData.getObject("mError"));
    }

    public override void InitData() 
    {
        if (mData!=null) mData.clear();
        mError = 0;
    }

    public override string MsgName()   { return "CS_RespLoadData"; }

};

//  请求更新保存个人数据
public class CS_ReqSaveUserData : BasePacket
{
    public Int64 mDBID		
    {
        set { mMsgData.set("mDBID",  value); }
        get { return (Int64)mMsgData.getObject("mDBID"); }
    }

    public NiceData mUserData		
    {
        set { mMsgData.set("mUserData",  value); }
        get { return mMsgData.getObject("mUserData") as NiceData; }
    }



    public CS_ReqSaveUserData() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mDBID", scrData.getObject("mDBID"));
        mMsgData.set("mUserData", scrData.getObject("mUserData"));
    }

    public override void InitData() 
    {
        mDBID = 0;
        if (mUserData!=null) mUserData.clear();
    }

    public override string MsgName()   { return "CS_ReqSaveUserData"; }

};

//  请求获取展示数据
public class CS_ReqLoadUserData : BasePacket
{
    public Int64 mDBID		
    {
        set { mMsgData.set("mDBID",  value); }
        get { return (Int64)mMsgData.getObject("mDBID"); }
    }



    public CS_ReqLoadUserData() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mDBID", scrData.getObject("mDBID"));
    }

    public override void InitData() 
    {
        mDBID = 0;
    }

    public override string MsgName()   { return "CS_ReqLoadUserData"; }

};

//  发送聊天消息
public class CS_SendTalkMsg : BasePacket
{
    public string mMsgText		
    {
        set { mMsgData.set("mMsgText",  value); }
        get { return (string)mMsgData.getObject("mMsgText"); }
    }

    public Int64 mSender		
    {
        set { mMsgData.set("mSender",  value); }
        get { return (Int64)mMsgData.getObject("mSender"); }
    }

    public Int64 mTargetID		
    {
        set { mMsgData.set("mTargetID",  value); }
        get { return (Int64)mMsgData.getObject("mTargetID"); }
    }



    public CS_SendTalkMsg() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mMsgText", scrData.getObject("mMsgText"));
        mMsgData.set("mSender", scrData.getObject("mSender"));
        mMsgData.set("mTargetID", scrData.getObject("mTargetID"));
    }

    public override void InitData() 
    {
        mMsgText = "";
        mSender = 0;
        mTargetID = 0;
    }

    public override string MsgName()   { return "CS_SendTalkMsg"; }

};

//  获取发向目标者的消息
public class CS_GetSendToTargetMsg : BasePacket
{
    public Int64 mSender		
    {
        set { mMsgData.set("mSender",  value); }
        get { return (Int64)mMsgData.getObject("mSender"); }
    }

    public Int64 mTarget		
    {
        set { mMsgData.set("mTarget",  value); }
        get { return (Int64)mMsgData.getObject("mTarget"); }
    }



    public CS_GetSendToTargetMsg() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mSender", scrData.getObject("mSender"));
        mMsgData.set("mTarget", scrData.getObject("mTarget"));
    }

    public override void InitData() 
    {
        mSender = 0;
        mTarget = 0;
    }

    public override string MsgName()   { return "CS_GetSendToTargetMsg"; }

};

public class CS_ResponseSendToTargetMsg : BasePacket
{
    public int mError		
    {
        set { mMsgData.set("mError",  value); }
        get { return (int)mMsgData.getObject("mError"); }
    }

    public NiceData mMsgList		
    {
        set { mMsgData.set("mMsgList",  value); }
        get { return mMsgData.getObject("mMsgList") as NiceData; }
    }



    public CS_ResponseSendToTargetMsg() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mError", scrData.getObject("mError"));
        mMsgData.set("mMsgList", scrData.getObject("mMsgList"));
    }

    public override void InitData() 
    {
        mError = 0;
        if (mMsgList!=null) mMsgList.clear();
    }

    public override string MsgName()   { return "CS_ResponseSendToTargetMsg"; }

};

//  清楚新消息标识
public class CS_ClearNewTalkMsgFlag : BasePacket
{
    public Int64 mDBID		
    {
        set { mMsgData.set("mDBID",  value); }
        get { return (Int64)mMsgData.getObject("mDBID"); }
    }

    public Int64 mSenderID		
    {
        set { mMsgData.set("mSenderID",  value); }
        get { return (Int64)mMsgData.getObject("mSenderID"); }
    }



    public CS_ClearNewTalkMsgFlag() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mDBID", scrData.getObject("mDBID"));
        mMsgData.set("mSenderID", scrData.getObject("mSenderID"));
    }

    public override void InitData() 
    {
        mDBID = 0;
        mSenderID = 0;
    }

    public override string MsgName()   { return "CS_ClearNewTalkMsgFlag"; }

};

//  加载需要检验的用户信息
public class MS_LoadCheckUserInfo : BasePacket
{
    public Int64 mDBID		
    {
        set { mMsgData.set("mDBID",  value); }
        get { return (Int64)mMsgData.getObject("mDBID"); }
    }

    public Int64 mLastPhone		
    {
        set { mMsgData.set("mLastPhone",  value); }
        get { return (Int64)mMsgData.getObject("mLastPhone"); }
    }

    public bool mbOnlyCheck		//  只调取需要检验的用户
    {
        set { mMsgData.set("mbOnlyCheck",  value); }
        get { return (bool)mMsgData.getObject("mbOnlyCheck"); }
    }



    public MS_LoadCheckUserInfo() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mDBID", scrData.getObject("mDBID"));
        mMsgData.set("mLastPhone", scrData.getObject("mLastPhone"));
        mMsgData.set("mbOnlyCheck", scrData.getObject("mbOnlyCheck"));
    }

    public override void InitData() 
    {
        mDBID = 0;
        mLastPhone = 0;
        mbOnlyCheck = false;
    }

    public override string MsgName()   { return "MS_LoadCheckUserInfo"; }

};

public class MS_ResponseCheckUserInfo : BasePacket
{
    public int mError		
    {
        set { mMsgData.set("mError",  value); }
        get { return (int)mMsgData.getObject("mError"); }
    }

    public int mState		
    {
        set { mMsgData.set("mState",  value); }
        get { return (int)mMsgData.getObject("mState"); }
    }

    public Int64 mUserID		
    {
        set { mMsgData.set("mUserID",  value); }
        get { return (Int64)mMsgData.getObject("mUserID"); }
    }

    public Int64 mUserPhone		
    {
        set { mMsgData.set("mUserPhone",  value); }
        get { return (Int64)mMsgData.getObject("mUserPhone"); }
    }



    public MS_ResponseCheckUserInfo() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mError", scrData.getObject("mError"));
        mMsgData.set("mState", scrData.getObject("mState"));
        mMsgData.set("mUserID", scrData.getObject("mUserID"));
        mMsgData.set("mUserPhone", scrData.getObject("mUserPhone"));
    }

    public override void InitData() 
    {
        mError = 0;
        mState = 0;
        mUserID = 0;
        mUserPhone = 0;
    }

    public override string MsgName()   { return "MS_ResponseCheckUserInfo"; }

};

//  修改用户状态
public class MS_SetUserCheckState : BasePacket
{
    public Int64 mDBID		
    {
        set { mMsgData.set("mDBID",  value); }
        get { return (Int64)mMsgData.getObject("mDBID"); }
    }

    public int mState		
    {
        set { mMsgData.set("mState",  value); }
        get { return (int)mMsgData.getObject("mState"); }
    }

    public Int64 mUserID		
    {
        set { mMsgData.set("mUserID",  value); }
        get { return (Int64)mMsgData.getObject("mUserID"); }
    }

    public bool mbAddToMainList		//  是否直接加入展示列表
    {
        set { mMsgData.set("mbAddToMainList",  value); }
        get { return (bool)mMsgData.getObject("mbAddToMainList"); }
    }



    public MS_SetUserCheckState() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mDBID", scrData.getObject("mDBID"));
        mMsgData.set("mState", scrData.getObject("mState"));
        mMsgData.set("mUserID", scrData.getObject("mUserID"));
        mMsgData.set("mbAddToMainList", scrData.getObject("mbAddToMainList"));
    }

    public override void InitData() 
    {
        mDBID = 0;
        mState = 0;
        mUserID = 0;
        mbAddToMainList = false;
    }

    public override string MsgName()   { return "MS_SetUserCheckState"; }

};

//  获取可展示的用户数据
public class MS_FindNormarUserInfo : BasePacket
{
    public Int64 mDBID		
    {
        set { mMsgData.set("mDBID",  value); }
        get { return (Int64)mMsgData.getObject("mDBID"); }
    }

    public Int64 mLastPhone		//  滚动查找
    {
        set { mMsgData.set("mLastPhone",  value); }
        get { return (Int64)mMsgData.getObject("mLastPhone"); }
    }



    public MS_FindNormarUserInfo() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mDBID", scrData.getObject("mDBID"));
        mMsgData.set("mLastPhone", scrData.getObject("mLastPhone"));
    }

    public override void InitData() 
    {
        mDBID = 0;
        mLastPhone = 0;
    }

    public override string MsgName()   { return "MS_FindNormarUserInfo"; }

};

public class MS_ResponseNormarUserInfo : BasePacket
{
    public int mError		
    {
        set { mMsgData.set("mError",  value); }
        get { return (int)mMsgData.getObject("mError"); }
    }

    public Int64 mUserID		
    {
        set { mMsgData.set("mUserID",  value); }
        get { return (Int64)mMsgData.getObject("mUserID"); }
    }

    public Int64 mUserPhone		
    {
        set { mMsgData.set("mUserPhone",  value); }
        get { return (Int64)mMsgData.getObject("mUserPhone"); }
    }



    public MS_ResponseNormarUserInfo() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mError", scrData.getObject("mError"));
        mMsgData.set("mUserID", scrData.getObject("mUserID"));
        mMsgData.set("mUserPhone", scrData.getObject("mUserPhone"));
    }

    public override void InitData() 
    {
        mError = 0;
        mUserID = 0;
        mUserPhone = 0;
    }

    public override string MsgName()   { return "MS_ResponseNormarUserInfo"; }

};

//  增加到展示列表
public class MS_AddToMainList : BasePacket
{
    public Int64 mDBID		
    {
        set { mMsgData.set("mDBID",  value); }
        get { return (Int64)mMsgData.getObject("mDBID"); }
    }

    public Int64 mUserID		
    {
        set { mMsgData.set("mUserID",  value); }
        get { return (Int64)mMsgData.getObject("mUserID"); }
    }



    public MS_AddToMainList() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mDBID", scrData.getObject("mDBID"));
        mMsgData.set("mUserID", scrData.getObject("mUserID"));
    }

    public override void InitData() 
    {
        mDBID = 0;
        mUserID = 0;
    }

    public override string MsgName()   { return "MS_AddToMainList"; }

};

//  从展示列表移除
public class MS_RemoveUserFromMainList : BasePacket
{
    public Int64 mDBID		
    {
        set { mMsgData.set("mDBID",  value); }
        get { return (Int64)mMsgData.getObject("mDBID"); }
    }

    public Int64 mUserID		
    {
        set { mMsgData.set("mUserID",  value); }
        get { return (Int64)mMsgData.getObject("mUserID"); }
    }



    public MS_RemoveUserFromMainList() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mDBID", scrData.getObject("mDBID"));
        mMsgData.set("mUserID", scrData.getObject("mUserID"));
    }

    public override void InitData() 
    {
        mDBID = 0;
        mUserID = 0;
    }

    public override string MsgName()   { return "MS_RemoveUserFromMainList"; }

};

//  拉取所有的展示列表
public class CS_LoadAllMainList : BasePacket
{
    public int mPageCount		
    {
        set { mMsgData.set("mPageCount",  value); }
        get { return (int)mMsgData.getObject("mPageCount"); }
    }

    public int mPageID		
    {
        set { mMsgData.set("mPageID",  value); }
        get { return (int)mMsgData.getObject("mPageID"); }
    }



    public CS_LoadAllMainList() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mPageCount", scrData.getObject("mPageCount"));
        mMsgData.set("mPageID", scrData.getObject("mPageID"));
    }

    public override void InitData() 
    {
        mPageCount = 0;
        mPageID = 0;
    }

    public override string MsgName()   { return "CS_LoadAllMainList"; }

};

public class CS_ResponseMainListData : BasePacket
{
    public int mError		
    {
        set { mMsgData.set("mError",  value); }
        get { return (int)mMsgData.getObject("mError"); }
    }

    ArrayList<Int64> mMainList;	


    public CS_ResponseMainListData() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mError", scrData.getObject("mError"));
        FullArray(scrData, mMainList, "mMainList");
    }

    public override void InitData() 
    {
        mError = 0;
        mMainList.clear(false);
    }

    public override string MsgName()   { return "CS_ResponseMainListData"; }

};

//  移除某个人送礼物的红点
public class CS_RemoveNewGiftFlag : BasePacket
{
    public Int64 mDBID		
    {
        set { mMsgData.set("mDBID",  value); }
        get { return (Int64)mMsgData.getObject("mDBID"); }
    }

    public Int64 mSenderID		
    {
        set { mMsgData.set("mSenderID",  value); }
        get { return (Int64)mMsgData.getObject("mSenderID"); }
    }



    public CS_RemoveNewGiftFlag() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mDBID", scrData.getObject("mDBID"));
        mMsgData.set("mSenderID", scrData.getObject("mSenderID"));
    }

    public override void InitData() 
    {
        mDBID = 0;
        mSenderID = 0;
    }

    public override string MsgName()   { return "CS_RemoveNewGiftFlag"; }

};

public class CS_ResponseRemoveNewGiftFlag : BasePacket
{
    public int mError		
    {
        set { mMsgData.set("mError",  value); }
        get { return (int)mMsgData.getObject("mError"); }
    }

    public NiceData mExData		//  回复当前红点功能数据
    {
        set { mMsgData.set("mExData",  value); }
        get { return mMsgData.getObject("mExData") as NiceData; }
    }



    public CS_ResponseRemoveNewGiftFlag() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mError", scrData.getObject("mError"));
        mMsgData.set("mExData", scrData.getObject("mExData"));
    }

    public override void InitData() 
    {
        mError = 0;
        if (mExData!=null) mExData.clear();
    }

    public override string MsgName()   { return "CS_ResponseRemoveNewGiftFlag"; }

};

//  压缩功能
public class CS_ZipData : BasePacket
{
    public DataBuffer mData		
    {
        set { mMsgData.set("mData",  value); }
        get { return mMsgData.getObject("mData") as DataBuffer; }
    }



    public CS_ZipData() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mData", scrData.getObject("mData"));
    }

    public override void InitData() 
    {
        if (mData!=null) mData.clear(true);
    }

    public override string MsgName()   { return "CS_ZipData"; }

};

public class CS_ResponseZipData : BasePacket
{
    public int mError		
    {
        set { mMsgData.set("mError",  value); }
        get { return (int)mMsgData.getObject("mError"); }
    }

    public DataBuffer mZipData		
    {
        set { mMsgData.set("mZipData",  value); }
        get { return mMsgData.getObject("mZipData") as DataBuffer; }
    }



    public CS_ResponseZipData() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mError", scrData.getObject("mError"));
        mMsgData.set("mZipData", scrData.getObject("mZipData"));
    }

    public override void InitData() 
    {
        mError = 0;
        if (mZipData!=null) mZipData.clear(true);
    }

    public override string MsgName()   { return "CS_ResponseZipData"; }

};

//  上传配置
public class MS_UpdateSaveResource : BasePacket
{
    public string mIndexName		
    {
        set { mMsgData.set("mIndexName",  value); }
        get { return (string)mMsgData.getObject("mIndexName"); }
    }

    public string mMD5		
    {
        set { mMsgData.set("mMD5",  value); }
        get { return (string)mMsgData.getObject("mMD5"); }
    }

    public DataBuffer mResourceData		
    {
        set { mMsgData.set("mResourceData",  value); }
        get { return mMsgData.getObject("mResourceData") as DataBuffer; }
    }



    public MS_UpdateSaveResource() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mIndexName", scrData.getObject("mIndexName"));
        mMsgData.set("mMD5", scrData.getObject("mMD5"));
        mMsgData.set("mResourceData", scrData.getObject("mResourceData"));
    }

    public override void InitData() 
    {
        mIndexName = "";
        mMD5 = "";
        if (mResourceData!=null) mResourceData.clear(true);
    }

    public override string MsgName()   { return "MS_UpdateSaveResource"; }

};

public class MS_ReponseSaveResult : BasePacket
{
    public int mError		
    {
        set { mMsgData.set("mError",  value); }
        get { return (int)mMsgData.getObject("mError"); }
    }

    public string mInfo		
    {
        set { mMsgData.set("mInfo",  value); }
        get { return (string)mMsgData.getObject("mInfo"); }
    }



    public MS_ReponseSaveResult() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mError", scrData.getObject("mError"));
        mMsgData.set("mInfo", scrData.getObject("mInfo"));
    }

    public override void InitData() 
    {
        mError = 0;
        mInfo = "";
    }

    public override string MsgName()   { return "MS_ReponseSaveResult"; }

};

//  上传应用客户端
public class MS_UploadApplyClient : BasePacket
{
    public int mUploadCacheID		
    {
        set { mMsgData.set("mUploadCacheID",  value); }
        get { return (int)mMsgData.getObject("mUploadCacheID"); }
    }

    public int mVersion		
    {
        set { mMsgData.set("mVersion",  value); }
        get { return (int)mMsgData.getObject("mVersion"); }
    }



    public MS_UploadApplyClient() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mUploadCacheID", scrData.getObject("mUploadCacheID"));
        mMsgData.set("mVersion", scrData.getObject("mVersion"));
    }

    public override void InitData() 
    {
        mUploadCacheID = 0;
        mVersion = 0;
    }

    public override string MsgName()   { return "MS_UploadApplyClient"; }

};

public class MS_ResponseUploadClient : BasePacket
{
    public int mError		
    {
        set { mMsgData.set("mError",  value); }
        get { return (int)mMsgData.getObject("mError"); }
    }

    public string mInfo		
    {
        set { mMsgData.set("mInfo",  value); }
        get { return (string)mMsgData.getObject("mInfo"); }
    }



    public MS_ResponseUploadClient() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mError", scrData.getObject("mError"));
        mMsgData.set("mInfo", scrData.getObject("mInfo"));
    }

    public override void InitData() 
    {
        mError = 0;
        mInfo = "";
    }

    public override string MsgName()   { return "MS_ResponseUploadClient"; }

};

//  上传应用客户端
public class MS_UploadApplyManagerClient : BasePacket
{
    public int mUploadCacheID		
    {
        set { mMsgData.set("mUploadCacheID",  value); }
        get { return (int)mMsgData.getObject("mUploadCacheID"); }
    }

    public int mVersion		
    {
        set { mMsgData.set("mVersion",  value); }
        get { return (int)mMsgData.getObject("mVersion"); }
    }



    public MS_UploadApplyManagerClient() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mUploadCacheID", scrData.getObject("mUploadCacheID"));
        mMsgData.set("mVersion", scrData.getObject("mVersion"));
    }

    public override void InitData() 
    {
        mUploadCacheID = 0;
        mVersion = 0;
    }

    public override string MsgName()   { return "MS_UploadApplyManagerClient"; }

};

//  创建DB
public class TS_CreateDBTable : BasePacket
{
    public NiceData mDBConfigData		
    {
        set { mMsgData.set("mDBConfigData",  value); }
        get { return mMsgData.getObject("mDBConfigData") as NiceData; }
    }

    public string mExportCodePath		
    {
        set { mMsgData.set("mExportCodePath",  value); }
        get { return (string)mMsgData.getObject("mExportCodePath"); }
    }



    public TS_CreateDBTable() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mDBConfigData", scrData.getObject("mDBConfigData"));
        mMsgData.set("mExportCodePath", scrData.getObject("mExportCodePath"));
    }

    public override void InitData() 
    {
        if (mDBConfigData!=null) mDBConfigData.clear();
        mExportCodePath = "";
    }

    public override string MsgName()   { return "TS_CreateDBTable"; }

};

public class TS_ResponseCreateDBTable : BasePacket
{
    public int mError		
    {
        set { mMsgData.set("mError",  value); }
        get { return (int)mMsgData.getObject("mError"); }
    }

    public string mResultInfo		
    {
        set { mMsgData.set("mResultInfo",  value); }
        get { return (string)mMsgData.getObject("mResultInfo"); }
    }



    public TS_ResponseCreateDBTable() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mError", scrData.getObject("mError"));
        mMsgData.set("mResultInfo", scrData.getObject("mResultInfo"));
    }

    public override void InitData() 
    {
        mError = 0;
        mResultInfo = "";
    }

    public override string MsgName()   { return "TS_ResponseCreateDBTable"; }

};

//  获取推荐
public class CS_RequestTuiJianShowList : BasePacket
{
    public Int64 mUserID		//  当前用户的DBID, 根据此用户信息获取推荐	
    {
        set { mMsgData.set("mUserID",  value); }
        get { return (Int64)mMsgData.getObject("mUserID"); }
    }



    public CS_RequestTuiJianShowList() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mUserID", scrData.getObject("mUserID"));
    }

    public override void InitData() 
    {
        mUserID = 0;
    }

    public override string MsgName()   { return "CS_RequestTuiJianShowList"; }

};

public class CS_ResponseTuiJainShowList : BasePacket
{
    public int mError		
    {
        set { mMsgData.set("mError",  value); }
        get { return (int)mMsgData.getObject("mError"); }
    }

    ArrayList<Int64> mMainList;	


    public CS_ResponseTuiJainShowList() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mError", scrData.getObject("mError"));
        FullArray(scrData, mMainList, "mMainList");
    }

    public override void InitData() 
    {
        mError = 0;
        mMainList.clear(false);
    }

    public override string MsgName()   { return "CS_ResponseTuiJainShowList"; }

};

//  获取消息
public class CS_LoadMsgList : BasePacket
{
    public Int64 mDBID		
    {
        set { mMsgData.set("mDBID",  value); }
        get { return (Int64)mMsgData.getObject("mDBID"); }
    }

    public int mTalkType		//  0  好友 1 红娘  2 其他
    {
        set { mMsgData.set("mTalkType",  value); }
        get { return (int)mMsgData.getObject("mTalkType"); }
    }



    public CS_LoadMsgList() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mDBID", scrData.getObject("mDBID"));
        mMsgData.set("mTalkType", scrData.getObject("mTalkType"));
    }

    public override void InitData() 
    {
        mDBID = 0;
        mTalkType = 0;
    }

    public override string MsgName()   { return "CS_LoadMsgList"; }

};

public class CS_ResponseMsgList : BasePacket
{
    public int mError		
    {
        set { mMsgData.set("mError",  value); }
        get { return (int)mMsgData.getObject("mError"); }
    }

    public NiceData mMsgData		
    {
        set { mMsgData.set("mMsgData",  value); }
        get { return mMsgData.getObject("mMsgData") as NiceData; }
    }



    public CS_ResponseMsgList() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mError", scrData.getObject("mError"));
        mMsgData.set("mMsgData", scrData.getObject("mMsgData"));
    }

    public override void InitData() 
    {
        mError = 0;
        if (mMsgData!=null) mMsgData.clear();
    }

    public override string MsgName()   { return "CS_ResponseMsgList"; }

};

