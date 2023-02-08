//Auto genereate msg data code		
#pragma once

#include "BaseMsg.h"
#include "Array.h"
#include "ArrayList.h"

//  请求加入到购物车
class CG_RequestAddCart : public tBaseMsg
{ 
public:
    int mCount;		
    Int64 mGoodsID;		
    AutoNice mParam;		

public:
    CG_RequestAddCart() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mCount);
        CheckGet(scrData, mGoodsID);
        mParam = (tNiceData*)scrData["mParam"];
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mCount"] = mCount;
        destData["mGoodsID"] = mGoodsID;
        destData["mParam"] = mParam.getPtr();
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)3);

        SAVE_MSG_VALUE(mCount, 1);
        SAVE_MSG_VALUE(mGoodsID, 8);
        SAVE_MSG_NICE(mParam);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mCount = 0;
        mGoodsID = 0;
        if (mParam) mParam.setNull();
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("CG_RequestAddCart", otherMsg.GetMsgName())!=0) { LOG("%s is not CG_RequestAddCart", otherMsg.GetMsgName()); return; }; const CG_RequestAddCart &other = *(const CG_RequestAddCart*)(&otherMsg);
        mCount = other.mCount;
        mGoodsID = other.mGoodsID;
        COPY_MSG_NICE(other.mParam, mParam);
    }

    virtual const char* GetMsgName() const override { return "CG_RequestAddCart"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mCount")==0) { AData value; value = mCount; return value; }
        if (strcmp(szMember, "mGoodsID")==0) { AData value; value = mGoodsID; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mCount")==0) { mCount = value; return true; };
        if (strcmp(szMember, "mGoodsID")==0) { mGoodsID = value; return true; };
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

class CG_ReponseAddCart : public tBaseMsg
{ 
public:
    Int64 mBuyID;		
    int mError;		

public:
    CG_ReponseAddCart() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mBuyID);
        CheckGet(scrData, mError);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mBuyID"] = mBuyID;
        destData["mError"] = mError;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)2);

        SAVE_MSG_VALUE(mBuyID, 8);
        SAVE_MSG_VALUE(mError, 1);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mBuyID = 0;
        mError = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("CG_ReponseAddCart", otherMsg.GetMsgName())!=0) { LOG("%s is not CG_ReponseAddCart", otherMsg.GetMsgName()); return; }; const CG_ReponseAddCart &other = *(const CG_ReponseAddCart*)(&otherMsg);
        mBuyID = other.mBuyID;
        mError = other.mError;
    }

    virtual const char* GetMsgName() const override { return "CG_ReponseAddCart"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mBuyID")==0) { AData value; value = mBuyID; return value; }
        if (strcmp(szMember, "mError")==0) { AData value; value = mError; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mBuyID")==0) { mBuyID = value; return true; };
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

