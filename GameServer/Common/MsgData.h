//Auto genereate msg data code		
#pragma once

#include "BaseMsg.h"
#include "Array.h"
#include "ArrayList.h"

#include "MsgStruct.h"

class RQ_CreateDBTable : public tBaseMsg
{ 
public:
    AutoNice mDBConfigData;		//  key: table name, Value: ConfigTable
    AString mExportCodePath;		//  生成DB代码路径

public:
    RQ_CreateDBTable() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        mDBConfigData = (tNiceData*)scrData["mDBConfigData"];
        CheckGet(scrData, mExportCodePath);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mDBConfigData"] = mDBConfigData.getPtr();
        destData["mExportCodePath"] = mExportCodePath;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)2);

        SAVE_MSG_NICE(mDBConfigData);
        SAVE_MSG_VALUE(mExportCodePath, 4);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        if (mDBConfigData) mDBConfigData.setNull();
        mExportCodePath.setNull();
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("RQ_CreateDBTable", otherMsg.GetMsgName())!=0) { LOG("%s is not RQ_CreateDBTable", otherMsg.GetMsgName()); return; }; const RQ_CreateDBTable &other = *(const RQ_CreateDBTable*)(&otherMsg);
        COPY_MSG_NICE(other.mDBConfigData, mDBConfigData);
        mExportCodePath = other.mExportCodePath;
    }

    virtual const char* GetMsgName() const override { return "RQ_CreateDBTable"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mExportCodePath")==0) { AData value; value = mExportCodePath; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mExportCodePath")==0) { mExportCodePath = value; return true; };
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

class RS_CreateDBTable : public tBaseMsg
{ 
public:
    AString mResultInfo;		

public:
    RS_CreateDBTable() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        CheckGet(scrData, mResultInfo);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mResultInfo"] = mResultInfo;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)1);

        SAVE_MSG_VALUE(mResultInfo, 4);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mResultInfo.setNull();
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("RS_CreateDBTable", otherMsg.GetMsgName())!=0) { LOG("%s is not RS_CreateDBTable", otherMsg.GetMsgName()); return; }; const RS_CreateDBTable &other = *(const RS_CreateDBTable*)(&otherMsg);
        mResultInfo = other.mResultInfo;
    }

    virtual const char* GetMsgName() const override { return "RS_CreateDBTable"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mResultInfo")==0) { AData value; value = mResultInfo; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mResultInfo")==0) { mResultInfo = value; return true; };
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

//  检查或创建帐号
class RQ_CheckAndCreateAccount : public tBaseMsg
{ 
public:
    AString mAccount;		
    AString mPassword;		
    int mServerID;		

public:
    RQ_CheckAndCreateAccount() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        CheckGet(scrData, mAccount);
        CheckGet(scrData, mPassword);
        CheckGet(scrData, mServerID);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mAccount"] = mAccount;
        destData["mPassword"] = mPassword;
        destData["mServerID"] = mServerID;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)3);

        SAVE_MSG_VALUE(mAccount, 4);
        SAVE_MSG_VALUE(mPassword, 4);
        SAVE_MSG_VALUE(mServerID, 1);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mAccount.setNull();
        mPassword.setNull();
        mServerID = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("RQ_CheckAndCreateAccount", otherMsg.GetMsgName())!=0) { LOG("%s is not RQ_CheckAndCreateAccount", otherMsg.GetMsgName()); return; }; const RQ_CheckAndCreateAccount &other = *(const RQ_CheckAndCreateAccount*)(&otherMsg);
        mAccount = other.mAccount;
        mPassword = other.mPassword;
        mServerID = other.mServerID;
    }

    virtual const char* GetMsgName() const override { return "RQ_CheckAndCreateAccount"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mAccount")==0) { AData value; value = mAccount; return value; }
        if (strcmp(szMember, "mPassword")==0) { AData value; value = mPassword; return value; }
        if (strcmp(szMember, "mServerID")==0) { AData value; value = mServerID; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mAccount")==0) { mAccount = value; return true; };
        if (strcmp(szMember, "mPassword")==0) { mPassword = value; return true; };
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

class RS_CheckAndCreateAccount : public tBaseMsg
{ 
public:
    Int64 mID;		
    int mResult;		//  结果
    bool mbNew;		

public:
    RS_CheckAndCreateAccount() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        CheckGet(scrData, mID);
        CheckGet(scrData, mResult);
        CheckGet(scrData, mbNew);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mID"] = mID;
        destData["mResult"] = mResult;
        destData["mbNew"] = mbNew;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)3);

        SAVE_MSG_VALUE(mID, 8);
        SAVE_MSG_VALUE(mResult, 1);
        SAVE_MSG_VALUE(mbNew, 5);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mID = 0;
        mResult = 0;
        mbNew = false;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("RS_CheckAndCreateAccount", otherMsg.GetMsgName())!=0) { LOG("%s is not RS_CheckAndCreateAccount", otherMsg.GetMsgName()); return; }; const RS_CheckAndCreateAccount &other = *(const RS_CheckAndCreateAccount*)(&otherMsg);
        mID = other.mID;
        mResult = other.mResult;
        mbNew = other.mbNew;
    }

    virtual const char* GetMsgName() const override { return "RS_CheckAndCreateAccount"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mID")==0) { AData value; value = mID; return value; }
        if (strcmp(szMember, "mResult")==0) { AData value; value = mResult; return value; }
        if (strcmp(szMember, "mbNew")==0) { AData value; value = mbNew; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mID")==0) { mID = value; return true; };
        if (strcmp(szMember, "mResult")==0) { mResult = value; return true; };
        if (strcmp(szMember, "mbNew")==0) { mbNew = value; return true; };
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

//  请求游戏区创建玩家数据
class RQ_CreatePlayerData : public tBaseMsg
{ 
public:
    AString mAccount;		

public:
    RQ_CreatePlayerData() { clear(false); };


    void Full(AutoNice scrData)
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
        if (strcmp("RQ_CreatePlayerData", otherMsg.GetMsgName())!=0) { LOG("%s is not RQ_CreatePlayerData", otherMsg.GetMsgName()); return; }; const RQ_CreatePlayerData &other = *(const RQ_CreatePlayerData*)(&otherMsg);
        mAccount = other.mAccount;
    }

    virtual const char* GetMsgName() const override { return "RQ_CreatePlayerData"; }

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

class RS_CreatePlayerData : public tBaseMsg
{ 
public:
    Int64 mPlayerID;		

public:
    RS_CreatePlayerData() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        CheckGet(scrData, mPlayerID);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mPlayerID"] = mPlayerID;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)1);

        SAVE_MSG_VALUE(mPlayerID, 8);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mPlayerID = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("RS_CreatePlayerData", otherMsg.GetMsgName())!=0) { LOG("%s is not RS_CreatePlayerData", otherMsg.GetMsgName()); return; }; const RS_CreatePlayerData &other = *(const RS_CreatePlayerData*)(&otherMsg);
        mPlayerID = other.mPlayerID;
    }

    virtual const char* GetMsgName() const override { return "RS_CreatePlayerData"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mPlayerID")==0) { AData value; value = mPlayerID; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mPlayerID")==0) { mPlayerID = value; return true; };
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

//  请求实例PlayerActor
class RQ_CreatePlayerActor : public tBaseMsg
{ 
public:
    Int64 mPlayerID;		

public:
    RQ_CreatePlayerActor() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        CheckGet(scrData, mPlayerID);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mPlayerID"] = mPlayerID;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)1);

        SAVE_MSG_VALUE(mPlayerID, 8);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mPlayerID = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("RQ_CreatePlayerActor", otherMsg.GetMsgName())!=0) { LOG("%s is not RQ_CreatePlayerActor", otherMsg.GetMsgName()); return; }; const RQ_CreatePlayerActor &other = *(const RQ_CreatePlayerActor*)(&otherMsg);
        mPlayerID = other.mPlayerID;
    }

    virtual const char* GetMsgName() const override { return "RQ_CreatePlayerActor"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mPlayerID")==0) { AData value; value = mPlayerID; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mPlayerID")==0) { mPlayerID = value; return true; };
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

class RS_CreatePlayerActor : public tBaseMsg
{ 
public:
    int mResult;		

public:
    RS_CreatePlayerActor() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        CheckGet(scrData, mResult);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mResult"] = mResult;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)1);

        SAVE_MSG_VALUE(mResult, 1);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mResult = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("RS_CreatePlayerActor", otherMsg.GetMsgName())!=0) { LOG("%s is not RS_CreatePlayerActor", otherMsg.GetMsgName()); return; }; const RS_CreatePlayerActor &other = *(const RS_CreatePlayerActor*)(&otherMsg);
        mResult = other.mResult;
    }

    virtual const char* GetMsgName() const override { return "RS_CreatePlayerActor"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mResult")==0) { AData value; value = mResult; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mResult")==0) { mResult = value; return true; };
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

//  请求玩家对象基本数据
class RQ_PlayerBaseData : public tBaseMsg
{ 
public:

public:
    RQ_PlayerBaseData() { clear(false); };


    void Full(AutoNice scrData)
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
        if (strcmp("RQ_PlayerBaseData", otherMsg.GetMsgName())!=0) { LOG("%s is not RQ_PlayerBaseData", otherMsg.GetMsgName()); return; }; const RQ_PlayerBaseData &other = *(const RQ_PlayerBaseData*)(&otherMsg);
    }

    virtual const char* GetMsgName() const override { return "RQ_PlayerBaseData"; }

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

class RS_PlayerBaseData : public tBaseMsg
{ 
public:
    AutoNice mPlayerData;		

public:
    RS_PlayerBaseData() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        mPlayerData = (tNiceData*)scrData["mPlayerData"];
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mPlayerData"] = mPlayerData.getPtr();
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)1);

        SAVE_MSG_NICE(mPlayerData);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        if (mPlayerData) mPlayerData.setNull();
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("RS_PlayerBaseData", otherMsg.GetMsgName())!=0) { LOG("%s is not RS_PlayerBaseData", otherMsg.GetMsgName()); return; }; const RS_PlayerBaseData &other = *(const RS_PlayerBaseData*)(&otherMsg);
        COPY_MSG_NICE(other.mPlayerData, mPlayerData);
    }

    virtual const char* GetMsgName() const override { return "RS_PlayerBaseData"; }

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

