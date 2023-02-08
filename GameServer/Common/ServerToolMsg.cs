//Auto genereate msg data code		
using System;
using System.Collections;
using System.Collections.Generic;
using Logic;


//  用于HttpRequestActorMsg (客户端直接请求访问服务器内部Actor)
public class HttpReqeustActorMsg : BasePacket
{
    public Int64 mActorID		
    {
        set { mMsgData.set("mActorID",  value); }
        get { return (Int64)mMsgData.getObject("mActorID"); }
    }

    public int mActorType		
    {
        set { mMsgData.set("mActorType",  value); }
        get { return (int)mMsgData.getObject("mActorType"); }
    }

    public string mMsgName		
    {
        set { mMsgData.set("mMsgName",  value); }
        get { return (string)mMsgData.getObject("mMsgName"); }
    }

    public DataBuffer mRequestMsgData		
    {
        set { mMsgData.set("mRequestMsgData",  value); }
        get { return mMsgData.getObject("mRequestMsgData") as DataBuffer; }
    }

    public DataBuffer mToken		
    {
        set { mMsgData.set("mToken",  value); }
        get { return mMsgData.getObject("mToken") as DataBuffer; }
    }



    public HttpReqeustActorMsg() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mActorID", scrData.getObject("mActorID"));
        mMsgData.set("mActorType", scrData.getObject("mActorType"));
        mMsgData.set("mMsgName", scrData.getObject("mMsgName"));
        mMsgData.set("mRequestMsgData", scrData.getObject("mRequestMsgData"));
        mMsgData.set("mToken", scrData.getObject("mToken"));
    }

    public override void InitData() 
    {
        mActorID = 0;
        mActorType = 0;
        mMsgName = "";
        if (mRequestMsgData!=null) mRequestMsgData.clear(true);
        if (mToken!=null) mToken.clear(true);
    }

    public override string MsgName()   { return "HttpReqeustActorMsg"; }

};

//  工具更新保存资源 > ResourcesActor
public class TR_UpdateSaveResource : BasePacket
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



    public TR_UpdateSaveResource() { InitData(); }


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

    public override string MsgName()   { return "TR_UpdateSaveResource"; }

};

public class RT_ReponseSaveResult : BasePacket
{
    public int mErrorCode		
    {
        set { mMsgData.set("mErrorCode",  value); }
        get { return (int)mMsgData.getObject("mErrorCode"); }
    }

    public string mInfo		
    {
        set { mMsgData.set("mInfo",  value); }
        get { return (string)mMsgData.getObject("mInfo"); }
    }



    public RT_ReponseSaveResult() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mErrorCode", scrData.getObject("mErrorCode"));
        mMsgData.set("mInfo", scrData.getObject("mInfo"));
    }

    public override void InitData() 
    {
        mErrorCode = 0;
        mInfo = "";
    }

    public override string MsgName()   { return "RT_ReponseSaveResult"; }

};

//  请求获取资源数据
public class CR_LoadResource : BasePacket
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



    public CR_LoadResource() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mIndexName", scrData.getObject("mIndexName"));
        mMsgData.set("mMD5", scrData.getObject("mMD5"));
    }

    public override void InitData() 
    {
        mIndexName = "";
        mMD5 = "";
    }

    public override string MsgName()   { return "CR_LoadResource"; }

};

public class RC_ResponseResource : BasePacket
{
    public int mErrorCode		
    {
        set { mMsgData.set("mErrorCode",  value); }
        get { return (int)mMsgData.getObject("mErrorCode"); }
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



    public RC_ResponseResource() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mErrorCode", scrData.getObject("mErrorCode"));
        mMsgData.set("mMD5", scrData.getObject("mMD5"));
        mMsgData.set("mResourceData", scrData.getObject("mResourceData"));
    }

    public override void InitData() 
    {
        mErrorCode = 0;
        mMD5 = "";
        if (mResourceData!=null) mResourceData.clear(true);
    }

    public override string MsgName()   { return "RC_ResponseResource"; }

};

//  数据更新保存
public class DB_UpdateSaveData : BasePacket
{
    public DataBuffer mData		
    {
        set { mMsgData.set("mData",  value); }
        get { return mMsgData.getObject("mData") as DataBuffer; }
    }

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



    public DB_UpdateSaveData() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mData", scrData.getObject("mData"));
        mMsgData.set("mIndexName", scrData.getObject("mIndexName"));
        mMsgData.set("mMD5", scrData.getObject("mMD5"));
    }

    public override void InitData() 
    {
        if (mData!=null) mData.clear(true);
        mIndexName = "";
        mMD5 = "";
    }

    public override string MsgName()   { return "DB_UpdateSaveData"; }

};

public class DB_ReponseSaveResult : BasePacket
{
    public int mErrorCode		
    {
        set { mMsgData.set("mErrorCode",  value); }
        get { return (int)mMsgData.getObject("mErrorCode"); }
    }

    public string mInfo		
    {
        set { mMsgData.set("mInfo",  value); }
        get { return (string)mMsgData.getObject("mInfo"); }
    }



    public DB_ReponseSaveResult() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mErrorCode", scrData.getObject("mErrorCode"));
        mMsgData.set("mInfo", scrData.getObject("mInfo"));
    }

    public override void InitData() 
    {
        mErrorCode = 0;
        mInfo = "";
    }

    public override string MsgName()   { return "DB_ReponseSaveResult"; }

};

//  请求获取DB数据
public class DB_LoadData : BasePacket
{
    public string mCheckMD5		
    {
        set { mMsgData.set("mCheckMD5",  value); }
        get { return (string)mMsgData.getObject("mCheckMD5"); }
    }

    public string mIndexName		
    {
        set { mMsgData.set("mIndexName",  value); }
        get { return (string)mMsgData.getObject("mIndexName"); }
    }



    public DB_LoadData() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mCheckMD5", scrData.getObject("mCheckMD5"));
        mMsgData.set("mIndexName", scrData.getObject("mIndexName"));
    }

    public override void InitData() 
    {
        mCheckMD5 = "";
        mIndexName = "";
    }

    public override string MsgName()   { return "DB_LoadData"; }

};

public class DB_ResponseData : BasePacket
{
    public DataBuffer mData		
    {
        set { mMsgData.set("mData",  value); }
        get { return mMsgData.getObject("mData") as DataBuffer; }
    }

    public int mErrorCode		
    {
        set { mMsgData.set("mErrorCode",  value); }
        get { return (int)mMsgData.getObject("mErrorCode"); }
    }

    public string mMD5		
    {
        set { mMsgData.set("mMD5",  value); }
        get { return (string)mMsgData.getObject("mMD5"); }
    }



    public DB_ResponseData() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mData", scrData.getObject("mData"));
        mMsgData.set("mErrorCode", scrData.getObject("mErrorCode"));
        mMsgData.set("mMD5", scrData.getObject("mMD5"));
    }

    public override void InitData() 
    {
        if (mData!=null) mData.clear(true);
        mErrorCode = 0;
        mMD5 = "";
    }

    public override string MsgName()   { return "DB_ResponseData"; }

};

//  请求分包上传
public class DB_ReqeustBigDataUpload : BasePacket
{
    public int mPartSize		
    {
        set { mMsgData.set("mPartSize",  value); }
        get { return (int)mMsgData.getObject("mPartSize"); }
    }

    public Int64 mSize		
    {
        set { mMsgData.set("mSize",  value); }
        get { return (Int64)mMsgData.getObject("mSize"); }
    }

    public string mTableName		
    {
        set { mMsgData.set("mTableName",  value); }
        get { return (string)mMsgData.getObject("mTableName"); }
    }



    public DB_ReqeustBigDataUpload() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mPartSize", scrData.getObject("mPartSize"));
        mMsgData.set("mSize", scrData.getObject("mSize"));
        mMsgData.set("mTableName", scrData.getObject("mTableName"));
    }

    public override void InitData() 
    {
        mPartSize = 0;
        mSize = 0;
        mTableName = "";
    }

    public override string MsgName()   { return "DB_ReqeustBigDataUpload"; }

};

//  回复缓存ID
public class DB_ResponseBigDataCacheID : BasePacket
{
    public Int64 mCacheID		
    {
        set { mMsgData.set("mCacheID",  value); }
        get { return (Int64)mMsgData.getObject("mCacheID"); }
    }

    public int mError		
    {
        set { mMsgData.set("mError",  value); }
        get { return (int)mMsgData.getObject("mError"); }
    }



    public DB_ResponseBigDataCacheID() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mCacheID", scrData.getObject("mCacheID"));
        mMsgData.set("mError", scrData.getObject("mError"));
    }

    public override void InitData() 
    {
        mCacheID = 0;
        mError = 0;
    }

    public override string MsgName()   { return "DB_ResponseBigDataCacheID"; }

};

//  分包上传
public class DB_UploadPartData : BasePacket
{
    public Int64 mCacheID		
    {
        set { mMsgData.set("mCacheID",  value); }
        get { return (Int64)mMsgData.getObject("mCacheID"); }
    }

    public DataBuffer mPartData		
    {
        set { mMsgData.set("mPartData",  value); }
        get { return mMsgData.getObject("mPartData") as DataBuffer; }
    }

    public int mPartIndex		
    {
        set { mMsgData.set("mPartIndex",  value); }
        get { return (int)mMsgData.getObject("mPartIndex"); }
    }



    public DB_UploadPartData() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mCacheID", scrData.getObject("mCacheID"));
        mMsgData.set("mPartData", scrData.getObject("mPartData"));
        mMsgData.set("mPartIndex", scrData.getObject("mPartIndex"));
    }

    public override void InitData() 
    {
        mCacheID = 0;
        if (mPartData!=null) mPartData.clear(true);
        mPartIndex = 0;
    }

    public override string MsgName()   { return "DB_UploadPartData"; }

};

public class DB_ResponseUploadPartDataResult : BasePacket
{
    public int mError		
    {
        set { mMsgData.set("mError",  value); }
        get { return (int)mMsgData.getObject("mError"); }
    }



    public DB_ResponseUploadPartDataResult() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mError", scrData.getObject("mError"));
    }

    public override void InitData() 
    {
        mError = 0;
    }

    public override string MsgName()   { return "DB_ResponseUploadPartDataResult"; }

};

//  完成处理
public class DB_UpdateSaveCacheBigData : BasePacket
{
    public Int64 mCacheID		
    {
        set { mMsgData.set("mCacheID",  value); }
        get { return (Int64)mMsgData.getObject("mCacheID"); }
    }

    public string mCheckMD5		
    {
        set { mMsgData.set("mCheckMD5",  value); }
        get { return (string)mMsgData.getObject("mCheckMD5"); }
    }

    public string mKey		//  如果为空, 则自增
    {
        set { mMsgData.set("mKey",  value); }
        get { return (string)mMsgData.getObject("mKey"); }
    }



    public DB_UpdateSaveCacheBigData() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mCacheID", scrData.getObject("mCacheID"));
        mMsgData.set("mCheckMD5", scrData.getObject("mCheckMD5"));
        mMsgData.set("mKey", scrData.getObject("mKey"));
    }

    public override void InitData() 
    {
        mCacheID = 0;
        mCheckMD5 = "";
        mKey = "";
    }

    public override string MsgName()   { return "DB_UpdateSaveCacheBigData"; }

};

//  处理结果
public class DB_ResponseUpdateSaveCacheDataResult : BasePacket
{
    public string mDBKey		
    {
        set { mMsgData.set("mDBKey",  value); }
        get { return (string)mMsgData.getObject("mDBKey"); }
    }

    public int mError		
    {
        set { mMsgData.set("mError",  value); }
        get { return (int)mMsgData.getObject("mError"); }
    }



    public DB_ResponseUpdateSaveCacheDataResult() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mDBKey", scrData.getObject("mDBKey"));
        mMsgData.set("mError", scrData.getObject("mError"));
    }

    public override void InitData() 
    {
        mDBKey = "";
        mError = 0;
    }

    public override string MsgName()   { return "DB_ResponseUpdateSaveCacheDataResult"; }

};

//  请求下载, 回复的MD5可用于检验本地缓存的数据是否有效
public class DB_RequestDownloadData : BasePacket
{
    public string mCheckMD5		
    {
        set { mMsgData.set("mCheckMD5",  value); }
        get { return (string)mMsgData.getObject("mCheckMD5"); }
    }

    public string mKey		
    {
        set { mMsgData.set("mKey",  value); }
        get { return (string)mMsgData.getObject("mKey"); }
    }

    public string mTableName		
    {
        set { mMsgData.set("mTableName",  value); }
        get { return (string)mMsgData.getObject("mTableName"); }
    }



    public DB_RequestDownloadData() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mCheckMD5", scrData.getObject("mCheckMD5"));
        mMsgData.set("mKey", scrData.getObject("mKey"));
        mMsgData.set("mTableName", scrData.getObject("mTableName"));
    }

    public override void InitData() 
    {
        mCheckMD5 = "";
        mKey = "";
        mTableName = "";
    }

    public override string MsgName()   { return "DB_RequestDownloadData"; }

};

//  回复数据信息
public class DB_ResponseDataInfo : BasePacket
{
    public int mError		
    {
        set { mMsgData.set("mError",  value); }
        get { return (int)mMsgData.getObject("mError"); }
    }

    public string mMD5		
    {
        set { mMsgData.set("mMD5",  value); }
        get { return (string)mMsgData.getObject("mMD5"); }
    }

    public Int64 mSize		
    {
        set { mMsgData.set("mSize",  value); }
        get { return (Int64)mMsgData.getObject("mSize"); }
    }



    public DB_ResponseDataInfo() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mError", scrData.getObject("mError"));
        mMsgData.set("mMD5", scrData.getObject("mMD5"));
        mMsgData.set("mSize", scrData.getObject("mSize"));
    }

    public override void InitData() 
    {
        mError = 0;
        mMD5 = "";
        mSize = 0;
    }

    public override string MsgName()   { return "DB_ResponseDataInfo"; }

};

//  分包下载
public class DB_RequestDownloadPartData : BasePacket
{
    public string mKey		
    {
        set { mMsgData.set("mKey",  value); }
        get { return (string)mMsgData.getObject("mKey"); }
    }

    public Int64 mPosition		
    {
        set { mMsgData.set("mPosition",  value); }
        get { return (Int64)mMsgData.getObject("mPosition"); }
    }

    public int mRequestSize		
    {
        set { mMsgData.set("mRequestSize",  value); }
        get { return (int)mMsgData.getObject("mRequestSize"); }
    }

    public string mTableName		
    {
        set { mMsgData.set("mTableName",  value); }
        get { return (string)mMsgData.getObject("mTableName"); }
    }



    public DB_RequestDownloadPartData() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mKey", scrData.getObject("mKey"));
        mMsgData.set("mPosition", scrData.getObject("mPosition"));
        mMsgData.set("mRequestSize", scrData.getObject("mRequestSize"));
        mMsgData.set("mTableName", scrData.getObject("mTableName"));
    }

    public override void InitData() 
    {
        mKey = "";
        mPosition = 0;
        mRequestSize = 0;
        mTableName = "";
    }

    public override string MsgName()   { return "DB_RequestDownloadPartData"; }

};

//  回复分包数据
public class DB_ResponsePartData : BasePacket
{
    public int mError		
    {
        set { mMsgData.set("mError",  value); }
        get { return (int)mMsgData.getObject("mError"); }
    }

    public DataBuffer mPartData		
    {
        set { mMsgData.set("mPartData",  value); }
        get { return mMsgData.getObject("mPartData") as DataBuffer; }
    }



    public DB_ResponsePartData() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mError", scrData.getObject("mError"));
        mMsgData.set("mPartData", scrData.getObject("mPartData"));
    }

    public override void InitData() 
    {
        mError = 0;
        if (mPartData!=null) mPartData.clear(true);
    }

    public override string MsgName()   { return "DB_ResponsePartData"; }

};

//  通用请求获取DB记录
public class DB_RequestLoadRecord : BasePacket
{
    public string mKey		
    {
        set { mMsgData.set("mKey",  value); }
        get { return (string)mMsgData.getObject("mKey"); }
    }

    public string mTableName		
    {
        set { mMsgData.set("mTableName",  value); }
        get { return (string)mMsgData.getObject("mTableName"); }
    }

    public bool mbNeedField		
    {
        set { mMsgData.set("mbNeedField",  value); }
        get { return (bool)mMsgData.getObject("mbNeedField"); }
    }



    public DB_RequestLoadRecord() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mKey", scrData.getObject("mKey"));
        mMsgData.set("mTableName", scrData.getObject("mTableName"));
        mMsgData.set("mbNeedField", scrData.getObject("mbNeedField"));
    }

    public override void InitData() 
    {
        mKey = "";
        mTableName = "";
        mbNeedField = false;
    }

    public override string MsgName()   { return "DB_RequestLoadRecord"; }

};

public class DB_ResponseRecord : BasePacket
{
    public int mError		
    {
        set { mMsgData.set("mError",  value); }
        get { return (int)mMsgData.getObject("mError"); }
    }

    public DataBuffer mFieldData		
    {
        set { mMsgData.set("mFieldData",  value); }
        get { return mMsgData.getObject("mFieldData") as DataBuffer; }
    }

    public DataBuffer mRecordData		
    {
        set { mMsgData.set("mRecordData",  value); }
        get { return mMsgData.getObject("mRecordData") as DataBuffer; }
    }



    public DB_ResponseRecord() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mError", scrData.getObject("mError"));
        mMsgData.set("mFieldData", scrData.getObject("mFieldData"));
        mMsgData.set("mRecordData", scrData.getObject("mRecordData"));
    }

    public override void InitData() 
    {
        mError = 0;
        if (mFieldData!=null) mFieldData.clear(true);
        if (mRecordData!=null) mRecordData.clear(true);
    }

    public override string MsgName()   { return "DB_ResponseRecord"; }

};

//  保存记录对应的字段数据
public class DB_RequestLoadRecordData : BasePacket
{
    ArrayList<AString> mFieldList;	
    public string mKey		
    {
        set { mMsgData.set("mKey",  value); }
        get { return (string)mMsgData.getObject("mKey"); }
    }

    public string mTableName		
    {
        set { mMsgData.set("mTableName",  value); }
        get { return (string)mMsgData.getObject("mTableName"); }
    }



    public DB_RequestLoadRecordData() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        FullArray(scrData, mFieldList, "mFieldList");
        mMsgData.set("mKey", scrData.getObject("mKey"));
        mMsgData.set("mTableName", scrData.getObject("mTableName"));
    }

    public override void InitData() 
    {
        mFieldList.clear(false);
        mKey = "";
        mTableName = "";
    }

    public override string MsgName()   { return "DB_RequestLoadRecordData"; }

};

//  回复对应的字段数据
public class DB_ResponseRecordData : BasePacket
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



    public DB_ResponseRecordData() { InitData(); }


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

    public override string MsgName()   { return "DB_ResponseRecordData"; }

};

//  获取当前所有记录的最大KEY值(查询SQL)
public class DB_LoadMaxKey : BasePacket
{
    public string mTableName		
    {
        set { mMsgData.set("mTableName",  value); }
        get { return (string)mMsgData.getObject("mTableName"); }
    }



    public DB_LoadMaxKey() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mTableName", scrData.getObject("mTableName"));
    }

    public override void InitData() 
    {
        mTableName = "";
    }

    public override string MsgName()   { return "DB_LoadMaxKey"; }

};

public class DB_ResponseMaxkey : BasePacket
{
    public int mError		
    {
        set { mMsgData.set("mError",  value); }
        get { return (int)mMsgData.getObject("mError"); }
    }

    public string mMaxKey		
    {
        set { mMsgData.set("mMaxKey",  value); }
        get { return (string)mMsgData.getObject("mMaxKey"); }
    }



    public DB_ResponseMaxkey() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mError", scrData.getObject("mError"));
        mMsgData.set("mMaxKey", scrData.getObject("mMaxKey"));
    }

    public override void InitData() 
    {
        mError = 0;
        mMaxKey = "";
    }

    public override string MsgName()   { return "DB_ResponseMaxkey"; }

};

//  上传更新保存记录
public class DB_RequestSaveRecord : BasePacket
{
    public string mKey		
    {
        set { mMsgData.set("mKey",  value); }
        get { return (string)mMsgData.getObject("mKey"); }
    }

    public DataBuffer mRecordData		
    {
        set { mMsgData.set("mRecordData",  value); }
        get { return mMsgData.getObject("mRecordData") as DataBuffer; }
    }

    public string mTableName		
    {
        set { mMsgData.set("mTableName",  value); }
        get { return (string)mMsgData.getObject("mTableName"); }
    }

    public bool mbGrowthKey		//  是否自增加Key插入
    {
        set { mMsgData.set("mbGrowthKey",  value); }
        get { return (bool)mMsgData.getObject("mbGrowthKey"); }
    }



    public DB_RequestSaveRecord() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mKey", scrData.getObject("mKey"));
        mMsgData.set("mRecordData", scrData.getObject("mRecordData"));
        mMsgData.set("mTableName", scrData.getObject("mTableName"));
        mMsgData.set("mbGrowthKey", scrData.getObject("mbGrowthKey"));
    }

    public override void InitData() 
    {
        mKey = "";
        if (mRecordData!=null) mRecordData.clear(true);
        mTableName = "";
        mbGrowthKey = false;
    }

    public override string MsgName()   { return "DB_RequestSaveRecord"; }

};

public class DB_ResponseSaveRecord : BasePacket
{
    public string mDBKey		
    {
        set { mMsgData.set("mDBKey",  value); }
        get { return (string)mMsgData.getObject("mDBKey"); }
    }

    public int mError		
    {
        set { mMsgData.set("mError",  value); }
        get { return (int)mMsgData.getObject("mError"); }
    }



    public DB_ResponseSaveRecord() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mDBKey", scrData.getObject("mDBKey"));
        mMsgData.set("mError", scrData.getObject("mError"));
    }

    public override void InitData() 
    {
        mDBKey = "";
        mError = 0;
    }

    public override string MsgName()   { return "DB_ResponseSaveRecord"; }

};

//  上传更新保存记录
public class DB_RequestSaveRecordByNice : BasePacket
{
    public string mKey		
    {
        set { mMsgData.set("mKey",  value); }
        get { return (string)mMsgData.getObject("mKey"); }
    }

    public NiceData mRecordData		
    {
        set { mMsgData.set("mRecordData",  value); }
        get { return mMsgData.getObject("mRecordData") as NiceData; }
    }

    public string mTableName		
    {
        set { mMsgData.set("mTableName",  value); }
        get { return (string)mMsgData.getObject("mTableName"); }
    }

    public bool mbGrowthKey		//  是否自增加Key插入
    {
        set { mMsgData.set("mbGrowthKey",  value); }
        get { return (bool)mMsgData.getObject("mbGrowthKey"); }
    }



    public DB_RequestSaveRecordByNice() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mKey", scrData.getObject("mKey"));
        mMsgData.set("mRecordData", scrData.getObject("mRecordData"));
        mMsgData.set("mTableName", scrData.getObject("mTableName"));
        mMsgData.set("mbGrowthKey", scrData.getObject("mbGrowthKey"));
    }

    public override void InitData() 
    {
        mKey = "";
        if (mRecordData!=null) mRecordData.clear();
        mTableName = "";
        mbGrowthKey = false;
    }

    public override string MsgName()   { return "DB_RequestSaveRecordByNice"; }

};

//  上传外部视频(先上传到缓存中), DB保存到视频网站内 (Python启动http), 由对应目录中的索引文件记录信息(如:最大值)
public class EX_SaveVideoData : BasePacket
{
    public int mCacheID		
    {
        set { mMsgData.set("mCacheID",  value); }
        get { return (int)mMsgData.getObject("mCacheID"); }
    }

    public string mExtName		
    {
        set { mMsgData.set("mExtName",  value); }
        get { return (string)mMsgData.getObject("mExtName"); }
    }

    public Int64 mKey		
    {
        set { mMsgData.set("mKey",  value); }
        get { return (Int64)mMsgData.getObject("mKey"); }
    }

    public string mType		//  分类, 用于子目录名
    {
        set { mMsgData.set("mType",  value); }
        get { return (string)mMsgData.getObject("mType"); }
    }

    public bool mbGrowth		
    {
        set { mMsgData.set("mbGrowth",  value); }
        get { return (bool)mMsgData.getObject("mbGrowth"); }
    }



    public EX_SaveVideoData() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mCacheID", scrData.getObject("mCacheID"));
        mMsgData.set("mExtName", scrData.getObject("mExtName"));
        mMsgData.set("mKey", scrData.getObject("mKey"));
        mMsgData.set("mType", scrData.getObject("mType"));
        mMsgData.set("mbGrowth", scrData.getObject("mbGrowth"));
    }

    public override void InitData() 
    {
        mCacheID = 0;
        mExtName = "";
        mKey = 0;
        mType = "";
        mbGrowth = false;
    }

    public override string MsgName()   { return "EX_SaveVideoData"; }

};

public class EX_ResponseSaveVideo : BasePacket
{
    public int mError		
    {
        set { mMsgData.set("mError",  value); }
        get { return (int)mMsgData.getObject("mError"); }
    }

    public Int64 mIndexKey		
    {
        set { mMsgData.set("mIndexKey",  value); }
        get { return (Int64)mMsgData.getObject("mIndexKey"); }
    }



    public EX_ResponseSaveVideo() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mError", scrData.getObject("mError"));
        mMsgData.set("mIndexKey", scrData.getObject("mIndexKey"));
    }

    public override void InitData() 
    {
        mError = 0;
        mIndexKey = 0;
    }

    public override string MsgName()   { return "EX_ResponseSaveVideo"; }

};

//  请求主地址
public class EX_RequestVideoMainAddress : BasePacket
{


    public EX_RequestVideoMainAddress() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
    }

    public override void InitData() 
    {
    }

    public override string MsgName()   { return "EX_RequestVideoMainAddress"; }

};

public class EX_ResponsetVideoMainAddress : BasePacket
{
    public int mError		
    {
        set { mMsgData.set("mError",  value); }
        get { return (int)mMsgData.getObject("mError"); }
    }

    public string mMainAddress		
    {
        set { mMsgData.set("mMainAddress",  value); }
        get { return (string)mMsgData.getObject("mMainAddress"); }
    }



    public EX_ResponsetVideoMainAddress() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mError", scrData.getObject("mError"));
        mMsgData.set("mMainAddress", scrData.getObject("mMainAddress"));
    }

    public override void InitData() 
    {
        mError = 0;
        mMainAddress = "";
    }

    public override string MsgName()   { return "EX_ResponsetVideoMainAddress"; }

};

//  索引文件结构(NiceData)
public class EX_VideoFileIndex : BasePacket
{
    public NiceData mExData		
    {
        set { mMsgData.set("mExData",  value); }
        get { return mMsgData.getObject("mExData") as NiceData; }
    }

    public Int64 mMaxKey		
    {
        set { mMsgData.set("mMaxKey",  value); }
        get { return (Int64)mMsgData.getObject("mMaxKey"); }
    }

    public string mType		
    {
        set { mMsgData.set("mType",  value); }
        get { return (string)mMsgData.getObject("mType"); }
    }



    public EX_VideoFileIndex() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mExData", scrData.getObject("mExData"));
        mMsgData.set("mMaxKey", scrData.getObject("mMaxKey"));
        mMsgData.set("mType", scrData.getObject("mType"));
    }

    public override void InitData() 
    {
        if (mExData!=null) mExData.clear();
        mMaxKey = 0;
        mType = "";
    }

    public override string MsgName()   { return "EX_VideoFileIndex"; }

};

// -------------------------------------------------------
public class TEST_BigHttpMsg : BasePacket
{
    public DataBuffer mBigData		
    {
        set { mMsgData.set("mBigData",  value); }
        get { return mMsgData.getObject("mBigData") as DataBuffer; }
    }



    public TEST_BigHttpMsg() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mBigData", scrData.getObject("mBigData"));
    }

    public override void InitData() 
    {
        if (mBigData!=null) mBigData.clear(true);
    }

    public override string MsgName()   { return "TEST_BigHttpMsg"; }

};

public class TEST_ResponseBigHttp : BasePacket
{
    public DataBuffer mResponseData		
    {
        set { mMsgData.set("mResponseData",  value); }
        get { return mMsgData.getObject("mResponseData") as DataBuffer; }
    }



    public TEST_ResponseBigHttp() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mResponseData", scrData.getObject("mResponseData"));
    }

    public override void InitData() 
    {
        if (mResponseData!=null) mResponseData.clear(true);
    }

    public override string MsgName()   { return "TEST_ResponseBigHttp"; }

};

//  请求WSS地址
public class EX_ReqeustWssAddress : BasePacket
{


    public EX_ReqeustWssAddress() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
    }

    public override void InitData() 
    {
    }

    public override string MsgName()   { return "EX_ReqeustWssAddress"; }

};

public class EX_ResponseWssAddress : BasePacket
{
    public string mAddress		
    {
        set { mMsgData.set("mAddress",  value); }
        get { return (string)mMsgData.getObject("mAddress"); }
    }

    public bool mbWss		
    {
        set { mMsgData.set("mbWss",  value); }
        get { return (bool)mMsgData.getObject("mbWss"); }
    }



    public EX_ResponseWssAddress() { InitData(); }


   public override  void Full(NiceData scrData) 
    {
        InitData();
        mMsgData.set("mAddress", scrData.getObject("mAddress"));
        mMsgData.set("mbWss", scrData.getObject("mbWss"));
    }

    public override void InitData() 
    {
        mAddress = "";
        mbWss = false;
    }

    public override string MsgName()   { return "EX_ResponseWssAddress"; }

};

