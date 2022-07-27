//Auto genereate msg data code		
#pragma once

#include "BaseMsg.h"
#include "Array.h"
#include "ArrayList.h"

class AC_RequestCreateAccount : public tBaseMsg
{ 
public:
    AString mAccount;		
    AutoNice mInfoData;		
    AString mPassword;		

public:
    AC_RequestCreateAccount() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mAccount);
        mInfoData = (tNiceData*)scrData["mInfoData"];
        CheckGet(scrData, mPassword);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mAccount"] = mAccount;
        destData["mInfoData"] = mInfoData.getPtr();
        destData["mPassword"] = mPassword;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)3);

        SAVE_MSG_VALUE(mAccount, 4);
        SAVE_MSG_NICE(mInfoData);
        SAVE_MSG_VALUE(mPassword, 4);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mAccount.setNull();
        if (mInfoData) mInfoData.setNull();
        mPassword.setNull();
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("AC_RequestCreateAccount", otherMsg.GetMsgName())!=0) { LOG("%s is not AC_RequestCreateAccount", otherMsg.GetMsgName()); return; }; const AC_RequestCreateAccount &other = *(const AC_RequestCreateAccount*)(&otherMsg);
        mAccount = other.mAccount;
        COPY_MSG_NICE(other.mInfoData, mInfoData);
        mPassword = other.mPassword;
    }

    virtual const char* GetMsgName() const override { return "AC_RequestCreateAccount"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mAccount")==0) { AData value; value = mAccount; return value; }
        if (strcmp(szMember, "mPassword")==0) { AData value; value = mPassword; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mAccount")==0) { mAccount = value; return true; };
        if (strcmp(szMember, "mPassword")==0) { mPassword = value; return true; };
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

class CA_ResponseCreateAccount : public tBaseMsg
{ 
public:
    Int64 mDBID;		
    int mErrorCode;		//  0表示成功

public:
    CA_ResponseCreateAccount() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mDBID);
        CheckGet(scrData, mErrorCode);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mDBID"] = mDBID;
        destData["mErrorCode"] = mErrorCode;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)2);

        SAVE_MSG_VALUE(mDBID, 8);
        SAVE_MSG_VALUE(mErrorCode, 1);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mDBID = 0;
        mErrorCode = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("CA_ResponseCreateAccount", otherMsg.GetMsgName())!=0) { LOG("%s is not CA_ResponseCreateAccount", otherMsg.GetMsgName()); return; }; const CA_ResponseCreateAccount &other = *(const CA_ResponseCreateAccount*)(&otherMsg);
        mDBID = other.mDBID;
        mErrorCode = other.mErrorCode;
    }

    virtual const char* GetMsgName() const override { return "CA_ResponseCreateAccount"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mDBID")==0) { AData value; value = mDBID; return value; }
        if (strcmp(szMember, "mErrorCode")==0) { AData value; value = mErrorCode; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mDBID")==0) { mDBID = value; return true; };
        if (strcmp(szMember, "mErrorCode")==0) { mErrorCode = value; return true; };
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

class AC_RequestAccountData : public tBaseMsg
{ 
public:
    AString mAccount;		

public:
    AC_RequestAccountData() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mAccount);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mAccount"] = mAccount;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)1);

        SAVE_MSG_VALUE(mAccount, 4);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mAccount.setNull();
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("AC_RequestAccountData", otherMsg.GetMsgName())!=0) { LOG("%s is not AC_RequestAccountData", otherMsg.GetMsgName()); return; }; const AC_RequestAccountData &other = *(const AC_RequestAccountData*)(&otherMsg);
        mAccount = other.mAccount;
    }

    virtual const char* GetMsgName() const override { return "AC_RequestAccountData"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mAccount")==0) { AData value; value = mAccount; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mAccount")==0) { mAccount = value; return true; };
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

class CA_ResponseAccountData : public tBaseMsg
{ 
public:
    Int64 mDBID;		
    int mErrorCode;		//  0表示成功
    AutoNice mInfoData;		
    AString mPassword;		

public:
    CA_ResponseAccountData() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mDBID);
        CheckGet(scrData, mErrorCode);
        mInfoData = (tNiceData*)scrData["mInfoData"];
        CheckGet(scrData, mPassword);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mDBID"] = mDBID;
        destData["mErrorCode"] = mErrorCode;
        destData["mInfoData"] = mInfoData.getPtr();
        destData["mPassword"] = mPassword;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)4);

        SAVE_MSG_VALUE(mDBID, 8);
        SAVE_MSG_VALUE(mErrorCode, 1);
        SAVE_MSG_NICE(mInfoData);
        SAVE_MSG_VALUE(mPassword, 4);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mDBID = 0;
        mErrorCode = 0;
        if (mInfoData) mInfoData.setNull();
        mPassword.setNull();
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("CA_ResponseAccountData", otherMsg.GetMsgName())!=0) { LOG("%s is not CA_ResponseAccountData", otherMsg.GetMsgName()); return; }; const CA_ResponseAccountData &other = *(const CA_ResponseAccountData*)(&otherMsg);
        mDBID = other.mDBID;
        mErrorCode = other.mErrorCode;
        COPY_MSG_NICE(other.mInfoData, mInfoData);
        mPassword = other.mPassword;
    }

    virtual const char* GetMsgName() const override { return "CA_ResponseAccountData"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mDBID")==0) { AData value; value = mDBID; return value; }
        if (strcmp(szMember, "mErrorCode")==0) { AData value; value = mErrorCode; return value; }
        if (strcmp(szMember, "mPassword")==0) { AData value; value = mPassword; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mDBID")==0) { mDBID = value; return true; };
        if (strcmp(szMember, "mErrorCode")==0) { mErrorCode = value; return true; };
        if (strcmp(szMember, "mPassword")==0) { mPassword = value; return true; };
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

//  请求分配游戏服务器, 并通知Server创建PlayerActor
class LW_RequestEnterServer : public tBaseMsg
{ 
public:
    Int64 mDBID;		

public:
    LW_RequestEnterServer() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mDBID);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mDBID"] = mDBID;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)1);

        SAVE_MSG_VALUE(mDBID, 8);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mDBID = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("LW_RequestEnterServer", otherMsg.GetMsgName())!=0) { LOG("%s is not LW_RequestEnterServer", otherMsg.GetMsgName()); return; }; const LW_RequestEnterServer &other = *(const LW_RequestEnterServer*)(&otherMsg);
        mDBID = other.mDBID;
    }

    virtual const char* GetMsgName() const override { return "LW_RequestEnterServer"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mDBID")==0) { AData value; value = mDBID; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mDBID")==0) { mDBID = value; return true; };
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

class WL_ResponseEnterServer : public tBaseMsg
{ 
public:
    int error;		
    int mServerID;		

public:
    WL_ResponseEnterServer() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, error);
        CheckGet(scrData, mServerID);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["error"] = error;
        destData["mServerID"] = mServerID;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)2);

        SAVE_MSG_VALUE(error, 1);
        SAVE_MSG_VALUE(mServerID, 1);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        error = 0;
        mServerID = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("WL_ResponseEnterServer", otherMsg.GetMsgName())!=0) { LOG("%s is not WL_ResponseEnterServer", otherMsg.GetMsgName()); return; }; const WL_ResponseEnterServer &other = *(const WL_ResponseEnterServer*)(&otherMsg);
        error = other.error;
        mServerID = other.mServerID;
    }

    virtual const char* GetMsgName() const override { return "WL_ResponseEnterServer"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "error")==0) { AData value; value = error; return value; }
        if (strcmp(szMember, "mServerID")==0) { AData value; value = mServerID; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "error")==0) { error = value; return true; };
        if (strcmp(szMember, "mServerID")==0) { mServerID = value; return true; };
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

//  世界服务器请求ServerActor创建PlayerActor
class WG_RequestCreatePlayerActor : public tBaseMsg
{ 
public:
    Int64 mDBID;		

public:
    WG_RequestCreatePlayerActor() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mDBID);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mDBID"] = mDBID;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)1);

        SAVE_MSG_VALUE(mDBID, 8);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mDBID = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("WG_RequestCreatePlayerActor", otherMsg.GetMsgName())!=0) { LOG("%s is not WG_RequestCreatePlayerActor", otherMsg.GetMsgName()); return; }; const WG_RequestCreatePlayerActor &other = *(const WG_RequestCreatePlayerActor*)(&otherMsg);
        mDBID = other.mDBID;
    }

    virtual const char* GetMsgName() const override { return "WG_RequestCreatePlayerActor"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mDBID")==0) { AData value; value = mDBID; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mDBID")==0) { mDBID = value; return true; };
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

class GW_ResponseCreatePlayer : public tBaseMsg
{ 
public:
    int error;		

public:
    GW_ResponseCreatePlayer() { clear(false); };


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
        if (strcmp("GW_ResponseCreatePlayer", otherMsg.GetMsgName())!=0) { LOG("%s is not GW_ResponseCreatePlayer", otherMsg.GetMsgName()); return; }; const GW_ResponseCreatePlayer &other = *(const GW_ResponseCreatePlayer*)(&otherMsg);
        error = other.error;
    }

    virtual const char* GetMsgName() const override { return "GW_ResponseCreatePlayer"; }

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

