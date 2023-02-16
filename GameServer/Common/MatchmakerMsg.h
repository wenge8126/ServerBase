//Auto genereate msg data code		
#pragma once

#include "BaseMsg.h"
#include "Array.h"
#include "ArrayList.h"

//  请求一个公用TOKEN,用于开放数据
class CS_GetPublicToken : public tBaseMsg
{ 
public:

public:
    CS_GetPublicToken() { clear(false); };


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
        if (strcmp("CS_GetPublicToken", otherMsg.GetMsgName())!=0) { LOG("%s is not CS_GetPublicToken", otherMsg.GetMsgName()); return; }; const CS_GetPublicToken &other = *(const CS_GetPublicToken*)(&otherMsg);
    }

    virtual const char* GetMsgName() const override { return "CS_GetPublicToken"; }

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

class CS_ResponsePublicToken : public tBaseMsg
{ 
public:
    int mError;		
    AutoData mToken;		

public:
    CS_ResponsePublicToken() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mError);
        mToken = (DataStream*)scrData["mToken"];
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mError"] = mError;
        destData["mToken"] = mToken.getPtr();
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)2);

        SAVE_MSG_VALUE(mError, 1);
        SAVE_MSG_DATA(mToken);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mError = 0;
        if (mToken) mToken.setNull();
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("CS_ResponsePublicToken", otherMsg.GetMsgName())!=0) { LOG("%s is not CS_ResponsePublicToken", otherMsg.GetMsgName()); return; }; const CS_ResponsePublicToken &other = *(const CS_ResponsePublicToken*)(&otherMsg);
        mError = other.mError;
        COPY_MSG_DATA(other.mToken, mToken);
    }

    virtual const char* GetMsgName() const override { return "CS_ResponsePublicToken"; }

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

//  请求发送验证码到手机
class CS_RequestSendCheckCode : public tBaseMsg
{ 
public:
    Int64 mPhone;		

public:
    CS_RequestSendCheckCode() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mPhone);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mPhone"] = mPhone;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)1);

        SAVE_MSG_VALUE(mPhone, 8);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mPhone = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("CS_RequestSendCheckCode", otherMsg.GetMsgName())!=0) { LOG("%s is not CS_RequestSendCheckCode", otherMsg.GetMsgName()); return; }; const CS_RequestSendCheckCode &other = *(const CS_RequestSendCheckCode*)(&otherMsg);
        mPhone = other.mPhone;
    }

    virtual const char* GetMsgName() const override { return "CS_RequestSendCheckCode"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mPhone")==0) { AData value; value = mPhone; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mPhone")==0) { mPhone = value; return true; };
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

class CS_ResponseSendCheckCode : public tBaseMsg
{ 
public:
    int mError;		

public:
    CS_ResponseSendCheckCode() { clear(false); };


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
        if (strcmp("CS_ResponseSendCheckCode", otherMsg.GetMsgName())!=0) { LOG("%s is not CS_ResponseSendCheckCode", otherMsg.GetMsgName()); return; }; const CS_ResponseSendCheckCode &other = *(const CS_ResponseSendCheckCode*)(&otherMsg);
        mError = other.mError;
    }

    virtual const char* GetMsgName() const override { return "CS_ResponseSendCheckCode"; }

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

//  请求创建用户
class CS_ReqCreateUser : public tBaseMsg
{ 
public:
    AString mCheckCode;		
    Int64 mPhone;		
    bool mbCheckManager;		//  是否检查为管理员

public:
    CS_ReqCreateUser() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mCheckCode);
        CheckGet(scrData, mPhone);
        CheckGet(scrData, mbCheckManager);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mCheckCode"] = mCheckCode;
        destData["mPhone"] = mPhone;
        destData["mbCheckManager"] = mbCheckManager;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)3);

        SAVE_MSG_VALUE(mCheckCode, 4);
        SAVE_MSG_VALUE(mPhone, 8);
        SAVE_MSG_VALUE(mbCheckManager, 5);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mCheckCode.setNull();
        mPhone = 0;
        mbCheckManager = false;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("CS_ReqCreateUser", otherMsg.GetMsgName())!=0) { LOG("%s is not CS_ReqCreateUser", otherMsg.GetMsgName()); return; }; const CS_ReqCreateUser &other = *(const CS_ReqCreateUser*)(&otherMsg);
        mCheckCode = other.mCheckCode;
        mPhone = other.mPhone;
        mbCheckManager = other.mbCheckManager;
    }

    virtual const char* GetMsgName() const override { return "CS_ReqCreateUser"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mCheckCode")==0) { AData value; value = mCheckCode; return value; }
        if (strcmp(szMember, "mPhone")==0) { AData value; value = mPhone; return value; }
        if (strcmp(szMember, "mbCheckManager")==0) { AData value; value = mbCheckManager; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mCheckCode")==0) { mCheckCode = value; return true; };
        if (strcmp(szMember, "mPhone")==0) { mPhone = value; return true; };
        if (strcmp(szMember, "mbCheckManager")==0) { mbCheckManager = value; return true; };
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

class CS_RespCreateUserResult : public tBaseMsg
{ 
public:
    int mError;		
    AutoData mToken;		//  Token数据
    Int64 mUserID;		
    bool mbNewUser;		//  是否为新用户

public:
    CS_RespCreateUserResult() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mError);
        mToken = (DataStream*)scrData["mToken"];
        CheckGet(scrData, mUserID);
        CheckGet(scrData, mbNewUser);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mError"] = mError;
        destData["mToken"] = mToken.getPtr();
        destData["mUserID"] = mUserID;
        destData["mbNewUser"] = mbNewUser;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)4);

        SAVE_MSG_VALUE(mError, 1);
        SAVE_MSG_DATA(mToken);
        SAVE_MSG_VALUE(mUserID, 8);
        SAVE_MSG_VALUE(mbNewUser, 5);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mError = 0;
        if (mToken) mToken.setNull();
        mUserID = 0;
        mbNewUser = false;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("CS_RespCreateUserResult", otherMsg.GetMsgName())!=0) { LOG("%s is not CS_RespCreateUserResult", otherMsg.GetMsgName()); return; }; const CS_RespCreateUserResult &other = *(const CS_RespCreateUserResult*)(&otherMsg);
        mError = other.mError;
        COPY_MSG_DATA(other.mToken, mToken);
        mUserID = other.mUserID;
        mbNewUser = other.mbNewUser;
    }

    virtual const char* GetMsgName() const override { return "CS_RespCreateUserResult"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mError")==0) { AData value; value = mError; return value; }
        if (strcmp(szMember, "mUserID")==0) { AData value; value = mUserID; return value; }
        if (strcmp(szMember, "mbNewUser")==0) { AData value; value = mbNewUser; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mError")==0) { mError = value; return true; };
        if (strcmp(szMember, "mUserID")==0) { mUserID = value; return true; };
        if (strcmp(szMember, "mbNewUser")==0) { mbNewUser = value; return true; };
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

//  应用保存推荐人手机号
class CS_ApplySaveTJPhone : public tBaseMsg
{ 
public:
    Int64 mDBID;		//  自己的ID
    Int64 mPhone;		//  推荐人的手机号码

public:
    CS_ApplySaveTJPhone() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mDBID);
        CheckGet(scrData, mPhone);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mDBID"] = mDBID;
        destData["mPhone"] = mPhone;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)2);

        SAVE_MSG_VALUE(mDBID, 8);
        SAVE_MSG_VALUE(mPhone, 8);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mDBID = 0;
        mPhone = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("CS_ApplySaveTJPhone", otherMsg.GetMsgName())!=0) { LOG("%s is not CS_ApplySaveTJPhone", otherMsg.GetMsgName()); return; }; const CS_ApplySaveTJPhone &other = *(const CS_ApplySaveTJPhone*)(&otherMsg);
        mDBID = other.mDBID;
        mPhone = other.mPhone;
    }

    virtual const char* GetMsgName() const override { return "CS_ApplySaveTJPhone"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mDBID")==0) { AData value; value = mDBID; return value; }
        if (strcmp(szMember, "mPhone")==0) { AData value; value = mPhone; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mDBID")==0) { mDBID = value; return true; };
        if (strcmp(szMember, "mPhone")==0) { mPhone = value; return true; };
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

class CS_ResponseApplyTJPhone : public tBaseMsg
{ 
public:
    int mError;		
    int mRewardRMB;		//  奖奖励金额分

public:
    CS_ResponseApplyTJPhone() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mError);
        CheckGet(scrData, mRewardRMB);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mError"] = mError;
        destData["mRewardRMB"] = mRewardRMB;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)2);

        SAVE_MSG_VALUE(mError, 1);
        SAVE_MSG_VALUE(mRewardRMB, 1);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mError = 0;
        mRewardRMB = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("CS_ResponseApplyTJPhone", otherMsg.GetMsgName())!=0) { LOG("%s is not CS_ResponseApplyTJPhone", otherMsg.GetMsgName()); return; }; const CS_ResponseApplyTJPhone &other = *(const CS_ResponseApplyTJPhone*)(&otherMsg);
        mError = other.mError;
        mRewardRMB = other.mRewardRMB;
    }

    virtual const char* GetMsgName() const override { return "CS_ResponseApplyTJPhone"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mError")==0) { AData value; value = mError; return value; }
        if (strcmp(szMember, "mRewardRMB")==0) { AData value; value = mRewardRMB; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mError")==0) { mError = value; return true; };
        if (strcmp(szMember, "mRewardRMB")==0) { mRewardRMB = value; return true; };
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

//  请求获取用户ID
class CS_ReqUserID : public tBaseMsg
{ 
public:
    AString mCheckCode;		
    Int64 mPhone;		

public:
    CS_ReqUserID() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mCheckCode);
        CheckGet(scrData, mPhone);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mCheckCode"] = mCheckCode;
        destData["mPhone"] = mPhone;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)2);

        SAVE_MSG_VALUE(mCheckCode, 4);
        SAVE_MSG_VALUE(mPhone, 8);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mCheckCode.setNull();
        mPhone = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("CS_ReqUserID", otherMsg.GetMsgName())!=0) { LOG("%s is not CS_ReqUserID", otherMsg.GetMsgName()); return; }; const CS_ReqUserID &other = *(const CS_ReqUserID*)(&otherMsg);
        mCheckCode = other.mCheckCode;
        mPhone = other.mPhone;
    }

    virtual const char* GetMsgName() const override { return "CS_ReqUserID"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mCheckCode")==0) { AData value; value = mCheckCode; return value; }
        if (strcmp(szMember, "mPhone")==0) { AData value; value = mPhone; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mCheckCode")==0) { mCheckCode = value; return true; };
        if (strcmp(szMember, "mPhone")==0) { mPhone = value; return true; };
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

class CS_RespUserID : public tBaseMsg
{ 
public:
    int mError;		
    Int64 mUserID;		

public:
    CS_RespUserID() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mError);
        CheckGet(scrData, mUserID);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mError"] = mError;
        destData["mUserID"] = mUserID;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)2);

        SAVE_MSG_VALUE(mError, 1);
        SAVE_MSG_VALUE(mUserID, 8);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mError = 0;
        mUserID = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("CS_RespUserID", otherMsg.GetMsgName())!=0) { LOG("%s is not CS_RespUserID", otherMsg.GetMsgName()); return; }; const CS_RespUserID &other = *(const CS_RespUserID*)(&otherMsg);
        mError = other.mError;
        mUserID = other.mUserID;
    }

    virtual const char* GetMsgName() const override { return "CS_RespUserID"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mError")==0) { AData value; value = mError; return value; }
        if (strcmp(szMember, "mUserID")==0) { AData value; value = mUserID; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mError")==0) { mError = value; return true; };
        if (strcmp(szMember, "mUserID")==0) { mUserID = value; return true; };
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

//  统一回复结果
class CS_ResponseResult : public tBaseMsg
{ 
public:
    int mError;		

public:
    CS_ResponseResult() { clear(false); };


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
        if (strcmp("CS_ResponseResult", otherMsg.GetMsgName())!=0) { LOG("%s is not CS_ResponseResult", otherMsg.GetMsgName()); return; }; const CS_ResponseResult &other = *(const CS_ResponseResult*)(&otherMsg);
        mError = other.mError;
    }

    virtual const char* GetMsgName() const override { return "CS_ResponseResult"; }

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

//  请求加入到喜欢关注列表
class CS_RequestAddLike : public tBaseMsg
{ 
public:
    Int64 mTargetID;		

public:
    CS_RequestAddLike() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mTargetID);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mTargetID"] = mTargetID;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)1);

        SAVE_MSG_VALUE(mTargetID, 8);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mTargetID = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("CS_RequestAddLike", otherMsg.GetMsgName())!=0) { LOG("%s is not CS_RequestAddLike", otherMsg.GetMsgName()); return; }; const CS_RequestAddLike &other = *(const CS_RequestAddLike*)(&otherMsg);
        mTargetID = other.mTargetID;
    }

    virtual const char* GetMsgName() const override { return "CS_RequestAddLike"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mTargetID")==0) { AData value; value = mTargetID; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mTargetID")==0) { mTargetID = value; return true; };
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

//  移除喜欢
class CS_ReqRemoveLike : public tBaseMsg
{ 
public:
    Int64 mTargetID;		

public:
    CS_ReqRemoveLike() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mTargetID);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mTargetID"] = mTargetID;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)1);

        SAVE_MSG_VALUE(mTargetID, 8);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mTargetID = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("CS_ReqRemoveLike", otherMsg.GetMsgName())!=0) { LOG("%s is not CS_ReqRemoveLike", otherMsg.GetMsgName()); return; }; const CS_ReqRemoveLike &other = *(const CS_ReqRemoveLike*)(&otherMsg);
        mTargetID = other.mTargetID;
    }

    virtual const char* GetMsgName() const override { return "CS_ReqRemoveLike"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mTargetID")==0) { AData value; value = mTargetID; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mTargetID")==0) { mTargetID = value; return true; };
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

class CS_RequestGiveGift : public tBaseMsg
{ 
public:
    Int64 mDBID;		
    int mGiftCount;		
    int mGiftType;		
    Int64 mTargetID;		

public:
    CS_RequestGiveGift() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mDBID);
        CheckGet(scrData, mGiftCount);
        CheckGet(scrData, mGiftType);
        CheckGet(scrData, mTargetID);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mDBID"] = mDBID;
        destData["mGiftCount"] = mGiftCount;
        destData["mGiftType"] = mGiftType;
        destData["mTargetID"] = mTargetID;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)4);

        SAVE_MSG_VALUE(mDBID, 8);
        SAVE_MSG_VALUE(mGiftCount, 1);
        SAVE_MSG_VALUE(mGiftType, 1);
        SAVE_MSG_VALUE(mTargetID, 8);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mDBID = 0;
        mGiftCount = 0;
        mGiftType = 0;
        mTargetID = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("CS_RequestGiveGift", otherMsg.GetMsgName())!=0) { LOG("%s is not CS_RequestGiveGift", otherMsg.GetMsgName()); return; }; const CS_RequestGiveGift &other = *(const CS_RequestGiveGift*)(&otherMsg);
        mDBID = other.mDBID;
        mGiftCount = other.mGiftCount;
        mGiftType = other.mGiftType;
        mTargetID = other.mTargetID;
    }

    virtual const char* GetMsgName() const override { return "CS_RequestGiveGift"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mDBID")==0) { AData value; value = mDBID; return value; }
        if (strcmp(szMember, "mGiftCount")==0) { AData value; value = mGiftCount; return value; }
        if (strcmp(szMember, "mGiftType")==0) { AData value; value = mGiftType; return value; }
        if (strcmp(szMember, "mTargetID")==0) { AData value; value = mTargetID; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mDBID")==0) { mDBID = value; return true; };
        if (strcmp(szMember, "mGiftCount")==0) { mGiftCount = value; return true; };
        if (strcmp(szMember, "mGiftType")==0) { mGiftType = value; return true; };
        if (strcmp(szMember, "mTargetID")==0) { mTargetID = value; return true; };
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

//  请求更新保存展示数据
class CS_ReqSaveShowData : public tBaseMsg
{ 
public:
    Int64 mDBID;		
    AutoNice mShowData;		
    bool mbNews;		

public:
    CS_ReqSaveShowData() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mDBID);
        mShowData = (tNiceData*)scrData["mShowData"];
        CheckGet(scrData, mbNews);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mDBID"] = mDBID;
        destData["mShowData"] = mShowData.getPtr();
        destData["mbNews"] = mbNews;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)3);

        SAVE_MSG_VALUE(mDBID, 8);
        SAVE_MSG_NICE(mShowData);
        SAVE_MSG_VALUE(mbNews, 5);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mDBID = 0;
        if (mShowData) mShowData.setNull();
        mbNews = false;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("CS_ReqSaveShowData", otherMsg.GetMsgName())!=0) { LOG("%s is not CS_ReqSaveShowData", otherMsg.GetMsgName()); return; }; const CS_ReqSaveShowData &other = *(const CS_ReqSaveShowData*)(&otherMsg);
        mDBID = other.mDBID;
        COPY_MSG_NICE(other.mShowData, mShowData);
        mbNews = other.mbNews;
    }

    virtual const char* GetMsgName() const override { return "CS_ReqSaveShowData"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mDBID")==0) { AData value; value = mDBID; return value; }
        if (strcmp(szMember, "mbNews")==0) { AData value; value = mbNews; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mDBID")==0) { mDBID = value; return true; };
        if (strcmp(szMember, "mbNews")==0) { mbNews = value; return true; };
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

//  请求获取展示数据
class CS_ReqLoadShowData : public tBaseMsg
{ 
public:
    Int64 mDBID;		

public:
    CS_ReqLoadShowData() { clear(false); };


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
        if (strcmp("CS_ReqLoadShowData", otherMsg.GetMsgName())!=0) { LOG("%s is not CS_ReqLoadShowData", otherMsg.GetMsgName()); return; }; const CS_ReqLoadShowData &other = *(const CS_ReqLoadShowData*)(&otherMsg);
        mDBID = other.mDBID;
    }

    virtual const char* GetMsgName() const override { return "CS_ReqLoadShowData"; }

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

//  统一回复获取数据
class CS_RespLoadData : public tBaseMsg
{ 
public:
    AutoNice mData;		
    int mError;		

public:
    CS_RespLoadData() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        mData = (tNiceData*)scrData["mData"];
        CheckGet(scrData, mError);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mData"] = mData.getPtr();
        destData["mError"] = mError;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)2);

        SAVE_MSG_NICE(mData);
        SAVE_MSG_VALUE(mError, 1);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        if (mData) mData.setNull();
        mError = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("CS_RespLoadData", otherMsg.GetMsgName())!=0) { LOG("%s is not CS_RespLoadData", otherMsg.GetMsgName()); return; }; const CS_RespLoadData &other = *(const CS_RespLoadData*)(&otherMsg);
        COPY_MSG_NICE(other.mData, mData);
        mError = other.mError;
    }

    virtual const char* GetMsgName() const override { return "CS_RespLoadData"; }

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

//  请求更新保存个人数据
class CS_ReqSaveUserData : public tBaseMsg
{ 
public:
    Int64 mDBID;		
    AutoNice mUserData;		

public:
    CS_ReqSaveUserData() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mDBID);
        mUserData = (tNiceData*)scrData["mUserData"];
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mDBID"] = mDBID;
        destData["mUserData"] = mUserData.getPtr();
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)2);

        SAVE_MSG_VALUE(mDBID, 8);
        SAVE_MSG_NICE(mUserData);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mDBID = 0;
        if (mUserData) mUserData.setNull();
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("CS_ReqSaveUserData", otherMsg.GetMsgName())!=0) { LOG("%s is not CS_ReqSaveUserData", otherMsg.GetMsgName()); return; }; const CS_ReqSaveUserData &other = *(const CS_ReqSaveUserData*)(&otherMsg);
        mDBID = other.mDBID;
        COPY_MSG_NICE(other.mUserData, mUserData);
    }

    virtual const char* GetMsgName() const override { return "CS_ReqSaveUserData"; }

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

//  请求获取展示数据
class CS_ReqLoadUserData : public tBaseMsg
{ 
public:
    Int64 mDBID;		

public:
    CS_ReqLoadUserData() { clear(false); };


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
        if (strcmp("CS_ReqLoadUserData", otherMsg.GetMsgName())!=0) { LOG("%s is not CS_ReqLoadUserData", otherMsg.GetMsgName()); return; }; const CS_ReqLoadUserData &other = *(const CS_ReqLoadUserData*)(&otherMsg);
        mDBID = other.mDBID;
    }

    virtual const char* GetMsgName() const override { return "CS_ReqLoadUserData"; }

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

//  发送聊天消息
class CS_SendTalkMsg : public tBaseMsg
{ 
public:
    AString mMsgText;		
    Int64 mSender;		
    Int64 mTargetID;		

public:
    CS_SendTalkMsg() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mMsgText);
        CheckGet(scrData, mSender);
        CheckGet(scrData, mTargetID);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mMsgText"] = mMsgText;
        destData["mSender"] = mSender;
        destData["mTargetID"] = mTargetID;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)3);

        SAVE_MSG_VALUE(mMsgText, 4);
        SAVE_MSG_VALUE(mSender, 8);
        SAVE_MSG_VALUE(mTargetID, 8);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mMsgText.setNull();
        mSender = 0;
        mTargetID = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("CS_SendTalkMsg", otherMsg.GetMsgName())!=0) { LOG("%s is not CS_SendTalkMsg", otherMsg.GetMsgName()); return; }; const CS_SendTalkMsg &other = *(const CS_SendTalkMsg*)(&otherMsg);
        mMsgText = other.mMsgText;
        mSender = other.mSender;
        mTargetID = other.mTargetID;
    }

    virtual const char* GetMsgName() const override { return "CS_SendTalkMsg"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mMsgText")==0) { AData value; value = mMsgText; return value; }
        if (strcmp(szMember, "mSender")==0) { AData value; value = mSender; return value; }
        if (strcmp(szMember, "mTargetID")==0) { AData value; value = mTargetID; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mMsgText")==0) { mMsgText = value; return true; };
        if (strcmp(szMember, "mSender")==0) { mSender = value; return true; };
        if (strcmp(szMember, "mTargetID")==0) { mTargetID = value; return true; };
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

//  获取发向目标者的消息
class CS_GetSendToTargetMsg : public tBaseMsg
{ 
public:
    Int64 mSender;		
    Int64 mTarget;		

public:
    CS_GetSendToTargetMsg() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mSender);
        CheckGet(scrData, mTarget);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mSender"] = mSender;
        destData["mTarget"] = mTarget;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)2);

        SAVE_MSG_VALUE(mSender, 8);
        SAVE_MSG_VALUE(mTarget, 8);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mSender = 0;
        mTarget = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("CS_GetSendToTargetMsg", otherMsg.GetMsgName())!=0) { LOG("%s is not CS_GetSendToTargetMsg", otherMsg.GetMsgName()); return; }; const CS_GetSendToTargetMsg &other = *(const CS_GetSendToTargetMsg*)(&otherMsg);
        mSender = other.mSender;
        mTarget = other.mTarget;
    }

    virtual const char* GetMsgName() const override { return "CS_GetSendToTargetMsg"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mSender")==0) { AData value; value = mSender; return value; }
        if (strcmp(szMember, "mTarget")==0) { AData value; value = mTarget; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mSender")==0) { mSender = value; return true; };
        if (strcmp(szMember, "mTarget")==0) { mTarget = value; return true; };
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

class CS_ResponseSendToTargetMsg : public tBaseMsg
{ 
public:
    int mError;		
    AutoNice mMsgList;		

public:
    CS_ResponseSendToTargetMsg() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mError);
        mMsgList = (tNiceData*)scrData["mMsgList"];
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mError"] = mError;
        destData["mMsgList"] = mMsgList.getPtr();
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)2);

        SAVE_MSG_VALUE(mError, 1);
        SAVE_MSG_NICE(mMsgList);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mError = 0;
        if (mMsgList) mMsgList.setNull();
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("CS_ResponseSendToTargetMsg", otherMsg.GetMsgName())!=0) { LOG("%s is not CS_ResponseSendToTargetMsg", otherMsg.GetMsgName()); return; }; const CS_ResponseSendToTargetMsg &other = *(const CS_ResponseSendToTargetMsg*)(&otherMsg);
        mError = other.mError;
        COPY_MSG_NICE(other.mMsgList, mMsgList);
    }

    virtual const char* GetMsgName() const override { return "CS_ResponseSendToTargetMsg"; }

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

//  清楚新消息标识
class CS_ClearNewTalkMsgFlag : public tBaseMsg
{ 
public:
    Int64 mDBID;		
    Int64 mSenderID;		

public:
    CS_ClearNewTalkMsgFlag() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mDBID);
        CheckGet(scrData, mSenderID);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mDBID"] = mDBID;
        destData["mSenderID"] = mSenderID;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)2);

        SAVE_MSG_VALUE(mDBID, 8);
        SAVE_MSG_VALUE(mSenderID, 8);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mDBID = 0;
        mSenderID = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("CS_ClearNewTalkMsgFlag", otherMsg.GetMsgName())!=0) { LOG("%s is not CS_ClearNewTalkMsgFlag", otherMsg.GetMsgName()); return; }; const CS_ClearNewTalkMsgFlag &other = *(const CS_ClearNewTalkMsgFlag*)(&otherMsg);
        mDBID = other.mDBID;
        mSenderID = other.mSenderID;
    }

    virtual const char* GetMsgName() const override { return "CS_ClearNewTalkMsgFlag"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mDBID")==0) { AData value; value = mDBID; return value; }
        if (strcmp(szMember, "mSenderID")==0) { AData value; value = mSenderID; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mDBID")==0) { mDBID = value; return true; };
        if (strcmp(szMember, "mSenderID")==0) { mSenderID = value; return true; };
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

//  加载需要检验的用户信息
class MS_LoadCheckUserInfo : public tBaseMsg
{ 
public:
    Int64 mDBID;		
    Int64 mLastPhone;		
    bool mbOnlyCheck;		//  只调取需要检验的用户

public:
    MS_LoadCheckUserInfo() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mDBID);
        CheckGet(scrData, mLastPhone);
        CheckGet(scrData, mbOnlyCheck);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mDBID"] = mDBID;
        destData["mLastPhone"] = mLastPhone;
        destData["mbOnlyCheck"] = mbOnlyCheck;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)3);

        SAVE_MSG_VALUE(mDBID, 8);
        SAVE_MSG_VALUE(mLastPhone, 8);
        SAVE_MSG_VALUE(mbOnlyCheck, 5);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mDBID = 0;
        mLastPhone = 0;
        mbOnlyCheck = false;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("MS_LoadCheckUserInfo", otherMsg.GetMsgName())!=0) { LOG("%s is not MS_LoadCheckUserInfo", otherMsg.GetMsgName()); return; }; const MS_LoadCheckUserInfo &other = *(const MS_LoadCheckUserInfo*)(&otherMsg);
        mDBID = other.mDBID;
        mLastPhone = other.mLastPhone;
        mbOnlyCheck = other.mbOnlyCheck;
    }

    virtual const char* GetMsgName() const override { return "MS_LoadCheckUserInfo"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mDBID")==0) { AData value; value = mDBID; return value; }
        if (strcmp(szMember, "mLastPhone")==0) { AData value; value = mLastPhone; return value; }
        if (strcmp(szMember, "mbOnlyCheck")==0) { AData value; value = mbOnlyCheck; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mDBID")==0) { mDBID = value; return true; };
        if (strcmp(szMember, "mLastPhone")==0) { mLastPhone = value; return true; };
        if (strcmp(szMember, "mbOnlyCheck")==0) { mbOnlyCheck = value; return true; };
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

class MS_ResponseCheckUserInfo : public tBaseMsg
{ 
public:
    int mError;		
    int mState;		
    Int64 mUserID;		
    Int64 mUserPhone;		

public:
    MS_ResponseCheckUserInfo() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mError);
        CheckGet(scrData, mState);
        CheckGet(scrData, mUserID);
        CheckGet(scrData, mUserPhone);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mError"] = mError;
        destData["mState"] = mState;
        destData["mUserID"] = mUserID;
        destData["mUserPhone"] = mUserPhone;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)4);

        SAVE_MSG_VALUE(mError, 1);
        SAVE_MSG_VALUE(mState, 1);
        SAVE_MSG_VALUE(mUserID, 8);
        SAVE_MSG_VALUE(mUserPhone, 8);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mError = 0;
        mState = 0;
        mUserID = 0;
        mUserPhone = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("MS_ResponseCheckUserInfo", otherMsg.GetMsgName())!=0) { LOG("%s is not MS_ResponseCheckUserInfo", otherMsg.GetMsgName()); return; }; const MS_ResponseCheckUserInfo &other = *(const MS_ResponseCheckUserInfo*)(&otherMsg);
        mError = other.mError;
        mState = other.mState;
        mUserID = other.mUserID;
        mUserPhone = other.mUserPhone;
    }

    virtual const char* GetMsgName() const override { return "MS_ResponseCheckUserInfo"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mError")==0) { AData value; value = mError; return value; }
        if (strcmp(szMember, "mState")==0) { AData value; value = mState; return value; }
        if (strcmp(szMember, "mUserID")==0) { AData value; value = mUserID; return value; }
        if (strcmp(szMember, "mUserPhone")==0) { AData value; value = mUserPhone; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mError")==0) { mError = value; return true; };
        if (strcmp(szMember, "mState")==0) { mState = value; return true; };
        if (strcmp(szMember, "mUserID")==0) { mUserID = value; return true; };
        if (strcmp(szMember, "mUserPhone")==0) { mUserPhone = value; return true; };
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

//  修改用户状态
class MS_SetUserCheckState : public tBaseMsg
{ 
public:
    Int64 mDBID;		
    int mState;		
    Int64 mUserID;		
    bool mbAddToMainList;		//  是否直接加入展示列表

public:
    MS_SetUserCheckState() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mDBID);
        CheckGet(scrData, mState);
        CheckGet(scrData, mUserID);
        CheckGet(scrData, mbAddToMainList);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mDBID"] = mDBID;
        destData["mState"] = mState;
        destData["mUserID"] = mUserID;
        destData["mbAddToMainList"] = mbAddToMainList;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)4);

        SAVE_MSG_VALUE(mDBID, 8);
        SAVE_MSG_VALUE(mState, 1);
        SAVE_MSG_VALUE(mUserID, 8);
        SAVE_MSG_VALUE(mbAddToMainList, 5);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mDBID = 0;
        mState = 0;
        mUserID = 0;
        mbAddToMainList = false;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("MS_SetUserCheckState", otherMsg.GetMsgName())!=0) { LOG("%s is not MS_SetUserCheckState", otherMsg.GetMsgName()); return; }; const MS_SetUserCheckState &other = *(const MS_SetUserCheckState*)(&otherMsg);
        mDBID = other.mDBID;
        mState = other.mState;
        mUserID = other.mUserID;
        mbAddToMainList = other.mbAddToMainList;
    }

    virtual const char* GetMsgName() const override { return "MS_SetUserCheckState"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mDBID")==0) { AData value; value = mDBID; return value; }
        if (strcmp(szMember, "mState")==0) { AData value; value = mState; return value; }
        if (strcmp(szMember, "mUserID")==0) { AData value; value = mUserID; return value; }
        if (strcmp(szMember, "mbAddToMainList")==0) { AData value; value = mbAddToMainList; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mDBID")==0) { mDBID = value; return true; };
        if (strcmp(szMember, "mState")==0) { mState = value; return true; };
        if (strcmp(szMember, "mUserID")==0) { mUserID = value; return true; };
        if (strcmp(szMember, "mbAddToMainList")==0) { mbAddToMainList = value; return true; };
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

//  获取可展示的用户数据
class MS_FindNormarUserInfo : public tBaseMsg
{ 
public:
    Int64 mDBID;		
    Int64 mLastPhone;		//  滚动查找

public:
    MS_FindNormarUserInfo() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mDBID);
        CheckGet(scrData, mLastPhone);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mDBID"] = mDBID;
        destData["mLastPhone"] = mLastPhone;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)2);

        SAVE_MSG_VALUE(mDBID, 8);
        SAVE_MSG_VALUE(mLastPhone, 8);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mDBID = 0;
        mLastPhone = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("MS_FindNormarUserInfo", otherMsg.GetMsgName())!=0) { LOG("%s is not MS_FindNormarUserInfo", otherMsg.GetMsgName()); return; }; const MS_FindNormarUserInfo &other = *(const MS_FindNormarUserInfo*)(&otherMsg);
        mDBID = other.mDBID;
        mLastPhone = other.mLastPhone;
    }

    virtual const char* GetMsgName() const override { return "MS_FindNormarUserInfo"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mDBID")==0) { AData value; value = mDBID; return value; }
        if (strcmp(szMember, "mLastPhone")==0) { AData value; value = mLastPhone; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mDBID")==0) { mDBID = value; return true; };
        if (strcmp(szMember, "mLastPhone")==0) { mLastPhone = value; return true; };
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

class MS_ResponseNormarUserInfo : public tBaseMsg
{ 
public:
    int mError;		
    Int64 mUserID;		
    Int64 mUserPhone;		

public:
    MS_ResponseNormarUserInfo() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mError);
        CheckGet(scrData, mUserID);
        CheckGet(scrData, mUserPhone);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mError"] = mError;
        destData["mUserID"] = mUserID;
        destData["mUserPhone"] = mUserPhone;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)3);

        SAVE_MSG_VALUE(mError, 1);
        SAVE_MSG_VALUE(mUserID, 8);
        SAVE_MSG_VALUE(mUserPhone, 8);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mError = 0;
        mUserID = 0;
        mUserPhone = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("MS_ResponseNormarUserInfo", otherMsg.GetMsgName())!=0) { LOG("%s is not MS_ResponseNormarUserInfo", otherMsg.GetMsgName()); return; }; const MS_ResponseNormarUserInfo &other = *(const MS_ResponseNormarUserInfo*)(&otherMsg);
        mError = other.mError;
        mUserID = other.mUserID;
        mUserPhone = other.mUserPhone;
    }

    virtual const char* GetMsgName() const override { return "MS_ResponseNormarUserInfo"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mError")==0) { AData value; value = mError; return value; }
        if (strcmp(szMember, "mUserID")==0) { AData value; value = mUserID; return value; }
        if (strcmp(szMember, "mUserPhone")==0) { AData value; value = mUserPhone; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mError")==0) { mError = value; return true; };
        if (strcmp(szMember, "mUserID")==0) { mUserID = value; return true; };
        if (strcmp(szMember, "mUserPhone")==0) { mUserPhone = value; return true; };
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

//  增加到展示列表
class MS_AddToMainList : public tBaseMsg
{ 
public:
    Int64 mDBID;		
    Int64 mUserID;		

public:
    MS_AddToMainList() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mDBID);
        CheckGet(scrData, mUserID);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mDBID"] = mDBID;
        destData["mUserID"] = mUserID;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)2);

        SAVE_MSG_VALUE(mDBID, 8);
        SAVE_MSG_VALUE(mUserID, 8);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mDBID = 0;
        mUserID = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("MS_AddToMainList", otherMsg.GetMsgName())!=0) { LOG("%s is not MS_AddToMainList", otherMsg.GetMsgName()); return; }; const MS_AddToMainList &other = *(const MS_AddToMainList*)(&otherMsg);
        mDBID = other.mDBID;
        mUserID = other.mUserID;
    }

    virtual const char* GetMsgName() const override { return "MS_AddToMainList"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mDBID")==0) { AData value; value = mDBID; return value; }
        if (strcmp(szMember, "mUserID")==0) { AData value; value = mUserID; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mDBID")==0) { mDBID = value; return true; };
        if (strcmp(szMember, "mUserID")==0) { mUserID = value; return true; };
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

//  从展示列表移除
class MS_RemoveUserFromMainList : public tBaseMsg
{ 
public:
    Int64 mDBID;		
    Int64 mUserID;		

public:
    MS_RemoveUserFromMainList() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mDBID);
        CheckGet(scrData, mUserID);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mDBID"] = mDBID;
        destData["mUserID"] = mUserID;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)2);

        SAVE_MSG_VALUE(mDBID, 8);
        SAVE_MSG_VALUE(mUserID, 8);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mDBID = 0;
        mUserID = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("MS_RemoveUserFromMainList", otherMsg.GetMsgName())!=0) { LOG("%s is not MS_RemoveUserFromMainList", otherMsg.GetMsgName()); return; }; const MS_RemoveUserFromMainList &other = *(const MS_RemoveUserFromMainList*)(&otherMsg);
        mDBID = other.mDBID;
        mUserID = other.mUserID;
    }

    virtual const char* GetMsgName() const override { return "MS_RemoveUserFromMainList"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mDBID")==0) { AData value; value = mDBID; return value; }
        if (strcmp(szMember, "mUserID")==0) { AData value; value = mUserID; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mDBID")==0) { mDBID = value; return true; };
        if (strcmp(szMember, "mUserID")==0) { mUserID = value; return true; };
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

//  拉取所有的展示列表
class CS_LoadAllMainList : public tBaseMsg
{ 
public:
    int mPageCount;		
    int mPageID;		

public:
    CS_LoadAllMainList() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mPageCount);
        CheckGet(scrData, mPageID);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mPageCount"] = mPageCount;
        destData["mPageID"] = mPageID;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)2);

        SAVE_MSG_VALUE(mPageCount, 1);
        SAVE_MSG_VALUE(mPageID, 1);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mPageCount = 0;
        mPageID = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("CS_LoadAllMainList", otherMsg.GetMsgName())!=0) { LOG("%s is not CS_LoadAllMainList", otherMsg.GetMsgName()); return; }; const CS_LoadAllMainList &other = *(const CS_LoadAllMainList*)(&otherMsg);
        mPageCount = other.mPageCount;
        mPageID = other.mPageID;
    }

    virtual const char* GetMsgName() const override { return "CS_LoadAllMainList"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mPageCount")==0) { AData value; value = mPageCount; return value; }
        if (strcmp(szMember, "mPageID")==0) { AData value; value = mPageID; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mPageCount")==0) { mPageCount = value; return true; };
        if (strcmp(szMember, "mPageID")==0) { mPageID = value; return true; };
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

class CS_ResponseMainListData : public tBaseMsg
{ 
public:
    int mError;		
    ArrayList<Int64> mMainList;	

public:
    CS_ResponseMainListData() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mError);
        FullArray(scrData, mMainList, "mMainList");
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mError"] = mError;
        ArrayToData(destData, mMainList, "mMainList");
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)2);

        SAVE_MSG_VALUE(mError, 1);
        SaveArray("mMainList", mMainList, destData);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mError = 0;
        mMainList.clear(false);
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("CS_ResponseMainListData", otherMsg.GetMsgName())!=0) { LOG("%s is not CS_ResponseMainListData", otherMsg.GetMsgName()); return; }; const CS_ResponseMainListData &other = *(const CS_ResponseMainListData*)(&otherMsg);
        mError = other.mError;
        mMainList = other.mMainList;
    }

    virtual const char* GetMsgName() const override { return "CS_ResponseMainListData"; }

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

//  移除某个人送礼物的红点
class CS_RemoveNewGiftFlag : public tBaseMsg
{ 
public:
    Int64 mDBID;		
    Int64 mSenderID;		

public:
    CS_RemoveNewGiftFlag() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mDBID);
        CheckGet(scrData, mSenderID);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mDBID"] = mDBID;
        destData["mSenderID"] = mSenderID;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)2);

        SAVE_MSG_VALUE(mDBID, 8);
        SAVE_MSG_VALUE(mSenderID, 8);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mDBID = 0;
        mSenderID = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("CS_RemoveNewGiftFlag", otherMsg.GetMsgName())!=0) { LOG("%s is not CS_RemoveNewGiftFlag", otherMsg.GetMsgName()); return; }; const CS_RemoveNewGiftFlag &other = *(const CS_RemoveNewGiftFlag*)(&otherMsg);
        mDBID = other.mDBID;
        mSenderID = other.mSenderID;
    }

    virtual const char* GetMsgName() const override { return "CS_RemoveNewGiftFlag"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mDBID")==0) { AData value; value = mDBID; return value; }
        if (strcmp(szMember, "mSenderID")==0) { AData value; value = mSenderID; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mDBID")==0) { mDBID = value; return true; };
        if (strcmp(szMember, "mSenderID")==0) { mSenderID = value; return true; };
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

class CS_ResponseRemoveNewGiftFlag : public tBaseMsg
{ 
public:
    int mError;		
    AutoNice mExData;		//  回复当前红点功能数据

public:
    CS_ResponseRemoveNewGiftFlag() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mError);
        mExData = (tNiceData*)scrData["mExData"];
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mError"] = mError;
        destData["mExData"] = mExData.getPtr();
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)2);

        SAVE_MSG_VALUE(mError, 1);
        SAVE_MSG_NICE(mExData);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mError = 0;
        if (mExData) mExData.setNull();
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("CS_ResponseRemoveNewGiftFlag", otherMsg.GetMsgName())!=0) { LOG("%s is not CS_ResponseRemoveNewGiftFlag", otherMsg.GetMsgName()); return; }; const CS_ResponseRemoveNewGiftFlag &other = *(const CS_ResponseRemoveNewGiftFlag*)(&otherMsg);
        mError = other.mError;
        COPY_MSG_NICE(other.mExData, mExData);
    }

    virtual const char* GetMsgName() const override { return "CS_ResponseRemoveNewGiftFlag"; }

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

//  压缩功能
class CS_ZipData : public tBaseMsg
{ 
public:
    AutoData mData;		

public:
    CS_ZipData() { clear(false); };


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
        if (strcmp("CS_ZipData", otherMsg.GetMsgName())!=0) { LOG("%s is not CS_ZipData", otherMsg.GetMsgName()); return; }; const CS_ZipData &other = *(const CS_ZipData*)(&otherMsg);
        COPY_MSG_DATA(other.mData, mData);
    }

    virtual const char* GetMsgName() const override { return "CS_ZipData"; }

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

class CS_ResponseZipData : public tBaseMsg
{ 
public:
    int mError;		
    AutoData mZipData;		

public:
    CS_ResponseZipData() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mError);
        mZipData = (DataStream*)scrData["mZipData"];
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mError"] = mError;
        destData["mZipData"] = mZipData.getPtr();
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)2);

        SAVE_MSG_VALUE(mError, 1);
        SAVE_MSG_DATA(mZipData);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mError = 0;
        if (mZipData) mZipData.setNull();
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("CS_ResponseZipData", otherMsg.GetMsgName())!=0) { LOG("%s is not CS_ResponseZipData", otherMsg.GetMsgName()); return; }; const CS_ResponseZipData &other = *(const CS_ResponseZipData*)(&otherMsg);
        mError = other.mError;
        COPY_MSG_DATA(other.mZipData, mZipData);
    }

    virtual const char* GetMsgName() const override { return "CS_ResponseZipData"; }

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

//  上传配置
class MS_UpdateSaveResource : public tBaseMsg
{ 
public:
    AString mIndexName;		
    AString mMD5;		
    AutoData mResourceData;		

public:
    MS_UpdateSaveResource() { clear(false); };


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
        if (strcmp("MS_UpdateSaveResource", otherMsg.GetMsgName())!=0) { LOG("%s is not MS_UpdateSaveResource", otherMsg.GetMsgName()); return; }; const MS_UpdateSaveResource &other = *(const MS_UpdateSaveResource*)(&otherMsg);
        mIndexName = other.mIndexName;
        mMD5 = other.mMD5;
        COPY_MSG_DATA(other.mResourceData, mResourceData);
    }

    virtual const char* GetMsgName() const override { return "MS_UpdateSaveResource"; }

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

class MS_ReponseSaveResult : public tBaseMsg
{ 
public:
    int mError;		
    AString mInfo;		

public:
    MS_ReponseSaveResult() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mError);
        CheckGet(scrData, mInfo);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mError"] = mError;
        destData["mInfo"] = mInfo;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)2);

        SAVE_MSG_VALUE(mError, 1);
        SAVE_MSG_VALUE(mInfo, 4);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mError = 0;
        mInfo.setNull();
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("MS_ReponseSaveResult", otherMsg.GetMsgName())!=0) { LOG("%s is not MS_ReponseSaveResult", otherMsg.GetMsgName()); return; }; const MS_ReponseSaveResult &other = *(const MS_ReponseSaveResult*)(&otherMsg);
        mError = other.mError;
        mInfo = other.mInfo;
    }

    virtual const char* GetMsgName() const override { return "MS_ReponseSaveResult"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mError")==0) { AData value; value = mError; return value; }
        if (strcmp(szMember, "mInfo")==0) { AData value; value = mInfo; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mError")==0) { mError = value; return true; };
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

//  上传应用客户端
class MS_UploadApplyClient : public tBaseMsg
{ 
public:
    int mUploadCacheID;		
    int mVersion;		

public:
    MS_UploadApplyClient() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mUploadCacheID);
        CheckGet(scrData, mVersion);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mUploadCacheID"] = mUploadCacheID;
        destData["mVersion"] = mVersion;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)2);

        SAVE_MSG_VALUE(mUploadCacheID, 1);
        SAVE_MSG_VALUE(mVersion, 1);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mUploadCacheID = 0;
        mVersion = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("MS_UploadApplyClient", otherMsg.GetMsgName())!=0) { LOG("%s is not MS_UploadApplyClient", otherMsg.GetMsgName()); return; }; const MS_UploadApplyClient &other = *(const MS_UploadApplyClient*)(&otherMsg);
        mUploadCacheID = other.mUploadCacheID;
        mVersion = other.mVersion;
    }

    virtual const char* GetMsgName() const override { return "MS_UploadApplyClient"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mUploadCacheID")==0) { AData value; value = mUploadCacheID; return value; }
        if (strcmp(szMember, "mVersion")==0) { AData value; value = mVersion; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mUploadCacheID")==0) { mUploadCacheID = value; return true; };
        if (strcmp(szMember, "mVersion")==0) { mVersion = value; return true; };
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

class MS_ResponseUploadClient : public tBaseMsg
{ 
public:
    int mError;		
    AString mInfo;		

public:
    MS_ResponseUploadClient() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mError);
        CheckGet(scrData, mInfo);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mError"] = mError;
        destData["mInfo"] = mInfo;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)2);

        SAVE_MSG_VALUE(mError, 1);
        SAVE_MSG_VALUE(mInfo, 4);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mError = 0;
        mInfo.setNull();
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("MS_ResponseUploadClient", otherMsg.GetMsgName())!=0) { LOG("%s is not MS_ResponseUploadClient", otherMsg.GetMsgName()); return; }; const MS_ResponseUploadClient &other = *(const MS_ResponseUploadClient*)(&otherMsg);
        mError = other.mError;
        mInfo = other.mInfo;
    }

    virtual const char* GetMsgName() const override { return "MS_ResponseUploadClient"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mError")==0) { AData value; value = mError; return value; }
        if (strcmp(szMember, "mInfo")==0) { AData value; value = mInfo; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mError")==0) { mError = value; return true; };
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

//  上传应用客户端
class MS_UploadApplyManagerClient : public tBaseMsg
{ 
public:
    int mUploadCacheID;		
    int mVersion;		

public:
    MS_UploadApplyManagerClient() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mUploadCacheID);
        CheckGet(scrData, mVersion);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mUploadCacheID"] = mUploadCacheID;
        destData["mVersion"] = mVersion;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)2);

        SAVE_MSG_VALUE(mUploadCacheID, 1);
        SAVE_MSG_VALUE(mVersion, 1);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mUploadCacheID = 0;
        mVersion = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("MS_UploadApplyManagerClient", otherMsg.GetMsgName())!=0) { LOG("%s is not MS_UploadApplyManagerClient", otherMsg.GetMsgName()); return; }; const MS_UploadApplyManagerClient &other = *(const MS_UploadApplyManagerClient*)(&otherMsg);
        mUploadCacheID = other.mUploadCacheID;
        mVersion = other.mVersion;
    }

    virtual const char* GetMsgName() const override { return "MS_UploadApplyManagerClient"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mUploadCacheID")==0) { AData value; value = mUploadCacheID; return value; }
        if (strcmp(szMember, "mVersion")==0) { AData value; value = mVersion; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mUploadCacheID")==0) { mUploadCacheID = value; return true; };
        if (strcmp(szMember, "mVersion")==0) { mVersion = value; return true; };
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

//  创建DB
class TS_CreateDBTable : public tBaseMsg
{ 
public:
    AutoNice mDBConfigData;		
    AString mExportCodePath;		

public:
    TS_CreateDBTable() { clear(false); };


   virtual  void Full(AutoNice scrData) override
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
        if (strcmp("TS_CreateDBTable", otherMsg.GetMsgName())!=0) { LOG("%s is not TS_CreateDBTable", otherMsg.GetMsgName()); return; }; const TS_CreateDBTable &other = *(const TS_CreateDBTable*)(&otherMsg);
        COPY_MSG_NICE(other.mDBConfigData, mDBConfigData);
        mExportCodePath = other.mExportCodePath;
    }

    virtual const char* GetMsgName() const override { return "TS_CreateDBTable"; }

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

class TS_ResponseCreateDBTable : public tBaseMsg
{ 
public:
    int mError;		
    AString mResultInfo;		

public:
    TS_ResponseCreateDBTable() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mError);
        CheckGet(scrData, mResultInfo);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mError"] = mError;
        destData["mResultInfo"] = mResultInfo;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)2);

        SAVE_MSG_VALUE(mError, 1);
        SAVE_MSG_VALUE(mResultInfo, 4);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mError = 0;
        mResultInfo.setNull();
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("TS_ResponseCreateDBTable", otherMsg.GetMsgName())!=0) { LOG("%s is not TS_ResponseCreateDBTable", otherMsg.GetMsgName()); return; }; const TS_ResponseCreateDBTable &other = *(const TS_ResponseCreateDBTable*)(&otherMsg);
        mError = other.mError;
        mResultInfo = other.mResultInfo;
    }

    virtual const char* GetMsgName() const override { return "TS_ResponseCreateDBTable"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mError")==0) { AData value; value = mError; return value; }
        if (strcmp(szMember, "mResultInfo")==0) { AData value; value = mResultInfo; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mError")==0) { mError = value; return true; };
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

//  获取推荐
class CS_RequestTuiJianShowList : public tBaseMsg
{ 
public:
    Int64 mUserID;		//  当前用户的DBID, 根据此用户信息获取推荐	

public:
    CS_RequestTuiJianShowList() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mUserID);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mUserID"] = mUserID;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)1);

        SAVE_MSG_VALUE(mUserID, 8);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mUserID = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("CS_RequestTuiJianShowList", otherMsg.GetMsgName())!=0) { LOG("%s is not CS_RequestTuiJianShowList", otherMsg.GetMsgName()); return; }; const CS_RequestTuiJianShowList &other = *(const CS_RequestTuiJianShowList*)(&otherMsg);
        mUserID = other.mUserID;
    }

    virtual const char* GetMsgName() const override { return "CS_RequestTuiJianShowList"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mUserID")==0) { AData value; value = mUserID; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mUserID")==0) { mUserID = value; return true; };
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

class CS_ResponseTuiJainShowList : public tBaseMsg
{ 
public:
    int mError;		
    ArrayList<Int64> mMainList;	

public:
    CS_ResponseTuiJainShowList() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mError);
        FullArray(scrData, mMainList, "mMainList");
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mError"] = mError;
        ArrayToData(destData, mMainList, "mMainList");
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)2);

        SAVE_MSG_VALUE(mError, 1);
        SaveArray("mMainList", mMainList, destData);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mError = 0;
        mMainList.clear(false);
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("CS_ResponseTuiJainShowList", otherMsg.GetMsgName())!=0) { LOG("%s is not CS_ResponseTuiJainShowList", otherMsg.GetMsgName()); return; }; const CS_ResponseTuiJainShowList &other = *(const CS_ResponseTuiJainShowList*)(&otherMsg);
        mError = other.mError;
        mMainList = other.mMainList;
    }

    virtual const char* GetMsgName() const override { return "CS_ResponseTuiJainShowList"; }

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

//  获取消息
class CS_LoadMsgList : public tBaseMsg
{ 
public:
    Int64 mDBID;		
    int mTalkType;		//  0  好友 1 红娘  2 其他

public:
    CS_LoadMsgList() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mDBID);
        CheckGet(scrData, mTalkType);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mDBID"] = mDBID;
        destData["mTalkType"] = mTalkType;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)2);

        SAVE_MSG_VALUE(mDBID, 8);
        SAVE_MSG_VALUE(mTalkType, 1);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mDBID = 0;
        mTalkType = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("CS_LoadMsgList", otherMsg.GetMsgName())!=0) { LOG("%s is not CS_LoadMsgList", otherMsg.GetMsgName()); return; }; const CS_LoadMsgList &other = *(const CS_LoadMsgList*)(&otherMsg);
        mDBID = other.mDBID;
        mTalkType = other.mTalkType;
    }

    virtual const char* GetMsgName() const override { return "CS_LoadMsgList"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mDBID")==0) { AData value; value = mDBID; return value; }
        if (strcmp(szMember, "mTalkType")==0) { AData value; value = mTalkType; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mDBID")==0) { mDBID = value; return true; };
        if (strcmp(szMember, "mTalkType")==0) { mTalkType = value; return true; };
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

class CS_ResponseMsgList : public tBaseMsg
{ 
public:
    int mError;		
    AutoNice mMsgData;		

public:
    CS_ResponseMsgList() { clear(false); };


   virtual  void Full(AutoNice scrData) override
    {
        clear(false);
        CheckGet(scrData, mError);
        mMsgData = (tNiceData*)scrData["mMsgData"];
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mError"] = mError;
        destData["mMsgData"] = mMsgData.getPtr();
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)2);

        SAVE_MSG_VALUE(mError, 1);
        SAVE_MSG_NICE(mMsgData);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mError = 0;
        if (mMsgData) mMsgData.setNull();
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("CS_ResponseMsgList", otherMsg.GetMsgName())!=0) { LOG("%s is not CS_ResponseMsgList", otherMsg.GetMsgName()); return; }; const CS_ResponseMsgList &other = *(const CS_ResponseMsgList*)(&otherMsg);
        mError = other.mError;
        COPY_MSG_NICE(other.mMsgData, mMsgData);
    }

    virtual const char* GetMsgName() const override { return "CS_ResponseMsgList"; }

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

