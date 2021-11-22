//Auto genereate msg data code		
#pragma once

#include "BaseMsg.h"
#include "Array.h"
#include "ArrayList.h"

class DBkvDataString : public tBaseMsg
{ 
public:
    AString key;		// key
    AString value;		// value

public:
    DBkvDataString() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        CheckGet(scrData, key);
        CheckGet(scrData, value);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["key"] = key;
        destData["value"] = value;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)2);

        SAVE_MSG_VALUE(key, 4);
        SAVE_MSG_VALUE(value, 4);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        key.setNull();
        value.setNull();
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("DBkvDataString", otherMsg.GetMsgName())!=0) { LOG("%s is not DBkvDataString", otherMsg.GetMsgName()); return; }; const DBkvDataString &other = *(const DBkvDataString*)(&otherMsg);
        key = other.key;
        value = other.value;
    }

    virtual const char* GetMsgName() const override { return "DBkvDataString"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "key")==0) { AData value; value = key; return value; }
        if (strcmp(szMember, "value")==0) { AData value; value = value; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "key")==0) { key = value; return true; };
        if (strcmp(szMember, "value")==0) { value = value; return true; };
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

class DBkvData : public tBaseMsg
{ 
public:
    int key;		// key
    int value;		// value

public:
    DBkvData() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        CheckGet(scrData, key);
        CheckGet(scrData, value);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["key"] = key;
        destData["value"] = value;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)2);

        SAVE_MSG_VALUE(key, 1);
        SAVE_MSG_VALUE(value, 1);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        key = 0;
        value = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("DBkvData", otherMsg.GetMsgName())!=0) { LOG("%s is not DBkvData", otherMsg.GetMsgName()); return; }; const DBkvData &other = *(const DBkvData*)(&otherMsg);
        key = other.key;
        value = other.value;
    }

    virtual const char* GetMsgName() const override { return "DBkvData"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "key")==0) { AData value; value = key; return value; }
        if (strcmp(szMember, "value")==0) { AData value; value = value; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "key")==0) { key = value; return true; };
        if (strcmp(szMember, "value")==0) { value = value; return true; };
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

class DBkvBoolData : public tBaseMsg
{ 
public:
    int key;		// key
    bool value;		// value

public:
    DBkvBoolData() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        CheckGet(scrData, key);
        CheckGet(scrData, value);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["key"] = key;
        destData["value"] = value;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)2);

        SAVE_MSG_VALUE(key, 1);
        SAVE_MSG_VALUE(value, 5);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        key = 0;
        value = false;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("DBkvBoolData", otherMsg.GetMsgName())!=0) { LOG("%s is not DBkvBoolData", otherMsg.GetMsgName()); return; }; const DBkvBoolData &other = *(const DBkvBoolData*)(&otherMsg);
        key = other.key;
        value = other.value;
    }

    virtual const char* GetMsgName() const override { return "DBkvBoolData"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "key")==0) { AData value; value = key; return value; }
        if (strcmp(szMember, "value")==0) { AData value; value = value; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "key")==0) { key = value; return true; };
        if (strcmp(szMember, "value")==0) { value = value; return true; };
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

class DBkvRankData : public tBaseMsg
{ 
public:
    int key;		// key
    Int64 playerID;		// 玩家id
    Int64 value;		// value

public:
    DBkvRankData() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        CheckGet(scrData, key);
        CheckGet(scrData, playerID);
        CheckGet(scrData, value);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["key"] = key;
        destData["playerID"] = playerID;
        destData["value"] = value;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)3);

        SAVE_MSG_VALUE(key, 1);
        SAVE_MSG_VALUE(playerID, 8);
        SAVE_MSG_VALUE(value, 8);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        key = 0;
        playerID = 0;
        value = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("DBkvRankData", otherMsg.GetMsgName())!=0) { LOG("%s is not DBkvRankData", otherMsg.GetMsgName()); return; }; const DBkvRankData &other = *(const DBkvRankData*)(&otherMsg);
        key = other.key;
        playerID = other.playerID;
        value = other.value;
    }

    virtual const char* GetMsgName() const override { return "DBkvRankData"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "key")==0) { AData value; value = key; return value; }
        if (strcmp(szMember, "playerID")==0) { AData value; value = playerID; return value; }
        if (strcmp(szMember, "value")==0) { AData value; value = value; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "key")==0) { key = value; return true; };
        if (strcmp(szMember, "playerID")==0) { playerID = value; return true; };
        if (strcmp(szMember, "value")==0) { value = value; return true; };
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

class EquipProp : public tBaseMsg
{ 
public:
    int mID;		// 装备的配置id
    int mLevel;		// 装备等级
    int mLevelExp;		// 装备等级经验(废弃)
    int mMaster;		// 装备精通等级
    int mMasterExp;		// 装备精通经验

public:
    EquipProp() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        CheckGet(scrData, mID);
        CheckGet(scrData, mLevel);
        CheckGet(scrData, mLevelExp);
        CheckGet(scrData, mMaster);
        CheckGet(scrData, mMasterExp);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mID"] = mID;
        destData["mLevel"] = mLevel;
        destData["mLevelExp"] = mLevelExp;
        destData["mMaster"] = mMaster;
        destData["mMasterExp"] = mMasterExp;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)5);

        SAVE_MSG_VALUE(mID, 1);
        SAVE_MSG_VALUE(mLevel, 1);
        SAVE_MSG_VALUE(mLevelExp, 1);
        SAVE_MSG_VALUE(mMaster, 1);
        SAVE_MSG_VALUE(mMasterExp, 1);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mID = 0;
        mLevel = 0;
        mLevelExp = 0;
        mMaster = 0;
        mMasterExp = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("EquipProp", otherMsg.GetMsgName())!=0) { LOG("%s is not EquipProp", otherMsg.GetMsgName()); return; }; const EquipProp &other = *(const EquipProp*)(&otherMsg);
        mID = other.mID;
        mLevel = other.mLevel;
        mLevelExp = other.mLevelExp;
        mMaster = other.mMaster;
        mMasterExp = other.mMasterExp;
    }

    virtual const char* GetMsgName() const override { return "EquipProp"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mID")==0) { AData value; value = mID; return value; }
        if (strcmp(szMember, "mLevel")==0) { AData value; value = mLevel; return value; }
        if (strcmp(szMember, "mLevelExp")==0) { AData value; value = mLevelExp; return value; }
        if (strcmp(szMember, "mMaster")==0) { AData value; value = mMaster; return value; }
        if (strcmp(szMember, "mMasterExp")==0) { AData value; value = mMasterExp; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mID")==0) { mID = value; return true; };
        if (strcmp(szMember, "mLevel")==0) { mLevel = value; return true; };
        if (strcmp(szMember, "mLevelExp")==0) { mLevelExp = value; return true; };
        if (strcmp(szMember, "mMaster")==0) { mMaster = value; return true; };
        if (strcmp(szMember, "mMasterExp")==0) { mMasterExp = value; return true; };
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

class ItemShopProp : public tBaseMsg
{ 
public:
    ArrayList<DBkvData> mItemShopList;	// 限购数量

public:
    ItemShopProp() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        FullStuctArray(scrData, mItemShopList, "mItemShopList");
    }

    virtual void ToData(AutoNice &destData) override
    {
        StuctArrayToData(destData, mItemShopList, "mItemShopList");
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)1);

        SaveMsgArray("mItemShopList", mItemShopList, destData);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mItemShopList.clear(false);
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("ItemShopProp", otherMsg.GetMsgName())!=0) { LOG("%s is not ItemShopProp", otherMsg.GetMsgName()); return; }; const ItemShopProp &other = *(const ItemShopProp*)(&otherMsg);
        CopyMsgArray(other.mItemShopList, mItemShopList);
    }

    virtual const char* GetMsgName() const override { return "ItemShopProp"; }

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

class ItemProperty : public tBaseMsg
{ 
public:
    int mCount;		// 道具数量
    EquipProp mEquipData;		// 装备属性
    Int64 mID;		// 道具GUID
    int mItemId;		// 道具的配置id

public:
    ItemProperty() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        CheckGet(scrData, mCount);
        {
            AutoNice d = (tNiceData*)scrData["mEquipData"];
            if (d) mEquipData.Full(d); else LOG("No exist data mEquipData");
        }
        CheckGet(scrData, mID);
        CheckGet(scrData, mItemId);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mCount"] = mCount;
        {
            AutoNice d = destData->NewNice(); d->msKey = "EquipProp"; mEquipData.ToData(d);
            destData["mEquipData"] = d.getPtr();
        }
        destData["mID"] = mID;
        destData["mItemId"] = mItemId;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)4);

        SAVE_MSG_VALUE(mCount, 1);
        SAVE_MSG_STRUCT(mEquipData);
        SAVE_MSG_VALUE(mID, 8);
        SAVE_MSG_VALUE(mItemId, 1);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mCount = 0;
        mEquipData.clear(false);
        mID = 0;
        mItemId = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("ItemProperty", otherMsg.GetMsgName())!=0) { LOG("%s is not ItemProperty", otherMsg.GetMsgName()); return; }; const ItemProperty &other = *(const ItemProperty*)(&otherMsg);
        mCount = other.mCount;
        mEquipData.copy(other.mEquipData);
        mID = other.mID;
        mItemId = other.mItemId;
    }

    virtual const char* GetMsgName() const override { return "ItemProperty"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mCount")==0) { AData value; value = mCount; return value; }
        if (strcmp(szMember, "mID")==0) { AData value; value = mID; return value; }
        if (strcmp(szMember, "mItemId")==0) { AData value; value = mItemId; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mCount")==0) { mCount = value; return true; };
        if (strcmp(szMember, "mID")==0) { mID = value; return true; };
        if (strcmp(szMember, "mItemId")==0) { mItemId = value; return true; };
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

class EntityProp : public tBaseMsg
{ 
public:
    int mAttack;		// 攻击值
    int mEntityId;		// 实体id
    bool mFlag;		// 是否影响攻击值
    int mHp;		// 精力值
    int mHpBefore;		// 战斗前的血量
    int mLevel;		// 实体等级
    int mMaxHp;		// 最大精力值
    Int64 mPlayerID;		// 归属者id
    int mQualityId;		// 品质id
    int mType;		// 0 怪物 1学生

public:
    EntityProp() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        CheckGet(scrData, mAttack);
        CheckGet(scrData, mEntityId);
        CheckGet(scrData, mFlag);
        CheckGet(scrData, mHp);
        CheckGet(scrData, mHpBefore);
        CheckGet(scrData, mLevel);
        CheckGet(scrData, mMaxHp);
        CheckGet(scrData, mPlayerID);
        CheckGet(scrData, mQualityId);
        CheckGet(scrData, mType);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mAttack"] = mAttack;
        destData["mEntityId"] = mEntityId;
        destData["mFlag"] = mFlag;
        destData["mHp"] = mHp;
        destData["mHpBefore"] = mHpBefore;
        destData["mLevel"] = mLevel;
        destData["mMaxHp"] = mMaxHp;
        destData["mPlayerID"] = mPlayerID;
        destData["mQualityId"] = mQualityId;
        destData["mType"] = mType;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)10);

        SAVE_MSG_VALUE(mAttack, 1);
        SAVE_MSG_VALUE(mEntityId, 1);
        SAVE_MSG_VALUE(mFlag, 5);
        SAVE_MSG_VALUE(mHp, 1);
        SAVE_MSG_VALUE(mHpBefore, 1);
        SAVE_MSG_VALUE(mLevel, 1);
        SAVE_MSG_VALUE(mMaxHp, 1);
        SAVE_MSG_VALUE(mPlayerID, 8);
        SAVE_MSG_VALUE(mQualityId, 1);
        SAVE_MSG_VALUE(mType, 1);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mAttack = 0;
        mEntityId = 0;
        mFlag = false;
        mHp = 0;
        mHpBefore = 0;
        mLevel = 0;
        mMaxHp = 0;
        mPlayerID = 0;
        mQualityId = 0;
        mType = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("EntityProp", otherMsg.GetMsgName())!=0) { LOG("%s is not EntityProp", otherMsg.GetMsgName()); return; }; const EntityProp &other = *(const EntityProp*)(&otherMsg);
        mAttack = other.mAttack;
        mEntityId = other.mEntityId;
        mFlag = other.mFlag;
        mHp = other.mHp;
        mHpBefore = other.mHpBefore;
        mLevel = other.mLevel;
        mMaxHp = other.mMaxHp;
        mPlayerID = other.mPlayerID;
        mQualityId = other.mQualityId;
        mType = other.mType;
    }

    virtual const char* GetMsgName() const override { return "EntityProp"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mAttack")==0) { AData value; value = mAttack; return value; }
        if (strcmp(szMember, "mEntityId")==0) { AData value; value = mEntityId; return value; }
        if (strcmp(szMember, "mFlag")==0) { AData value; value = mFlag; return value; }
        if (strcmp(szMember, "mHp")==0) { AData value; value = mHp; return value; }
        if (strcmp(szMember, "mHpBefore")==0) { AData value; value = mHpBefore; return value; }
        if (strcmp(szMember, "mLevel")==0) { AData value; value = mLevel; return value; }
        if (strcmp(szMember, "mMaxHp")==0) { AData value; value = mMaxHp; return value; }
        if (strcmp(szMember, "mPlayerID")==0) { AData value; value = mPlayerID; return value; }
        if (strcmp(szMember, "mQualityId")==0) { AData value; value = mQualityId; return value; }
        if (strcmp(szMember, "mType")==0) { AData value; value = mType; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mAttack")==0) { mAttack = value; return true; };
        if (strcmp(szMember, "mEntityId")==0) { mEntityId = value; return true; };
        if (strcmp(szMember, "mFlag")==0) { mFlag = value; return true; };
        if (strcmp(szMember, "mHp")==0) { mHp = value; return true; };
        if (strcmp(szMember, "mHpBefore")==0) { mHpBefore = value; return true; };
        if (strcmp(szMember, "mLevel")==0) { mLevel = value; return true; };
        if (strcmp(szMember, "mMaxHp")==0) { mMaxHp = value; return true; };
        if (strcmp(szMember, "mPlayerID")==0) { mPlayerID = value; return true; };
        if (strcmp(szMember, "mQualityId")==0) { mQualityId = value; return true; };
        if (strcmp(szMember, "mType")==0) { mType = value; return true; };
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

class BaseProperty : public tBaseMsg
{ 
public:
    Int64 mAcademicLayer;		// 学府之路最高层
    Int64 mAllianceDamage;		// 校盟副本累计伤害
    Int64 mAllianceFubenCount;		// 累计校盟副本挑战次数
    Int64 mAllianceJob;		// 校盟职位
    Int64 mAllianceSayHelloCount;		// 校盟打招呼当天次数(最后时间不是当天则为0)
    Int64 mBestStudentQuality;		// 废弃
    Int64 mBossChallengeCount;		// 累计挑战限时boss次数
    Int64 mBossDamage;		// 限时boss累计伤害
    Int64 mChalkCount;		// 购买的粉笔次数
    Int64 mChalkFree;		// 免费的粉笔次数
    Int64 mChalkLimit;		// 粉笔已购买次数
    Int64 mClassScore;		// 班级分数
    Int64 mCompetitionCount;		// 累计个人竞赛次数
    Int64 mCompetitionTime;		// 个人竞赛赛季结束时间
    Int64 mContribute;		// 校盟声望用于【校盟升级】，每天进行【校盟捐献】可获得声望
    Int64 mContributeCount;		// 累计获得声望贡献
    Int64 mContributeLastTime;		// 校盟捐款的最后时间(用于限制一天只能捐款一次)
    Int64 mContributeTimeCount;		// 每日校盟捐款次数
    Int64 mContributeTuition;		// 累计捐献学费
    Int64 mCostDiamonds;		// 消费钻石记录
    Int64 mCoursesLevel;		// 进修班等级
    Int64 mCreateTime;		// 创建时间
    Int64 mCultureCount;		// 学生培养累计次数
    Int64 mDayActiveFinishInfo;		// 每日活跃度领取信息(二进制开关)
    Int64 mDayAddBossChallenge;		// 每日挑战限时boss次数
    Int64 mDayAddDiamonds;		// 每日充值钻石
    Int64 mDayAddExperience;		// 每日获得心得
    Int64 mDayAddTuition;		// 每日获得学费
    Int64 mDayAllianceFubenCount;		// 每日校盟副本挑战次数
    Int64 mDayCompetitionCount;		// 每日个人竞赛挑战次数
    Int64 mDayCount;		// 登陆天数
    Int64 mDayDaTiCount;		// 每日累计答题次数(用于统计答题多次后可以使用剩余的)
    Int64 mDayDiscussCount;		// 每日学术研讨挑战次数
    Int64 mDayDonateTuition;		// 每日捐献学费
    Int64 mDayFinishRefresherCount;		// 完成进修任务的数量,用于限制每日一次
    Int64 mDayLeagueCount;		// 每日学科联赛挑战次数
    Int64 mDayRecharge;		// 每日累计充值
    Int64 mDaySpringOut;		// 每日春游次数
    Int64 mDaySubDiamonds;		// 每日消费钻石
    Int64 mDaySubExperience;		// 每日消耗心得
    Int64 mDaySubTuition;		// 每日消耗学费
    Int64 mDayTaskActiveValue;		// 每日任务活跃度
    Int64 mDayTeachingCount;		// 每日获得教研币
    Int64 mDayUnmannedDroneCount;		
    Int64 mDayWishCount;		// 每日许愿的次数
    Int64 mDeblockingTime;		// 解封时间
    Int64 mDiamonds;		// 很重要的货币，可以买各种好东西，主要来源就是氪金，你懂的
    Int64 mDiscussCount;		// 累计学术研讨挑战次数
    Int64 mDiscussTime;		// 学术研讨赛季结束时间
    Int64 mDrawCount;		// 抽卡累计次数
    Int64 mEngage;		// 在【进修班】里获得的银币，可以用它在【进修商店】里购买道具
    Int64 mEngageCount;		// 累计获得进修币
    Int64 mEquipLevelExp;		// 装备强化未消耗经验
    Int64 mEquipLevelRate;		// 装备强化大暴击概率
    Int64 mEquipLvUp;		// 累计装备强化次数
    Int64 mExclusiveEquipCount;		// 累计学科资料(专属学习用品)
    Int64 mExperience;		// 容纳了多位老师的心得体会，主要用于【学生升级】
    Int64 mFetterCount;		// 累计羁绊关系数量
    Int64 mForbiddenChatTime;		// 被禁言解放时间(0表示无禁言, -1永久禁言)
    Int64 mFriendly;		// 通过【好友赠送】可以获得友情点，借助【外援】时需要消耗友情点
    Int64 mFriendlyCount;		// 累计获得友情点
    Int64 mGMLevel;		// gm等级
    Int64 mHonour;		// 【单人竞赛】挑战后获得的荣誉点，可以在【荣誉商店】用来购买一些稀有道具
    Int64 mHonourCount;		// 累计获得荣誉点
    Int64 mIcon;		// 头像id
    Int64 mIconFrame;		// 头像框id
    Int64 mIntegralCompetition;		// 个人竞赛积分
    Int64 mIntegralDiscuess;		// 学术研讨积分
    Int64 mJoinAllianceTime;		// 加入到校盟的时间
    Int64 mLeagueCount;		// 累计学科联赛挑战次数
    Int64 mLeagueLayer1;		// 学科联赛人文
    Int64 mLeagueLayer2;		// 学科联赛数理
    Int64 mLeagueLayer3;		// 学科联赛艺术
    Int64 mLeagueLayer4;		// 学科联赛体育
    Int64 mLeagueLayer5;		// 学科联赛综合
    Int64 mLevel;		// 学校等级
    Int64 mLevelExp;		// 学校升级所需任务列表
    Int64 mLoginOut;		// 玩家上次离线时间
    Int64 mOffLineTime;		// 离线时间单位为分钟
    Int64 mOilDrumCount;		// 满满的一桶汽油，是【春游】的必需品，可以在【荣誉商店】进行购买
    Int64 mOilDrumNum;		// 累计获得油桶
    Int64 mOnLine;		// 玩家本次登陆时间
    Int64 mPower;		// 体力
    Int64 mPowerCount;		// 购买体力的上限次数
    Int64 mPowerTime;		// 体力更新时间
    Int64 mQuicken;		// 快速教学次数
    Int64 mQuitAllianceTime;		
    Int64 mRealRecharge;		// 真实充值
    Int64 mRecharge;		// 充值总金额
    Int64 mRechargeDiamond;		// 累计充值钻石
    Int64 mResearchDesktop;		// 教研室的桌子数量
    Int64 mRevengeCompetitionCount;		// 累计个人复赛竞赛次数
    Int64 mSayHelloLastTime;		// 校盟打招呼最后时间(用于统计当天打招呼次数)
    Int64 mSchoolAlliaExp;		// 校盟的经验,可以升级校盟
    Int64 mSchoolUnion;		// 主要来自【校盟捐献】的奖励，可以在【校盟商店】购买一些学生和老师的碎片
    Int64 mSchoolUnionCount;		// 累计获得校盟币
    Int64 mSchoolUnionId;		// 校盟ID
    Int64 mScience;		// 在【学术研讨】中通过验证学生的成长，而获得的奖励，可以在【学术商店】进行消费
    Int64 mScienceCount;		// 累计获得学术币
    Int64 mScore;		// 总成绩
    Int64 mSecretaryExp;		// 秘书好感度,提升秘书等级
    Int64 mSecretaryLevel;		// 秘书等级
    Int64 mSendFriendly;		// 累计赠送友情点
    Int64 mServerLoginCount;		// 累计7日签到
    Int64 mSex;		// 性别
    Int64 mShopBuyCount;		// 累计购物次数
    Int64 mSpringCount;		// 春游累计次数
    Int64 mSpringLeiFengCount;		// 春游学雷锋累计次数
    Int64 mSpringOutingCount;		// 春游事件累计次数
    Int64 mStrengExp;		// 学习知识具象化的产物，用于强化学习用品。
    Int64 mStudentScore;		// 学生分数
    Int64 mStudentUpCount;		// 学生升级次数
    Int64 mSystemFlag;		// 系统功能解锁字段
    Int64 mTeaching;		// 【单人竞赛】后获得的奖励，让老师们在【教研室】进行教研，需要支付他们教研币
    Int64 mTeachingCount;		// 累计获得教研币
    Int64 mTeachingExtra;		// 狂欢额外收益值
    Int64 mTodayOnlineTime;		// 统计玩家当天时间
    Int64 mTotalAllianceContributeCount;		// 统计校盟捐献次数
    Int64 mTotalSelfCoursesTaskCount;		// 进修班升级累计自己的任务数
    Int64 mTrashCount;		// 累计垃圾数量
    Int64 mTuition;		// 学费是基础货币，【学校升级】和【扩建】都需要它
    Int64 mVipAwardIndex;		// vip奖励二进制
    Int64 mVipExp;		// vip经验
    Int64 mVipLv;		// VIP等级
    Int64 mWishReward;		// 许愿获取的奖励
    Int64 mbReceiveFirstRechargeReward;		// 是否领取了首充奖励

public:
    BaseProperty() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        CheckGet(scrData, mAcademicLayer);
        CheckGet(scrData, mAllianceDamage);
        CheckGet(scrData, mAllianceFubenCount);
        CheckGet(scrData, mAllianceJob);
        CheckGet(scrData, mAllianceSayHelloCount);
        CheckGet(scrData, mBestStudentQuality);
        CheckGet(scrData, mBossChallengeCount);
        CheckGet(scrData, mBossDamage);
        CheckGet(scrData, mChalkCount);
        CheckGet(scrData, mChalkFree);
        CheckGet(scrData, mChalkLimit);
        CheckGet(scrData, mClassScore);
        CheckGet(scrData, mCompetitionCount);
        CheckGet(scrData, mCompetitionTime);
        CheckGet(scrData, mContribute);
        CheckGet(scrData, mContributeCount);
        CheckGet(scrData, mContributeLastTime);
        CheckGet(scrData, mContributeTimeCount);
        CheckGet(scrData, mContributeTuition);
        CheckGet(scrData, mCostDiamonds);
        CheckGet(scrData, mCoursesLevel);
        CheckGet(scrData, mCreateTime);
        CheckGet(scrData, mCultureCount);
        CheckGet(scrData, mDayActiveFinishInfo);
        CheckGet(scrData, mDayAddBossChallenge);
        CheckGet(scrData, mDayAddDiamonds);
        CheckGet(scrData, mDayAddExperience);
        CheckGet(scrData, mDayAddTuition);
        CheckGet(scrData, mDayAllianceFubenCount);
        CheckGet(scrData, mDayCompetitionCount);
        CheckGet(scrData, mDayCount);
        CheckGet(scrData, mDayDaTiCount);
        CheckGet(scrData, mDayDiscussCount);
        CheckGet(scrData, mDayDonateTuition);
        CheckGet(scrData, mDayFinishRefresherCount);
        CheckGet(scrData, mDayLeagueCount);
        CheckGet(scrData, mDayRecharge);
        CheckGet(scrData, mDaySpringOut);
        CheckGet(scrData, mDaySubDiamonds);
        CheckGet(scrData, mDaySubExperience);
        CheckGet(scrData, mDaySubTuition);
        CheckGet(scrData, mDayTaskActiveValue);
        CheckGet(scrData, mDayTeachingCount);
        CheckGet(scrData, mDayUnmannedDroneCount);
        CheckGet(scrData, mDayWishCount);
        CheckGet(scrData, mDeblockingTime);
        CheckGet(scrData, mDiamonds);
        CheckGet(scrData, mDiscussCount);
        CheckGet(scrData, mDiscussTime);
        CheckGet(scrData, mDrawCount);
        CheckGet(scrData, mEngage);
        CheckGet(scrData, mEngageCount);
        CheckGet(scrData, mEquipLevelExp);
        CheckGet(scrData, mEquipLevelRate);
        CheckGet(scrData, mEquipLvUp);
        CheckGet(scrData, mExclusiveEquipCount);
        CheckGet(scrData, mExperience);
        CheckGet(scrData, mFetterCount);
        CheckGet(scrData, mForbiddenChatTime);
        CheckGet(scrData, mFriendly);
        CheckGet(scrData, mFriendlyCount);
        CheckGet(scrData, mGMLevel);
        CheckGet(scrData, mHonour);
        CheckGet(scrData, mHonourCount);
        CheckGet(scrData, mIcon);
        CheckGet(scrData, mIconFrame);
        CheckGet(scrData, mIntegralCompetition);
        CheckGet(scrData, mIntegralDiscuess);
        CheckGet(scrData, mJoinAllianceTime);
        CheckGet(scrData, mLeagueCount);
        CheckGet(scrData, mLeagueLayer1);
        CheckGet(scrData, mLeagueLayer2);
        CheckGet(scrData, mLeagueLayer3);
        CheckGet(scrData, mLeagueLayer4);
        CheckGet(scrData, mLeagueLayer5);
        CheckGet(scrData, mLevel);
        CheckGet(scrData, mLevelExp);
        CheckGet(scrData, mLoginOut);
        CheckGet(scrData, mOffLineTime);
        CheckGet(scrData, mOilDrumCount);
        CheckGet(scrData, mOilDrumNum);
        CheckGet(scrData, mOnLine);
        CheckGet(scrData, mPower);
        CheckGet(scrData, mPowerCount);
        CheckGet(scrData, mPowerTime);
        CheckGet(scrData, mQuicken);
        CheckGet(scrData, mQuitAllianceTime);
        CheckGet(scrData, mRealRecharge);
        CheckGet(scrData, mRecharge);
        CheckGet(scrData, mRechargeDiamond);
        CheckGet(scrData, mResearchDesktop);
        CheckGet(scrData, mRevengeCompetitionCount);
        CheckGet(scrData, mSayHelloLastTime);
        CheckGet(scrData, mSchoolAlliaExp);
        CheckGet(scrData, mSchoolUnion);
        CheckGet(scrData, mSchoolUnionCount);
        CheckGet(scrData, mSchoolUnionId);
        CheckGet(scrData, mScience);
        CheckGet(scrData, mScienceCount);
        CheckGet(scrData, mScore);
        CheckGet(scrData, mSecretaryExp);
        CheckGet(scrData, mSecretaryLevel);
        CheckGet(scrData, mSendFriendly);
        CheckGet(scrData, mServerLoginCount);
        CheckGet(scrData, mSex);
        CheckGet(scrData, mShopBuyCount);
        CheckGet(scrData, mSpringCount);
        CheckGet(scrData, mSpringLeiFengCount);
        CheckGet(scrData, mSpringOutingCount);
        CheckGet(scrData, mStrengExp);
        CheckGet(scrData, mStudentScore);
        CheckGet(scrData, mStudentUpCount);
        CheckGet(scrData, mSystemFlag);
        CheckGet(scrData, mTeaching);
        CheckGet(scrData, mTeachingCount);
        CheckGet(scrData, mTeachingExtra);
        CheckGet(scrData, mTodayOnlineTime);
        CheckGet(scrData, mTotalAllianceContributeCount);
        CheckGet(scrData, mTotalSelfCoursesTaskCount);
        CheckGet(scrData, mTrashCount);
        CheckGet(scrData, mTuition);
        CheckGet(scrData, mVipAwardIndex);
        CheckGet(scrData, mVipExp);
        CheckGet(scrData, mVipLv);
        CheckGet(scrData, mWishReward);
        CheckGet(scrData, mbReceiveFirstRechargeReward);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mAcademicLayer"] = mAcademicLayer;
        destData["mAllianceDamage"] = mAllianceDamage;
        destData["mAllianceFubenCount"] = mAllianceFubenCount;
        destData["mAllianceJob"] = mAllianceJob;
        destData["mAllianceSayHelloCount"] = mAllianceSayHelloCount;
        destData["mBestStudentQuality"] = mBestStudentQuality;
        destData["mBossChallengeCount"] = mBossChallengeCount;
        destData["mBossDamage"] = mBossDamage;
        destData["mChalkCount"] = mChalkCount;
        destData["mChalkFree"] = mChalkFree;
        destData["mChalkLimit"] = mChalkLimit;
        destData["mClassScore"] = mClassScore;
        destData["mCompetitionCount"] = mCompetitionCount;
        destData["mCompetitionTime"] = mCompetitionTime;
        destData["mContribute"] = mContribute;
        destData["mContributeCount"] = mContributeCount;
        destData["mContributeLastTime"] = mContributeLastTime;
        destData["mContributeTimeCount"] = mContributeTimeCount;
        destData["mContributeTuition"] = mContributeTuition;
        destData["mCostDiamonds"] = mCostDiamonds;
        destData["mCoursesLevel"] = mCoursesLevel;
        destData["mCreateTime"] = mCreateTime;
        destData["mCultureCount"] = mCultureCount;
        destData["mDayActiveFinishInfo"] = mDayActiveFinishInfo;
        destData["mDayAddBossChallenge"] = mDayAddBossChallenge;
        destData["mDayAddDiamonds"] = mDayAddDiamonds;
        destData["mDayAddExperience"] = mDayAddExperience;
        destData["mDayAddTuition"] = mDayAddTuition;
        destData["mDayAllianceFubenCount"] = mDayAllianceFubenCount;
        destData["mDayCompetitionCount"] = mDayCompetitionCount;
        destData["mDayCount"] = mDayCount;
        destData["mDayDaTiCount"] = mDayDaTiCount;
        destData["mDayDiscussCount"] = mDayDiscussCount;
        destData["mDayDonateTuition"] = mDayDonateTuition;
        destData["mDayFinishRefresherCount"] = mDayFinishRefresherCount;
        destData["mDayLeagueCount"] = mDayLeagueCount;
        destData["mDayRecharge"] = mDayRecharge;
        destData["mDaySpringOut"] = mDaySpringOut;
        destData["mDaySubDiamonds"] = mDaySubDiamonds;
        destData["mDaySubExperience"] = mDaySubExperience;
        destData["mDaySubTuition"] = mDaySubTuition;
        destData["mDayTaskActiveValue"] = mDayTaskActiveValue;
        destData["mDayTeachingCount"] = mDayTeachingCount;
        destData["mDayUnmannedDroneCount"] = mDayUnmannedDroneCount;
        destData["mDayWishCount"] = mDayWishCount;
        destData["mDeblockingTime"] = mDeblockingTime;
        destData["mDiamonds"] = mDiamonds;
        destData["mDiscussCount"] = mDiscussCount;
        destData["mDiscussTime"] = mDiscussTime;
        destData["mDrawCount"] = mDrawCount;
        destData["mEngage"] = mEngage;
        destData["mEngageCount"] = mEngageCount;
        destData["mEquipLevelExp"] = mEquipLevelExp;
        destData["mEquipLevelRate"] = mEquipLevelRate;
        destData["mEquipLvUp"] = mEquipLvUp;
        destData["mExclusiveEquipCount"] = mExclusiveEquipCount;
        destData["mExperience"] = mExperience;
        destData["mFetterCount"] = mFetterCount;
        destData["mForbiddenChatTime"] = mForbiddenChatTime;
        destData["mFriendly"] = mFriendly;
        destData["mFriendlyCount"] = mFriendlyCount;
        destData["mGMLevel"] = mGMLevel;
        destData["mHonour"] = mHonour;
        destData["mHonourCount"] = mHonourCount;
        destData["mIcon"] = mIcon;
        destData["mIconFrame"] = mIconFrame;
        destData["mIntegralCompetition"] = mIntegralCompetition;
        destData["mIntegralDiscuess"] = mIntegralDiscuess;
        destData["mJoinAllianceTime"] = mJoinAllianceTime;
        destData["mLeagueCount"] = mLeagueCount;
        destData["mLeagueLayer1"] = mLeagueLayer1;
        destData["mLeagueLayer2"] = mLeagueLayer2;
        destData["mLeagueLayer3"] = mLeagueLayer3;
        destData["mLeagueLayer4"] = mLeagueLayer4;
        destData["mLeagueLayer5"] = mLeagueLayer5;
        destData["mLevel"] = mLevel;
        destData["mLevelExp"] = mLevelExp;
        destData["mLoginOut"] = mLoginOut;
        destData["mOffLineTime"] = mOffLineTime;
        destData["mOilDrumCount"] = mOilDrumCount;
        destData["mOilDrumNum"] = mOilDrumNum;
        destData["mOnLine"] = mOnLine;
        destData["mPower"] = mPower;
        destData["mPowerCount"] = mPowerCount;
        destData["mPowerTime"] = mPowerTime;
        destData["mQuicken"] = mQuicken;
        destData["mQuitAllianceTime"] = mQuitAllianceTime;
        destData["mRealRecharge"] = mRealRecharge;
        destData["mRecharge"] = mRecharge;
        destData["mRechargeDiamond"] = mRechargeDiamond;
        destData["mResearchDesktop"] = mResearchDesktop;
        destData["mRevengeCompetitionCount"] = mRevengeCompetitionCount;
        destData["mSayHelloLastTime"] = mSayHelloLastTime;
        destData["mSchoolAlliaExp"] = mSchoolAlliaExp;
        destData["mSchoolUnion"] = mSchoolUnion;
        destData["mSchoolUnionCount"] = mSchoolUnionCount;
        destData["mSchoolUnionId"] = mSchoolUnionId;
        destData["mScience"] = mScience;
        destData["mScienceCount"] = mScienceCount;
        destData["mScore"] = mScore;
        destData["mSecretaryExp"] = mSecretaryExp;
        destData["mSecretaryLevel"] = mSecretaryLevel;
        destData["mSendFriendly"] = mSendFriendly;
        destData["mServerLoginCount"] = mServerLoginCount;
        destData["mSex"] = mSex;
        destData["mShopBuyCount"] = mShopBuyCount;
        destData["mSpringCount"] = mSpringCount;
        destData["mSpringLeiFengCount"] = mSpringLeiFengCount;
        destData["mSpringOutingCount"] = mSpringOutingCount;
        destData["mStrengExp"] = mStrengExp;
        destData["mStudentScore"] = mStudentScore;
        destData["mStudentUpCount"] = mStudentUpCount;
        destData["mSystemFlag"] = mSystemFlag;
        destData["mTeaching"] = mTeaching;
        destData["mTeachingCount"] = mTeachingCount;
        destData["mTeachingExtra"] = mTeachingExtra;
        destData["mTodayOnlineTime"] = mTodayOnlineTime;
        destData["mTotalAllianceContributeCount"] = mTotalAllianceContributeCount;
        destData["mTotalSelfCoursesTaskCount"] = mTotalSelfCoursesTaskCount;
        destData["mTrashCount"] = mTrashCount;
        destData["mTuition"] = mTuition;
        destData["mVipAwardIndex"] = mVipAwardIndex;
        destData["mVipExp"] = mVipExp;
        destData["mVipLv"] = mVipLv;
        destData["mWishReward"] = mWishReward;
        destData["mbReceiveFirstRechargeReward"] = mbReceiveFirstRechargeReward;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)126);

        SAVE_MSG_VALUE(mAcademicLayer, 8);
        SAVE_MSG_VALUE(mAllianceDamage, 8);
        SAVE_MSG_VALUE(mAllianceFubenCount, 8);
        SAVE_MSG_VALUE(mAllianceJob, 8);
        SAVE_MSG_VALUE(mAllianceSayHelloCount, 8);
        SAVE_MSG_VALUE(mBestStudentQuality, 8);
        SAVE_MSG_VALUE(mBossChallengeCount, 8);
        SAVE_MSG_VALUE(mBossDamage, 8);
        SAVE_MSG_VALUE(mChalkCount, 8);
        SAVE_MSG_VALUE(mChalkFree, 8);
        SAVE_MSG_VALUE(mChalkLimit, 8);
        SAVE_MSG_VALUE(mClassScore, 8);
        SAVE_MSG_VALUE(mCompetitionCount, 8);
        SAVE_MSG_VALUE(mCompetitionTime, 8);
        SAVE_MSG_VALUE(mContribute, 8);
        SAVE_MSG_VALUE(mContributeCount, 8);
        SAVE_MSG_VALUE(mContributeLastTime, 8);
        SAVE_MSG_VALUE(mContributeTimeCount, 8);
        SAVE_MSG_VALUE(mContributeTuition, 8);
        SAVE_MSG_VALUE(mCostDiamonds, 8);
        SAVE_MSG_VALUE(mCoursesLevel, 8);
        SAVE_MSG_VALUE(mCreateTime, 8);
        SAVE_MSG_VALUE(mCultureCount, 8);
        SAVE_MSG_VALUE(mDayActiveFinishInfo, 8);
        SAVE_MSG_VALUE(mDayAddBossChallenge, 8);
        SAVE_MSG_VALUE(mDayAddDiamonds, 8);
        SAVE_MSG_VALUE(mDayAddExperience, 8);
        SAVE_MSG_VALUE(mDayAddTuition, 8);
        SAVE_MSG_VALUE(mDayAllianceFubenCount, 8);
        SAVE_MSG_VALUE(mDayCompetitionCount, 8);
        SAVE_MSG_VALUE(mDayCount, 8);
        SAVE_MSG_VALUE(mDayDaTiCount, 8);
        SAVE_MSG_VALUE(mDayDiscussCount, 8);
        SAVE_MSG_VALUE(mDayDonateTuition, 8);
        SAVE_MSG_VALUE(mDayFinishRefresherCount, 8);
        SAVE_MSG_VALUE(mDayLeagueCount, 8);
        SAVE_MSG_VALUE(mDayRecharge, 8);
        SAVE_MSG_VALUE(mDaySpringOut, 8);
        SAVE_MSG_VALUE(mDaySubDiamonds, 8);
        SAVE_MSG_VALUE(mDaySubExperience, 8);
        SAVE_MSG_VALUE(mDaySubTuition, 8);
        SAVE_MSG_VALUE(mDayTaskActiveValue, 8);
        SAVE_MSG_VALUE(mDayTeachingCount, 8);
        SAVE_MSG_VALUE(mDayUnmannedDroneCount, 8);
        SAVE_MSG_VALUE(mDayWishCount, 8);
        SAVE_MSG_VALUE(mDeblockingTime, 8);
        SAVE_MSG_VALUE(mDiamonds, 8);
        SAVE_MSG_VALUE(mDiscussCount, 8);
        SAVE_MSG_VALUE(mDiscussTime, 8);
        SAVE_MSG_VALUE(mDrawCount, 8);
        SAVE_MSG_VALUE(mEngage, 8);
        SAVE_MSG_VALUE(mEngageCount, 8);
        SAVE_MSG_VALUE(mEquipLevelExp, 8);
        SAVE_MSG_VALUE(mEquipLevelRate, 8);
        SAVE_MSG_VALUE(mEquipLvUp, 8);
        SAVE_MSG_VALUE(mExclusiveEquipCount, 8);
        SAVE_MSG_VALUE(mExperience, 8);
        SAVE_MSG_VALUE(mFetterCount, 8);
        SAVE_MSG_VALUE(mForbiddenChatTime, 8);
        SAVE_MSG_VALUE(mFriendly, 8);
        SAVE_MSG_VALUE(mFriendlyCount, 8);
        SAVE_MSG_VALUE(mGMLevel, 8);
        SAVE_MSG_VALUE(mHonour, 8);
        SAVE_MSG_VALUE(mHonourCount, 8);
        SAVE_MSG_VALUE(mIcon, 8);
        SAVE_MSG_VALUE(mIconFrame, 8);
        SAVE_MSG_VALUE(mIntegralCompetition, 8);
        SAVE_MSG_VALUE(mIntegralDiscuess, 8);
        SAVE_MSG_VALUE(mJoinAllianceTime, 8);
        SAVE_MSG_VALUE(mLeagueCount, 8);
        SAVE_MSG_VALUE(mLeagueLayer1, 8);
        SAVE_MSG_VALUE(mLeagueLayer2, 8);
        SAVE_MSG_VALUE(mLeagueLayer3, 8);
        SAVE_MSG_VALUE(mLeagueLayer4, 8);
        SAVE_MSG_VALUE(mLeagueLayer5, 8);
        SAVE_MSG_VALUE(mLevel, 8);
        SAVE_MSG_VALUE(mLevelExp, 8);
        SAVE_MSG_VALUE(mLoginOut, 8);
        SAVE_MSG_VALUE(mOffLineTime, 8);
        SAVE_MSG_VALUE(mOilDrumCount, 8);
        SAVE_MSG_VALUE(mOilDrumNum, 8);
        SAVE_MSG_VALUE(mOnLine, 8);
        SAVE_MSG_VALUE(mPower, 8);
        SAVE_MSG_VALUE(mPowerCount, 8);
        SAVE_MSG_VALUE(mPowerTime, 8);
        SAVE_MSG_VALUE(mQuicken, 8);
        SAVE_MSG_VALUE(mQuitAllianceTime, 8);
        SAVE_MSG_VALUE(mRealRecharge, 8);
        SAVE_MSG_VALUE(mRecharge, 8);
        SAVE_MSG_VALUE(mRechargeDiamond, 8);
        SAVE_MSG_VALUE(mResearchDesktop, 8);
        SAVE_MSG_VALUE(mRevengeCompetitionCount, 8);
        SAVE_MSG_VALUE(mSayHelloLastTime, 8);
        SAVE_MSG_VALUE(mSchoolAlliaExp, 8);
        SAVE_MSG_VALUE(mSchoolUnion, 8);
        SAVE_MSG_VALUE(mSchoolUnionCount, 8);
        SAVE_MSG_VALUE(mSchoolUnionId, 8);
        SAVE_MSG_VALUE(mScience, 8);
        SAVE_MSG_VALUE(mScienceCount, 8);
        SAVE_MSG_VALUE(mScore, 8);
        SAVE_MSG_VALUE(mSecretaryExp, 8);
        SAVE_MSG_VALUE(mSecretaryLevel, 8);
        SAVE_MSG_VALUE(mSendFriendly, 8);
        SAVE_MSG_VALUE(mServerLoginCount, 8);
        SAVE_MSG_VALUE(mSex, 8);
        SAVE_MSG_VALUE(mShopBuyCount, 8);
        SAVE_MSG_VALUE(mSpringCount, 8);
        SAVE_MSG_VALUE(mSpringLeiFengCount, 8);
        SAVE_MSG_VALUE(mSpringOutingCount, 8);
        SAVE_MSG_VALUE(mStrengExp, 8);
        SAVE_MSG_VALUE(mStudentScore, 8);
        SAVE_MSG_VALUE(mStudentUpCount, 8);
        SAVE_MSG_VALUE(mSystemFlag, 8);
        SAVE_MSG_VALUE(mTeaching, 8);
        SAVE_MSG_VALUE(mTeachingCount, 8);
        SAVE_MSG_VALUE(mTeachingExtra, 8);
        SAVE_MSG_VALUE(mTodayOnlineTime, 8);
        SAVE_MSG_VALUE(mTotalAllianceContributeCount, 8);
        SAVE_MSG_VALUE(mTotalSelfCoursesTaskCount, 8);
        SAVE_MSG_VALUE(mTrashCount, 8);
        SAVE_MSG_VALUE(mTuition, 8);
        SAVE_MSG_VALUE(mVipAwardIndex, 8);
        SAVE_MSG_VALUE(mVipExp, 8);
        SAVE_MSG_VALUE(mVipLv, 8);
        SAVE_MSG_VALUE(mWishReward, 8);
        SAVE_MSG_VALUE(mbReceiveFirstRechargeReward, 8);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mAcademicLayer = 0;
        mAllianceDamage = 0;
        mAllianceFubenCount = 0;
        mAllianceJob = 0;
        mAllianceSayHelloCount = 0;
        mBestStudentQuality = 0;
        mBossChallengeCount = 0;
        mBossDamage = 0;
        mChalkCount = 0;
        mChalkFree = 0;
        mChalkLimit = 0;
        mClassScore = 0;
        mCompetitionCount = 0;
        mCompetitionTime = 0;
        mContribute = 0;
        mContributeCount = 0;
        mContributeLastTime = 0;
        mContributeTimeCount = 0;
        mContributeTuition = 0;
        mCostDiamonds = 0;
        mCoursesLevel = 0;
        mCreateTime = 0;
        mCultureCount = 0;
        mDayActiveFinishInfo = 0;
        mDayAddBossChallenge = 0;
        mDayAddDiamonds = 0;
        mDayAddExperience = 0;
        mDayAddTuition = 0;
        mDayAllianceFubenCount = 0;
        mDayCompetitionCount = 0;
        mDayCount = 0;
        mDayDaTiCount = 0;
        mDayDiscussCount = 0;
        mDayDonateTuition = 0;
        mDayFinishRefresherCount = 0;
        mDayLeagueCount = 0;
        mDayRecharge = 0;
        mDaySpringOut = 0;
        mDaySubDiamonds = 0;
        mDaySubExperience = 0;
        mDaySubTuition = 0;
        mDayTaskActiveValue = 0;
        mDayTeachingCount = 0;
        mDayUnmannedDroneCount = 0;
        mDayWishCount = 0;
        mDeblockingTime = 0;
        mDiamonds = 0;
        mDiscussCount = 0;
        mDiscussTime = 0;
        mDrawCount = 0;
        mEngage = 0;
        mEngageCount = 0;
        mEquipLevelExp = 0;
        mEquipLevelRate = 0;
        mEquipLvUp = 0;
        mExclusiveEquipCount = 0;
        mExperience = 0;
        mFetterCount = 0;
        mForbiddenChatTime = 0;
        mFriendly = 0;
        mFriendlyCount = 0;
        mGMLevel = 0;
        mHonour = 0;
        mHonourCount = 0;
        mIcon = 0;
        mIconFrame = 0;
        mIntegralCompetition = 0;
        mIntegralDiscuess = 0;
        mJoinAllianceTime = 0;
        mLeagueCount = 0;
        mLeagueLayer1 = 0;
        mLeagueLayer2 = 0;
        mLeagueLayer3 = 0;
        mLeagueLayer4 = 0;
        mLeagueLayer5 = 0;
        mLevel = 0;
        mLevelExp = 0;
        mLoginOut = 0;
        mOffLineTime = 0;
        mOilDrumCount = 0;
        mOilDrumNum = 0;
        mOnLine = 0;
        mPower = 0;
        mPowerCount = 0;
        mPowerTime = 0;
        mQuicken = 0;
        mQuitAllianceTime = 0;
        mRealRecharge = 0;
        mRecharge = 0;
        mRechargeDiamond = 0;
        mResearchDesktop = 0;
        mRevengeCompetitionCount = 0;
        mSayHelloLastTime = 0;
        mSchoolAlliaExp = 0;
        mSchoolUnion = 0;
        mSchoolUnionCount = 0;
        mSchoolUnionId = 0;
        mScience = 0;
        mScienceCount = 0;
        mScore = 0;
        mSecretaryExp = 0;
        mSecretaryLevel = 0;
        mSendFriendly = 0;
        mServerLoginCount = 0;
        mSex = 0;
        mShopBuyCount = 0;
        mSpringCount = 0;
        mSpringLeiFengCount = 0;
        mSpringOutingCount = 0;
        mStrengExp = 0;
        mStudentScore = 0;
        mStudentUpCount = 0;
        mSystemFlag = 0;
        mTeaching = 0;
        mTeachingCount = 0;
        mTeachingExtra = 0;
        mTodayOnlineTime = 0;
        mTotalAllianceContributeCount = 0;
        mTotalSelfCoursesTaskCount = 0;
        mTrashCount = 0;
        mTuition = 0;
        mVipAwardIndex = 0;
        mVipExp = 0;
        mVipLv = 0;
        mWishReward = 0;
        mbReceiveFirstRechargeReward = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("BaseProperty", otherMsg.GetMsgName())!=0) { LOG("%s is not BaseProperty", otherMsg.GetMsgName()); return; }; const BaseProperty &other = *(const BaseProperty*)(&otherMsg);
        mAcademicLayer = other.mAcademicLayer;
        mAllianceDamage = other.mAllianceDamage;
        mAllianceFubenCount = other.mAllianceFubenCount;
        mAllianceJob = other.mAllianceJob;
        mAllianceSayHelloCount = other.mAllianceSayHelloCount;
        mBestStudentQuality = other.mBestStudentQuality;
        mBossChallengeCount = other.mBossChallengeCount;
        mBossDamage = other.mBossDamage;
        mChalkCount = other.mChalkCount;
        mChalkFree = other.mChalkFree;
        mChalkLimit = other.mChalkLimit;
        mClassScore = other.mClassScore;
        mCompetitionCount = other.mCompetitionCount;
        mCompetitionTime = other.mCompetitionTime;
        mContribute = other.mContribute;
        mContributeCount = other.mContributeCount;
        mContributeLastTime = other.mContributeLastTime;
        mContributeTimeCount = other.mContributeTimeCount;
        mContributeTuition = other.mContributeTuition;
        mCostDiamonds = other.mCostDiamonds;
        mCoursesLevel = other.mCoursesLevel;
        mCreateTime = other.mCreateTime;
        mCultureCount = other.mCultureCount;
        mDayActiveFinishInfo = other.mDayActiveFinishInfo;
        mDayAddBossChallenge = other.mDayAddBossChallenge;
        mDayAddDiamonds = other.mDayAddDiamonds;
        mDayAddExperience = other.mDayAddExperience;
        mDayAddTuition = other.mDayAddTuition;
        mDayAllianceFubenCount = other.mDayAllianceFubenCount;
        mDayCompetitionCount = other.mDayCompetitionCount;
        mDayCount = other.mDayCount;
        mDayDaTiCount = other.mDayDaTiCount;
        mDayDiscussCount = other.mDayDiscussCount;
        mDayDonateTuition = other.mDayDonateTuition;
        mDayFinishRefresherCount = other.mDayFinishRefresherCount;
        mDayLeagueCount = other.mDayLeagueCount;
        mDayRecharge = other.mDayRecharge;
        mDaySpringOut = other.mDaySpringOut;
        mDaySubDiamonds = other.mDaySubDiamonds;
        mDaySubExperience = other.mDaySubExperience;
        mDaySubTuition = other.mDaySubTuition;
        mDayTaskActiveValue = other.mDayTaskActiveValue;
        mDayTeachingCount = other.mDayTeachingCount;
        mDayUnmannedDroneCount = other.mDayUnmannedDroneCount;
        mDayWishCount = other.mDayWishCount;
        mDeblockingTime = other.mDeblockingTime;
        mDiamonds = other.mDiamonds;
        mDiscussCount = other.mDiscussCount;
        mDiscussTime = other.mDiscussTime;
        mDrawCount = other.mDrawCount;
        mEngage = other.mEngage;
        mEngageCount = other.mEngageCount;
        mEquipLevelExp = other.mEquipLevelExp;
        mEquipLevelRate = other.mEquipLevelRate;
        mEquipLvUp = other.mEquipLvUp;
        mExclusiveEquipCount = other.mExclusiveEquipCount;
        mExperience = other.mExperience;
        mFetterCount = other.mFetterCount;
        mForbiddenChatTime = other.mForbiddenChatTime;
        mFriendly = other.mFriendly;
        mFriendlyCount = other.mFriendlyCount;
        mGMLevel = other.mGMLevel;
        mHonour = other.mHonour;
        mHonourCount = other.mHonourCount;
        mIcon = other.mIcon;
        mIconFrame = other.mIconFrame;
        mIntegralCompetition = other.mIntegralCompetition;
        mIntegralDiscuess = other.mIntegralDiscuess;
        mJoinAllianceTime = other.mJoinAllianceTime;
        mLeagueCount = other.mLeagueCount;
        mLeagueLayer1 = other.mLeagueLayer1;
        mLeagueLayer2 = other.mLeagueLayer2;
        mLeagueLayer3 = other.mLeagueLayer3;
        mLeagueLayer4 = other.mLeagueLayer4;
        mLeagueLayer5 = other.mLeagueLayer5;
        mLevel = other.mLevel;
        mLevelExp = other.mLevelExp;
        mLoginOut = other.mLoginOut;
        mOffLineTime = other.mOffLineTime;
        mOilDrumCount = other.mOilDrumCount;
        mOilDrumNum = other.mOilDrumNum;
        mOnLine = other.mOnLine;
        mPower = other.mPower;
        mPowerCount = other.mPowerCount;
        mPowerTime = other.mPowerTime;
        mQuicken = other.mQuicken;
        mQuitAllianceTime = other.mQuitAllianceTime;
        mRealRecharge = other.mRealRecharge;
        mRecharge = other.mRecharge;
        mRechargeDiamond = other.mRechargeDiamond;
        mResearchDesktop = other.mResearchDesktop;
        mRevengeCompetitionCount = other.mRevengeCompetitionCount;
        mSayHelloLastTime = other.mSayHelloLastTime;
        mSchoolAlliaExp = other.mSchoolAlliaExp;
        mSchoolUnion = other.mSchoolUnion;
        mSchoolUnionCount = other.mSchoolUnionCount;
        mSchoolUnionId = other.mSchoolUnionId;
        mScience = other.mScience;
        mScienceCount = other.mScienceCount;
        mScore = other.mScore;
        mSecretaryExp = other.mSecretaryExp;
        mSecretaryLevel = other.mSecretaryLevel;
        mSendFriendly = other.mSendFriendly;
        mServerLoginCount = other.mServerLoginCount;
        mSex = other.mSex;
        mShopBuyCount = other.mShopBuyCount;
        mSpringCount = other.mSpringCount;
        mSpringLeiFengCount = other.mSpringLeiFengCount;
        mSpringOutingCount = other.mSpringOutingCount;
        mStrengExp = other.mStrengExp;
        mStudentScore = other.mStudentScore;
        mStudentUpCount = other.mStudentUpCount;
        mSystemFlag = other.mSystemFlag;
        mTeaching = other.mTeaching;
        mTeachingCount = other.mTeachingCount;
        mTeachingExtra = other.mTeachingExtra;
        mTodayOnlineTime = other.mTodayOnlineTime;
        mTotalAllianceContributeCount = other.mTotalAllianceContributeCount;
        mTotalSelfCoursesTaskCount = other.mTotalSelfCoursesTaskCount;
        mTrashCount = other.mTrashCount;
        mTuition = other.mTuition;
        mVipAwardIndex = other.mVipAwardIndex;
        mVipExp = other.mVipExp;
        mVipLv = other.mVipLv;
        mWishReward = other.mWishReward;
        mbReceiveFirstRechargeReward = other.mbReceiveFirstRechargeReward;
    }

    virtual const char* GetMsgName() const override { return "BaseProperty"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mAcademicLayer")==0) { AData value; value = mAcademicLayer; return value; }
        if (strcmp(szMember, "mAllianceDamage")==0) { AData value; value = mAllianceDamage; return value; }
        if (strcmp(szMember, "mAllianceFubenCount")==0) { AData value; value = mAllianceFubenCount; return value; }
        if (strcmp(szMember, "mAllianceJob")==0) { AData value; value = mAllianceJob; return value; }
        if (strcmp(szMember, "mAllianceSayHelloCount")==0) { AData value; value = mAllianceSayHelloCount; return value; }
        if (strcmp(szMember, "mBestStudentQuality")==0) { AData value; value = mBestStudentQuality; return value; }
        if (strcmp(szMember, "mBossChallengeCount")==0) { AData value; value = mBossChallengeCount; return value; }
        if (strcmp(szMember, "mBossDamage")==0) { AData value; value = mBossDamage; return value; }
        if (strcmp(szMember, "mChalkCount")==0) { AData value; value = mChalkCount; return value; }
        if (strcmp(szMember, "mChalkFree")==0) { AData value; value = mChalkFree; return value; }
        if (strcmp(szMember, "mChalkLimit")==0) { AData value; value = mChalkLimit; return value; }
        if (strcmp(szMember, "mClassScore")==0) { AData value; value = mClassScore; return value; }
        if (strcmp(szMember, "mCompetitionCount")==0) { AData value; value = mCompetitionCount; return value; }
        if (strcmp(szMember, "mCompetitionTime")==0) { AData value; value = mCompetitionTime; return value; }
        if (strcmp(szMember, "mContribute")==0) { AData value; value = mContribute; return value; }
        if (strcmp(szMember, "mContributeCount")==0) { AData value; value = mContributeCount; return value; }
        if (strcmp(szMember, "mContributeLastTime")==0) { AData value; value = mContributeLastTime; return value; }
        if (strcmp(szMember, "mContributeTimeCount")==0) { AData value; value = mContributeTimeCount; return value; }
        if (strcmp(szMember, "mContributeTuition")==0) { AData value; value = mContributeTuition; return value; }
        if (strcmp(szMember, "mCostDiamonds")==0) { AData value; value = mCostDiamonds; return value; }
        if (strcmp(szMember, "mCoursesLevel")==0) { AData value; value = mCoursesLevel; return value; }
        if (strcmp(szMember, "mCreateTime")==0) { AData value; value = mCreateTime; return value; }
        if (strcmp(szMember, "mCultureCount")==0) { AData value; value = mCultureCount; return value; }
        if (strcmp(szMember, "mDayActiveFinishInfo")==0) { AData value; value = mDayActiveFinishInfo; return value; }
        if (strcmp(szMember, "mDayAddBossChallenge")==0) { AData value; value = mDayAddBossChallenge; return value; }
        if (strcmp(szMember, "mDayAddDiamonds")==0) { AData value; value = mDayAddDiamonds; return value; }
        if (strcmp(szMember, "mDayAddExperience")==0) { AData value; value = mDayAddExperience; return value; }
        if (strcmp(szMember, "mDayAddTuition")==0) { AData value; value = mDayAddTuition; return value; }
        if (strcmp(szMember, "mDayAllianceFubenCount")==0) { AData value; value = mDayAllianceFubenCount; return value; }
        if (strcmp(szMember, "mDayCompetitionCount")==0) { AData value; value = mDayCompetitionCount; return value; }
        if (strcmp(szMember, "mDayCount")==0) { AData value; value = mDayCount; return value; }
        if (strcmp(szMember, "mDayDaTiCount")==0) { AData value; value = mDayDaTiCount; return value; }
        if (strcmp(szMember, "mDayDiscussCount")==0) { AData value; value = mDayDiscussCount; return value; }
        if (strcmp(szMember, "mDayDonateTuition")==0) { AData value; value = mDayDonateTuition; return value; }
        if (strcmp(szMember, "mDayFinishRefresherCount")==0) { AData value; value = mDayFinishRefresherCount; return value; }
        if (strcmp(szMember, "mDayLeagueCount")==0) { AData value; value = mDayLeagueCount; return value; }
        if (strcmp(szMember, "mDayRecharge")==0) { AData value; value = mDayRecharge; return value; }
        if (strcmp(szMember, "mDaySpringOut")==0) { AData value; value = mDaySpringOut; return value; }
        if (strcmp(szMember, "mDaySubDiamonds")==0) { AData value; value = mDaySubDiamonds; return value; }
        if (strcmp(szMember, "mDaySubExperience")==0) { AData value; value = mDaySubExperience; return value; }
        if (strcmp(szMember, "mDaySubTuition")==0) { AData value; value = mDaySubTuition; return value; }
        if (strcmp(szMember, "mDayTaskActiveValue")==0) { AData value; value = mDayTaskActiveValue; return value; }
        if (strcmp(szMember, "mDayTeachingCount")==0) { AData value; value = mDayTeachingCount; return value; }
        if (strcmp(szMember, "mDayUnmannedDroneCount")==0) { AData value; value = mDayUnmannedDroneCount; return value; }
        if (strcmp(szMember, "mDayWishCount")==0) { AData value; value = mDayWishCount; return value; }
        if (strcmp(szMember, "mDeblockingTime")==0) { AData value; value = mDeblockingTime; return value; }
        if (strcmp(szMember, "mDiamonds")==0) { AData value; value = mDiamonds; return value; }
        if (strcmp(szMember, "mDiscussCount")==0) { AData value; value = mDiscussCount; return value; }
        if (strcmp(szMember, "mDiscussTime")==0) { AData value; value = mDiscussTime; return value; }
        if (strcmp(szMember, "mDrawCount")==0) { AData value; value = mDrawCount; return value; }
        if (strcmp(szMember, "mEngage")==0) { AData value; value = mEngage; return value; }
        if (strcmp(szMember, "mEngageCount")==0) { AData value; value = mEngageCount; return value; }
        if (strcmp(szMember, "mEquipLevelExp")==0) { AData value; value = mEquipLevelExp; return value; }
        if (strcmp(szMember, "mEquipLevelRate")==0) { AData value; value = mEquipLevelRate; return value; }
        if (strcmp(szMember, "mEquipLvUp")==0) { AData value; value = mEquipLvUp; return value; }
        if (strcmp(szMember, "mExclusiveEquipCount")==0) { AData value; value = mExclusiveEquipCount; return value; }
        if (strcmp(szMember, "mExperience")==0) { AData value; value = mExperience; return value; }
        if (strcmp(szMember, "mFetterCount")==0) { AData value; value = mFetterCount; return value; }
        if (strcmp(szMember, "mForbiddenChatTime")==0) { AData value; value = mForbiddenChatTime; return value; }
        if (strcmp(szMember, "mFriendly")==0) { AData value; value = mFriendly; return value; }
        if (strcmp(szMember, "mFriendlyCount")==0) { AData value; value = mFriendlyCount; return value; }
        if (strcmp(szMember, "mGMLevel")==0) { AData value; value = mGMLevel; return value; }
        if (strcmp(szMember, "mHonour")==0) { AData value; value = mHonour; return value; }
        if (strcmp(szMember, "mHonourCount")==0) { AData value; value = mHonourCount; return value; }
        if (strcmp(szMember, "mIcon")==0) { AData value; value = mIcon; return value; }
        if (strcmp(szMember, "mIconFrame")==0) { AData value; value = mIconFrame; return value; }
        if (strcmp(szMember, "mIntegralCompetition")==0) { AData value; value = mIntegralCompetition; return value; }
        if (strcmp(szMember, "mIntegralDiscuess")==0) { AData value; value = mIntegralDiscuess; return value; }
        if (strcmp(szMember, "mJoinAllianceTime")==0) { AData value; value = mJoinAllianceTime; return value; }
        if (strcmp(szMember, "mLeagueCount")==0) { AData value; value = mLeagueCount; return value; }
        if (strcmp(szMember, "mLeagueLayer1")==0) { AData value; value = mLeagueLayer1; return value; }
        if (strcmp(szMember, "mLeagueLayer2")==0) { AData value; value = mLeagueLayer2; return value; }
        if (strcmp(szMember, "mLeagueLayer3")==0) { AData value; value = mLeagueLayer3; return value; }
        if (strcmp(szMember, "mLeagueLayer4")==0) { AData value; value = mLeagueLayer4; return value; }
        if (strcmp(szMember, "mLeagueLayer5")==0) { AData value; value = mLeagueLayer5; return value; }
        if (strcmp(szMember, "mLevel")==0) { AData value; value = mLevel; return value; }
        if (strcmp(szMember, "mLevelExp")==0) { AData value; value = mLevelExp; return value; }
        if (strcmp(szMember, "mLoginOut")==0) { AData value; value = mLoginOut; return value; }
        if (strcmp(szMember, "mOffLineTime")==0) { AData value; value = mOffLineTime; return value; }
        if (strcmp(szMember, "mOilDrumCount")==0) { AData value; value = mOilDrumCount; return value; }
        if (strcmp(szMember, "mOilDrumNum")==0) { AData value; value = mOilDrumNum; return value; }
        if (strcmp(szMember, "mOnLine")==0) { AData value; value = mOnLine; return value; }
        if (strcmp(szMember, "mPower")==0) { AData value; value = mPower; return value; }
        if (strcmp(szMember, "mPowerCount")==0) { AData value; value = mPowerCount; return value; }
        if (strcmp(szMember, "mPowerTime")==0) { AData value; value = mPowerTime; return value; }
        if (strcmp(szMember, "mQuicken")==0) { AData value; value = mQuicken; return value; }
        if (strcmp(szMember, "mQuitAllianceTime")==0) { AData value; value = mQuitAllianceTime; return value; }
        if (strcmp(szMember, "mRealRecharge")==0) { AData value; value = mRealRecharge; return value; }
        if (strcmp(szMember, "mRecharge")==0) { AData value; value = mRecharge; return value; }
        if (strcmp(szMember, "mRechargeDiamond")==0) { AData value; value = mRechargeDiamond; return value; }
        if (strcmp(szMember, "mResearchDesktop")==0) { AData value; value = mResearchDesktop; return value; }
        if (strcmp(szMember, "mRevengeCompetitionCount")==0) { AData value; value = mRevengeCompetitionCount; return value; }
        if (strcmp(szMember, "mSayHelloLastTime")==0) { AData value; value = mSayHelloLastTime; return value; }
        if (strcmp(szMember, "mSchoolAlliaExp")==0) { AData value; value = mSchoolAlliaExp; return value; }
        if (strcmp(szMember, "mSchoolUnion")==0) { AData value; value = mSchoolUnion; return value; }
        if (strcmp(szMember, "mSchoolUnionCount")==0) { AData value; value = mSchoolUnionCount; return value; }
        if (strcmp(szMember, "mSchoolUnionId")==0) { AData value; value = mSchoolUnionId; return value; }
        if (strcmp(szMember, "mScience")==0) { AData value; value = mScience; return value; }
        if (strcmp(szMember, "mScienceCount")==0) { AData value; value = mScienceCount; return value; }
        if (strcmp(szMember, "mScore")==0) { AData value; value = mScore; return value; }
        if (strcmp(szMember, "mSecretaryExp")==0) { AData value; value = mSecretaryExp; return value; }
        if (strcmp(szMember, "mSecretaryLevel")==0) { AData value; value = mSecretaryLevel; return value; }
        if (strcmp(szMember, "mSendFriendly")==0) { AData value; value = mSendFriendly; return value; }
        if (strcmp(szMember, "mServerLoginCount")==0) { AData value; value = mServerLoginCount; return value; }
        if (strcmp(szMember, "mSex")==0) { AData value; value = mSex; return value; }
        if (strcmp(szMember, "mShopBuyCount")==0) { AData value; value = mShopBuyCount; return value; }
        if (strcmp(szMember, "mSpringCount")==0) { AData value; value = mSpringCount; return value; }
        if (strcmp(szMember, "mSpringLeiFengCount")==0) { AData value; value = mSpringLeiFengCount; return value; }
        if (strcmp(szMember, "mSpringOutingCount")==0) { AData value; value = mSpringOutingCount; return value; }
        if (strcmp(szMember, "mStrengExp")==0) { AData value; value = mStrengExp; return value; }
        if (strcmp(szMember, "mStudentScore")==0) { AData value; value = mStudentScore; return value; }
        if (strcmp(szMember, "mStudentUpCount")==0) { AData value; value = mStudentUpCount; return value; }
        if (strcmp(szMember, "mSystemFlag")==0) { AData value; value = mSystemFlag; return value; }
        if (strcmp(szMember, "mTeaching")==0) { AData value; value = mTeaching; return value; }
        if (strcmp(szMember, "mTeachingCount")==0) { AData value; value = mTeachingCount; return value; }
        if (strcmp(szMember, "mTeachingExtra")==0) { AData value; value = mTeachingExtra; return value; }
        if (strcmp(szMember, "mTodayOnlineTime")==0) { AData value; value = mTodayOnlineTime; return value; }
        if (strcmp(szMember, "mTotalAllianceContributeCount")==0) { AData value; value = mTotalAllianceContributeCount; return value; }
        if (strcmp(szMember, "mTotalSelfCoursesTaskCount")==0) { AData value; value = mTotalSelfCoursesTaskCount; return value; }
        if (strcmp(szMember, "mTrashCount")==0) { AData value; value = mTrashCount; return value; }
        if (strcmp(szMember, "mTuition")==0) { AData value; value = mTuition; return value; }
        if (strcmp(szMember, "mVipAwardIndex")==0) { AData value; value = mVipAwardIndex; return value; }
        if (strcmp(szMember, "mVipExp")==0) { AData value; value = mVipExp; return value; }
        if (strcmp(szMember, "mVipLv")==0) { AData value; value = mVipLv; return value; }
        if (strcmp(szMember, "mWishReward")==0) { AData value; value = mWishReward; return value; }
        if (strcmp(szMember, "mbReceiveFirstRechargeReward")==0) { AData value; value = mbReceiveFirstRechargeReward; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mAcademicLayer")==0) { mAcademicLayer = value; return true; };
        if (strcmp(szMember, "mAllianceDamage")==0) { mAllianceDamage = value; return true; };
        if (strcmp(szMember, "mAllianceFubenCount")==0) { mAllianceFubenCount = value; return true; };
        if (strcmp(szMember, "mAllianceJob")==0) { mAllianceJob = value; return true; };
        if (strcmp(szMember, "mAllianceSayHelloCount")==0) { mAllianceSayHelloCount = value; return true; };
        if (strcmp(szMember, "mBestStudentQuality")==0) { mBestStudentQuality = value; return true; };
        if (strcmp(szMember, "mBossChallengeCount")==0) { mBossChallengeCount = value; return true; };
        if (strcmp(szMember, "mBossDamage")==0) { mBossDamage = value; return true; };
        if (strcmp(szMember, "mChalkCount")==0) { mChalkCount = value; return true; };
        if (strcmp(szMember, "mChalkFree")==0) { mChalkFree = value; return true; };
        if (strcmp(szMember, "mChalkLimit")==0) { mChalkLimit = value; return true; };
        if (strcmp(szMember, "mClassScore")==0) { mClassScore = value; return true; };
        if (strcmp(szMember, "mCompetitionCount")==0) { mCompetitionCount = value; return true; };
        if (strcmp(szMember, "mCompetitionTime")==0) { mCompetitionTime = value; return true; };
        if (strcmp(szMember, "mContribute")==0) { mContribute = value; return true; };
        if (strcmp(szMember, "mContributeCount")==0) { mContributeCount = value; return true; };
        if (strcmp(szMember, "mContributeLastTime")==0) { mContributeLastTime = value; return true; };
        if (strcmp(szMember, "mContributeTimeCount")==0) { mContributeTimeCount = value; return true; };
        if (strcmp(szMember, "mContributeTuition")==0) { mContributeTuition = value; return true; };
        if (strcmp(szMember, "mCostDiamonds")==0) { mCostDiamonds = value; return true; };
        if (strcmp(szMember, "mCoursesLevel")==0) { mCoursesLevel = value; return true; };
        if (strcmp(szMember, "mCreateTime")==0) { mCreateTime = value; return true; };
        if (strcmp(szMember, "mCultureCount")==0) { mCultureCount = value; return true; };
        if (strcmp(szMember, "mDayActiveFinishInfo")==0) { mDayActiveFinishInfo = value; return true; };
        if (strcmp(szMember, "mDayAddBossChallenge")==0) { mDayAddBossChallenge = value; return true; };
        if (strcmp(szMember, "mDayAddDiamonds")==0) { mDayAddDiamonds = value; return true; };
        if (strcmp(szMember, "mDayAddExperience")==0) { mDayAddExperience = value; return true; };
        if (strcmp(szMember, "mDayAddTuition")==0) { mDayAddTuition = value; return true; };
        if (strcmp(szMember, "mDayAllianceFubenCount")==0) { mDayAllianceFubenCount = value; return true; };
        if (strcmp(szMember, "mDayCompetitionCount")==0) { mDayCompetitionCount = value; return true; };
        if (strcmp(szMember, "mDayCount")==0) { mDayCount = value; return true; };
        if (strcmp(szMember, "mDayDaTiCount")==0) { mDayDaTiCount = value; return true; };
        if (strcmp(szMember, "mDayDiscussCount")==0) { mDayDiscussCount = value; return true; };
        if (strcmp(szMember, "mDayDonateTuition")==0) { mDayDonateTuition = value; return true; };
        if (strcmp(szMember, "mDayFinishRefresherCount")==0) { mDayFinishRefresherCount = value; return true; };
        if (strcmp(szMember, "mDayLeagueCount")==0) { mDayLeagueCount = value; return true; };
        if (strcmp(szMember, "mDayRecharge")==0) { mDayRecharge = value; return true; };
        if (strcmp(szMember, "mDaySpringOut")==0) { mDaySpringOut = value; return true; };
        if (strcmp(szMember, "mDaySubDiamonds")==0) { mDaySubDiamonds = value; return true; };
        if (strcmp(szMember, "mDaySubExperience")==0) { mDaySubExperience = value; return true; };
        if (strcmp(szMember, "mDaySubTuition")==0) { mDaySubTuition = value; return true; };
        if (strcmp(szMember, "mDayTaskActiveValue")==0) { mDayTaskActiveValue = value; return true; };
        if (strcmp(szMember, "mDayTeachingCount")==0) { mDayTeachingCount = value; return true; };
        if (strcmp(szMember, "mDayUnmannedDroneCount")==0) { mDayUnmannedDroneCount = value; return true; };
        if (strcmp(szMember, "mDayWishCount")==0) { mDayWishCount = value; return true; };
        if (strcmp(szMember, "mDeblockingTime")==0) { mDeblockingTime = value; return true; };
        if (strcmp(szMember, "mDiamonds")==0) { mDiamonds = value; return true; };
        if (strcmp(szMember, "mDiscussCount")==0) { mDiscussCount = value; return true; };
        if (strcmp(szMember, "mDiscussTime")==0) { mDiscussTime = value; return true; };
        if (strcmp(szMember, "mDrawCount")==0) { mDrawCount = value; return true; };
        if (strcmp(szMember, "mEngage")==0) { mEngage = value; return true; };
        if (strcmp(szMember, "mEngageCount")==0) { mEngageCount = value; return true; };
        if (strcmp(szMember, "mEquipLevelExp")==0) { mEquipLevelExp = value; return true; };
        if (strcmp(szMember, "mEquipLevelRate")==0) { mEquipLevelRate = value; return true; };
        if (strcmp(szMember, "mEquipLvUp")==0) { mEquipLvUp = value; return true; };
        if (strcmp(szMember, "mExclusiveEquipCount")==0) { mExclusiveEquipCount = value; return true; };
        if (strcmp(szMember, "mExperience")==0) { mExperience = value; return true; };
        if (strcmp(szMember, "mFetterCount")==0) { mFetterCount = value; return true; };
        if (strcmp(szMember, "mForbiddenChatTime")==0) { mForbiddenChatTime = value; return true; };
        if (strcmp(szMember, "mFriendly")==0) { mFriendly = value; return true; };
        if (strcmp(szMember, "mFriendlyCount")==0) { mFriendlyCount = value; return true; };
        if (strcmp(szMember, "mGMLevel")==0) { mGMLevel = value; return true; };
        if (strcmp(szMember, "mHonour")==0) { mHonour = value; return true; };
        if (strcmp(szMember, "mHonourCount")==0) { mHonourCount = value; return true; };
        if (strcmp(szMember, "mIcon")==0) { mIcon = value; return true; };
        if (strcmp(szMember, "mIconFrame")==0) { mIconFrame = value; return true; };
        if (strcmp(szMember, "mIntegralCompetition")==0) { mIntegralCompetition = value; return true; };
        if (strcmp(szMember, "mIntegralDiscuess")==0) { mIntegralDiscuess = value; return true; };
        if (strcmp(szMember, "mJoinAllianceTime")==0) { mJoinAllianceTime = value; return true; };
        if (strcmp(szMember, "mLeagueCount")==0) { mLeagueCount = value; return true; };
        if (strcmp(szMember, "mLeagueLayer1")==0) { mLeagueLayer1 = value; return true; };
        if (strcmp(szMember, "mLeagueLayer2")==0) { mLeagueLayer2 = value; return true; };
        if (strcmp(szMember, "mLeagueLayer3")==0) { mLeagueLayer3 = value; return true; };
        if (strcmp(szMember, "mLeagueLayer4")==0) { mLeagueLayer4 = value; return true; };
        if (strcmp(szMember, "mLeagueLayer5")==0) { mLeagueLayer5 = value; return true; };
        if (strcmp(szMember, "mLevel")==0) { mLevel = value; return true; };
        if (strcmp(szMember, "mLevelExp")==0) { mLevelExp = value; return true; };
        if (strcmp(szMember, "mLoginOut")==0) { mLoginOut = value; return true; };
        if (strcmp(szMember, "mOffLineTime")==0) { mOffLineTime = value; return true; };
        if (strcmp(szMember, "mOilDrumCount")==0) { mOilDrumCount = value; return true; };
        if (strcmp(szMember, "mOilDrumNum")==0) { mOilDrumNum = value; return true; };
        if (strcmp(szMember, "mOnLine")==0) { mOnLine = value; return true; };
        if (strcmp(szMember, "mPower")==0) { mPower = value; return true; };
        if (strcmp(szMember, "mPowerCount")==0) { mPowerCount = value; return true; };
        if (strcmp(szMember, "mPowerTime")==0) { mPowerTime = value; return true; };
        if (strcmp(szMember, "mQuicken")==0) { mQuicken = value; return true; };
        if (strcmp(szMember, "mQuitAllianceTime")==0) { mQuitAllianceTime = value; return true; };
        if (strcmp(szMember, "mRealRecharge")==0) { mRealRecharge = value; return true; };
        if (strcmp(szMember, "mRecharge")==0) { mRecharge = value; return true; };
        if (strcmp(szMember, "mRechargeDiamond")==0) { mRechargeDiamond = value; return true; };
        if (strcmp(szMember, "mResearchDesktop")==0) { mResearchDesktop = value; return true; };
        if (strcmp(szMember, "mRevengeCompetitionCount")==0) { mRevengeCompetitionCount = value; return true; };
        if (strcmp(szMember, "mSayHelloLastTime")==0) { mSayHelloLastTime = value; return true; };
        if (strcmp(szMember, "mSchoolAlliaExp")==0) { mSchoolAlliaExp = value; return true; };
        if (strcmp(szMember, "mSchoolUnion")==0) { mSchoolUnion = value; return true; };
        if (strcmp(szMember, "mSchoolUnionCount")==0) { mSchoolUnionCount = value; return true; };
        if (strcmp(szMember, "mSchoolUnionId")==0) { mSchoolUnionId = value; return true; };
        if (strcmp(szMember, "mScience")==0) { mScience = value; return true; };
        if (strcmp(szMember, "mScienceCount")==0) { mScienceCount = value; return true; };
        if (strcmp(szMember, "mScore")==0) { mScore = value; return true; };
        if (strcmp(szMember, "mSecretaryExp")==0) { mSecretaryExp = value; return true; };
        if (strcmp(szMember, "mSecretaryLevel")==0) { mSecretaryLevel = value; return true; };
        if (strcmp(szMember, "mSendFriendly")==0) { mSendFriendly = value; return true; };
        if (strcmp(szMember, "mServerLoginCount")==0) { mServerLoginCount = value; return true; };
        if (strcmp(szMember, "mSex")==0) { mSex = value; return true; };
        if (strcmp(szMember, "mShopBuyCount")==0) { mShopBuyCount = value; return true; };
        if (strcmp(szMember, "mSpringCount")==0) { mSpringCount = value; return true; };
        if (strcmp(szMember, "mSpringLeiFengCount")==0) { mSpringLeiFengCount = value; return true; };
        if (strcmp(szMember, "mSpringOutingCount")==0) { mSpringOutingCount = value; return true; };
        if (strcmp(szMember, "mStrengExp")==0) { mStrengExp = value; return true; };
        if (strcmp(szMember, "mStudentScore")==0) { mStudentScore = value; return true; };
        if (strcmp(szMember, "mStudentUpCount")==0) { mStudentUpCount = value; return true; };
        if (strcmp(szMember, "mSystemFlag")==0) { mSystemFlag = value; return true; };
        if (strcmp(szMember, "mTeaching")==0) { mTeaching = value; return true; };
        if (strcmp(szMember, "mTeachingCount")==0) { mTeachingCount = value; return true; };
        if (strcmp(szMember, "mTeachingExtra")==0) { mTeachingExtra = value; return true; };
        if (strcmp(szMember, "mTodayOnlineTime")==0) { mTodayOnlineTime = value; return true; };
        if (strcmp(szMember, "mTotalAllianceContributeCount")==0) { mTotalAllianceContributeCount = value; return true; };
        if (strcmp(szMember, "mTotalSelfCoursesTaskCount")==0) { mTotalSelfCoursesTaskCount = value; return true; };
        if (strcmp(szMember, "mTrashCount")==0) { mTrashCount = value; return true; };
        if (strcmp(szMember, "mTuition")==0) { mTuition = value; return true; };
        if (strcmp(szMember, "mVipAwardIndex")==0) { mVipAwardIndex = value; return true; };
        if (strcmp(szMember, "mVipExp")==0) { mVipExp = value; return true; };
        if (strcmp(szMember, "mVipLv")==0) { mVipLv = value; return true; };
        if (strcmp(szMember, "mWishReward")==0) { mWishReward = value; return true; };
        if (strcmp(szMember, "mbReceiveFirstRechargeReward")==0) { mbReceiveFirstRechargeReward = value; return true; };
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

class CultureRecord : public tBaseMsg
{ 
public:
    int mCount;		// 所消耗培养道具的次数
    int mID;		// 培养道具的index

public:
    CultureRecord() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        CheckGet(scrData, mCount);
        CheckGet(scrData, mID);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mCount"] = mCount;
        destData["mID"] = mID;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)2);

        SAVE_MSG_VALUE(mCount, 1);
        SAVE_MSG_VALUE(mID, 1);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mCount = 0;
        mID = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("CultureRecord", otherMsg.GetMsgName())!=0) { LOG("%s is not CultureRecord", otherMsg.GetMsgName()); return; }; const CultureRecord &other = *(const CultureRecord*)(&otherMsg);
        mCount = other.mCount;
        mID = other.mID;
    }

    virtual const char* GetMsgName() const override { return "CultureRecord"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mCount")==0) { AData value; value = mCount; return value; }
        if (strcmp(szMember, "mID")==0) { AData value; value = mID; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mCount")==0) { mCount = value; return true; };
        if (strcmp(szMember, "mID")==0) { mID = value; return true; };
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

class StudentProperty : public tBaseMsg
{ 
public:
    int mCultureCount;		// 培养次数
    ArrayList<CultureRecord> mCultureList;	// 培养记录列表
    ArrayList<EquipProp> mEquipList;	// 装备属性列表
    int mID;		// 对应ID
    int mLv;		// 等级
    int mQualityId;		// 品质id

public:
    StudentProperty() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        CheckGet(scrData, mCultureCount);
        FullStuctArray(scrData, mCultureList, "mCultureList");
        FullStuctArray(scrData, mEquipList, "mEquipList");
        CheckGet(scrData, mID);
        CheckGet(scrData, mLv);
        CheckGet(scrData, mQualityId);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mCultureCount"] = mCultureCount;
        StuctArrayToData(destData, mCultureList, "mCultureList");
        StuctArrayToData(destData, mEquipList, "mEquipList");
        destData["mID"] = mID;
        destData["mLv"] = mLv;
        destData["mQualityId"] = mQualityId;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)6);

        SAVE_MSG_VALUE(mCultureCount, 1);
        SaveMsgArray("mCultureList", mCultureList, destData);
        SaveMsgArray("mEquipList", mEquipList, destData);
        SAVE_MSG_VALUE(mID, 1);
        SAVE_MSG_VALUE(mLv, 1);
        SAVE_MSG_VALUE(mQualityId, 1);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mCultureCount = 0;
        mCultureList.clear(false);
        mEquipList.clear(false);
        mID = 0;
        mLv = 0;
        mQualityId = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("StudentProperty", otherMsg.GetMsgName())!=0) { LOG("%s is not StudentProperty", otherMsg.GetMsgName()); return; }; const StudentProperty &other = *(const StudentProperty*)(&otherMsg);
        mCultureCount = other.mCultureCount;
        CopyMsgArray(other.mCultureList, mCultureList);
        CopyMsgArray(other.mEquipList, mEquipList);
        mID = other.mID;
        mLv = other.mLv;
        mQualityId = other.mQualityId;
    }

    virtual const char* GetMsgName() const override { return "StudentProperty"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mCultureCount")==0) { AData value; value = mCultureCount; return value; }
        if (strcmp(szMember, "mID")==0) { AData value; value = mID; return value; }
        if (strcmp(szMember, "mLv")==0) { AData value; value = mLv; return value; }
        if (strcmp(szMember, "mQualityId")==0) { AData value; value = mQualityId; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mCultureCount")==0) { mCultureCount = value; return true; };
        if (strcmp(szMember, "mID")==0) { mID = value; return true; };
        if (strcmp(szMember, "mLv")==0) { mLv = value; return true; };
        if (strcmp(szMember, "mQualityId")==0) { mQualityId = value; return true; };
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

class TeacherProperty : public tBaseMsg
{ 
public:
    int mExp;		// 当前经验
    int mID;		// 老师id
    int mLevel;		// 老师等级
    int mQualityId;		// 老师品质id

public:
    TeacherProperty() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        CheckGet(scrData, mExp);
        CheckGet(scrData, mID);
        CheckGet(scrData, mLevel);
        CheckGet(scrData, mQualityId);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mExp"] = mExp;
        destData["mID"] = mID;
        destData["mLevel"] = mLevel;
        destData["mQualityId"] = mQualityId;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)4);

        SAVE_MSG_VALUE(mExp, 1);
        SAVE_MSG_VALUE(mID, 1);
        SAVE_MSG_VALUE(mLevel, 1);
        SAVE_MSG_VALUE(mQualityId, 1);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mExp = 0;
        mID = 0;
        mLevel = 0;
        mQualityId = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("TeacherProperty", otherMsg.GetMsgName())!=0) { LOG("%s is not TeacherProperty", otherMsg.GetMsgName()); return; }; const TeacherProperty &other = *(const TeacherProperty*)(&otherMsg);
        mExp = other.mExp;
        mID = other.mID;
        mLevel = other.mLevel;
        mQualityId = other.mQualityId;
    }

    virtual const char* GetMsgName() const override { return "TeacherProperty"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mExp")==0) { AData value; value = mExp; return value; }
        if (strcmp(szMember, "mID")==0) { AData value; value = mID; return value; }
        if (strcmp(szMember, "mLevel")==0) { AData value; value = mLevel; return value; }
        if (strcmp(szMember, "mQualityId")==0) { AData value; value = mQualityId; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mExp")==0) { mExp = value; return true; };
        if (strcmp(szMember, "mID")==0) { mID = value; return true; };
        if (strcmp(szMember, "mLevel")==0) { mLevel = value; return true; };
        if (strcmp(szMember, "mQualityId")==0) { mQualityId = value; return true; };
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

class DeskProperty : public tBaseMsg
{ 
public:
    int mID;		// 对应ID
    int mLv;		// 对应配置等级
    Int64 mTime;		// 解锁时间 (0为已解锁)

public:
    DeskProperty() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        CheckGet(scrData, mID);
        CheckGet(scrData, mLv);
        CheckGet(scrData, mTime);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mID"] = mID;
        destData["mLv"] = mLv;
        destData["mTime"] = mTime;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)3);

        SAVE_MSG_VALUE(mID, 1);
        SAVE_MSG_VALUE(mLv, 1);
        SAVE_MSG_VALUE(mTime, 8);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mID = 0;
        mLv = 0;
        mTime = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("DeskProperty", otherMsg.GetMsgName())!=0) { LOG("%s is not DeskProperty", otherMsg.GetMsgName()); return; }; const DeskProperty &other = *(const DeskProperty*)(&otherMsg);
        mID = other.mID;
        mLv = other.mLv;
        mTime = other.mTime;
    }

    virtual const char* GetMsgName() const override { return "DeskProperty"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mID")==0) { AData value; value = mID; return value; }
        if (strcmp(szMember, "mLv")==0) { AData value; value = mLv; return value; }
        if (strcmp(szMember, "mTime")==0) { AData value; value = mTime; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mID")==0) { mID = value; return true; };
        if (strcmp(szMember, "mLv")==0) { mLv = value; return true; };
        if (strcmp(szMember, "mTime")==0) { mTime = value; return true; };
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

class DecorateProperty : public tBaseMsg
{ 
public:
    int mID;		// 对应ID
    int mLv;		// 对应配置等级
    Int64 mTime;		// 解锁时间 (0为已解锁)

public:
    DecorateProperty() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        CheckGet(scrData, mID);
        CheckGet(scrData, mLv);
        CheckGet(scrData, mTime);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mID"] = mID;
        destData["mLv"] = mLv;
        destData["mTime"] = mTime;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)3);

        SAVE_MSG_VALUE(mID, 1);
        SAVE_MSG_VALUE(mLv, 1);
        SAVE_MSG_VALUE(mTime, 8);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mID = 0;
        mLv = 0;
        mTime = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("DecorateProperty", otherMsg.GetMsgName())!=0) { LOG("%s is not DecorateProperty", otherMsg.GetMsgName()); return; }; const DecorateProperty &other = *(const DecorateProperty*)(&otherMsg);
        mID = other.mID;
        mLv = other.mLv;
        mTime = other.mTime;
    }

    virtual const char* GetMsgName() const override { return "DecorateProperty"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mID")==0) { AData value; value = mID; return value; }
        if (strcmp(szMember, "mLv")==0) { AData value; value = mLv; return value; }
        if (strcmp(szMember, "mTime")==0) { AData value; value = mTime; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mID")==0) { mID = value; return true; };
        if (strcmp(szMember, "mLv")==0) { mLv = value; return true; };
        if (strcmp(szMember, "mTime")==0) { mTime = value; return true; };
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

class WishProp : public tBaseMsg
{ 
public:
    int mIndex;		// 心愿角色
    int mWishRate;		// 心愿角色概率值

public:
    WishProp() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        CheckGet(scrData, mIndex);
        CheckGet(scrData, mWishRate);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mIndex"] = mIndex;
        destData["mWishRate"] = mWishRate;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)2);

        SAVE_MSG_VALUE(mIndex, 1);
        SAVE_MSG_VALUE(mWishRate, 1);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mIndex = 0;
        mWishRate = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("WishProp", otherMsg.GetMsgName())!=0) { LOG("%s is not WishProp", otherMsg.GetMsgName()); return; }; const WishProp &other = *(const WishProp*)(&otherMsg);
        mIndex = other.mIndex;
        mWishRate = other.mWishRate;
    }

    virtual const char* GetMsgName() const override { return "WishProp"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mIndex")==0) { AData value; value = mIndex; return value; }
        if (strcmp(szMember, "mWishRate")==0) { AData value; value = mWishRate; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mIndex")==0) { mIndex = value; return true; };
        if (strcmp(szMember, "mWishRate")==0) { mWishRate = value; return true; };
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

class KeepProp : public tBaseMsg
{ 
public:
    int mCount;		// 抽卡次数
    bool mFlag;		// 是否抽中
    int mID;		// DrawCardKeep的 index

public:
    KeepProp() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        CheckGet(scrData, mCount);
        CheckGet(scrData, mFlag);
        CheckGet(scrData, mID);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mCount"] = mCount;
        destData["mFlag"] = mFlag;
        destData["mID"] = mID;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)3);

        SAVE_MSG_VALUE(mCount, 1);
        SAVE_MSG_VALUE(mFlag, 5);
        SAVE_MSG_VALUE(mID, 1);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mCount = 0;
        mFlag = false;
        mID = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("KeepProp", otherMsg.GetMsgName())!=0) { LOG("%s is not KeepProp", otherMsg.GetMsgName()); return; }; const KeepProp &other = *(const KeepProp*)(&otherMsg);
        mCount = other.mCount;
        mFlag = other.mFlag;
        mID = other.mID;
    }

    virtual const char* GetMsgName() const override { return "KeepProp"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mCount")==0) { AData value; value = mCount; return value; }
        if (strcmp(szMember, "mFlag")==0) { AData value; value = mFlag; return value; }
        if (strcmp(szMember, "mID")==0) { AData value; value = mID; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mCount")==0) { mCount = value; return true; };
        if (strcmp(szMember, "mFlag")==0) { mFlag = value; return true; };
        if (strcmp(szMember, "mID")==0) { mID = value; return true; };
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

class DrawCardProp : public tBaseMsg
{ 
public:
    int mCount;		// 抽奖次数
    int mFreeCount;		// 每天免费抽卡
    int mIndex;		// 抽卡类型
    ArrayList<KeepProp> mKeepList;	// 周期抽奖数据
    int mLimitCount;		// 抽奖次数上限
    ArrayList<int> mList;	// 已抽奖数据
    WishProp mWishList;		// 心愿角色

public:
    DrawCardProp() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        CheckGet(scrData, mCount);
        CheckGet(scrData, mFreeCount);
        CheckGet(scrData, mIndex);
        FullStuctArray(scrData, mKeepList, "mKeepList");
        CheckGet(scrData, mLimitCount);
        FullArray(scrData, mList, "mList");
        {
            AutoNice d = (tNiceData*)scrData["mWishList"];
            if (d) mWishList.Full(d); else LOG("No exist data mWishList");
        }
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mCount"] = mCount;
        destData["mFreeCount"] = mFreeCount;
        destData["mIndex"] = mIndex;
        StuctArrayToData(destData, mKeepList, "mKeepList");
        destData["mLimitCount"] = mLimitCount;
        ArrayToData(destData, mList, "mList");
        {
            AutoNice d = destData->NewNice(); d->msKey = "WishProp"; mWishList.ToData(d);
            destData["mWishList"] = d.getPtr();
        }
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)7);

        SAVE_MSG_VALUE(mCount, 1);
        SAVE_MSG_VALUE(mFreeCount, 1);
        SAVE_MSG_VALUE(mIndex, 1);
        SaveMsgArray("mKeepList", mKeepList, destData);
        SAVE_MSG_VALUE(mLimitCount, 1);
        SaveArray("mList", mList, destData);
        SAVE_MSG_STRUCT(mWishList);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mCount = 0;
        mFreeCount = 0;
        mIndex = 0;
        mKeepList.clear(false);
        mLimitCount = 0;
        mList.clear(false);
        mWishList.clear(false);
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("DrawCardProp", otherMsg.GetMsgName())!=0) { LOG("%s is not DrawCardProp", otherMsg.GetMsgName()); return; }; const DrawCardProp &other = *(const DrawCardProp*)(&otherMsg);
        mCount = other.mCount;
        mFreeCount = other.mFreeCount;
        mIndex = other.mIndex;
        CopyMsgArray(other.mKeepList, mKeepList);
        mLimitCount = other.mLimitCount;
        mList = other.mList;
        mWishList.copy(other.mWishList);
    }

    virtual const char* GetMsgName() const override { return "DrawCardProp"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mCount")==0) { AData value; value = mCount; return value; }
        if (strcmp(szMember, "mFreeCount")==0) { AData value; value = mFreeCount; return value; }
        if (strcmp(szMember, "mIndex")==0) { AData value; value = mIndex; return value; }
        if (strcmp(szMember, "mLimitCount")==0) { AData value; value = mLimitCount; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mCount")==0) { mCount = value; return true; };
        if (strcmp(szMember, "mFreeCount")==0) { mFreeCount = value; return true; };
        if (strcmp(szMember, "mIndex")==0) { mIndex = value; return true; };
        if (strcmp(szMember, "mLimitCount")==0) { mLimitCount = value; return true; };
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

class DrawInfoProp : public tBaseMsg
{ 
public:
    ArrayList<DrawCardProp> mInfo;	// 抽卡数据

public:
    DrawInfoProp() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        FullStuctArray(scrData, mInfo, "mInfo");
    }

    virtual void ToData(AutoNice &destData) override
    {
        StuctArrayToData(destData, mInfo, "mInfo");
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)1);

        SaveMsgArray("mInfo", mInfo, destData);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mInfo.clear(false);
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("DrawInfoProp", otherMsg.GetMsgName())!=0) { LOG("%s is not DrawInfoProp", otherMsg.GetMsgName()); return; }; const DrawInfoProp &other = *(const DrawInfoProp*)(&otherMsg);
        CopyMsgArray(other.mInfo, mInfo);
    }

    virtual const char* GetMsgName() const override { return "DrawInfoProp"; }

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

class IconProp : public tBaseMsg
{ 
public:
    ArrayList<int> mIconList;	// 头像列表

public:
    IconProp() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        FullArray(scrData, mIconList, "mIconList");
    }

    virtual void ToData(AutoNice &destData) override
    {
        ArrayToData(destData, mIconList, "mIconList");
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)1);

        SaveArray("mIconList", mIconList, destData);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mIconList.clear(false);
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("IconProp", otherMsg.GetMsgName())!=0) { LOG("%s is not IconProp", otherMsg.GetMsgName()); return; }; const IconProp &other = *(const IconProp*)(&otherMsg);
        mIconList = other.mIconList;
    }

    virtual const char* GetMsgName() const override { return "IconProp"; }

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

class SpringProp : public tBaseMsg
{ 
public:
    int mCount;		// 抽奖次数
    int mEventCount;		// 写日记
    int mLeifeng;		// 学雷锋数量
    ArrayList<int> mSpringIds;	// 春游的学生们

public:
    SpringProp() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        CheckGet(scrData, mCount);
        CheckGet(scrData, mEventCount);
        CheckGet(scrData, mLeifeng);
        FullArray(scrData, mSpringIds, "mSpringIds");
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mCount"] = mCount;
        destData["mEventCount"] = mEventCount;
        destData["mLeifeng"] = mLeifeng;
        ArrayToData(destData, mSpringIds, "mSpringIds");
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)4);

        SAVE_MSG_VALUE(mCount, 1);
        SAVE_MSG_VALUE(mEventCount, 1);
        SAVE_MSG_VALUE(mLeifeng, 1);
        SaveArray("mSpringIds", mSpringIds, destData);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mCount = 0;
        mEventCount = 0;
        mLeifeng = 0;
        mSpringIds.clear(false);
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("SpringProp", otherMsg.GetMsgName())!=0) { LOG("%s is not SpringProp", otherMsg.GetMsgName()); return; }; const SpringProp &other = *(const SpringProp*)(&otherMsg);
        mCount = other.mCount;
        mEventCount = other.mEventCount;
        mLeifeng = other.mLeifeng;
        mSpringIds = other.mSpringIds;
    }

    virtual const char* GetMsgName() const override { return "SpringProp"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mCount")==0) { AData value; value = mCount; return value; }
        if (strcmp(szMember, "mEventCount")==0) { AData value; value = mEventCount; return value; }
        if (strcmp(szMember, "mLeifeng")==0) { AData value; value = mLeifeng; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mCount")==0) { mCount = value; return true; };
        if (strcmp(szMember, "mEventCount")==0) { mEventCount = value; return true; };
        if (strcmp(szMember, "mLeifeng")==0) { mLeifeng = value; return true; };
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

class ShopItemProp : public tBaseMsg
{ 
public:
    int mCount;		// 商品剩余数量
    int mIndex;		// 商品的索引

public:
    ShopItemProp() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        CheckGet(scrData, mCount);
        CheckGet(scrData, mIndex);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mCount"] = mCount;
        destData["mIndex"] = mIndex;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)2);

        SAVE_MSG_VALUE(mCount, 1);
        SAVE_MSG_VALUE(mIndex, 1);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mCount = 0;
        mIndex = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("ShopItemProp", otherMsg.GetMsgName())!=0) { LOG("%s is not ShopItemProp", otherMsg.GetMsgName()); return; }; const ShopItemProp &other = *(const ShopItemProp*)(&otherMsg);
        mCount = other.mCount;
        mIndex = other.mIndex;
    }

    virtual const char* GetMsgName() const override { return "ShopItemProp"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mCount")==0) { AData value; value = mCount; return value; }
        if (strcmp(szMember, "mIndex")==0) { AData value; value = mIndex; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mCount")==0) { mCount = value; return true; };
        if (strcmp(szMember, "mIndex")==0) { mIndex = value; return true; };
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

class ShopProp : public tBaseMsg
{ 
public:
    int mId;		// shopType
    ArrayList<ShopItemProp> mItems;	// 商品数据

public:
    ShopProp() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        CheckGet(scrData, mId);
        FullStuctArray(scrData, mItems, "mItems");
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mId"] = mId;
        StuctArrayToData(destData, mItems, "mItems");
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)2);

        SAVE_MSG_VALUE(mId, 1);
        SaveMsgArray("mItems", mItems, destData);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mId = 0;
        mItems.clear(false);
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("ShopProp", otherMsg.GetMsgName())!=0) { LOG("%s is not ShopProp", otherMsg.GetMsgName()); return; }; const ShopProp &other = *(const ShopProp*)(&otherMsg);
        mId = other.mId;
        CopyMsgArray(other.mItems, mItems);
    }

    virtual const char* GetMsgName() const override { return "ShopProp"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mId")==0) { AData value; value = mId; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mId")==0) { mId = value; return true; };
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

class FriendApplyProp : public tBaseMsg
{ 
public:
    Int64 mLevel;		// 玩家等级
    AString mName;		// 玩家名字
    Int64 mPlayerID;		// 玩家id
    Int64 mScore;		// 玩家总成绩
    Int64 mTime;		// 申请时间

public:
    FriendApplyProp() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        CheckGet(scrData, mLevel);
        CheckGet(scrData, mName);
        CheckGet(scrData, mPlayerID);
        CheckGet(scrData, mScore);
        CheckGet(scrData, mTime);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mLevel"] = mLevel;
        destData["mName"] = mName;
        destData["mPlayerID"] = mPlayerID;
        destData["mScore"] = mScore;
        destData["mTime"] = mTime;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)5);

        SAVE_MSG_VALUE(mLevel, 8);
        SAVE_MSG_VALUE(mName, 4);
        SAVE_MSG_VALUE(mPlayerID, 8);
        SAVE_MSG_VALUE(mScore, 8);
        SAVE_MSG_VALUE(mTime, 8);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mLevel = 0;
        mName.setNull();
        mPlayerID = 0;
        mScore = 0;
        mTime = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("FriendApplyProp", otherMsg.GetMsgName())!=0) { LOG("%s is not FriendApplyProp", otherMsg.GetMsgName()); return; }; const FriendApplyProp &other = *(const FriendApplyProp*)(&otherMsg);
        mLevel = other.mLevel;
        mName = other.mName;
        mPlayerID = other.mPlayerID;
        mScore = other.mScore;
        mTime = other.mTime;
    }

    virtual const char* GetMsgName() const override { return "FriendApplyProp"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mLevel")==0) { AData value; value = mLevel; return value; }
        if (strcmp(szMember, "mName")==0) { AData value; value = mName; return value; }
        if (strcmp(szMember, "mPlayerID")==0) { AData value; value = mPlayerID; return value; }
        if (strcmp(szMember, "mScore")==0) { AData value; value = mScore; return value; }
        if (strcmp(szMember, "mTime")==0) { AData value; value = mTime; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mLevel")==0) { mLevel = value; return true; };
        if (strcmp(szMember, "mName")==0) { mName = value; return true; };
        if (strcmp(szMember, "mPlayerID")==0) { mPlayerID = value; return true; };
        if (strcmp(szMember, "mScore")==0) { mScore = value; return true; };
        if (strcmp(szMember, "mTime")==0) { mTime = value; return true; };
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

class FriendDataProp : public tBaseMsg
{ 
public:
    Int64 mFriendId;		// 好友id
    int mRecFriendly;		// 接收友情点次数
    int mSendFriendly;		// 赠送友情点次数

public:
    FriendDataProp() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        CheckGet(scrData, mFriendId);
        CheckGet(scrData, mRecFriendly);
        CheckGet(scrData, mSendFriendly);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mFriendId"] = mFriendId;
        destData["mRecFriendly"] = mRecFriendly;
        destData["mSendFriendly"] = mSendFriendly;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)3);

        SAVE_MSG_VALUE(mFriendId, 8);
        SAVE_MSG_VALUE(mRecFriendly, 1);
        SAVE_MSG_VALUE(mSendFriendly, 1);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mFriendId = 0;
        mRecFriendly = 0;
        mSendFriendly = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("FriendDataProp", otherMsg.GetMsgName())!=0) { LOG("%s is not FriendDataProp", otherMsg.GetMsgName()); return; }; const FriendDataProp &other = *(const FriendDataProp*)(&otherMsg);
        mFriendId = other.mFriendId;
        mRecFriendly = other.mRecFriendly;
        mSendFriendly = other.mSendFriendly;
    }

    virtual const char* GetMsgName() const override { return "FriendDataProp"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mFriendId")==0) { AData value; value = mFriendId; return value; }
        if (strcmp(szMember, "mRecFriendly")==0) { AData value; value = mRecFriendly; return value; }
        if (strcmp(szMember, "mSendFriendly")==0) { AData value; value = mSendFriendly; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mFriendId")==0) { mFriendId = value; return true; };
        if (strcmp(szMember, "mRecFriendly")==0) { mRecFriendly = value; return true; };
        if (strcmp(szMember, "mSendFriendly")==0) { mSendFriendly = value; return true; };
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

class FriendProp : public tBaseMsg
{ 
public:
    ArrayList<FriendDataProp> mFriends;	// 好友列表
    int mWeekFriendly;		// 每周收取的友情点

public:
    FriendProp() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        FullStuctArray(scrData, mFriends, "mFriends");
        CheckGet(scrData, mWeekFriendly);
    }

    virtual void ToData(AutoNice &destData) override
    {
        StuctArrayToData(destData, mFriends, "mFriends");
        destData["mWeekFriendly"] = mWeekFriendly;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)2);

        SaveMsgArray("mFriends", mFriends, destData);
        SAVE_MSG_VALUE(mWeekFriendly, 1);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mFriends.clear(false);
        mWeekFriendly = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("FriendProp", otherMsg.GetMsgName())!=0) { LOG("%s is not FriendProp", otherMsg.GetMsgName()); return; }; const FriendProp &other = *(const FriendProp*)(&otherMsg);
        CopyMsgArray(other.mFriends, mFriends);
        mWeekFriendly = other.mWeekFriendly;
    }

    virtual const char* GetMsgName() const override { return "FriendProp"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mWeekFriendly")==0) { AData value; value = mWeekFriendly; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mWeekFriendly")==0) { mWeekFriendly = value; return true; };
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

class HelpDataProp : public tBaseMsg
{ 
public:
    Int64 mFriendId;		// 玩家id
    int mFriendly;		// 抵押的友情点
    int mHelpId;		// 学生或者老师id
    int mLevel;		// 实体等级
    AString mName;		// 玩家名字
    int mQualityId;		// 品质id 配置id
    int mStatus;		// 详见HelpStatus

public:
    HelpDataProp() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        CheckGet(scrData, mFriendId);
        CheckGet(scrData, mFriendly);
        CheckGet(scrData, mHelpId);
        CheckGet(scrData, mLevel);
        CheckGet(scrData, mName);
        CheckGet(scrData, mQualityId);
        CheckGet(scrData, mStatus);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mFriendId"] = mFriendId;
        destData["mFriendly"] = mFriendly;
        destData["mHelpId"] = mHelpId;
        destData["mLevel"] = mLevel;
        destData["mName"] = mName;
        destData["mQualityId"] = mQualityId;
        destData["mStatus"] = mStatus;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)7);

        SAVE_MSG_VALUE(mFriendId, 8);
        SAVE_MSG_VALUE(mFriendly, 1);
        SAVE_MSG_VALUE(mHelpId, 1);
        SAVE_MSG_VALUE(mLevel, 1);
        SAVE_MSG_VALUE(mName, 4);
        SAVE_MSG_VALUE(mQualityId, 1);
        SAVE_MSG_VALUE(mStatus, 1);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mFriendId = 0;
        mFriendly = 0;
        mHelpId = 0;
        mLevel = 0;
        mName.setNull();
        mQualityId = 0;
        mStatus = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("HelpDataProp", otherMsg.GetMsgName())!=0) { LOG("%s is not HelpDataProp", otherMsg.GetMsgName()); return; }; const HelpDataProp &other = *(const HelpDataProp*)(&otherMsg);
        mFriendId = other.mFriendId;
        mFriendly = other.mFriendly;
        mHelpId = other.mHelpId;
        mLevel = other.mLevel;
        mName = other.mName;
        mQualityId = other.mQualityId;
        mStatus = other.mStatus;
    }

    virtual const char* GetMsgName() const override { return "HelpDataProp"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mFriendId")==0) { AData value; value = mFriendId; return value; }
        if (strcmp(szMember, "mFriendly")==0) { AData value; value = mFriendly; return value; }
        if (strcmp(szMember, "mHelpId")==0) { AData value; value = mHelpId; return value; }
        if (strcmp(szMember, "mLevel")==0) { AData value; value = mLevel; return value; }
        if (strcmp(szMember, "mName")==0) { AData value; value = mName; return value; }
        if (strcmp(szMember, "mQualityId")==0) { AData value; value = mQualityId; return value; }
        if (strcmp(szMember, "mStatus")==0) { AData value; value = mStatus; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mFriendId")==0) { mFriendId = value; return true; };
        if (strcmp(szMember, "mFriendly")==0) { mFriendly = value; return true; };
        if (strcmp(szMember, "mHelpId")==0) { mHelpId = value; return true; };
        if (strcmp(szMember, "mLevel")==0) { mLevel = value; return true; };
        if (strcmp(szMember, "mName")==0) { mName = value; return true; };
        if (strcmp(szMember, "mQualityId")==0) { mQualityId = value; return true; };
        if (strcmp(szMember, "mStatus")==0) { mStatus = value; return true; };
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

class HelpProp : public tBaseMsg
{ 
public:
    int mFriendly;		// 每天累计友情点
    ArrayList<HelpDataProp> mHelpStudentList;	// 学生外援数据
    ArrayList<HelpDataProp> mHelpTeacherList;	// 老师外援数据

public:
    HelpProp() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        CheckGet(scrData, mFriendly);
        FullStuctArray(scrData, mHelpStudentList, "mHelpStudentList");
        FullStuctArray(scrData, mHelpTeacherList, "mHelpTeacherList");
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mFriendly"] = mFriendly;
        StuctArrayToData(destData, mHelpStudentList, "mHelpStudentList");
        StuctArrayToData(destData, mHelpTeacherList, "mHelpTeacherList");
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)3);

        SAVE_MSG_VALUE(mFriendly, 1);
        SaveMsgArray("mHelpStudentList", mHelpStudentList, destData);
        SaveMsgArray("mHelpTeacherList", mHelpTeacherList, destData);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mFriendly = 0;
        mHelpStudentList.clear(false);
        mHelpTeacherList.clear(false);
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("HelpProp", otherMsg.GetMsgName())!=0) { LOG("%s is not HelpProp", otherMsg.GetMsgName()); return; }; const HelpProp &other = *(const HelpProp*)(&otherMsg);
        mFriendly = other.mFriendly;
        CopyMsgArray(other.mHelpStudentList, mHelpStudentList);
        CopyMsgArray(other.mHelpTeacherList, mHelpTeacherList);
    }

    virtual const char* GetMsgName() const override { return "HelpProp"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mFriendly")==0) { AData value; value = mFriendly; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mFriendly")==0) { mFriendly = value; return true; };
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

class FetterFriendProp : public tBaseMsg
{ 
public:
    int mEntityID;		// 学生或者老师id
    Int64 mFriendID;		// 羁绊老友ID
    int mQualityID;		// 学生或者老师的品质颜色
    Int64 mTime;		// 羁绊期限时间戳

public:
    FetterFriendProp() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        CheckGet(scrData, mEntityID);
        CheckGet(scrData, mFriendID);
        CheckGet(scrData, mQualityID);
        CheckGet(scrData, mTime);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mEntityID"] = mEntityID;
        destData["mFriendID"] = mFriendID;
        destData["mQualityID"] = mQualityID;
        destData["mTime"] = mTime;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)4);

        SAVE_MSG_VALUE(mEntityID, 1);
        SAVE_MSG_VALUE(mFriendID, 8);
        SAVE_MSG_VALUE(mQualityID, 1);
        SAVE_MSG_VALUE(mTime, 8);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mEntityID = 0;
        mFriendID = 0;
        mQualityID = 0;
        mTime = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("FetterFriendProp", otherMsg.GetMsgName())!=0) { LOG("%s is not FetterFriendProp", otherMsg.GetMsgName()); return; }; const FetterFriendProp &other = *(const FetterFriendProp*)(&otherMsg);
        mEntityID = other.mEntityID;
        mFriendID = other.mFriendID;
        mQualityID = other.mQualityID;
        mTime = other.mTime;
    }

    virtual const char* GetMsgName() const override { return "FetterFriendProp"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mEntityID")==0) { AData value; value = mEntityID; return value; }
        if (strcmp(szMember, "mFriendID")==0) { AData value; value = mFriendID; return value; }
        if (strcmp(szMember, "mQualityID")==0) { AData value; value = mQualityID; return value; }
        if (strcmp(szMember, "mTime")==0) { AData value; value = mTime; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mEntityID")==0) { mEntityID = value; return true; };
        if (strcmp(szMember, "mFriendID")==0) { mFriendID = value; return true; };
        if (strcmp(szMember, "mQualityID")==0) { mQualityID = value; return true; };
        if (strcmp(szMember, "mTime")==0) { mTime = value; return true; };
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

class FetterDataProp : public tBaseMsg
{ 
public:
    FetterFriendProp mFetterFriend;		// 租来的
    int mFetterID;		// 羁绊id
    int mQuality;		// 羁绊品质
    bool mRent;		// 是否使用租来的
    bool mStatus;		// 是否激活

public:
    FetterDataProp() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        {
            AutoNice d = (tNiceData*)scrData["mFetterFriend"];
            if (d) mFetterFriend.Full(d); else LOG("No exist data mFetterFriend");
        }
        CheckGet(scrData, mFetterID);
        CheckGet(scrData, mQuality);
        CheckGet(scrData, mRent);
        CheckGet(scrData, mStatus);
    }

    virtual void ToData(AutoNice &destData) override
    {
        {
            AutoNice d = destData->NewNice(); d->msKey = "FetterFriendProp"; mFetterFriend.ToData(d);
            destData["mFetterFriend"] = d.getPtr();
        }
        destData["mFetterID"] = mFetterID;
        destData["mQuality"] = mQuality;
        destData["mRent"] = mRent;
        destData["mStatus"] = mStatus;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)5);

        SAVE_MSG_STRUCT(mFetterFriend);
        SAVE_MSG_VALUE(mFetterID, 1);
        SAVE_MSG_VALUE(mQuality, 1);
        SAVE_MSG_VALUE(mRent, 5);
        SAVE_MSG_VALUE(mStatus, 5);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mFetterFriend.clear(false);
        mFetterID = 0;
        mQuality = 0;
        mRent = false;
        mStatus = false;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("FetterDataProp", otherMsg.GetMsgName())!=0) { LOG("%s is not FetterDataProp", otherMsg.GetMsgName()); return; }; const FetterDataProp &other = *(const FetterDataProp*)(&otherMsg);
        mFetterFriend.copy(other.mFetterFriend);
        mFetterID = other.mFetterID;
        mQuality = other.mQuality;
        mRent = other.mRent;
        mStatus = other.mStatus;
    }

    virtual const char* GetMsgName() const override { return "FetterDataProp"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mFetterID")==0) { AData value; value = mFetterID; return value; }
        if (strcmp(szMember, "mQuality")==0) { AData value; value = mQuality; return value; }
        if (strcmp(szMember, "mRent")==0) { AData value; value = mRent; return value; }
        if (strcmp(szMember, "mStatus")==0) { AData value; value = mStatus; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mFetterID")==0) { mFetterID = value; return true; };
        if (strcmp(szMember, "mQuality")==0) { mQuality = value; return true; };
        if (strcmp(szMember, "mRent")==0) { mRent = value; return true; };
        if (strcmp(szMember, "mStatus")==0) { mStatus = value; return true; };
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

class FetterProp : public tBaseMsg
{ 
public:
    ArrayList<FetterDataProp> mFetterList;	// 羁绊数据

public:
    FetterProp() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        FullStuctArray(scrData, mFetterList, "mFetterList");
    }

    virtual void ToData(AutoNice &destData) override
    {
        StuctArrayToData(destData, mFetterList, "mFetterList");
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)1);

        SaveMsgArray("mFetterList", mFetterList, destData);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mFetterList.clear(false);
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("FetterProp", otherMsg.GetMsgName())!=0) { LOG("%s is not FetterProp", otherMsg.GetMsgName()); return; }; const FetterProp &other = *(const FetterProp*)(&otherMsg);
        CopyMsgArray(other.mFetterList, mFetterList);
    }

    virtual const char* GetMsgName() const override { return "FetterProp"; }

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

class ProgressInfoProp : public tBaseMsg
{ 
public:
    ArrayList<DBkvRankData> mInfoList;	// 首次上榜时的前三名

public:
    ProgressInfoProp() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        FullStuctArray(scrData, mInfoList, "mInfoList");
    }

    virtual void ToData(AutoNice &destData) override
    {
        StuctArrayToData(destData, mInfoList, "mInfoList");
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)1);

        SaveMsgArray("mInfoList", mInfoList, destData);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mInfoList.clear(false);
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("ProgressInfoProp", otherMsg.GetMsgName())!=0) { LOG("%s is not ProgressInfoProp", otherMsg.GetMsgName()); return; }; const ProgressInfoProp &other = *(const ProgressInfoProp*)(&otherMsg);
        CopyMsgArray(other.mInfoList, mInfoList);
    }

    virtual const char* GetMsgName() const override { return "ProgressInfoProp"; }

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

class RankDataProp : public tBaseMsg
{ 
public:
    ArrayList<bool> mAwardList;	// 已领取的进度奖励数据
    int mCount;		// 膜拜次数

public:
    RankDataProp() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        FullArray(scrData, mAwardList, "mAwardList");
        CheckGet(scrData, mCount);
    }

    virtual void ToData(AutoNice &destData) override
    {
        ArrayToData(destData, mAwardList, "mAwardList");
        destData["mCount"] = mCount;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)2);

        SaveArray("mAwardList", mAwardList, destData);
        SAVE_MSG_VALUE(mCount, 1);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mAwardList.clear(false);
        mCount = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("RankDataProp", otherMsg.GetMsgName())!=0) { LOG("%s is not RankDataProp", otherMsg.GetMsgName()); return; }; const RankDataProp &other = *(const RankDataProp*)(&otherMsg);
        mAwardList = other.mAwardList;
        mCount = other.mCount;
    }

    virtual const char* GetMsgName() const override { return "RankDataProp"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mCount")==0) { AData value; value = mCount; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mCount")==0) { mCount = value; return true; };
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

class ProgressProp : public tBaseMsg
{ 
public:
    ArrayList<ProgressInfoProp> mProgressList;	// 首位上榜的玩家列表

public:
    ProgressProp() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        FullStuctArray(scrData, mProgressList, "mProgressList");
    }

    virtual void ToData(AutoNice &destData) override
    {
        StuctArrayToData(destData, mProgressList, "mProgressList");
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)1);

        SaveMsgArray("mProgressList", mProgressList, destData);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mProgressList.clear(false);
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("ProgressProp", otherMsg.GetMsgName())!=0) { LOG("%s is not ProgressProp", otherMsg.GetMsgName()); return; }; const ProgressProp &other = *(const ProgressProp*)(&otherMsg);
        CopyMsgArray(other.mProgressList, mProgressList);
    }

    virtual const char* GetMsgName() const override { return "ProgressProp"; }

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

class RankProgressProp : public tBaseMsg
{ 
public:
    ArrayList<ProgressProp> mRankProgressList;	// 进度奖励

public:
    RankProgressProp() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        FullStuctArray(scrData, mRankProgressList, "mRankProgressList");
    }

    virtual void ToData(AutoNice &destData) override
    {
        StuctArrayToData(destData, mRankProgressList, "mRankProgressList");
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)1);

        SaveMsgArray("mRankProgressList", mRankProgressList, destData);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mRankProgressList.clear(false);
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("RankProgressProp", otherMsg.GetMsgName())!=0) { LOG("%s is not RankProgressProp", otherMsg.GetMsgName()); return; }; const RankProgressProp &other = *(const RankProgressProp*)(&otherMsg);
        CopyMsgArray(other.mRankProgressList, mRankProgressList);
    }

    virtual const char* GetMsgName() const override { return "RankProgressProp"; }

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

class RankProp : public tBaseMsg
{ 
public:
    ArrayList<RankDataProp> mCountList;	// 玩家排行榜数据(包含膜拜和进度奖励数据)

public:
    RankProp() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        FullStuctArray(scrData, mCountList, "mCountList");
    }

    virtual void ToData(AutoNice &destData) override
    {
        StuctArrayToData(destData, mCountList, "mCountList");
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)1);

        SaveMsgArray("mCountList", mCountList, destData);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mCountList.clear(false);
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("RankProp", otherMsg.GetMsgName())!=0) { LOG("%s is not RankProp", otherMsg.GetMsgName()); return; }; const RankProp &other = *(const RankProp*)(&otherMsg);
        CopyMsgArray(other.mCountList, mCountList);
    }

    virtual const char* GetMsgName() const override { return "RankProp"; }

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

class DiscussProp : public tBaseMsg
{ 
public:
    int mAwardIndex;		// 连胜奖励(下标用二进制)
    int mChallengeCount;		// 已挑战次数
    int mCount;		// 累计次数
    ArrayList<int> mStudentListAttack;	// 进攻阵容
    ArrayList<int> mStudentListDefend;	// 防守阵容
    int mWinCount;		// 连胜次数

public:
    DiscussProp() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        CheckGet(scrData, mAwardIndex);
        CheckGet(scrData, mChallengeCount);
        CheckGet(scrData, mCount);
        FullArray(scrData, mStudentListAttack, "mStudentListAttack");
        FullArray(scrData, mStudentListDefend, "mStudentListDefend");
        CheckGet(scrData, mWinCount);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mAwardIndex"] = mAwardIndex;
        destData["mChallengeCount"] = mChallengeCount;
        destData["mCount"] = mCount;
        ArrayToData(destData, mStudentListAttack, "mStudentListAttack");
        ArrayToData(destData, mStudentListDefend, "mStudentListDefend");
        destData["mWinCount"] = mWinCount;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)6);

        SAVE_MSG_VALUE(mAwardIndex, 1);
        SAVE_MSG_VALUE(mChallengeCount, 1);
        SAVE_MSG_VALUE(mCount, 1);
        SaveArray("mStudentListAttack", mStudentListAttack, destData);
        SaveArray("mStudentListDefend", mStudentListDefend, destData);
        SAVE_MSG_VALUE(mWinCount, 1);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mAwardIndex = 0;
        mChallengeCount = 0;
        mCount = 0;
        mStudentListAttack.clear(false);
        mStudentListDefend.clear(false);
        mWinCount = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("DiscussProp", otherMsg.GetMsgName())!=0) { LOG("%s is not DiscussProp", otherMsg.GetMsgName()); return; }; const DiscussProp &other = *(const DiscussProp*)(&otherMsg);
        mAwardIndex = other.mAwardIndex;
        mChallengeCount = other.mChallengeCount;
        mCount = other.mCount;
        mStudentListAttack = other.mStudentListAttack;
        mStudentListDefend = other.mStudentListDefend;
        mWinCount = other.mWinCount;
    }

    virtual const char* GetMsgName() const override { return "DiscussProp"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mAwardIndex")==0) { AData value; value = mAwardIndex; return value; }
        if (strcmp(szMember, "mChallengeCount")==0) { AData value; value = mChallengeCount; return value; }
        if (strcmp(szMember, "mCount")==0) { AData value; value = mCount; return value; }
        if (strcmp(szMember, "mWinCount")==0) { AData value; value = mWinCount; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mAwardIndex")==0) { mAwardIndex = value; return true; };
        if (strcmp(szMember, "mChallengeCount")==0) { mChallengeCount = value; return true; };
        if (strcmp(szMember, "mCount")==0) { mCount = value; return true; };
        if (strcmp(szMember, "mWinCount")==0) { mWinCount = value; return true; };
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

class AcademicExtraProp : public tBaseMsg
{ 
public:
    Int64 mDelayTime;		// 放置时间
    bool mPass;		// 是否通关boss
    ArrayList<int> mStudentList;	// 放置学生

public:
    AcademicExtraProp() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        CheckGet(scrData, mDelayTime);
        CheckGet(scrData, mPass);
        FullArray(scrData, mStudentList, "mStudentList");
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mDelayTime"] = mDelayTime;
        destData["mPass"] = mPass;
        ArrayToData(destData, mStudentList, "mStudentList");
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)3);

        SAVE_MSG_VALUE(mDelayTime, 8);
        SAVE_MSG_VALUE(mPass, 5);
        SaveArray("mStudentList", mStudentList, destData);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mDelayTime = 0;
        mPass = false;
        mStudentList.clear(false);
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("AcademicExtraProp", otherMsg.GetMsgName())!=0) { LOG("%s is not AcademicExtraProp", otherMsg.GetMsgName()); return; }; const AcademicExtraProp &other = *(const AcademicExtraProp*)(&otherMsg);
        mDelayTime = other.mDelayTime;
        mPass = other.mPass;
        mStudentList = other.mStudentList;
    }

    virtual const char* GetMsgName() const override { return "AcademicExtraProp"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mDelayTime")==0) { AData value; value = mDelayTime; return value; }
        if (strcmp(szMember, "mPass")==0) { AData value; value = mPass; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mDelayTime")==0) { mDelayTime = value; return true; };
        if (strcmp(szMember, "mPass")==0) { mPass = value; return true; };
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

class AcademicPowerProp : public tBaseMsg
{ 
public:
    int mResCount;		// 剩余扫荡次数
    int mResSetCount;		// 重置扫荡次数
    int mStar;		// 通关星数

public:
    AcademicPowerProp() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        CheckGet(scrData, mResCount);
        CheckGet(scrData, mResSetCount);
        CheckGet(scrData, mStar);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mResCount"] = mResCount;
        destData["mResSetCount"] = mResSetCount;
        destData["mStar"] = mStar;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)3);

        SAVE_MSG_VALUE(mResCount, 1);
        SAVE_MSG_VALUE(mResSetCount, 1);
        SAVE_MSG_VALUE(mStar, 1);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mResCount = 0;
        mResSetCount = 0;
        mStar = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("AcademicPowerProp", otherMsg.GetMsgName())!=0) { LOG("%s is not AcademicPowerProp", otherMsg.GetMsgName()); return; }; const AcademicPowerProp &other = *(const AcademicPowerProp*)(&otherMsg);
        mResCount = other.mResCount;
        mResSetCount = other.mResSetCount;
        mStar = other.mStar;
    }

    virtual const char* GetMsgName() const override { return "AcademicPowerProp"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mResCount")==0) { AData value; value = mResCount; return value; }
        if (strcmp(szMember, "mResSetCount")==0) { AData value; value = mResSetCount; return value; }
        if (strcmp(szMember, "mStar")==0) { AData value; value = mStar; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mResCount")==0) { mResCount = value; return true; };
        if (strcmp(szMember, "mResSetCount")==0) { mResSetCount = value; return true; };
        if (strcmp(szMember, "mStar")==0) { mStar = value; return true; };
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

class AcademicProp : public tBaseMsg
{ 
public:
    ArrayList<int> mChapterList;	// 章节奖励数据
    ArrayList<AcademicPowerProp> mEliteLayerList;	// 精英关卡数据
    ArrayList<AcademicExtraProp> mExtraList;	// 放置数据
    ArrayList<AcademicPowerProp> mLayerList;	// 关卡数据
    ArrayList<int> mStudentList;	// 上阵学生

public:
    AcademicProp() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        FullArray(scrData, mChapterList, "mChapterList");
        FullStuctArray(scrData, mEliteLayerList, "mEliteLayerList");
        FullStuctArray(scrData, mExtraList, "mExtraList");
        FullStuctArray(scrData, mLayerList, "mLayerList");
        FullArray(scrData, mStudentList, "mStudentList");
    }

    virtual void ToData(AutoNice &destData) override
    {
        ArrayToData(destData, mChapterList, "mChapterList");
        StuctArrayToData(destData, mEliteLayerList, "mEliteLayerList");
        StuctArrayToData(destData, mExtraList, "mExtraList");
        StuctArrayToData(destData, mLayerList, "mLayerList");
        ArrayToData(destData, mStudentList, "mStudentList");
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)5);

        SaveArray("mChapterList", mChapterList, destData);
        SaveMsgArray("mEliteLayerList", mEliteLayerList, destData);
        SaveMsgArray("mExtraList", mExtraList, destData);
        SaveMsgArray("mLayerList", mLayerList, destData);
        SaveArray("mStudentList", mStudentList, destData);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mChapterList.clear(false);
        mEliteLayerList.clear(false);
        mExtraList.clear(false);
        mLayerList.clear(false);
        mStudentList.clear(false);
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("AcademicProp", otherMsg.GetMsgName())!=0) { LOG("%s is not AcademicProp", otherMsg.GetMsgName()); return; }; const AcademicProp &other = *(const AcademicProp*)(&otherMsg);
        mChapterList = other.mChapterList;
        CopyMsgArray(other.mEliteLayerList, mEliteLayerList);
        CopyMsgArray(other.mExtraList, mExtraList);
        CopyMsgArray(other.mLayerList, mLayerList);
        mStudentList = other.mStudentList;
    }

    virtual const char* GetMsgName() const override { return "AcademicProp"; }

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

class CompetitionDataProp : public tBaseMsg
{ 
public:
    Int64 mIcon;		// 玩家头像
    Int64 mIconFrame;		// 玩家头像框
    Int64 mInterGral;		// 对手积分
    Int64 mLevel;		// 玩家等级
    AString mName;		// 玩家名字
    Int64 mPlayerID;		// 玩家id
    ArrayList<int> mSubjectList;	// 挑战学科类型
    Int64 mTime;		// 记录时间戳
    int mValue;		// 本次下降排名

public:
    CompetitionDataProp() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        CheckGet(scrData, mIcon);
        CheckGet(scrData, mIconFrame);
        CheckGet(scrData, mInterGral);
        CheckGet(scrData, mLevel);
        CheckGet(scrData, mName);
        CheckGet(scrData, mPlayerID);
        FullArray(scrData, mSubjectList, "mSubjectList");
        CheckGet(scrData, mTime);
        CheckGet(scrData, mValue);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mIcon"] = mIcon;
        destData["mIconFrame"] = mIconFrame;
        destData["mInterGral"] = mInterGral;
        destData["mLevel"] = mLevel;
        destData["mName"] = mName;
        destData["mPlayerID"] = mPlayerID;
        ArrayToData(destData, mSubjectList, "mSubjectList");
        destData["mTime"] = mTime;
        destData["mValue"] = mValue;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)9);

        SAVE_MSG_VALUE(mIcon, 8);
        SAVE_MSG_VALUE(mIconFrame, 8);
        SAVE_MSG_VALUE(mInterGral, 8);
        SAVE_MSG_VALUE(mLevel, 8);
        SAVE_MSG_VALUE(mName, 4);
        SAVE_MSG_VALUE(mPlayerID, 8);
        SaveArray("mSubjectList", mSubjectList, destData);
        SAVE_MSG_VALUE(mTime, 8);
        SAVE_MSG_VALUE(mValue, 1);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mIcon = 0;
        mIconFrame = 0;
        mInterGral = 0;
        mLevel = 0;
        mName.setNull();
        mPlayerID = 0;
        mSubjectList.clear(false);
        mTime = 0;
        mValue = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("CompetitionDataProp", otherMsg.GetMsgName())!=0) { LOG("%s is not CompetitionDataProp", otherMsg.GetMsgName()); return; }; const CompetitionDataProp &other = *(const CompetitionDataProp*)(&otherMsg);
        mIcon = other.mIcon;
        mIconFrame = other.mIconFrame;
        mInterGral = other.mInterGral;
        mLevel = other.mLevel;
        mName = other.mName;
        mPlayerID = other.mPlayerID;
        mSubjectList = other.mSubjectList;
        mTime = other.mTime;
        mValue = other.mValue;
    }

    virtual const char* GetMsgName() const override { return "CompetitionDataProp"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mIcon")==0) { AData value; value = mIcon; return value; }
        if (strcmp(szMember, "mIconFrame")==0) { AData value; value = mIconFrame; return value; }
        if (strcmp(szMember, "mInterGral")==0) { AData value; value = mInterGral; return value; }
        if (strcmp(szMember, "mLevel")==0) { AData value; value = mLevel; return value; }
        if (strcmp(szMember, "mName")==0) { AData value; value = mName; return value; }
        if (strcmp(szMember, "mPlayerID")==0) { AData value; value = mPlayerID; return value; }
        if (strcmp(szMember, "mTime")==0) { AData value; value = mTime; return value; }
        if (strcmp(szMember, "mValue")==0) { AData value; value = mValue; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mIcon")==0) { mIcon = value; return true; };
        if (strcmp(szMember, "mIconFrame")==0) { mIconFrame = value; return true; };
        if (strcmp(szMember, "mInterGral")==0) { mInterGral = value; return true; };
        if (strcmp(szMember, "mLevel")==0) { mLevel = value; return true; };
        if (strcmp(szMember, "mName")==0) { mName = value; return true; };
        if (strcmp(szMember, "mPlayerID")==0) { mPlayerID = value; return true; };
        if (strcmp(szMember, "mTime")==0) { mTime = value; return true; };
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

class CompetitionProp : public tBaseMsg
{ 
public:
    int mAwardIndex;		// 累胜奖励(下标用二进制)
    int mChallengeCount;		// 已挑战次数
    ArrayList<int> mChallengeList;	// 已挑战的学生
    int mCount;		// 累计次数
    ArrayList<DBkvData> mRecordChallengeList;	// 记录刷新学生的挑战次数
    ArrayList<CompetitionDataProp> mRecordList;	// 复赛记录
    int mRevengeCount;		// 已复赛次数
    ArrayList<int> mStudentListAttack;	// 进攻阵容
    ArrayList<int> mStudentListDefend;	// 防守阵容
    int mWinCount;		// 累胜次数

public:
    CompetitionProp() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        CheckGet(scrData, mAwardIndex);
        CheckGet(scrData, mChallengeCount);
        FullArray(scrData, mChallengeList, "mChallengeList");
        CheckGet(scrData, mCount);
        FullStuctArray(scrData, mRecordChallengeList, "mRecordChallengeList");
        FullStuctArray(scrData, mRecordList, "mRecordList");
        CheckGet(scrData, mRevengeCount);
        FullArray(scrData, mStudentListAttack, "mStudentListAttack");
        FullArray(scrData, mStudentListDefend, "mStudentListDefend");
        CheckGet(scrData, mWinCount);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mAwardIndex"] = mAwardIndex;
        destData["mChallengeCount"] = mChallengeCount;
        ArrayToData(destData, mChallengeList, "mChallengeList");
        destData["mCount"] = mCount;
        StuctArrayToData(destData, mRecordChallengeList, "mRecordChallengeList");
        StuctArrayToData(destData, mRecordList, "mRecordList");
        destData["mRevengeCount"] = mRevengeCount;
        ArrayToData(destData, mStudentListAttack, "mStudentListAttack");
        ArrayToData(destData, mStudentListDefend, "mStudentListDefend");
        destData["mWinCount"] = mWinCount;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)10);

        SAVE_MSG_VALUE(mAwardIndex, 1);
        SAVE_MSG_VALUE(mChallengeCount, 1);
        SaveArray("mChallengeList", mChallengeList, destData);
        SAVE_MSG_VALUE(mCount, 1);
        SaveMsgArray("mRecordChallengeList", mRecordChallengeList, destData);
        SaveMsgArray("mRecordList", mRecordList, destData);
        SAVE_MSG_VALUE(mRevengeCount, 1);
        SaveArray("mStudentListAttack", mStudentListAttack, destData);
        SaveArray("mStudentListDefend", mStudentListDefend, destData);
        SAVE_MSG_VALUE(mWinCount, 1);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mAwardIndex = 0;
        mChallengeCount = 0;
        mChallengeList.clear(false);
        mCount = 0;
        mRecordChallengeList.clear(false);
        mRecordList.clear(false);
        mRevengeCount = 0;
        mStudentListAttack.clear(false);
        mStudentListDefend.clear(false);
        mWinCount = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("CompetitionProp", otherMsg.GetMsgName())!=0) { LOG("%s is not CompetitionProp", otherMsg.GetMsgName()); return; }; const CompetitionProp &other = *(const CompetitionProp*)(&otherMsg);
        mAwardIndex = other.mAwardIndex;
        mChallengeCount = other.mChallengeCount;
        mChallengeList = other.mChallengeList;
        mCount = other.mCount;
        CopyMsgArray(other.mRecordChallengeList, mRecordChallengeList);
        CopyMsgArray(other.mRecordList, mRecordList);
        mRevengeCount = other.mRevengeCount;
        mStudentListAttack = other.mStudentListAttack;
        mStudentListDefend = other.mStudentListDefend;
        mWinCount = other.mWinCount;
    }

    virtual const char* GetMsgName() const override { return "CompetitionProp"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mAwardIndex")==0) { AData value; value = mAwardIndex; return value; }
        if (strcmp(szMember, "mChallengeCount")==0) { AData value; value = mChallengeCount; return value; }
        if (strcmp(szMember, "mCount")==0) { AData value; value = mCount; return value; }
        if (strcmp(szMember, "mRevengeCount")==0) { AData value; value = mRevengeCount; return value; }
        if (strcmp(szMember, "mWinCount")==0) { AData value; value = mWinCount; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mAwardIndex")==0) { mAwardIndex = value; return true; };
        if (strcmp(szMember, "mChallengeCount")==0) { mChallengeCount = value; return true; };
        if (strcmp(szMember, "mCount")==0) { mCount = value; return true; };
        if (strcmp(szMember, "mRevengeCount")==0) { mRevengeCount = value; return true; };
        if (strcmp(szMember, "mWinCount")==0) { mWinCount = value; return true; };
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

class LeagueDataProp : public tBaseMsg
{ 
public:
    ArrayList<int> mDieList;	// 已阵亡的学生
    ArrayList<EntityProp> mEntityList;	// 上阵的学生们
    int mHelpCount;		// 外援次数
    int mLayer;		// 第几层
    int mType;		// 学科类型

public:
    LeagueDataProp() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        FullArray(scrData, mDieList, "mDieList");
        FullStuctArray(scrData, mEntityList, "mEntityList");
        CheckGet(scrData, mHelpCount);
        CheckGet(scrData, mLayer);
        CheckGet(scrData, mType);
    }

    virtual void ToData(AutoNice &destData) override
    {
        ArrayToData(destData, mDieList, "mDieList");
        StuctArrayToData(destData, mEntityList, "mEntityList");
        destData["mHelpCount"] = mHelpCount;
        destData["mLayer"] = mLayer;
        destData["mType"] = mType;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)5);

        SaveArray("mDieList", mDieList, destData);
        SaveMsgArray("mEntityList", mEntityList, destData);
        SAVE_MSG_VALUE(mHelpCount, 1);
        SAVE_MSG_VALUE(mLayer, 1);
        SAVE_MSG_VALUE(mType, 1);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mDieList.clear(false);
        mEntityList.clear(false);
        mHelpCount = 0;
        mLayer = 0;
        mType = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("LeagueDataProp", otherMsg.GetMsgName())!=0) { LOG("%s is not LeagueDataProp", otherMsg.GetMsgName()); return; }; const LeagueDataProp &other = *(const LeagueDataProp*)(&otherMsg);
        mDieList = other.mDieList;
        CopyMsgArray(other.mEntityList, mEntityList);
        mHelpCount = other.mHelpCount;
        mLayer = other.mLayer;
        mType = other.mType;
    }

    virtual const char* GetMsgName() const override { return "LeagueDataProp"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mHelpCount")==0) { AData value; value = mHelpCount; return value; }
        if (strcmp(szMember, "mLayer")==0) { AData value; value = mLayer; return value; }
        if (strcmp(szMember, "mType")==0) { AData value; value = mType; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mHelpCount")==0) { mHelpCount = value; return true; };
        if (strcmp(szMember, "mLayer")==0) { mLayer = value; return true; };
        if (strcmp(szMember, "mType")==0) { mType = value; return true; };
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

class LeagueProp : public tBaseMsg
{ 
public:
    ArrayList<LeagueDataProp> mList;	// 学科数据

public:
    LeagueProp() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        FullStuctArray(scrData, mList, "mList");
    }

    virtual void ToData(AutoNice &destData) override
    {
        StuctArrayToData(destData, mList, "mList");
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)1);

        SaveMsgArray("mList", mList, destData);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mList.clear(false);
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("LeagueProp", otherMsg.GetMsgName())!=0) { LOG("%s is not LeagueProp", otherMsg.GetMsgName()); return; }; const LeagueProp &other = *(const LeagueProp*)(&otherMsg);
        CopyMsgArray(other.mList, mList);
    }

    virtual const char* GetMsgName() const override { return "LeagueProp"; }

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

class BossProp : public tBaseMsg
{ 
public:
    ArrayList<bool> mAwardIndex;	// 普通累计伤害奖励
    EntityProp mBoss;		// boss属性
    int mBossIndex;		// 第几阶段的boss
    int mBuyCount;		// 棒棒糖购买次数
    ArrayList<int> mDieList;	// 已阵亡的学生
    Int64 mFlushItem;		// 刷道具时间
    Int64 mLifeTime;		// 存活时间戳
    ArrayList<bool> mPulloverAward;	// 奖学金累计伤害奖励
    bool mPulloverFlag;		// 是否购买了学习基金
    Int64 mSportsTime;		// 赛季结束时间
    ArrayList<int> mStudentListAttack;	// 进攻阵容
    Int64 mTime;		// 复活时间戳

public:
    BossProp() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        FullArray(scrData, mAwardIndex, "mAwardIndex");
        {
            AutoNice d = (tNiceData*)scrData["mBoss"];
            if (d) mBoss.Full(d); else LOG("No exist data mBoss");
        }
        CheckGet(scrData, mBossIndex);
        CheckGet(scrData, mBuyCount);
        FullArray(scrData, mDieList, "mDieList");
        CheckGet(scrData, mFlushItem);
        CheckGet(scrData, mLifeTime);
        FullArray(scrData, mPulloverAward, "mPulloverAward");
        CheckGet(scrData, mPulloverFlag);
        CheckGet(scrData, mSportsTime);
        FullArray(scrData, mStudentListAttack, "mStudentListAttack");
        CheckGet(scrData, mTime);
    }

    virtual void ToData(AutoNice &destData) override
    {
        ArrayToData(destData, mAwardIndex, "mAwardIndex");
        {
            AutoNice d = destData->NewNice(); d->msKey = "EntityProp"; mBoss.ToData(d);
            destData["mBoss"] = d.getPtr();
        }
        destData["mBossIndex"] = mBossIndex;
        destData["mBuyCount"] = mBuyCount;
        ArrayToData(destData, mDieList, "mDieList");
        destData["mFlushItem"] = mFlushItem;
        destData["mLifeTime"] = mLifeTime;
        ArrayToData(destData, mPulloverAward, "mPulloverAward");
        destData["mPulloverFlag"] = mPulloverFlag;
        destData["mSportsTime"] = mSportsTime;
        ArrayToData(destData, mStudentListAttack, "mStudentListAttack");
        destData["mTime"] = mTime;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)12);

        SaveArray("mAwardIndex", mAwardIndex, destData);
        SAVE_MSG_STRUCT(mBoss);
        SAVE_MSG_VALUE(mBossIndex, 1);
        SAVE_MSG_VALUE(mBuyCount, 1);
        SaveArray("mDieList", mDieList, destData);
        SAVE_MSG_VALUE(mFlushItem, 8);
        SAVE_MSG_VALUE(mLifeTime, 8);
        SaveArray("mPulloverAward", mPulloverAward, destData);
        SAVE_MSG_VALUE(mPulloverFlag, 5);
        SAVE_MSG_VALUE(mSportsTime, 8);
        SaveArray("mStudentListAttack", mStudentListAttack, destData);
        SAVE_MSG_VALUE(mTime, 8);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mAwardIndex.clear(false);
        mBoss.clear(false);
        mBossIndex = 0;
        mBuyCount = 0;
        mDieList.clear(false);
        mFlushItem = 0;
        mLifeTime = 0;
        mPulloverAward.clear(false);
        mPulloverFlag = false;
        mSportsTime = 0;
        mStudentListAttack.clear(false);
        mTime = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("BossProp", otherMsg.GetMsgName())!=0) { LOG("%s is not BossProp", otherMsg.GetMsgName()); return; }; const BossProp &other = *(const BossProp*)(&otherMsg);
        mAwardIndex = other.mAwardIndex;
        mBoss.copy(other.mBoss);
        mBossIndex = other.mBossIndex;
        mBuyCount = other.mBuyCount;
        mDieList = other.mDieList;
        mFlushItem = other.mFlushItem;
        mLifeTime = other.mLifeTime;
        mPulloverAward = other.mPulloverAward;
        mPulloverFlag = other.mPulloverFlag;
        mSportsTime = other.mSportsTime;
        mStudentListAttack = other.mStudentListAttack;
        mTime = other.mTime;
    }

    virtual const char* GetMsgName() const override { return "BossProp"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mBossIndex")==0) { AData value; value = mBossIndex; return value; }
        if (strcmp(szMember, "mBuyCount")==0) { AData value; value = mBuyCount; return value; }
        if (strcmp(szMember, "mFlushItem")==0) { AData value; value = mFlushItem; return value; }
        if (strcmp(szMember, "mLifeTime")==0) { AData value; value = mLifeTime; return value; }
        if (strcmp(szMember, "mPulloverFlag")==0) { AData value; value = mPulloverFlag; return value; }
        if (strcmp(szMember, "mSportsTime")==0) { AData value; value = mSportsTime; return value; }
        if (strcmp(szMember, "mTime")==0) { AData value; value = mTime; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mBossIndex")==0) { mBossIndex = value; return true; };
        if (strcmp(szMember, "mBuyCount")==0) { mBuyCount = value; return true; };
        if (strcmp(szMember, "mFlushItem")==0) { mFlushItem = value; return true; };
        if (strcmp(szMember, "mLifeTime")==0) { mLifeTime = value; return true; };
        if (strcmp(szMember, "mPulloverFlag")==0) { mPulloverFlag = value; return true; };
        if (strcmp(szMember, "mSportsTime")==0) { mSportsTime = value; return true; };
        if (strcmp(szMember, "mTime")==0) { mTime = value; return true; };
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

class AllianceFubenLayerProp : public tBaseMsg
{ 
public:
    bool mDie;		// 是否死亡
    int mIndex;		// AllianceFubenLayerConfig的id
    ArrayList<EntityProp> mMonsters;	// 野怪剩余属性
    ArrayList<Int64> mPlayerList;	// 通关后记录哪些可领取的玩家

public:
    AllianceFubenLayerProp() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        CheckGet(scrData, mDie);
        CheckGet(scrData, mIndex);
        FullStuctArray(scrData, mMonsters, "mMonsters");
        FullArray(scrData, mPlayerList, "mPlayerList");
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mDie"] = mDie;
        destData["mIndex"] = mIndex;
        StuctArrayToData(destData, mMonsters, "mMonsters");
        ArrayToData(destData, mPlayerList, "mPlayerList");
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)4);

        SAVE_MSG_VALUE(mDie, 5);
        SAVE_MSG_VALUE(mIndex, 1);
        SaveMsgArray("mMonsters", mMonsters, destData);
        SaveArray("mPlayerList", mPlayerList, destData);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mDie = false;
        mIndex = 0;
        mMonsters.clear(false);
        mPlayerList.clear(false);
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("AllianceFubenLayerProp", otherMsg.GetMsgName())!=0) { LOG("%s is not AllianceFubenLayerProp", otherMsg.GetMsgName()); return; }; const AllianceFubenLayerProp &other = *(const AllianceFubenLayerProp*)(&otherMsg);
        mDie = other.mDie;
        mIndex = other.mIndex;
        CopyMsgArray(other.mMonsters, mMonsters);
        mPlayerList = other.mPlayerList;
    }

    virtual const char* GetMsgName() const override { return "AllianceFubenLayerProp"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mDie")==0) { AData value; value = mDie; return value; }
        if (strcmp(szMember, "mIndex")==0) { AData value; value = mIndex; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mDie")==0) { mDie = value; return true; };
        if (strcmp(szMember, "mIndex")==0) { mIndex = value; return true; };
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

class AllianceFubenChapterProp : public tBaseMsg
{ 
public:
    int mIndex;		// 第几章
    ArrayList<AllianceFubenLayerProp> mLayerList;	// 每章数据信息
    bool mPass;		// 是否通关

public:
    AllianceFubenChapterProp() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        CheckGet(scrData, mIndex);
        FullStuctArray(scrData, mLayerList, "mLayerList");
        CheckGet(scrData, mPass);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mIndex"] = mIndex;
        StuctArrayToData(destData, mLayerList, "mLayerList");
        destData["mPass"] = mPass;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)3);

        SAVE_MSG_VALUE(mIndex, 1);
        SaveMsgArray("mLayerList", mLayerList, destData);
        SAVE_MSG_VALUE(mPass, 5);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mIndex = 0;
        mLayerList.clear(false);
        mPass = false;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("AllianceFubenChapterProp", otherMsg.GetMsgName())!=0) { LOG("%s is not AllianceFubenChapterProp", otherMsg.GetMsgName()); return; }; const AllianceFubenChapterProp &other = *(const AllianceFubenChapterProp*)(&otherMsg);
        mIndex = other.mIndex;
        CopyMsgArray(other.mLayerList, mLayerList);
        mPass = other.mPass;
    }

    virtual const char* GetMsgName() const override { return "AllianceFubenChapterProp"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mIndex")==0) { AData value; value = mIndex; return value; }
        if (strcmp(szMember, "mPass")==0) { AData value; value = mPass; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mIndex")==0) { mIndex = value; return true; };
        if (strcmp(szMember, "mPass")==0) { mPass = value; return true; };
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

class AllianceDataProp : public tBaseMsg
{ 
public:
    int mBuyNum;		// 已购买辣条次数
    ArrayList<bool> mChapterIndex;	// 每章奖励二进制
    int mFreeNum;		// 已使用的免费次数
    ArrayList<int> mStudentList;	// 玩家上阵列表

public:
    AllianceDataProp() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        CheckGet(scrData, mBuyNum);
        FullArray(scrData, mChapterIndex, "mChapterIndex");
        CheckGet(scrData, mFreeNum);
        FullArray(scrData, mStudentList, "mStudentList");
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mBuyNum"] = mBuyNum;
        ArrayToData(destData, mChapterIndex, "mChapterIndex");
        destData["mFreeNum"] = mFreeNum;
        ArrayToData(destData, mStudentList, "mStudentList");
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)4);

        SAVE_MSG_VALUE(mBuyNum, 1);
        SaveArray("mChapterIndex", mChapterIndex, destData);
        SAVE_MSG_VALUE(mFreeNum, 1);
        SaveArray("mStudentList", mStudentList, destData);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mBuyNum = 0;
        mChapterIndex.clear(false);
        mFreeNum = 0;
        mStudentList.clear(false);
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("AllianceDataProp", otherMsg.GetMsgName())!=0) { LOG("%s is not AllianceDataProp", otherMsg.GetMsgName()); return; }; const AllianceDataProp &other = *(const AllianceDataProp*)(&otherMsg);
        mBuyNum = other.mBuyNum;
        mChapterIndex = other.mChapterIndex;
        mFreeNum = other.mFreeNum;
        mStudentList = other.mStudentList;
    }

    virtual const char* GetMsgName() const override { return "AllianceDataProp"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mBuyNum")==0) { AData value; value = mBuyNum; return value; }
        if (strcmp(szMember, "mFreeNum")==0) { AData value; value = mFreeNum; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mBuyNum")==0) { mBuyNum = value; return true; };
        if (strcmp(szMember, "mFreeNum")==0) { mFreeNum = value; return true; };
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

class AllianceFubenProp : public tBaseMsg
{ 
public:
    int mBestChapter;		// 最高章节
    ArrayList<AllianceFubenChapterProp> mChapterList;	// 章节数据

public:
    AllianceFubenProp() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        CheckGet(scrData, mBestChapter);
        FullStuctArray(scrData, mChapterList, "mChapterList");
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mBestChapter"] = mBestChapter;
        StuctArrayToData(destData, mChapterList, "mChapterList");
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)2);

        SAVE_MSG_VALUE(mBestChapter, 1);
        SaveMsgArray("mChapterList", mChapterList, destData);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mBestChapter = 0;
        mChapterList.clear(false);
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("AllianceFubenProp", otherMsg.GetMsgName())!=0) { LOG("%s is not AllianceFubenProp", otherMsg.GetMsgName()); return; }; const AllianceFubenProp &other = *(const AllianceFubenProp*)(&otherMsg);
        mBestChapter = other.mBestChapter;
        CopyMsgArray(other.mChapterList, mChapterList);
    }

    virtual const char* GetMsgName() const override { return "AllianceFubenProp"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mBestChapter")==0) { AData value; value = mBestChapter; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mBestChapter")==0) { mBestChapter = value; return true; };
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

class MonthCardDataProp : public tBaseMsg
{ 
public:
    bool mAwardIdx;		// 每日奖励是否已领取
    int mIndex;		// 月卡配置id
    Int64 mTime;		// 到期时间

public:
    MonthCardDataProp() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        CheckGet(scrData, mAwardIdx);
        CheckGet(scrData, mIndex);
        CheckGet(scrData, mTime);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mAwardIdx"] = mAwardIdx;
        destData["mIndex"] = mIndex;
        destData["mTime"] = mTime;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)3);

        SAVE_MSG_VALUE(mAwardIdx, 5);
        SAVE_MSG_VALUE(mIndex, 1);
        SAVE_MSG_VALUE(mTime, 8);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mAwardIdx = false;
        mIndex = 0;
        mTime = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("MonthCardDataProp", otherMsg.GetMsgName())!=0) { LOG("%s is not MonthCardDataProp", otherMsg.GetMsgName()); return; }; const MonthCardDataProp &other = *(const MonthCardDataProp*)(&otherMsg);
        mAwardIdx = other.mAwardIdx;
        mIndex = other.mIndex;
        mTime = other.mTime;
    }

    virtual const char* GetMsgName() const override { return "MonthCardDataProp"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mAwardIdx")==0) { AData value; value = mAwardIdx; return value; }
        if (strcmp(szMember, "mIndex")==0) { AData value; value = mIndex; return value; }
        if (strcmp(szMember, "mTime")==0) { AData value; value = mTime; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mAwardIdx")==0) { mAwardIdx = value; return true; };
        if (strcmp(szMember, "mIndex")==0) { mIndex = value; return true; };
        if (strcmp(szMember, "mTime")==0) { mTime = value; return true; };
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

class MonthCardProp : public tBaseMsg
{ 
public:
    ArrayList<MonthCardDataProp> mCardList;	// 月卡信息

public:
    MonthCardProp() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        FullStuctArray(scrData, mCardList, "mCardList");
    }

    virtual void ToData(AutoNice &destData) override
    {
        StuctArrayToData(destData, mCardList, "mCardList");
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)1);

        SaveMsgArray("mCardList", mCardList, destData);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mCardList.clear(false);
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("MonthCardProp", otherMsg.GetMsgName())!=0) { LOG("%s is not MonthCardProp", otherMsg.GetMsgName()); return; }; const MonthCardProp &other = *(const MonthCardProp*)(&otherMsg);
        CopyMsgArray(other.mCardList, mCardList);
    }

    virtual const char* GetMsgName() const override { return "MonthCardProp"; }

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

class AllianceSkillDataProp : public tBaseMsg
{ 
public:
    int mIndex;		// 校盟技能配置
    int mLevel;		// 校盟技能等级
    int mSkillId;		// 校盟技能id

public:
    AllianceSkillDataProp() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        CheckGet(scrData, mIndex);
        CheckGet(scrData, mLevel);
        CheckGet(scrData, mSkillId);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mIndex"] = mIndex;
        destData["mLevel"] = mLevel;
        destData["mSkillId"] = mSkillId;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)3);

        SAVE_MSG_VALUE(mIndex, 1);
        SAVE_MSG_VALUE(mLevel, 1);
        SAVE_MSG_VALUE(mSkillId, 1);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mIndex = 0;
        mLevel = 0;
        mSkillId = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("AllianceSkillDataProp", otherMsg.GetMsgName())!=0) { LOG("%s is not AllianceSkillDataProp", otherMsg.GetMsgName()); return; }; const AllianceSkillDataProp &other = *(const AllianceSkillDataProp*)(&otherMsg);
        mIndex = other.mIndex;
        mLevel = other.mLevel;
        mSkillId = other.mSkillId;
    }

    virtual const char* GetMsgName() const override { return "AllianceSkillDataProp"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mIndex")==0) { AData value; value = mIndex; return value; }
        if (strcmp(szMember, "mLevel")==0) { AData value; value = mLevel; return value; }
        if (strcmp(szMember, "mSkillId")==0) { AData value; value = mSkillId; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mIndex")==0) { mIndex = value; return true; };
        if (strcmp(szMember, "mLevel")==0) { mLevel = value; return true; };
        if (strcmp(szMember, "mSkillId")==0) { mSkillId = value; return true; };
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

class AllianceSkillProp : public tBaseMsg
{ 
public:
    ArrayList<AllianceSkillDataProp> mSkillList;	// 技能列表

public:
    AllianceSkillProp() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        FullStuctArray(scrData, mSkillList, "mSkillList");
    }

    virtual void ToData(AutoNice &destData) override
    {
        StuctArrayToData(destData, mSkillList, "mSkillList");
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)1);

        SaveMsgArray("mSkillList", mSkillList, destData);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mSkillList.clear(false);
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("AllianceSkillProp", otherMsg.GetMsgName())!=0) { LOG("%s is not AllianceSkillProp", otherMsg.GetMsgName()); return; }; const AllianceSkillProp &other = *(const AllianceSkillProp*)(&otherMsg);
        CopyMsgArray(other.mSkillList, mSkillList);
    }

    virtual const char* GetMsgName() const override { return "AllianceSkillProp"; }

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

class FundProp : public tBaseMsg
{ 
public:
    ArrayList<bool> mAwardList;	// 已领取奖励列表
    bool mFlag;		// 是否购买奖学金

public:
    FundProp() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        FullArray(scrData, mAwardList, "mAwardList");
        CheckGet(scrData, mFlag);
    }

    virtual void ToData(AutoNice &destData) override
    {
        ArrayToData(destData, mAwardList, "mAwardList");
        destData["mFlag"] = mFlag;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)2);

        SaveArray("mAwardList", mAwardList, destData);
        SAVE_MSG_VALUE(mFlag, 5);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mAwardList.clear(false);
        mFlag = false;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("FundProp", otherMsg.GetMsgName())!=0) { LOG("%s is not FundProp", otherMsg.GetMsgName()); return; }; const FundProp &other = *(const FundProp*)(&otherMsg);
        mAwardList = other.mAwardList;
        mFlag = other.mFlag;
    }

    virtual const char* GetMsgName() const override { return "FundProp"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mFlag")==0) { AData value; value = mFlag; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mFlag")==0) { mFlag = value; return true; };
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

class GiftDataProp : public tBaseMsg
{ 
public:
    int mCount;		// 已购买数量
    int mIndex;		// 礼包的id

public:
    GiftDataProp() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        CheckGet(scrData, mCount);
        CheckGet(scrData, mIndex);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mCount"] = mCount;
        destData["mIndex"] = mIndex;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)2);

        SAVE_MSG_VALUE(mCount, 1);
        SAVE_MSG_VALUE(mIndex, 1);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mCount = 0;
        mIndex = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("GiftDataProp", otherMsg.GetMsgName())!=0) { LOG("%s is not GiftDataProp", otherMsg.GetMsgName()); return; }; const GiftDataProp &other = *(const GiftDataProp*)(&otherMsg);
        mCount = other.mCount;
        mIndex = other.mIndex;
    }

    virtual const char* GetMsgName() const override { return "GiftDataProp"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mCount")==0) { AData value; value = mCount; return value; }
        if (strcmp(szMember, "mIndex")==0) { AData value; value = mIndex; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mCount")==0) { mCount = value; return true; };
        if (strcmp(szMember, "mIndex")==0) { mIndex = value; return true; };
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

class GiftProp : public tBaseMsg
{ 
public:
    ArrayList<GiftDataProp> mGiftDayInfo;	// 每日礼包数据
    ArrayList<GiftDataProp> mGiftInfo;	// 礼包数据
    ArrayList<GiftDataProp> mGiftMonthInfo;	// 每月礼包数据
    ArrayList<GiftDataProp> mGiftWeekInfo;	// 每周礼包数据

public:
    GiftProp() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        FullStuctArray(scrData, mGiftDayInfo, "mGiftDayInfo");
        FullStuctArray(scrData, mGiftInfo, "mGiftInfo");
        FullStuctArray(scrData, mGiftMonthInfo, "mGiftMonthInfo");
        FullStuctArray(scrData, mGiftWeekInfo, "mGiftWeekInfo");
    }

    virtual void ToData(AutoNice &destData) override
    {
        StuctArrayToData(destData, mGiftDayInfo, "mGiftDayInfo");
        StuctArrayToData(destData, mGiftInfo, "mGiftInfo");
        StuctArrayToData(destData, mGiftMonthInfo, "mGiftMonthInfo");
        StuctArrayToData(destData, mGiftWeekInfo, "mGiftWeekInfo");
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)4);

        SaveMsgArray("mGiftDayInfo", mGiftDayInfo, destData);
        SaveMsgArray("mGiftInfo", mGiftInfo, destData);
        SaveMsgArray("mGiftMonthInfo", mGiftMonthInfo, destData);
        SaveMsgArray("mGiftWeekInfo", mGiftWeekInfo, destData);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mGiftDayInfo.clear(false);
        mGiftInfo.clear(false);
        mGiftMonthInfo.clear(false);
        mGiftWeekInfo.clear(false);
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("GiftProp", otherMsg.GetMsgName())!=0) { LOG("%s is not GiftProp", otherMsg.GetMsgName()); return; }; const GiftProp &other = *(const GiftProp*)(&otherMsg);
        CopyMsgArray(other.mGiftDayInfo, mGiftDayInfo);
        CopyMsgArray(other.mGiftInfo, mGiftInfo);
        CopyMsgArray(other.mGiftMonthInfo, mGiftMonthInfo);
        CopyMsgArray(other.mGiftWeekInfo, mGiftWeekInfo);
    }

    virtual const char* GetMsgName() const override { return "GiftProp"; }

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

class SevenLoginProp : public tBaseMsg
{ 
public:
    ArrayList<bool> mAwardList;	// 已领取奖励列表
    int mDay;		// 已登录第几天
    bool mFlag;		// 是否已领取

public:
    SevenLoginProp() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        FullArray(scrData, mAwardList, "mAwardList");
        CheckGet(scrData, mDay);
        CheckGet(scrData, mFlag);
    }

    virtual void ToData(AutoNice &destData) override
    {
        ArrayToData(destData, mAwardList, "mAwardList");
        destData["mDay"] = mDay;
        destData["mFlag"] = mFlag;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)3);

        SaveArray("mAwardList", mAwardList, destData);
        SAVE_MSG_VALUE(mDay, 1);
        SAVE_MSG_VALUE(mFlag, 5);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mAwardList.clear(false);
        mDay = 0;
        mFlag = false;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("SevenLoginProp", otherMsg.GetMsgName())!=0) { LOG("%s is not SevenLoginProp", otherMsg.GetMsgName()); return; }; const SevenLoginProp &other = *(const SevenLoginProp*)(&otherMsg);
        mAwardList = other.mAwardList;
        mDay = other.mDay;
        mFlag = other.mFlag;
    }

    virtual const char* GetMsgName() const override { return "SevenLoginProp"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mDay")==0) { AData value; value = mDay; return value; }
        if (strcmp(szMember, "mFlag")==0) { AData value; value = mFlag; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mDay")==0) { mDay = value; return true; };
        if (strcmp(szMember, "mFlag")==0) { mFlag = value; return true; };
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

class TicketData : public tBaseMsg
{ 
public:
    int mBuyCount;		// 购买钻石次数
    int mCount;		// 购买的次数
    int mFreeCount;		// 免费的次数

public:
    TicketData() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        CheckGet(scrData, mBuyCount);
        CheckGet(scrData, mCount);
        CheckGet(scrData, mFreeCount);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mBuyCount"] = mBuyCount;
        destData["mCount"] = mCount;
        destData["mFreeCount"] = mFreeCount;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)3);

        SAVE_MSG_VALUE(mBuyCount, 1);
        SAVE_MSG_VALUE(mCount, 1);
        SAVE_MSG_VALUE(mFreeCount, 1);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mBuyCount = 0;
        mCount = 0;
        mFreeCount = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("TicketData", otherMsg.GetMsgName())!=0) { LOG("%s is not TicketData", otherMsg.GetMsgName()); return; }; const TicketData &other = *(const TicketData*)(&otherMsg);
        mBuyCount = other.mBuyCount;
        mCount = other.mCount;
        mFreeCount = other.mFreeCount;
    }

    virtual const char* GetMsgName() const override { return "TicketData"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mBuyCount")==0) { AData value; value = mBuyCount; return value; }
        if (strcmp(szMember, "mCount")==0) { AData value; value = mCount; return value; }
        if (strcmp(szMember, "mFreeCount")==0) { AData value; value = mFreeCount; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mBuyCount")==0) { mBuyCount = value; return true; };
        if (strcmp(szMember, "mCount")==0) { mCount = value; return true; };
        if (strcmp(szMember, "mFreeCount")==0) { mFreeCount = value; return true; };
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

class TicketProp : public tBaseMsg
{ 
public:
    ArrayList<DBkvData> mExchangeList;	// 购买道具数据
    ArrayList<TicketData> mTickets;	// 门票数据

public:
    TicketProp() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        FullStuctArray(scrData, mExchangeList, "mExchangeList");
        FullStuctArray(scrData, mTickets, "mTickets");
    }

    virtual void ToData(AutoNice &destData) override
    {
        StuctArrayToData(destData, mExchangeList, "mExchangeList");
        StuctArrayToData(destData, mTickets, "mTickets");
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)2);

        SaveMsgArray("mExchangeList", mExchangeList, destData);
        SaveMsgArray("mTickets", mTickets, destData);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mExchangeList.clear(false);
        mTickets.clear(false);
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("TicketProp", otherMsg.GetMsgName())!=0) { LOG("%s is not TicketProp", otherMsg.GetMsgName()); return; }; const TicketProp &other = *(const TicketProp*)(&otherMsg);
        CopyMsgArray(other.mExchangeList, mExchangeList);
        CopyMsgArray(other.mTickets, mTickets);
    }

    virtual const char* GetMsgName() const override { return "TicketProp"; }

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

class CodeData : public tBaseMsg
{ 
public:
    int mLevel;		// 玩家等级
    AString mName;		// 玩家名称
    Int64 mPlayerID;		// 玩家id
    int mServerID;		// 服务器id
    Int64 mTime;		// 领取时间
    AString mUser;		// 39用户id

public:
    CodeData() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        CheckGet(scrData, mLevel);
        CheckGet(scrData, mName);
        CheckGet(scrData, mPlayerID);
        CheckGet(scrData, mServerID);
        CheckGet(scrData, mTime);
        CheckGet(scrData, mUser);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mLevel"] = mLevel;
        destData["mName"] = mName;
        destData["mPlayerID"] = mPlayerID;
        destData["mServerID"] = mServerID;
        destData["mTime"] = mTime;
        destData["mUser"] = mUser;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)6);

        SAVE_MSG_VALUE(mLevel, 1);
        SAVE_MSG_VALUE(mName, 4);
        SAVE_MSG_VALUE(mPlayerID, 8);
        SAVE_MSG_VALUE(mServerID, 1);
        SAVE_MSG_VALUE(mTime, 8);
        SAVE_MSG_VALUE(mUser, 4);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mLevel = 0;
        mName.setNull();
        mPlayerID = 0;
        mServerID = 0;
        mTime = 0;
        mUser.setNull();
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("CodeData", otherMsg.GetMsgName())!=0) { LOG("%s is not CodeData", otherMsg.GetMsgName()); return; }; const CodeData &other = *(const CodeData*)(&otherMsg);
        mLevel = other.mLevel;
        mName = other.mName;
        mPlayerID = other.mPlayerID;
        mServerID = other.mServerID;
        mTime = other.mTime;
        mUser = other.mUser;
    }

    virtual const char* GetMsgName() const override { return "CodeData"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mLevel")==0) { AData value; value = mLevel; return value; }
        if (strcmp(szMember, "mName")==0) { AData value; value = mName; return value; }
        if (strcmp(szMember, "mPlayerID")==0) { AData value; value = mPlayerID; return value; }
        if (strcmp(szMember, "mServerID")==0) { AData value; value = mServerID; return value; }
        if (strcmp(szMember, "mTime")==0) { AData value; value = mTime; return value; }
        if (strcmp(szMember, "mUser")==0) { AData value; value = mUser; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mLevel")==0) { mLevel = value; return true; };
        if (strcmp(szMember, "mName")==0) { mName = value; return true; };
        if (strcmp(szMember, "mPlayerID")==0) { mPlayerID = value; return true; };
        if (strcmp(szMember, "mServerID")==0) { mServerID = value; return true; };
        if (strcmp(szMember, "mTime")==0) { mTime = value; return true; };
        if (strcmp(szMember, "mUser")==0) { mUser = value; return true; };
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

class CodeProp : public tBaseMsg
{ 
public:
    int mCount;		// 已用数量
    bool mNewday;		// 更新标志
    ArrayList<CodeData> mPlayerList;	// 记录已领取玩家
    Int64 mTime;		// 更新记录时间

public:
    CodeProp() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        CheckGet(scrData, mCount);
        CheckGet(scrData, mNewday);
        FullStuctArray(scrData, mPlayerList, "mPlayerList");
        CheckGet(scrData, mTime);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mCount"] = mCount;
        destData["mNewday"] = mNewday;
        StuctArrayToData(destData, mPlayerList, "mPlayerList");
        destData["mTime"] = mTime;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)4);

        SAVE_MSG_VALUE(mCount, 1);
        SAVE_MSG_VALUE(mNewday, 5);
        SaveMsgArray("mPlayerList", mPlayerList, destData);
        SAVE_MSG_VALUE(mTime, 8);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mCount = 0;
        mNewday = false;
        mPlayerList.clear(false);
        mTime = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("CodeProp", otherMsg.GetMsgName())!=0) { LOG("%s is not CodeProp", otherMsg.GetMsgName()); return; }; const CodeProp &other = *(const CodeProp*)(&otherMsg);
        mCount = other.mCount;
        mNewday = other.mNewday;
        CopyMsgArray(other.mPlayerList, mPlayerList);
        mTime = other.mTime;
    }

    virtual const char* GetMsgName() const override { return "CodeProp"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mCount")==0) { AData value; value = mCount; return value; }
        if (strcmp(szMember, "mNewday")==0) { AData value; value = mNewday; return value; }
        if (strcmp(szMember, "mTime")==0) { AData value; value = mTime; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mCount")==0) { mCount = value; return true; };
        if (strcmp(szMember, "mNewday")==0) { mNewday = value; return true; };
        if (strcmp(szMember, "mTime")==0) { mTime = value; return true; };
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

class AutonymProp : public tBaseMsg
{ 
public:
    bool mFlag;		// 是否领取奖励
    int mIndex;		// Autonym的配置id

public:
    AutonymProp() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        CheckGet(scrData, mFlag);
        CheckGet(scrData, mIndex);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mFlag"] = mFlag;
        destData["mIndex"] = mIndex;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)2);

        SAVE_MSG_VALUE(mFlag, 5);
        SAVE_MSG_VALUE(mIndex, 1);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mFlag = false;
        mIndex = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("AutonymProp", otherMsg.GetMsgName())!=0) { LOG("%s is not AutonymProp", otherMsg.GetMsgName()); return; }; const AutonymProp &other = *(const AutonymProp*)(&otherMsg);
        mFlag = other.mFlag;
        mIndex = other.mIndex;
    }

    virtual const char* GetMsgName() const override { return "AutonymProp"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mFlag")==0) { AData value; value = mFlag; return value; }
        if (strcmp(szMember, "mIndex")==0) { AData value; value = mIndex; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mFlag")==0) { mFlag = value; return true; };
        if (strcmp(szMember, "mIndex")==0) { mIndex = value; return true; };
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

class UpdateItem : public tBaseMsg
{ 
public:
    ItemProperty mItem;		// 道具数量
    Int64 mType;		//  0 添加 1 更新 2删除
    int mUpdateNum;		// 变化的数量

public:
    UpdateItem() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        {
            AutoNice d = (tNiceData*)scrData["mItem"];
            if (d) mItem.Full(d); else LOG("No exist data mItem");
        }
        CheckGet(scrData, mType);
        CheckGet(scrData, mUpdateNum);
    }

    virtual void ToData(AutoNice &destData) override
    {
        {
            AutoNice d = destData->NewNice(); d->msKey = "ItemProperty"; mItem.ToData(d);
            destData["mItem"] = d.getPtr();
        }
        destData["mType"] = mType;
        destData["mUpdateNum"] = mUpdateNum;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)3);

        SAVE_MSG_STRUCT(mItem);
        SAVE_MSG_VALUE(mType, 8);
        SAVE_MSG_VALUE(mUpdateNum, 1);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mItem.clear(false);
        mType = 0;
        mUpdateNum = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("UpdateItem", otherMsg.GetMsgName())!=0) { LOG("%s is not UpdateItem", otherMsg.GetMsgName()); return; }; const UpdateItem &other = *(const UpdateItem*)(&otherMsg);
        mItem.copy(other.mItem);
        mType = other.mType;
        mUpdateNum = other.mUpdateNum;
    }

    virtual const char* GetMsgName() const override { return "UpdateItem"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mType")==0) { AData value; value = mType; return value; }
        if (strcmp(szMember, "mUpdateNum")==0) { AData value; value = mUpdateNum; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mType")==0) { mType = value; return true; };
        if (strcmp(szMember, "mUpdateNum")==0) { mUpdateNum = value; return true; };
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

class DBActivityData : public tBaseMsg
{ 
public:
    int mActId;		// 活动id
    int mActType;		// 活动状态
    Int64 mETime;		// 结束时间
    int mParam1;		// 辅助参数1
    int mParam2;		// 辅助参数2
    int mParam3;		// 辅助参数3
    Int64 mSTime;		// 开始时间

public:
    DBActivityData() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        CheckGet(scrData, mActId);
        CheckGet(scrData, mActType);
        CheckGet(scrData, mETime);
        CheckGet(scrData, mParam1);
        CheckGet(scrData, mParam2);
        CheckGet(scrData, mParam3);
        CheckGet(scrData, mSTime);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mActId"] = mActId;
        destData["mActType"] = mActType;
        destData["mETime"] = mETime;
        destData["mParam1"] = mParam1;
        destData["mParam2"] = mParam2;
        destData["mParam3"] = mParam3;
        destData["mSTime"] = mSTime;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)7);

        SAVE_MSG_VALUE(mActId, 1);
        SAVE_MSG_VALUE(mActType, 1);
        SAVE_MSG_VALUE(mETime, 8);
        SAVE_MSG_VALUE(mParam1, 1);
        SAVE_MSG_VALUE(mParam2, 1);
        SAVE_MSG_VALUE(mParam3, 1);
        SAVE_MSG_VALUE(mSTime, 8);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mActId = 0;
        mActType = 0;
        mETime = 0;
        mParam1 = 0;
        mParam2 = 0;
        mParam3 = 0;
        mSTime = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("DBActivityData", otherMsg.GetMsgName())!=0) { LOG("%s is not DBActivityData", otherMsg.GetMsgName()); return; }; const DBActivityData &other = *(const DBActivityData*)(&otherMsg);
        mActId = other.mActId;
        mActType = other.mActType;
        mETime = other.mETime;
        mParam1 = other.mParam1;
        mParam2 = other.mParam2;
        mParam3 = other.mParam3;
        mSTime = other.mSTime;
    }

    virtual const char* GetMsgName() const override { return "DBActivityData"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mActId")==0) { AData value; value = mActId; return value; }
        if (strcmp(szMember, "mActType")==0) { AData value; value = mActType; return value; }
        if (strcmp(szMember, "mETime")==0) { AData value; value = mETime; return value; }
        if (strcmp(szMember, "mParam1")==0) { AData value; value = mParam1; return value; }
        if (strcmp(szMember, "mParam2")==0) { AData value; value = mParam2; return value; }
        if (strcmp(szMember, "mParam3")==0) { AData value; value = mParam3; return value; }
        if (strcmp(szMember, "mSTime")==0) { AData value; value = mSTime; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mActId")==0) { mActId = value; return true; };
        if (strcmp(szMember, "mActType")==0) { mActType = value; return true; };
        if (strcmp(szMember, "mETime")==0) { mETime = value; return true; };
        if (strcmp(szMember, "mParam1")==0) { mParam1 = value; return true; };
        if (strcmp(szMember, "mParam2")==0) { mParam2 = value; return true; };
        if (strcmp(szMember, "mParam3")==0) { mParam3 = value; return true; };
        if (strcmp(szMember, "mSTime")==0) { mSTime = value; return true; };
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

class ActProp : public tBaseMsg
{ 
public:
    ArrayList<DBActivityData> mActList;	// 活动数据

public:
    ActProp() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        FullStuctArray(scrData, mActList, "mActList");
    }

    virtual void ToData(AutoNice &destData) override
    {
        StuctArrayToData(destData, mActList, "mActList");
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)1);

        SaveMsgArray("mActList", mActList, destData);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mActList.clear(false);
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("ActProp", otherMsg.GetMsgName())!=0) { LOG("%s is not ActProp", otherMsg.GetMsgName()); return; }; const ActProp &other = *(const ActProp*)(&otherMsg);
        CopyMsgArray(other.mActList, mActList);
    }

    virtual const char* GetMsgName() const override { return "ActProp"; }

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

class SWKeepProp : public tBaseMsg
{ 
public:
    int mCount;		// 抽卡次数
    bool mFlag;		// 是否抽中
    int mID;		// DrawCardKeep的 index

public:
    SWKeepProp() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        CheckGet(scrData, mCount);
        CheckGet(scrData, mFlag);
        CheckGet(scrData, mID);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mCount"] = mCount;
        destData["mFlag"] = mFlag;
        destData["mID"] = mID;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)3);

        SAVE_MSG_VALUE(mCount, 1);
        SAVE_MSG_VALUE(mFlag, 5);
        SAVE_MSG_VALUE(mID, 1);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mCount = 0;
        mFlag = false;
        mID = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("SWKeepProp", otherMsg.GetMsgName())!=0) { LOG("%s is not SWKeepProp", otherMsg.GetMsgName()); return; }; const SWKeepProp &other = *(const SWKeepProp*)(&otherMsg);
        mCount = other.mCount;
        mFlag = other.mFlag;
        mID = other.mID;
    }

    virtual const char* GetMsgName() const override { return "SWKeepProp"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mCount")==0) { AData value; value = mCount; return value; }
        if (strcmp(szMember, "mFlag")==0) { AData value; value = mFlag; return value; }
        if (strcmp(szMember, "mID")==0) { AData value; value = mID; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mCount")==0) { mCount = value; return true; };
        if (strcmp(szMember, "mFlag")==0) { mFlag = value; return true; };
        if (strcmp(szMember, "mID")==0) { mID = value; return true; };
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

class SWProp : public tBaseMsg
{ 
public:
    int mCount;		// 抽奖次数
    int mCycleCount;		// 周期内切换学科的次数
    int mIndex;		// 当前轮到第几个
    ArrayList<SWKeepProp> mKeepList;	// 周期抽奖数据
    int mLimitCount;		// 抽奖次数上限
    ArrayList<int> mList;	// 已抽奖数据
    int mSelectIndex;		// 用钻石切换第几个
    Int64 mUpdateTime;		// 更新时间

public:
    SWProp() { clear(false); };


    void Full(AutoNice scrData)
    {
        clear(false);
        CheckGet(scrData, mCount);
        CheckGet(scrData, mCycleCount);
        CheckGet(scrData, mIndex);
        FullStuctArray(scrData, mKeepList, "mKeepList");
        CheckGet(scrData, mLimitCount);
        FullArray(scrData, mList, "mList");
        CheckGet(scrData, mSelectIndex);
        CheckGet(scrData, mUpdateTime);
    }

    virtual void ToData(AutoNice &destData) override
    {
        destData["mCount"] = mCount;
        destData["mCycleCount"] = mCycleCount;
        destData["mIndex"] = mIndex;
        StuctArrayToData(destData, mKeepList, "mKeepList");
        destData["mLimitCount"] = mLimitCount;
        ArrayToData(destData, mList, "mList");
        destData["mSelectIndex"] = mSelectIndex;
        destData["mUpdateTime"] = mUpdateTime;
    }

    bool serialize(DataStream *destData) const override
    {
        destData->write((short)8);

        SAVE_MSG_VALUE(mCount, 1);
        SAVE_MSG_VALUE(mCycleCount, 1);
        SAVE_MSG_VALUE(mIndex, 1);
        SaveMsgArray("mKeepList", mKeepList, destData);
        SAVE_MSG_VALUE(mLimitCount, 1);
        SaveArray("mList", mList, destData);
        SAVE_MSG_VALUE(mSelectIndex, 1);
        SAVE_MSG_VALUE(mUpdateTime, 8);
        return true;
    }

    void clear(bool bClearBuffer=false) override 
    {
        mCount = 0;
        mCycleCount = 0;
        mIndex = 0;
        mKeepList.clear(false);
        mLimitCount = 0;
        mList.clear(false);
        mSelectIndex = 0;
        mUpdateTime = 0;
    }

    void copy(const tBaseMsg &otherMsg) override 
    {
        if (strcmp("SWProp", otherMsg.GetMsgName())!=0) { LOG("%s is not SWProp", otherMsg.GetMsgName()); return; }; const SWProp &other = *(const SWProp*)(&otherMsg);
        mCount = other.mCount;
        mCycleCount = other.mCycleCount;
        mIndex = other.mIndex;
        CopyMsgArray(other.mKeepList, mKeepList);
        mLimitCount = other.mLimitCount;
        mList = other.mList;
        mSelectIndex = other.mSelectIndex;
        mUpdateTime = other.mUpdateTime;
    }

    virtual const char* GetMsgName() const override { return "SWProp"; }

    AData get(const char *szMember) const 
    {
        if (strcmp(szMember, "mCount")==0) { AData value; value = mCount; return value; }
        if (strcmp(szMember, "mCycleCount")==0) { AData value; value = mCycleCount; return value; }
        if (strcmp(szMember, "mIndex")==0) { AData value; value = mIndex; return value; }
        if (strcmp(szMember, "mLimitCount")==0) { AData value; value = mLimitCount; return value; }
        if (strcmp(szMember, "mSelectIndex")==0) { AData value; value = mSelectIndex; return value; }
        if (strcmp(szMember, "mUpdateTime")==0) { AData value; value = mUpdateTime; return value; }
        return AData();
    }

    bool set(const char *szMember, AData value) 
    {
        if (strcmp(szMember, "mCount")==0) { mCount = value; return true; };
        if (strcmp(szMember, "mCycleCount")==0) { mCycleCount = value; return true; };
        if (strcmp(szMember, "mIndex")==0) { mIndex = value; return true; };
        if (strcmp(szMember, "mLimitCount")==0) { mLimitCount = value; return true; };
        if (strcmp(szMember, "mSelectIndex")==0) { mSelectIndex = value; return true; };
        if (strcmp(szMember, "mUpdateTime")==0) { mUpdateTime = value; return true; };
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

