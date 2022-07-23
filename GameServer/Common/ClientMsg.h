//Auto genereate msg data code		
#pragma once

#include "BaseMsg.h"
#include "Array.h"
#include "ArrayList.h"

class CS_ClientRequest : public tRequestMsg
{ 
public:
    int mRequestID;		
    AutoData mRequestMsgData;		
    AString mRequestMsgName;		
    UInt64 mTargetActorID;		

public:
    CS_ClientRequest() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mRequestID);
        mRequestMsgData = (DataStream*)scrData["mRequestMsgData"];
        CheckGet(scrData, mRequestMsgName);
        CheckGet(scrData, mTargetActorID);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mRequestID"] = mRequestID;
        destData["mRequestMsgData"] = mRequestMsgData.getPtr();
        destData["mRequestMsgName"] = mRequestMsgName;
        destData["mTargetActorID"] = mTargetActorID;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)4);

        SAVE_MSG_VALUE(mRequestID, 1);
        SAVE_MSG_DATA(mRequestMsgData);
        SAVE_MSG_VALUE(mRequestMsgName, 4);
        SAVE_MSG_VALUE(mTargetActorID, 18);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mRequestID = 0;
        if (mRequestMsgData) mRequestMsgData.setNull();
        mRequestMsgName.setNull();
        mTargetActorID = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("CS_ClientRequest", otherMsg.GetMsgName())!=0) { LOG("%s is not CS_ClientRequest", otherMsg.GetMsgName()); return; }; const CS_ClientRequest &other = *(const CS_ClientRequest*)(&otherMsg);
        mRequestID = other.mRequestID;
        COPY_MSG_DATA(other.mRequestMsgData, mRequestMsgData);
        mRequestMsgName = other.mRequestMsgName;
        mTargetActorID = other.mTargetActorID;
    }

    virtual const char* GetMsgName() const override { return "CS_ClientRequest"; }

    virtual void SetRequestID(MSG_ID id)  override { mRequestID = (int)id; }

    virtual MSG_ID GetRequestID()  override { return (MSG_ID)mRequestID; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mRequestID")==0) { AData value; value = mRequestID; return value; }
        if (strcmp(szMember, "mRequestMsgName")==0) { AData value; value = mRequestMsgName; return value; }
        if (strcmp(szMember, "mTargetActorID")==0) { AData value; value = mTargetActorID; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mRequestID")==0) { mRequestID = value; return true; };
        if (strcmp(szMember, "mRequestMsgName")==0) { mRequestMsgName = value; return true; };
        if (strcmp(szMember, "mTargetActorID")==0) { mTargetActorID = value; return true; };
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

//  服务器内部Actor请求客户端消息发向ClientActor, 再通过ClientActor 请求客户端SC_ServerRequestClientMsg
class SC_ActorRequestClientMsg : public tBaseMsg
{ 
public:
    UInt64 mClientActorID;		
    AutoData mRequestMsgData;		
    AString mRequestMsgName;		

public:
    SC_ActorRequestClientMsg() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mClientActorID);
        mRequestMsgData = (DataStream*)scrData["mRequestMsgData"];
        CheckGet(scrData, mRequestMsgName);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mClientActorID"] = mClientActorID;
        destData["mRequestMsgData"] = mRequestMsgData.getPtr();
        destData["mRequestMsgName"] = mRequestMsgName;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)3);

        SAVE_MSG_VALUE(mClientActorID, 18);
        SAVE_MSG_DATA(mRequestMsgData);
        SAVE_MSG_VALUE(mRequestMsgName, 4);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mClientActorID = 0;
        if (mRequestMsgData) mRequestMsgData.setNull();
        mRequestMsgName.setNull();
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("SC_ActorRequestClientMsg", otherMsg.GetMsgName())!=0) { LOG("%s is not SC_ActorRequestClientMsg", otherMsg.GetMsgName()); return; }; const SC_ActorRequestClientMsg &other = *(const SC_ActorRequestClientMsg*)(&otherMsg);
        mClientActorID = other.mClientActorID;
        COPY_MSG_DATA(other.mRequestMsgData, mRequestMsgData);
        mRequestMsgName = other.mRequestMsgName;
    }

    virtual const char* GetMsgName() const override { return "SC_ActorRequestClientMsg"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mClientActorID")==0) { AData value; value = mClientActorID; return value; }
        if (strcmp(szMember, "mRequestMsgName")==0) { AData value; value = mRequestMsgName; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mClientActorID")==0) { mClientActorID = value; return true; };
        if (strcmp(szMember, "mRequestMsgName")==0) { mRequestMsgName = value; return true; };
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

class CS_ResponceServerActorMsg : public tBaseMsg
{ 
public:
    AutoData mResponseMsgData;		

public:
    CS_ResponceServerActorMsg() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        mResponseMsgData = (DataStream*)scrData["mResponseMsgData"];
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mResponseMsgData"] = mResponseMsgData.getPtr();
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)1);

        SAVE_MSG_DATA(mResponseMsgData);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        if (mResponseMsgData) mResponseMsgData.setNull();
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("CS_ResponceServerActorMsg", otherMsg.GetMsgName())!=0) { LOG("%s is not CS_ResponceServerActorMsg", otherMsg.GetMsgName()); return; }; const CS_ResponceServerActorMsg &other = *(const CS_ResponceServerActorMsg*)(&otherMsg);
        COPY_MSG_DATA(other.mResponseMsgData, mResponseMsgData);
    }

    virtual const char* GetMsgName() const override { return "CS_ResponceServerActorMsg"; }

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

class SC_ResponseMsg : public tBaseMsg
{ 
public:
    AutoNice mResponseData;		

public:
    SC_ResponseMsg() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        mResponseData = (tNiceData*)scrData["mResponseData"];
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mResponseData"] = mResponseData.getPtr();
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)1);

        SAVE_MSG_NICE(mResponseData);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        if (mResponseData) mResponseData.setNull();
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("SC_ResponseMsg", otherMsg.GetMsgName())!=0) { LOG("%s is not SC_ResponseMsg", otherMsg.GetMsgName()); return; }; const SC_ResponseMsg &other = *(const SC_ResponseMsg*)(&otherMsg);
        COPY_MSG_NICE(other.mResponseData, mResponseData);
    }

    virtual const char* GetMsgName() const override { return "SC_ResponseMsg"; }

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

class SC_ResponseTest : public tBaseMsg
{ 
public:
    AString mInfo;		

public:
    SC_ResponseTest() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mInfo);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mInfo"] = mInfo;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)1);

        SAVE_MSG_VALUE(mInfo, 4);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mInfo.setNull();
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("SC_ResponseTest", otherMsg.GetMsgName())!=0) { LOG("%s is not SC_ResponseTest", otherMsg.GetMsgName()); return; }; const SC_ResponseTest &other = *(const SC_ResponseTest*)(&otherMsg);
        mInfo = other.mInfo;
    }

    virtual const char* GetMsgName() const override { return "SC_ResponseTest"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mInfo")==0) { AData value; value = mInfo; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
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

class CS_RequestTest : public tRequestMsg
{ 
public:
    AString mInfo;		
    int mRequestID;		

public:
    CS_RequestTest() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mInfo);
        CheckGet(scrData, mRequestID);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mInfo"] = mInfo;
        destData["mRequestID"] = mRequestID;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)2);

        SAVE_MSG_VALUE(mInfo, 4);
        SAVE_MSG_VALUE(mRequestID, 1);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mInfo.setNull();
        mRequestID = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("CS_RequestTest", otherMsg.GetMsgName())!=0) { LOG("%s is not CS_RequestTest", otherMsg.GetMsgName()); return; }; const CS_RequestTest &other = *(const CS_RequestTest*)(&otherMsg);
        mInfo = other.mInfo;
        mRequestID = other.mRequestID;
    }

    virtual const char* GetMsgName() const override { return "CS_RequestTest"; }

    virtual void SetRequestID(MSG_ID id)  override { mRequestID = (int)id; }

    virtual MSG_ID GetRequestID()  override { return (MSG_ID)mRequestID; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mInfo")==0) { AData value; value = mInfo; return value; }
        if (strcmp(szMember, "mRequestID")==0) { AData value; value = mRequestID; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mInfo")==0) { mInfo = value; return true; };
        if (strcmp(szMember, "mRequestID")==0) { mRequestID = value; return true; };
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

class MSG_Test : public tBaseMsg
{ 
public:
    AString mTest;		

public:
    MSG_Test() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mTest);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mTest"] = mTest;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)1);

        SAVE_MSG_VALUE(mTest, 4);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mTest.setNull();
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("MSG_Test", otherMsg.GetMsgName())!=0) { LOG("%s is not MSG_Test", otherMsg.GetMsgName()); return; }; const MSG_Test &other = *(const MSG_Test*)(&otherMsg);
        mTest = other.mTest;
    }

    virtual const char* GetMsgName() const override { return "MSG_Test"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mTest")==0) { AData value; value = mTest; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mTest")==0) { mTest = value; return true; };
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

