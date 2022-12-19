//Auto genereate msg data code		
#pragma once

#include "BaseMsg.h"
#include "Array.h"
#include "ArrayList.h"

//  工具更新保存资源 > ResourcesActor
class TR_UpdateSaveResource : public tBaseMsg
{ 
public:
    AString mIndexName;		
    AString mMD5;		
    AutoData mResourceData;		

public:
    TR_UpdateSaveResource() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mIndexName);
        CheckGet(scrData, mMD5);
        mResourceData = (DataStream*)scrData["mResourceData"];
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mIndexName"] = mIndexName;
        destData["mMD5"] = mMD5;
        destData["mResourceData"] = mResourceData.getPtr();
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)3);

        SAVE_MSG_VALUE(mIndexName, 4);
        SAVE_MSG_VALUE(mMD5, 4);
        SAVE_MSG_DATA(mResourceData);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mIndexName.setNull();
        mMD5.setNull();
        if (mResourceData) mResourceData.setNull();
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("TR_UpdateSaveResource", otherMsg.GetMsgName())!=0) { LOG("%s is not TR_UpdateSaveResource", otherMsg.GetMsgName()); return; }; const TR_UpdateSaveResource &other = *(const TR_UpdateSaveResource*)(&otherMsg);
        mIndexName = other.mIndexName;
        mMD5 = other.mMD5;
        COPY_MSG_DATA(other.mResourceData, mResourceData);
    }

    virtual const char* GetMsgName() const override { return "TR_UpdateSaveResource"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mIndexName")==0) { AData value; value = mIndexName; return value; }
        if (strcmp(szMember, "mMD5")==0) { AData value; value = mMD5; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mIndexName")==0) { mIndexName = value; return true; };
        if (strcmp(szMember, "mMD5")==0) { mMD5 = value; return true; };
        LOG("No exist > %%s", szMember);  return false;
    }

    AData operator [] (const char *szMember) const 
    {
        return get(szMember);
    }

    AData operator [] (const AString &member) const 
    {
        return get(member.c_str());
    }

};

class RT_ReponseSaveResult : public tBaseMsg
{ 
public:
    int mErrorCode;		
    AString mInfo;		

public:
    RT_ReponseSaveResult() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mErrorCode);
        CheckGet(scrData, mInfo);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mErrorCode"] = mErrorCode;
        destData["mInfo"] = mInfo;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)2);

        SAVE_MSG_VALUE(mErrorCode, 1);
        SAVE_MSG_VALUE(mInfo, 4);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mErrorCode = 0;
        mInfo.setNull();
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("RT_ReponseSaveResult", otherMsg.GetMsgName())!=0) { LOG("%s is not RT_ReponseSaveResult", otherMsg.GetMsgName()); return; }; const RT_ReponseSaveResult &other = *(const RT_ReponseSaveResult*)(&otherMsg);
        mErrorCode = other.mErrorCode;
        mInfo = other.mInfo;
    }

    virtual const char* GetMsgName() const override { return "RT_ReponseSaveResult"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mErrorCode")==0) { AData value; value = mErrorCode; return value; }
        if (strcmp(szMember, "mInfo")==0) { AData value; value = mInfo; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mErrorCode")==0) { mErrorCode = value; return true; };
        if (strcmp(szMember, "mInfo")==0) { mInfo = value; return true; };
        LOG("No exist > %%s", szMember);  return false;
    }

    AData operator [] (const char *szMember) const 
    {
        return get(szMember);
    }

    AData operator [] (const AString &member) const 
    {
        return get(member.c_str());
    }

};

//  请求获取资源数据
class CR_LoadResource : public tBaseMsg
{ 
public:
    AString mIndexName;		
    AString mMD5;		

public:
    CR_LoadResource() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mIndexName);
        CheckGet(scrData, mMD5);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mIndexName"] = mIndexName;
        destData["mMD5"] = mMD5;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)2);

        SAVE_MSG_VALUE(mIndexName, 4);
        SAVE_MSG_VALUE(mMD5, 4);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mIndexName.setNull();
        mMD5.setNull();
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("CR_LoadResource", otherMsg.GetMsgName())!=0) { LOG("%s is not CR_LoadResource", otherMsg.GetMsgName()); return; }; const CR_LoadResource &other = *(const CR_LoadResource*)(&otherMsg);
        mIndexName = other.mIndexName;
        mMD5 = other.mMD5;
    }

    virtual const char* GetMsgName() const override { return "CR_LoadResource"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mIndexName")==0) { AData value; value = mIndexName; return value; }
        if (strcmp(szMember, "mMD5")==0) { AData value; value = mMD5; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mIndexName")==0) { mIndexName = value; return true; };
        if (strcmp(szMember, "mMD5")==0) { mMD5 = value; return true; };
        LOG("No exist > %%s", szMember);  return false;
    }

    AData operator [] (const char *szMember) const 
    {
        return get(szMember);
    }

    AData operator [] (const AString &member) const 
    {
        return get(member.c_str());
    }

};

class RC_ResponseResource : public tBaseMsg
{ 
public:
    int mErrorCode;		
    AString mMD5;		
    AutoData mResourceData;		

public:
    RC_ResponseResource() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mErrorCode);
        CheckGet(scrData, mMD5);
        mResourceData = (DataStream*)scrData["mResourceData"];
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mErrorCode"] = mErrorCode;
        destData["mMD5"] = mMD5;
        destData["mResourceData"] = mResourceData.getPtr();
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)3);

        SAVE_MSG_VALUE(mErrorCode, 1);
        SAVE_MSG_VALUE(mMD5, 4);
        SAVE_MSG_DATA(mResourceData);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mErrorCode = 0;
        mMD5.setNull();
        if (mResourceData) mResourceData.setNull();
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("RC_ResponseResource", otherMsg.GetMsgName())!=0) { LOG("%s is not RC_ResponseResource", otherMsg.GetMsgName()); return; }; const RC_ResponseResource &other = *(const RC_ResponseResource*)(&otherMsg);
        mErrorCode = other.mErrorCode;
        mMD5 = other.mMD5;
        COPY_MSG_DATA(other.mResourceData, mResourceData);
    }

    virtual const char* GetMsgName() const override { return "RC_ResponseResource"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mErrorCode")==0) { AData value; value = mErrorCode; return value; }
        if (strcmp(szMember, "mMD5")==0) { AData value; value = mMD5; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mErrorCode")==0) { mErrorCode = value; return true; };
        if (strcmp(szMember, "mMD5")==0) { mMD5 = value; return true; };
        LOG("No exist > %%s", szMember);  return false;
    }

    AData operator [] (const char *szMember) const 
    {
        return get(szMember);
    }

    AData operator [] (const AString &member) const 
    {
        return get(member.c_str());
    }

};

//  数据更新保存
class DB_UpdateSaveData : public tBaseMsg
{ 
public:
    AutoData mData;		
    AString mIndexName;		
    AString mMD5;		

public:
    DB_UpdateSaveData() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        mData = (DataStream*)scrData["mData"];
        CheckGet(scrData, mIndexName);
        CheckGet(scrData, mMD5);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mData"] = mData.getPtr();
        destData["mIndexName"] = mIndexName;
        destData["mMD5"] = mMD5;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)3);

        SAVE_MSG_DATA(mData);
        SAVE_MSG_VALUE(mIndexName, 4);
        SAVE_MSG_VALUE(mMD5, 4);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        if (mData) mData.setNull();
        mIndexName.setNull();
        mMD5.setNull();
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("DB_UpdateSaveData", otherMsg.GetMsgName())!=0) { LOG("%s is not DB_UpdateSaveData", otherMsg.GetMsgName()); return; }; const DB_UpdateSaveData &other = *(const DB_UpdateSaveData*)(&otherMsg);
        COPY_MSG_DATA(other.mData, mData);
        mIndexName = other.mIndexName;
        mMD5 = other.mMD5;
    }

    virtual const char* GetMsgName() const override { return "DB_UpdateSaveData"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mIndexName")==0) { AData value; value = mIndexName; return value; }
        if (strcmp(szMember, "mMD5")==0) { AData value; value = mMD5; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mIndexName")==0) { mIndexName = value; return true; };
        if (strcmp(szMember, "mMD5")==0) { mMD5 = value; return true; };
        LOG("No exist > %%s", szMember);  return false;
    }

    AData operator [] (const char *szMember) const 
    {
        return get(szMember);
    }

    AData operator [] (const AString &member) const 
    {
        return get(member.c_str());
    }

};

class DB_ReponseSaveResult : public tBaseMsg
{ 
public:
    int mErrorCode;		
    AString mInfo;		

public:
    DB_ReponseSaveResult() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mErrorCode);
        CheckGet(scrData, mInfo);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mErrorCode"] = mErrorCode;
        destData["mInfo"] = mInfo;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)2);

        SAVE_MSG_VALUE(mErrorCode, 1);
        SAVE_MSG_VALUE(mInfo, 4);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mErrorCode = 0;
        mInfo.setNull();
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("DB_ReponseSaveResult", otherMsg.GetMsgName())!=0) { LOG("%s is not DB_ReponseSaveResult", otherMsg.GetMsgName()); return; }; const DB_ReponseSaveResult &other = *(const DB_ReponseSaveResult*)(&otherMsg);
        mErrorCode = other.mErrorCode;
        mInfo = other.mInfo;
    }

    virtual const char* GetMsgName() const override { return "DB_ReponseSaveResult"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mErrorCode")==0) { AData value; value = mErrorCode; return value; }
        if (strcmp(szMember, "mInfo")==0) { AData value; value = mInfo; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mErrorCode")==0) { mErrorCode = value; return true; };
        if (strcmp(szMember, "mInfo")==0) { mInfo = value; return true; };
        LOG("No exist > %%s", szMember);  return false;
    }

    AData operator [] (const char *szMember) const 
    {
        return get(szMember);
    }

    AData operator [] (const AString &member) const 
    {
        return get(member.c_str());
    }

};

//  请求获取DB数据
class DB_LoadData : public tBaseMsg
{ 
public:
    AString mCheckMD5;		
    AString mIndexName;		

public:
    DB_LoadData() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mCheckMD5);
        CheckGet(scrData, mIndexName);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mCheckMD5"] = mCheckMD5;
        destData["mIndexName"] = mIndexName;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)2);

        SAVE_MSG_VALUE(mCheckMD5, 4);
        SAVE_MSG_VALUE(mIndexName, 4);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mCheckMD5.setNull();
        mIndexName.setNull();
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("DB_LoadData", otherMsg.GetMsgName())!=0) { LOG("%s is not DB_LoadData", otherMsg.GetMsgName()); return; }; const DB_LoadData &other = *(const DB_LoadData*)(&otherMsg);
        mCheckMD5 = other.mCheckMD5;
        mIndexName = other.mIndexName;
    }

    virtual const char* GetMsgName() const override { return "DB_LoadData"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mCheckMD5")==0) { AData value; value = mCheckMD5; return value; }
        if (strcmp(szMember, "mIndexName")==0) { AData value; value = mIndexName; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mCheckMD5")==0) { mCheckMD5 = value; return true; };
        if (strcmp(szMember, "mIndexName")==0) { mIndexName = value; return true; };
        LOG("No exist > %%s", szMember);  return false;
    }

    AData operator [] (const char *szMember) const 
    {
        return get(szMember);
    }

    AData operator [] (const AString &member) const 
    {
        return get(member.c_str());
    }

};

class DB_ResponseData : public tBaseMsg
{ 
public:
    AutoData mData;		
    int mErrorCode;		
    AString mMD5;		

public:
    DB_ResponseData() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        mData = (DataStream*)scrData["mData"];
        CheckGet(scrData, mErrorCode);
        CheckGet(scrData, mMD5);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mData"] = mData.getPtr();
        destData["mErrorCode"] = mErrorCode;
        destData["mMD5"] = mMD5;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)3);

        SAVE_MSG_DATA(mData);
        SAVE_MSG_VALUE(mErrorCode, 1);
        SAVE_MSG_VALUE(mMD5, 4);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        if (mData) mData.setNull();
        mErrorCode = 0;
        mMD5.setNull();
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("DB_ResponseData", otherMsg.GetMsgName())!=0) { LOG("%s is not DB_ResponseData", otherMsg.GetMsgName()); return; }; const DB_ResponseData &other = *(const DB_ResponseData*)(&otherMsg);
        COPY_MSG_DATA(other.mData, mData);
        mErrorCode = other.mErrorCode;
        mMD5 = other.mMD5;
    }

    virtual const char* GetMsgName() const override { return "DB_ResponseData"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mErrorCode")==0) { AData value; value = mErrorCode; return value; }
        if (strcmp(szMember, "mMD5")==0) { AData value; value = mMD5; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mErrorCode")==0) { mErrorCode = value; return true; };
        if (strcmp(szMember, "mMD5")==0) { mMD5 = value; return true; };
        LOG("No exist > %%s", szMember);  return false;
    }

    AData operator [] (const char *szMember) const 
    {
        return get(szMember);
    }

    AData operator [] (const AString &member) const 
    {
        return get(member.c_str());
    }

};

//  请求分包上传
class DB_ReqeustBigDataUpload : public tBaseMsg
{ 
public:
    int mPartSize;		
    Int64 mSize;		
    AString mTableName;		

public:
    DB_ReqeustBigDataUpload() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mPartSize);
        CheckGet(scrData, mSize);
        CheckGet(scrData, mTableName);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mPartSize"] = mPartSize;
        destData["mSize"] = mSize;
        destData["mTableName"] = mTableName;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)3);

        SAVE_MSG_VALUE(mPartSize, 1);
        SAVE_MSG_VALUE(mSize, 8);
        SAVE_MSG_VALUE(mTableName, 4);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mPartSize = 0;
        mSize = 0;
        mTableName.setNull();
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("DB_ReqeustBigDataUpload", otherMsg.GetMsgName())!=0) { LOG("%s is not DB_ReqeustBigDataUpload", otherMsg.GetMsgName()); return; }; const DB_ReqeustBigDataUpload &other = *(const DB_ReqeustBigDataUpload*)(&otherMsg);
        mPartSize = other.mPartSize;
        mSize = other.mSize;
        mTableName = other.mTableName;
    }

    virtual const char* GetMsgName() const override { return "DB_ReqeustBigDataUpload"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mPartSize")==0) { AData value; value = mPartSize; return value; }
        if (strcmp(szMember, "mSize")==0) { AData value; value = mSize; return value; }
        if (strcmp(szMember, "mTableName")==0) { AData value; value = mTableName; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mPartSize")==0) { mPartSize = value; return true; };
        if (strcmp(szMember, "mSize")==0) { mSize = value; return true; };
        if (strcmp(szMember, "mTableName")==0) { mTableName = value; return true; };
        LOG("No exist > %%s", szMember);  return false;
    }

    AData operator [] (const char *szMember) const 
    {
        return get(szMember);
    }

    AData operator [] (const AString &member) const 
    {
        return get(member.c_str());
    }

};

//  回复缓存ID
class DB_ResponseBigDataCacheID : public tBaseMsg
{ 
public:
    Int64 mCacheID;		
    int mError;		

public:
    DB_ResponseBigDataCacheID() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mCacheID);
        CheckGet(scrData, mError);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mCacheID"] = mCacheID;
        destData["mError"] = mError;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)2);

        SAVE_MSG_VALUE(mCacheID, 8);
        SAVE_MSG_VALUE(mError, 1);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mCacheID = 0;
        mError = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("DB_ResponseBigDataCacheID", otherMsg.GetMsgName())!=0) { LOG("%s is not DB_ResponseBigDataCacheID", otherMsg.GetMsgName()); return; }; const DB_ResponseBigDataCacheID &other = *(const DB_ResponseBigDataCacheID*)(&otherMsg);
        mCacheID = other.mCacheID;
        mError = other.mError;
    }

    virtual const char* GetMsgName() const override { return "DB_ResponseBigDataCacheID"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mCacheID")==0) { AData value; value = mCacheID; return value; }
        if (strcmp(szMember, "mError")==0) { AData value; value = mError; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mCacheID")==0) { mCacheID = value; return true; };
        if (strcmp(szMember, "mError")==0) { mError = value; return true; };
        LOG("No exist > %%s", szMember);  return false;
    }

    AData operator [] (const char *szMember) const 
    {
        return get(szMember);
    }

    AData operator [] (const AString &member) const 
    {
        return get(member.c_str());
    }

};

//  分包上传
class DB_UploadPartData : public tBaseMsg
{ 
public:
    Int64 mCacheID;		
    AutoData mPartData;		
    int mPartIndex;		

public:
    DB_UploadPartData() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mCacheID);
        mPartData = (DataStream*)scrData["mPartData"];
        CheckGet(scrData, mPartIndex);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mCacheID"] = mCacheID;
        destData["mPartData"] = mPartData.getPtr();
        destData["mPartIndex"] = mPartIndex;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)3);

        SAVE_MSG_VALUE(mCacheID, 8);
        SAVE_MSG_DATA(mPartData);
        SAVE_MSG_VALUE(mPartIndex, 1);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mCacheID = 0;
        if (mPartData) mPartData.setNull();
        mPartIndex = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("DB_UploadPartData", otherMsg.GetMsgName())!=0) { LOG("%s is not DB_UploadPartData", otherMsg.GetMsgName()); return; }; const DB_UploadPartData &other = *(const DB_UploadPartData*)(&otherMsg);
        mCacheID = other.mCacheID;
        COPY_MSG_DATA(other.mPartData, mPartData);
        mPartIndex = other.mPartIndex;
    }

    virtual const char* GetMsgName() const override { return "DB_UploadPartData"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mCacheID")==0) { AData value; value = mCacheID; return value; }
        if (strcmp(szMember, "mPartIndex")==0) { AData value; value = mPartIndex; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mCacheID")==0) { mCacheID = value; return true; };
        if (strcmp(szMember, "mPartIndex")==0) { mPartIndex = value; return true; };
        LOG("No exist > %%s", szMember);  return false;
    }

    AData operator [] (const char *szMember) const 
    {
        return get(szMember);
    }

    AData operator [] (const AString &member) const 
    {
        return get(member.c_str());
    }

};

class DB_ResponseUploadPartDataResult : public tBaseMsg
{ 
public:
    int mError;		

public:
    DB_ResponseUploadPartDataResult() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mError);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mError"] = mError;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)1);

        SAVE_MSG_VALUE(mError, 1);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mError = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("DB_ResponseUploadPartDataResult", otherMsg.GetMsgName())!=0) { LOG("%s is not DB_ResponseUploadPartDataResult", otherMsg.GetMsgName()); return; }; const DB_ResponseUploadPartDataResult &other = *(const DB_ResponseUploadPartDataResult*)(&otherMsg);
        mError = other.mError;
    }

    virtual const char* GetMsgName() const override { return "DB_ResponseUploadPartDataResult"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mError")==0) { AData value; value = mError; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mError")==0) { mError = value; return true; };
        LOG("No exist > %%s", szMember);  return false;
    }

    AData operator [] (const char *szMember) const 
    {
        return get(szMember);
    }

    AData operator [] (const AString &member) const 
    {
        return get(member.c_str());
    }

};

//  完成处理
class DB_UpdateSaveCacheBigData : public tBaseMsg
{ 
public:
    Int64 mCacheID;		
    AString mCheckMD5;		
    AString mKey;		//  如果为空, 则自增

public:
    DB_UpdateSaveCacheBigData() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mCacheID);
        CheckGet(scrData, mCheckMD5);
        CheckGet(scrData, mKey);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mCacheID"] = mCacheID;
        destData["mCheckMD5"] = mCheckMD5;
        destData["mKey"] = mKey;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)3);

        SAVE_MSG_VALUE(mCacheID, 8);
        SAVE_MSG_VALUE(mCheckMD5, 4);
        SAVE_MSG_VALUE(mKey, 4);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mCacheID = 0;
        mCheckMD5.setNull();
        mKey.setNull();
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("DB_UpdateSaveCacheBigData", otherMsg.GetMsgName())!=0) { LOG("%s is not DB_UpdateSaveCacheBigData", otherMsg.GetMsgName()); return; }; const DB_UpdateSaveCacheBigData &other = *(const DB_UpdateSaveCacheBigData*)(&otherMsg);
        mCacheID = other.mCacheID;
        mCheckMD5 = other.mCheckMD5;
        mKey = other.mKey;
    }

    virtual const char* GetMsgName() const override { return "DB_UpdateSaveCacheBigData"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mCacheID")==0) { AData value; value = mCacheID; return value; }
        if (strcmp(szMember, "mCheckMD5")==0) { AData value; value = mCheckMD5; return value; }
        if (strcmp(szMember, "mKey")==0) { AData value; value = mKey; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mCacheID")==0) { mCacheID = value; return true; };
        if (strcmp(szMember, "mCheckMD5")==0) { mCheckMD5 = value; return true; };
        if (strcmp(szMember, "mKey")==0) { mKey = value; return true; };
        LOG("No exist > %%s", szMember);  return false;
    }

    AData operator [] (const char *szMember) const 
    {
        return get(szMember);
    }

    AData operator [] (const AString &member) const 
    {
        return get(member.c_str());
    }

};

//  处理结果
class DB_ResponseUpdateSaveCacheDataResult : public tBaseMsg
{ 
public:
    AString mDBKey;		
    int mError;		

public:
    DB_ResponseUpdateSaveCacheDataResult() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mDBKey);
        CheckGet(scrData, mError);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mDBKey"] = mDBKey;
        destData["mError"] = mError;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)2);

        SAVE_MSG_VALUE(mDBKey, 4);
        SAVE_MSG_VALUE(mError, 1);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mDBKey.setNull();
        mError = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("DB_ResponseUpdateSaveCacheDataResult", otherMsg.GetMsgName())!=0) { LOG("%s is not DB_ResponseUpdateSaveCacheDataResult", otherMsg.GetMsgName()); return; }; const DB_ResponseUpdateSaveCacheDataResult &other = *(const DB_ResponseUpdateSaveCacheDataResult*)(&otherMsg);
        mDBKey = other.mDBKey;
        mError = other.mError;
    }

    virtual const char* GetMsgName() const override { return "DB_ResponseUpdateSaveCacheDataResult"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mDBKey")==0) { AData value; value = mDBKey; return value; }
        if (strcmp(szMember, "mError")==0) { AData value; value = mError; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mDBKey")==0) { mDBKey = value; return true; };
        if (strcmp(szMember, "mError")==0) { mError = value; return true; };
        LOG("No exist > %%s", szMember);  return false;
    }

    AData operator [] (const char *szMember) const 
    {
        return get(szMember);
    }

    AData operator [] (const AString &member) const 
    {
        return get(member.c_str());
    }

};

//  请求下载, 回复的MD5可用于检验本地缓存的数据是否有效
class DB_RequestDownloadData : public tBaseMsg
{ 
public:
    AString mCheckMD5;		
    AString mKey;		
    AString mTableName;		

public:
    DB_RequestDownloadData() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mCheckMD5);
        CheckGet(scrData, mKey);
        CheckGet(scrData, mTableName);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mCheckMD5"] = mCheckMD5;
        destData["mKey"] = mKey;
        destData["mTableName"] = mTableName;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)3);

        SAVE_MSG_VALUE(mCheckMD5, 4);
        SAVE_MSG_VALUE(mKey, 4);
        SAVE_MSG_VALUE(mTableName, 4);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mCheckMD5.setNull();
        mKey.setNull();
        mTableName.setNull();
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("DB_RequestDownloadData", otherMsg.GetMsgName())!=0) { LOG("%s is not DB_RequestDownloadData", otherMsg.GetMsgName()); return; }; const DB_RequestDownloadData &other = *(const DB_RequestDownloadData*)(&otherMsg);
        mCheckMD5 = other.mCheckMD5;
        mKey = other.mKey;
        mTableName = other.mTableName;
    }

    virtual const char* GetMsgName() const override { return "DB_RequestDownloadData"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mCheckMD5")==0) { AData value; value = mCheckMD5; return value; }
        if (strcmp(szMember, "mKey")==0) { AData value; value = mKey; return value; }
        if (strcmp(szMember, "mTableName")==0) { AData value; value = mTableName; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mCheckMD5")==0) { mCheckMD5 = value; return true; };
        if (strcmp(szMember, "mKey")==0) { mKey = value; return true; };
        if (strcmp(szMember, "mTableName")==0) { mTableName = value; return true; };
        LOG("No exist > %%s", szMember);  return false;
    }

    AData operator [] (const char *szMember) const 
    {
        return get(szMember);
    }

    AData operator [] (const AString &member) const 
    {
        return get(member.c_str());
    }

};

//  回复数据信息
class DB_ResponseDataInfo : public tBaseMsg
{ 
public:
    int mError;		
    AString mMD5;		
    Int64 mSize;		

public:
    DB_ResponseDataInfo() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mError);
        CheckGet(scrData, mMD5);
        CheckGet(scrData, mSize);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mError"] = mError;
        destData["mMD5"] = mMD5;
        destData["mSize"] = mSize;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)3);

        SAVE_MSG_VALUE(mError, 1);
        SAVE_MSG_VALUE(mMD5, 4);
        SAVE_MSG_VALUE(mSize, 8);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mError = 0;
        mMD5.setNull();
        mSize = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("DB_ResponseDataInfo", otherMsg.GetMsgName())!=0) { LOG("%s is not DB_ResponseDataInfo", otherMsg.GetMsgName()); return; }; const DB_ResponseDataInfo &other = *(const DB_ResponseDataInfo*)(&otherMsg);
        mError = other.mError;
        mMD5 = other.mMD5;
        mSize = other.mSize;
    }

    virtual const char* GetMsgName() const override { return "DB_ResponseDataInfo"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mError")==0) { AData value; value = mError; return value; }
        if (strcmp(szMember, "mMD5")==0) { AData value; value = mMD5; return value; }
        if (strcmp(szMember, "mSize")==0) { AData value; value = mSize; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mError")==0) { mError = value; return true; };
        if (strcmp(szMember, "mMD5")==0) { mMD5 = value; return true; };
        if (strcmp(szMember, "mSize")==0) { mSize = value; return true; };
        LOG("No exist > %%s", szMember);  return false;
    }

    AData operator [] (const char *szMember) const 
    {
        return get(szMember);
    }

    AData operator [] (const AString &member) const 
    {
        return get(member.c_str());
    }

};

//  分包下载
class DB_RequestDownloadPartData : public tBaseMsg
{ 
public:
    AString mKey;		
    Int64 mPosition;		
    int mRequestSize;		
    AString mTableName;		

public:
    DB_RequestDownloadPartData() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mKey);
        CheckGet(scrData, mPosition);
        CheckGet(scrData, mRequestSize);
        CheckGet(scrData, mTableName);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mKey"] = mKey;
        destData["mPosition"] = mPosition;
        destData["mRequestSize"] = mRequestSize;
        destData["mTableName"] = mTableName;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)4);

        SAVE_MSG_VALUE(mKey, 4);
        SAVE_MSG_VALUE(mPosition, 8);
        SAVE_MSG_VALUE(mRequestSize, 1);
        SAVE_MSG_VALUE(mTableName, 4);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mKey.setNull();
        mPosition = 0;
        mRequestSize = 0;
        mTableName.setNull();
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("DB_RequestDownloadPartData", otherMsg.GetMsgName())!=0) { LOG("%s is not DB_RequestDownloadPartData", otherMsg.GetMsgName()); return; }; const DB_RequestDownloadPartData &other = *(const DB_RequestDownloadPartData*)(&otherMsg);
        mKey = other.mKey;
        mPosition = other.mPosition;
        mRequestSize = other.mRequestSize;
        mTableName = other.mTableName;
    }

    virtual const char* GetMsgName() const override { return "DB_RequestDownloadPartData"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mKey")==0) { AData value; value = mKey; return value; }
        if (strcmp(szMember, "mPosition")==0) { AData value; value = mPosition; return value; }
        if (strcmp(szMember, "mRequestSize")==0) { AData value; value = mRequestSize; return value; }
        if (strcmp(szMember, "mTableName")==0) { AData value; value = mTableName; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mKey")==0) { mKey = value; return true; };
        if (strcmp(szMember, "mPosition")==0) { mPosition = value; return true; };
        if (strcmp(szMember, "mRequestSize")==0) { mRequestSize = value; return true; };
        if (strcmp(szMember, "mTableName")==0) { mTableName = value; return true; };
        LOG("No exist > %%s", szMember);  return false;
    }

    AData operator [] (const char *szMember) const 
    {
        return get(szMember);
    }

    AData operator [] (const AString &member) const 
    {
        return get(member.c_str());
    }

};

//  回复分包数据
class DB_ResponsePartData : public tBaseMsg
{ 
public:
    int mError;		
    AutoData mPartData;		

public:
    DB_ResponsePartData() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mError);
        mPartData = (DataStream*)scrData["mPartData"];
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mError"] = mError;
        destData["mPartData"] = mPartData.getPtr();
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)2);

        SAVE_MSG_VALUE(mError, 1);
        SAVE_MSG_DATA(mPartData);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mError = 0;
        if (mPartData) mPartData.setNull();
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("DB_ResponsePartData", otherMsg.GetMsgName())!=0) { LOG("%s is not DB_ResponsePartData", otherMsg.GetMsgName()); return; }; const DB_ResponsePartData &other = *(const DB_ResponsePartData*)(&otherMsg);
        mError = other.mError;
        COPY_MSG_DATA(other.mPartData, mPartData);
    }

    virtual const char* GetMsgName() const override { return "DB_ResponsePartData"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mError")==0) { AData value; value = mError; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mError")==0) { mError = value; return true; };
        LOG("No exist > %%s", szMember);  return false;
    }

    AData operator [] (const char *szMember) const 
    {
        return get(szMember);
    }

    AData operator [] (const AString &member) const 
    {
        return get(member.c_str());
    }

};

//  通用请求获取DB记录
class DB_RequestLoadRecord : public tBaseMsg
{ 
public:
    AString mKey;		
    AString mTableName;		
    bool mbNeedField;		

public:
    DB_RequestLoadRecord() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mKey);
        CheckGet(scrData, mTableName);
        CheckGet(scrData, mbNeedField);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mKey"] = mKey;
        destData["mTableName"] = mTableName;
        destData["mbNeedField"] = mbNeedField;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)3);

        SAVE_MSG_VALUE(mKey, 4);
        SAVE_MSG_VALUE(mTableName, 4);
        SAVE_MSG_VALUE(mbNeedField, 5);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mKey.setNull();
        mTableName.setNull();
        mbNeedField = false;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("DB_RequestLoadRecord", otherMsg.GetMsgName())!=0) { LOG("%s is not DB_RequestLoadRecord", otherMsg.GetMsgName()); return; }; const DB_RequestLoadRecord &other = *(const DB_RequestLoadRecord*)(&otherMsg);
        mKey = other.mKey;
        mTableName = other.mTableName;
        mbNeedField = other.mbNeedField;
    }

    virtual const char* GetMsgName() const override { return "DB_RequestLoadRecord"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mKey")==0) { AData value; value = mKey; return value; }
        if (strcmp(szMember, "mTableName")==0) { AData value; value = mTableName; return value; }
        if (strcmp(szMember, "mbNeedField")==0) { AData value; value = mbNeedField; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mKey")==0) { mKey = value; return true; };
        if (strcmp(szMember, "mTableName")==0) { mTableName = value; return true; };
        if (strcmp(szMember, "mbNeedField")==0) { mbNeedField = value; return true; };
        LOG("No exist > %%s", szMember);  return false;
    }

    AData operator [] (const char *szMember) const 
    {
        return get(szMember);
    }

    AData operator [] (const AString &member) const 
    {
        return get(member.c_str());
    }

};

class DB_ResponseRecord : public tBaseMsg
{ 
public:
    int mError;		
    AutoData mFieldData;		
    AutoData mRecordData;		

public:
    DB_ResponseRecord() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mError);
        mFieldData = (DataStream*)scrData["mFieldData"];
        mRecordData = (DataStream*)scrData["mRecordData"];
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mError"] = mError;
        destData["mFieldData"] = mFieldData.getPtr();
        destData["mRecordData"] = mRecordData.getPtr();
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)3);

        SAVE_MSG_VALUE(mError, 1);
        SAVE_MSG_DATA(mFieldData);
        SAVE_MSG_DATA(mRecordData);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mError = 0;
        if (mFieldData) mFieldData.setNull();
        if (mRecordData) mRecordData.setNull();
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("DB_ResponseRecord", otherMsg.GetMsgName())!=0) { LOG("%s is not DB_ResponseRecord", otherMsg.GetMsgName()); return; }; const DB_ResponseRecord &other = *(const DB_ResponseRecord*)(&otherMsg);
        mError = other.mError;
        COPY_MSG_DATA(other.mFieldData, mFieldData);
        COPY_MSG_DATA(other.mRecordData, mRecordData);
    }

    virtual const char* GetMsgName() const override { return "DB_ResponseRecord"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mError")==0) { AData value; value = mError; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mError")==0) { mError = value; return true; };
        LOG("No exist > %%s", szMember);  return false;
    }

    AData operator [] (const char *szMember) const 
    {
        return get(szMember);
    }

    AData operator [] (const AString &member) const 
    {
        return get(member.c_str());
    }

};

//  上传更新保存记录
class DB_RequestSaveRecord : public tBaseMsg
{ 
public:
    AString mKey;		
    AutoData mRecordData;		
    AString mTableName;		

public:
    DB_RequestSaveRecord() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mKey);
        mRecordData = (DataStream*)scrData["mRecordData"];
        CheckGet(scrData, mTableName);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mKey"] = mKey;
        destData["mRecordData"] = mRecordData.getPtr();
        destData["mTableName"] = mTableName;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)3);

        SAVE_MSG_VALUE(mKey, 4);
        SAVE_MSG_DATA(mRecordData);
        SAVE_MSG_VALUE(mTableName, 4);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mKey.setNull();
        if (mRecordData) mRecordData.setNull();
        mTableName.setNull();
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("DB_RequestSaveRecord", otherMsg.GetMsgName())!=0) { LOG("%s is not DB_RequestSaveRecord", otherMsg.GetMsgName()); return; }; const DB_RequestSaveRecord &other = *(const DB_RequestSaveRecord*)(&otherMsg);
        mKey = other.mKey;
        COPY_MSG_DATA(other.mRecordData, mRecordData);
        mTableName = other.mTableName;
    }

    virtual const char* GetMsgName() const override { return "DB_RequestSaveRecord"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mKey")==0) { AData value; value = mKey; return value; }
        if (strcmp(szMember, "mTableName")==0) { AData value; value = mTableName; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mKey")==0) { mKey = value; return true; };
        if (strcmp(szMember, "mTableName")==0) { mTableName = value; return true; };
        LOG("No exist > %%s", szMember);  return false;
    }

    AData operator [] (const char *szMember) const 
    {
        return get(szMember);
    }

    AData operator [] (const AString &member) const 
    {
        return get(member.c_str());
    }

};

class DB_ResponseSaveRecord : public tBaseMsg
{ 
public:
    int mError;		

public:
    DB_ResponseSaveRecord() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mError);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mError"] = mError;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)1);

        SAVE_MSG_VALUE(mError, 1);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mError = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("DB_ResponseSaveRecord", otherMsg.GetMsgName())!=0) { LOG("%s is not DB_ResponseSaveRecord", otherMsg.GetMsgName()); return; }; const DB_ResponseSaveRecord &other = *(const DB_ResponseSaveRecord*)(&otherMsg);
        mError = other.mError;
    }

    virtual const char* GetMsgName() const override { return "DB_ResponseSaveRecord"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mError")==0) { AData value; value = mError; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mError")==0) { mError = value; return true; };
        LOG("No exist > %%s", szMember);  return false;
    }

    AData operator [] (const char *szMember) const 
    {
        return get(szMember);
    }

    AData operator [] (const AString &member) const 
    {
        return get(member.c_str());
    }

};

