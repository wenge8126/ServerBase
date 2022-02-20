//Auto genereate msg data code		
#pragma once

#include "BaseMsg.h"
#include "Array.h"
#include "ArrayList.h"

class RQ_RequestMeshInfo : public tRequestMsg
{ 
public:
    UInt64 mNodeAddr;		
    int mRequestID;		

public:
    RQ_RequestMeshInfo() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        CheckGet(scrData, mNodeAddr);
        CheckGet(scrData, mRequestID);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mNodeAddr"] = mNodeAddr;
        destData["mRequestID"] = mRequestID;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)2);

        SAVE_MSG_VALUE(mNodeAddr, 18);
        SAVE_MSG_VALUE(mRequestID, 1);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mNodeAddr = 0;
        mRequestID = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("RQ_RequestMeshInfo", otherMsg.GetMsgName())!=0) { LOG("%s is not RQ_RequestMeshInfo", otherMsg.GetMsgName()); return; }; const RQ_RequestMeshInfo &other = *(const RQ_RequestMeshInfo*)(&otherMsg);
        mNodeAddr = other.mNodeAddr;
        mRequestID = other.mRequestID;
    }

    virtual const char* GetMsgName() const override { return "RQ_RequestMeshInfo"; }

    virtual void SetRequestID(MSG_ID id)  override { mRequestID = (int)id; }

    virtual MSG_ID GetRequestID()  override { return (MSG_ID)mRequestID; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mNodeAddr")==0) { AData value; value = mNodeAddr; return value; }
        if (strcmp(szMember, "mRequestID")==0) { AData value; value = mRequestID; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mNodeAddr")==0) { mNodeAddr = value; return true; };
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

public:
    RS_MeshNodeInfo() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        FullArray(scrData, mNodeList, "mNodeList");
    }

    virtual void ToData(AutoNice &destData) override
    {
        ArrayToData(destData, mNodeList, "mNodeList");
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)1);

        SaveArray("mNodeList", mNodeList, destData);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mNodeList.clear(false);
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("RS_MeshNodeInfo", otherMsg.GetMsgName())!=0) { LOG("%s is not RS_MeshNodeInfo", otherMsg.GetMsgName()); return; }; const RS_MeshNodeInfo &other = *(const RS_MeshNodeInfo*)(&otherMsg);
        mNodeList = other.mNodeList;
    }

    virtual const char* GetMsgName() const override { return "RS_MeshNodeInfo"; }

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

class _comment {
public:
    static const char* CommentString()
     {
        static const char *gCommentString = 
                "ewogICAiUlFfUmVxdWVzdE1lc2hJbmZvIiA6IHsKICAgICAgIl9zdHJ1Y3RfIiA6ICIiLAogICAgICAibU5vZGVBZGRyIiA6ICIiLAogICAgICAibVJlcXVlc3RJRCIgOiAiIgogICB9LAogICAiUlFfUmVxdWVzdE1lc2hJbmZvX19saXN0X18iIDogWyAibU5vZGVBZGRyIiBdLAogICAiUlFfUmVxdWVzdE1lc2hJbmZvX2luZm9fIiA6ICIiLAogICAiUlNfTWVzaE5vZGVJbmZvIiA6IHsKICAgICAgIl9zdHJ1Y3RfIiA6ICIiLAogICAgICAibU5vZGVMaXN0IiA6ICIiCiAgIH0sCiAgICJSU19NZXNoTm9kZUluZm9fX2xpc3RfXyIgOiBbICJtTm9kZUxpc3QiIF0sCiAgICJSU19NZXNoTm9kZUluZm9faW5mb18iIDogIiIKfQo=";
        return gCommentString; 
    }
 };

