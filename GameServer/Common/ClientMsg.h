//Auto genereate msg data code		
#pragma once

#include "BaseMsg.h"
#include "Array.h"
#include "ArrayList.h"

class CS_RequestLogin : public tRequestMsg
{ 
public:
    Int64 mDBID;		
    int mRequestID;		
    AString mToken;		

public:
    CS_RequestLogin() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mDBID);
        CheckGet(scrData, mRequestID);
        CheckGet(scrData, mToken);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mDBID"] = mDBID;
        destData["mRequestID"] = mRequestID;
        destData["mToken"] = mToken;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)3);

        SAVE_MSG_VALUE(mDBID, 8);
        SAVE_MSG_VALUE(mRequestID, 1);
        SAVE_MSG_VALUE(mToken, 4);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mDBID = 0;
        mRequestID = 0;
        mToken.setNull();
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("CS_RequestLogin", otherMsg.GetMsgName())!=0) { LOG("%s is not CS_RequestLogin", otherMsg.GetMsgName()); return; }; const CS_RequestLogin &other = *(const CS_RequestLogin*)(&otherMsg);
        mDBID = other.mDBID;
        mRequestID = other.mRequestID;
        mToken = other.mToken;
    }

    virtual const char* GetMsgName() const override { return "CS_RequestLogin"; }

    virtual void SetRequestID(MSG_ID id)  override { mRequestID = (int)id; }

    virtual MSG_ID GetRequestID()  override { return (MSG_ID)mRequestID; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mDBID")==0) { AData value; value = mDBID; return value; }
        if (strcmp(szMember, "mRequestID")==0) { AData value; value = mRequestID; return value; }
        if (strcmp(szMember, "mToken")==0) { AData value; value = mToken; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mDBID")==0) { mDBID = value; return true; };
        if (strcmp(szMember, "mRequestID")==0) { mRequestID = value; return true; };
        if (strcmp(szMember, "mToken")==0) { mToken = value; return true; };
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

class SC_ResponseLogin : public tBaseMsg
{ 
public:
    int error;		

public:
    SC_ResponseLogin() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, error);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["error"] = error;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)1);

        SAVE_MSG_VALUE(error, 1);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        error = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("SC_ResponseLogin", otherMsg.GetMsgName())!=0) { LOG("%s is not SC_ResponseLogin", otherMsg.GetMsgName()); return; }; const SC_ResponseLogin &other = *(const SC_ResponseLogin*)(&otherMsg);
        error = other.error;
    }

    virtual const char* GetMsgName() const override { return "SC_ResponseLogin"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "error")==0) { AData value; value = error; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "error")==0) { error = value; return true; };
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

//  请求游戏数据
class CG_RequestPlayerData : public tBaseMsg
{ 
public:

public:
    CG_RequestPlayerData() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
    }

    virtual void ToData(AutoNice &destData) override
    {
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)0);

        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("CG_RequestPlayerData", otherMsg.GetMsgName())!=0) { LOG("%s is not CG_RequestPlayerData", otherMsg.GetMsgName()); return; }; const CG_RequestPlayerData &other = *(const CG_RequestPlayerData*)(&otherMsg);
    }

    virtual const char* GetMsgName() const override { return "CG_RequestPlayerData"; }

    AData get(const char *szMember) const 
    {
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
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

//  回复玩家数据
class GC_ResponsePlayerData : public tBaseMsg
{ 
public:
    AutoNice mData;		
    AString mName;		

public:
    GC_ResponsePlayerData() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        mData = (tNiceData*)scrData["mData"];
        CheckGet(scrData, mName);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mData"] = mData.getPtr();
        destData["mName"] = mName;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)2);

        SAVE_MSG_NICE(mData);
        SAVE_MSG_VALUE(mName, 4);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        if (mData) mData.setNull();
        mName.setNull();
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("GC_ResponsePlayerData", otherMsg.GetMsgName())!=0) { LOG("%s is not GC_ResponsePlayerData", otherMsg.GetMsgName()); return; }; const GC_ResponsePlayerData &other = *(const GC_ResponsePlayerData*)(&otherMsg);
        COPY_MSG_NICE(other.mData, mData);
        mName = other.mName;
    }

    virtual const char* GetMsgName() const override { return "GC_ResponsePlayerData"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mName")==0) { AData value; value = mName; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mName")==0) { mName = value; return true; };
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

//  游戏服务器请求客户端状态
class GC_RequestClientState : public tBaseMsg
{ 
public:
    int mValue;		

public:
    GC_RequestClientState() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mValue);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mValue"] = mValue;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)1);

        SAVE_MSG_VALUE(mValue, 1);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mValue = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("GC_RequestClientState", otherMsg.GetMsgName())!=0) { LOG("%s is not GC_RequestClientState", otherMsg.GetMsgName()); return; }; const GC_RequestClientState &other = *(const GC_RequestClientState*)(&otherMsg);
        mValue = other.mValue;
    }

    virtual const char* GetMsgName() const override { return "GC_RequestClientState"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mValue")==0) { AData value; value = mValue; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mValue")==0) { mValue = value; return true; };
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

class CG_ResponseClientState : public tBaseMsg
{ 
public:
    int mBeginSecond;		
    AString mInfo;		

public:
    CG_ResponseClientState() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mBeginSecond);
        CheckGet(scrData, mInfo);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mBeginSecond"] = mBeginSecond;
        destData["mInfo"] = mInfo;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)2);

        SAVE_MSG_VALUE(mBeginSecond, 1);
        SAVE_MSG_VALUE(mInfo, 4);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mBeginSecond = 0;
        mInfo.setNull();
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("CG_ResponseClientState", otherMsg.GetMsgName())!=0) { LOG("%s is not CG_ResponseClientState", otherMsg.GetMsgName()); return; }; const CG_ResponseClientState &other = *(const CG_ResponseClientState*)(&otherMsg);
        mBeginSecond = other.mBeginSecond;
        mInfo = other.mInfo;
    }

    virtual const char* GetMsgName() const override { return "CG_ResponseClientState"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mBeginSecond")==0) { AData value; value = mBeginSecond; return value; }
        if (strcmp(szMember, "mInfo")==0) { AData value; value = mInfo; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mBeginSecond")==0) { mBeginSecond = value; return true; };
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

