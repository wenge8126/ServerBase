//Auto genereate msg data code		
#pragma once

#include "BaseMsg.h"
#include "Array.h"
#include "ArrayList.h"

class RQ_RequestMeshInfo : public tRequestMsg
{ 
public:
    UInt64 mNodeAddr;		
    int mNodeCode;		
    int mRequestID;		

public:
    RQ_RequestMeshInfo() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        CheckGet(scrData, mNodeAddr);
        CheckGet(scrData, mNodeCode);
        CheckGet(scrData, mRequestID);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mNodeAddr"] = mNodeAddr;
        destData["mNodeCode"] = mNodeCode;
        destData["mRequestID"] = mRequestID;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)3);

        SAVE_MSG_VALUE(mNodeAddr, 18);
        SAVE_MSG_VALUE(mNodeCode, 1);
        SAVE_MSG_VALUE(mRequestID, 1);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mNodeAddr = 0;
        mNodeCode = 0;
        mRequestID = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("RQ_RequestMeshInfo", otherMsg.GetMsgName())!=0) { LOG("%s is not RQ_RequestMeshInfo", otherMsg.GetMsgName()); return; }; const RQ_RequestMeshInfo &other = *(const RQ_RequestMeshInfo*)(&otherMsg);
        mNodeAddr = other.mNodeAddr;
        mNodeCode = other.mNodeCode;
        mRequestID = other.mRequestID;
    }

    virtual const char* GetMsgName() const override { return "RQ_RequestMeshInfo"; }

    virtual void SetRequestID(MSG_ID id)  override { mRequestID = (int)id; }

    virtual MSG_ID GetRequestID()  override { return (MSG_ID)mRequestID; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mNodeAddr")==0) { AData value; value = mNodeAddr; return value; }
        if (strcmp(szMember, "mNodeCode")==0) { AData value; value = mNodeCode; return value; }
        if (strcmp(szMember, "mRequestID")==0) { AData value; value = mRequestID; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mNodeAddr")==0) { mNodeAddr = value; return true; };
        if (strcmp(szMember, "mNodeCode")==0) { mNodeCode = value; return true; };
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

class RS_MeshNodeInfo : public tBaseMsg
{ 
public:
    ArrayList<UInt64> mNodeList;	
    int mTargetNodeCode;		

public:
    RS_MeshNodeInfo() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        FullArray(scrData, mNodeList, "mNodeList");
        CheckGet(scrData, mTargetNodeCode);
    }

    virtual void ToData(AutoNice &destData) override
    {
        ArrayToData(destData, mNodeList, "mNodeList");
        destData["mTargetNodeCode"] = mTargetNodeCode;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)2);

        SaveArray("mNodeList", mNodeList, destData);
        SAVE_MSG_VALUE(mTargetNodeCode, 1);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mNodeList.clear(false);
        mTargetNodeCode = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("RS_MeshNodeInfo", otherMsg.GetMsgName())!=0) { LOG("%s is not RS_MeshNodeInfo", otherMsg.GetMsgName()); return; }; const RS_MeshNodeInfo &other = *(const RS_MeshNodeInfo*)(&otherMsg);
        mNodeList = other.mNodeList;
        mTargetNodeCode = other.mTargetNodeCode;
    }

    virtual const char* GetMsgName() const override { return "RS_MeshNodeInfo"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mTargetNodeCode")==0) { AData value; value = mTargetNodeCode; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mTargetNodeCode")==0) { mTargetNodeCode = value; return true; };
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

//  广播节点关闭
class MS_BroadcastNodeClose : public tBaseMsg
{ 
public:
    UInt64 mNodeKey;		

public:
    MS_BroadcastNodeClose() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        CheckGet(scrData, mNodeKey);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mNodeKey"] = mNodeKey;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)1);

        SAVE_MSG_VALUE(mNodeKey, 18);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mNodeKey = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("MS_BroadcastNodeClose", otherMsg.GetMsgName())!=0) { LOG("%s is not MS_BroadcastNodeClose", otherMsg.GetMsgName()); return; }; const MS_BroadcastNodeClose &other = *(const MS_BroadcastNodeClose*)(&otherMsg);
        mNodeKey = other.mNodeKey;
    }

    virtual const char* GetMsgName() const override { return "MS_BroadcastNodeClose"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mNodeKey")==0) { AData value; value = mNodeKey; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mNodeKey")==0) { mNodeKey = value; return true; };
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

class NG_RequestGateInfo : public tRequestMsg
{ 
public:
    UInt64 mNodeKey;		
    int mRequestID;		

public:
    NG_RequestGateInfo() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        CheckGet(scrData, mNodeKey);
        CheckGet(scrData, mRequestID);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mNodeKey"] = mNodeKey;
        destData["mRequestID"] = mRequestID;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)2);

        SAVE_MSG_VALUE(mNodeKey, 18);
        SAVE_MSG_VALUE(mRequestID, 1);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mNodeKey = 0;
        mRequestID = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("NG_RequestGateInfo", otherMsg.GetMsgName())!=0) { LOG("%s is not NG_RequestGateInfo", otherMsg.GetMsgName()); return; }; const NG_RequestGateInfo &other = *(const NG_RequestGateInfo*)(&otherMsg);
        mNodeKey = other.mNodeKey;
        mRequestID = other.mRequestID;
    }

    virtual const char* GetMsgName() const override { return "NG_RequestGateInfo"; }

    virtual void SetRequestID(MSG_ID id)  override { mRequestID = (int)id; }

    virtual MSG_ID GetRequestID()  override { return (MSG_ID)mRequestID; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mNodeKey")==0) { AData value; value = mNodeKey; return value; }
        if (strcmp(szMember, "mRequestID")==0) { AData value; value = mRequestID; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mNodeKey")==0) { mNodeKey = value; return true; };
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

class GN_ResponseGateListInfo : public tBaseMsg
{ 
public:
    int mGateCode;		
    int mGateCount;		
    ArrayList<UInt64> mGateList;	

public:
    GN_ResponseGateListInfo() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        CheckGet(scrData, mGateCode);
        CheckGet(scrData, mGateCount);
        FullArray(scrData, mGateList, "mGateList");
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mGateCode"] = mGateCode;
        destData["mGateCount"] = mGateCount;
        ArrayToData(destData, mGateList, "mGateList");
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)3);

        SAVE_MSG_VALUE(mGateCode, 1);
        SAVE_MSG_VALUE(mGateCount, 1);
        SaveArray("mGateList", mGateList, destData);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mGateCode = 0;
        mGateCount = 0;
        mGateList.clear(false);
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("GN_ResponseGateListInfo", otherMsg.GetMsgName())!=0) { LOG("%s is not GN_ResponseGateListInfo", otherMsg.GetMsgName()); return; }; const GN_ResponseGateListInfo &other = *(const GN_ResponseGateListInfo*)(&otherMsg);
        mGateCode = other.mGateCode;
        mGateCount = other.mGateCount;
        mGateList = other.mGateList;
    }

    virtual const char* GetMsgName() const override { return "GN_ResponseGateListInfo"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mGateCode")==0) { AData value; value = mGateCode; return value; }
        if (strcmp(szMember, "mGateCount")==0) { AData value; value = mGateCount; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mGateCode")==0) { mGateCode = value; return true; };
        if (strcmp(szMember, "mGateCount")==0) { mGateCount = value; return true; };
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

//  追加Actor
class NG_AppendUnit : public tBaseMsg
{ 
public:
    UInt64 mUintID;		

public:
    NG_AppendUnit() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        CheckGet(scrData, mUintID);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mUintID"] = mUintID;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)1);

        SAVE_MSG_VALUE(mUintID, 18);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mUintID = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("NG_AppendUnit", otherMsg.GetMsgName())!=0) { LOG("%s is not NG_AppendUnit", otherMsg.GetMsgName()); return; }; const NG_AppendUnit &other = *(const NG_AppendUnit*)(&otherMsg);
        mUintID = other.mUintID;
    }

    virtual const char* GetMsgName() const override { return "NG_AppendUnit"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mUintID")==0) { AData value; value = mUintID; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mUintID")==0) { mUintID = value; return true; };
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

//  通知节点信息
class GN_NotifyNodeInfo : public tBaseMsg
{ 
public:
    UInt64 mNodeKey;		

public:
    GN_NotifyNodeInfo() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        CheckGet(scrData, mNodeKey);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mNodeKey"] = mNodeKey;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)1);

        SAVE_MSG_VALUE(mNodeKey, 18);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mNodeKey = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("GN_NotifyNodeInfo", otherMsg.GetMsgName())!=0) { LOG("%s is not GN_NotifyNodeInfo", otherMsg.GetMsgName()); return; }; const GN_NotifyNodeInfo &other = *(const GN_NotifyNodeInfo*)(&otherMsg);
        mNodeKey = other.mNodeKey;
    }

    virtual const char* GetMsgName() const override { return "GN_NotifyNodeInfo"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mNodeKey")==0) { AData value; value = mNodeKey; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mNodeKey")==0) { mNodeKey = value; return true; };
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

class NN_RequestUnitListInfo : public tRequestMsg
{ 
public:
    int mRequestID;		

public:
    NN_RequestUnitListInfo() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        CheckGet(scrData, mRequestID);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mRequestID"] = mRequestID;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)1);

        SAVE_MSG_VALUE(mRequestID, 1);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mRequestID = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("NN_RequestUnitListInfo", otherMsg.GetMsgName())!=0) { LOG("%s is not NN_RequestUnitListInfo", otherMsg.GetMsgName()); return; }; const NN_RequestUnitListInfo &other = *(const NN_RequestUnitListInfo*)(&otherMsg);
        mRequestID = other.mRequestID;
    }

    virtual const char* GetMsgName() const override { return "NN_RequestUnitListInfo"; }

    virtual void SetRequestID(MSG_ID id)  override { mRequestID = (int)id; }

    virtual MSG_ID GetRequestID()  override { return (MSG_ID)mRequestID; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mRequestID")==0) { AData value; value = mRequestID; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
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

class NN_ResponseUnitInfoList : public tBaseMsg
{ 
public:
    ArrayList<UInt64> mUnitList;	

public:
    NN_ResponseUnitInfoList() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        FullArray(scrData, mUnitList, "mUnitList");
    }

    virtual void ToData(AutoNice &destData) override
    {
        ArrayToData(destData, mUnitList, "mUnitList");
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)1);

        SaveArray("mUnitList", mUnitList, destData);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mUnitList.clear(false);
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("NN_ResponseUnitInfoList", otherMsg.GetMsgName())!=0) { LOG("%s is not NN_ResponseUnitInfoList", otherMsg.GetMsgName()); return; }; const NN_ResponseUnitInfoList &other = *(const NN_ResponseUnitInfoList*)(&otherMsg);
        mUnitList = other.mUnitList;
    }

    virtual const char* GetMsgName() const override { return "NN_ResponseUnitInfoList"; }

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

//  通知Unit不存在
class NG_NotifyUnitNoExist : public tBaseMsg
{ 
public:
    UInt64 mNoExistUnitID;		

public:
    NG_NotifyUnitNoExist() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        CheckGet(scrData, mNoExistUnitID);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mNoExistUnitID"] = mNoExistUnitID;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)1);

        SAVE_MSG_VALUE(mNoExistUnitID, 18);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mNoExistUnitID = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("NG_NotifyUnitNoExist", otherMsg.GetMsgName())!=0) { LOG("%s is not NG_NotifyUnitNoExist", otherMsg.GetMsgName()); return; }; const NG_NotifyUnitNoExist &other = *(const NG_NotifyUnitNoExist*)(&otherMsg);
        mNoExistUnitID = other.mNoExistUnitID;
    }

    virtual const char* GetMsgName() const override { return "NG_NotifyUnitNoExist"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mNoExistUnitID")==0) { AData value; value = mNoExistUnitID; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mNoExistUnitID")==0) { mNoExistUnitID = value; return true; };
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

//  广播其他节点Unit不存在
class NN_BroadcastUnitNoExist : public tBaseMsg
{ 
public:
    UInt64 mNoExistUnitID;		

public:
    NN_BroadcastUnitNoExist() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        CheckGet(scrData, mNoExistUnitID);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mNoExistUnitID"] = mNoExistUnitID;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)1);

        SAVE_MSG_VALUE(mNoExistUnitID, 18);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mNoExistUnitID = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("NN_BroadcastUnitNoExist", otherMsg.GetMsgName())!=0) { LOG("%s is not NN_BroadcastUnitNoExist", otherMsg.GetMsgName()); return; }; const NN_BroadcastUnitNoExist &other = *(const NN_BroadcastUnitNoExist*)(&otherMsg);
        mNoExistUnitID = other.mNoExistUnitID;
    }

    virtual const char* GetMsgName() const override { return "NN_BroadcastUnitNoExist"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mNoExistUnitID")==0) { AData value; value = mNoExistUnitID; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mNoExistUnitID")==0) { mNoExistUnitID = value; return true; };
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

//  通知节点关闭
class NG_NotifyNodeClose : public tBaseMsg
{ 
public:

public:
    NG_NotifyNodeClose() { clear(false); };


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
        if (strcmp("NG_NotifyNodeClose", otherMsg.GetMsgName())!=0) { LOG("%s is not NG_NotifyNodeClose", otherMsg.GetMsgName()); return; }; const NG_NotifyNodeClose &other = *(const NG_NotifyNodeClose*)(&otherMsg);
    }

    virtual const char* GetMsgName() const override { return "NG_NotifyNodeClose"; }

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

