//Auto genereate msg data code		
#pragma once

#include "BaseMsg.h"
#include "Array.h"
#include "ArrayList.h"

//  保存
class SQL_SaveNoSQLData : public tBaseMsg
{ 
public:
    AutoData mData;		
    AString mKey;		

public:
    SQL_SaveNoSQLData() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        mData = (DataStream*)scrData["mData"];
        CheckGet(scrData, mKey);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mData"] = mData.getPtr();
        destData["mKey"] = mKey;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)2);

        SAVE_MSG_DATA(mData);
        SAVE_MSG_VALUE(mKey, 4);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        if (mData) mData.setNull();
        mKey.setNull();
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("SQL_SaveNoSQLData", otherMsg.GetMsgName())!=0) { LOG("%s is not SQL_SaveNoSQLData", otherMsg.GetMsgName()); return; }; const SQL_SaveNoSQLData &other = *(const SQL_SaveNoSQLData*)(&otherMsg);
        COPY_MSG_DATA(other.mData, mData);
        mKey = other.mKey;
    }

    virtual const char* GetMsgName() const override { return "SQL_SaveNoSQLData"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mKey")==0) { AData value; value = mKey; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
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

//  请求提供字段数据
class SQL_RequestFieldData : public tBaseMsg
{ 
public:
    int mHash;		

public:
    SQL_RequestFieldData() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mHash);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mHash"] = mHash;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)1);

        SAVE_MSG_VALUE(mHash, 1);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mHash = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("SQL_RequestFieldData", otherMsg.GetMsgName())!=0) { LOG("%s is not SQL_RequestFieldData", otherMsg.GetMsgName()); return; }; const SQL_RequestFieldData &other = *(const SQL_RequestFieldData*)(&otherMsg);
        mHash = other.mHash;
    }

    virtual const char* GetMsgName() const override { return "SQL_RequestFieldData"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mHash")==0) { AData value; value = mHash; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mHash")==0) { mHash = value; return true; };
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

class SQL_ResponseFieldData : public tBaseMsg
{ 
public:
    AutoData mData;		

public:
    SQL_ResponseFieldData() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        mData = (DataStream*)scrData["mData"];
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mData"] = mData.getPtr();
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)1);

        SAVE_MSG_DATA(mData);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        if (mData) mData.setNull();
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("SQL_ResponseFieldData", otherMsg.GetMsgName())!=0) { LOG("%s is not SQL_ResponseFieldData", otherMsg.GetMsgName()); return; }; const SQL_ResponseFieldData &other = *(const SQL_ResponseFieldData*)(&otherMsg);
        COPY_MSG_DATA(other.mData, mData);
    }

    virtual const char* GetMsgName() const override { return "SQL_ResponseFieldData"; }

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

//  调取数据
class SQL_LoadNoSQLData : public tBaseMsg
{ 
public:
    AString mKey;		
    bool mbNeedField;		

public:
    SQL_LoadNoSQLData() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mKey);
        CheckGet(scrData, mbNeedField);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mKey"] = mKey;
        destData["mbNeedField"] = mbNeedField;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)2);

        SAVE_MSG_VALUE(mKey, 4);
        SAVE_MSG_VALUE(mbNeedField, 5);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mKey.setNull();
        mbNeedField = false;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("SQL_LoadNoSQLData", otherMsg.GetMsgName())!=0) { LOG("%s is not SQL_LoadNoSQLData", otherMsg.GetMsgName()); return; }; const SQL_LoadNoSQLData &other = *(const SQL_LoadNoSQLData*)(&otherMsg);
        mKey = other.mKey;
        mbNeedField = other.mbNeedField;
    }

    virtual const char* GetMsgName() const override { return "SQL_LoadNoSQLData"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mKey")==0) { AData value; value = mKey; return value; }
        if (strcmp(szMember, "mbNeedField")==0) { AData value; value = mbNeedField; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mKey")==0) { mKey = value; return true; };
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

//  回复数据
class SQL_ResponseNoSQLData : public tBaseMsg
{ 
public:
    AutoData mData;		
    AutoData mFieldData;		

public:
    SQL_ResponseNoSQLData() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        mData = (DataStream*)scrData["mData"];
        mFieldData = (DataStream*)scrData["mFieldData"];
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mData"] = mData.getPtr();
        destData["mFieldData"] = mFieldData.getPtr();
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)2);

        SAVE_MSG_DATA(mData);
        SAVE_MSG_DATA(mFieldData);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        if (mData) mData.setNull();
        if (mFieldData) mFieldData.setNull();
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("SQL_ResponseNoSQLData", otherMsg.GetMsgName())!=0) { LOG("%s is not SQL_ResponseNoSQLData", otherMsg.GetMsgName()); return; }; const SQL_ResponseNoSQLData &other = *(const SQL_ResponseNoSQLData*)(&otherMsg);
        COPY_MSG_DATA(other.mData, mData);
        COPY_MSG_DATA(other.mFieldData, mFieldData);
    }

    virtual const char* GetMsgName() const override { return "SQL_ResponseNoSQLData"; }

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

