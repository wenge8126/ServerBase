#pragma once

#include "Actor.h"
#include "DBUser_t_goods.h"
#include "DBUser_t_buy_record.h"
#include "LogicMsg.h"
//-------------------------------------------------------------------------
//  商品数据记录
//-------------------------------------------------------------------------
class DBGoodsComponent : public DBUser_t_goods
{
public:
	virtual AutoData GetRecordData(const char *szKey) override
	{
		AutoData d = DBUser_t_goods::GetRecordData(szKey);
		if (mDataRecord)
		{
			NOTE_LOG("Good >>> \r\n%s", mDataRecord->dump().c_str());
		}
		return d;
	}

	virtual bool SaveRecordByNiceData(const char *szKey, AutoNice d, bool bGrowthKey, AString &resultKey) override
	{
		NOTE_LOG("Good save data === \r\n%s", d->dump().c_str());
		return DBUser_t_goods::SaveRecordByNiceData(szKey, d, bGrowthKey, resultKey);
	}
};

//-------------------------------------------------------------------------
// 购买记录
class DBGoodsBuyComponent : public DBUser_t_buy_record
{
public:
	void On(CG_RequestAddCart &msg, CG_ReponseAddCart &response, UnitID, int)
	{		
		// 先检查GoodsID
		Hand<DBUser_t_goods> comp = GetActor()->GetDBUserComponent(TABLE_GOODS);
		ARecord goodsRecord = comp->LoadRecord(msg.mGoodsID);
		if (!comp->IsValid())
		{
			response.mError = eError_GoodsNoExist;
			return;
		}

		if (GrowthNewRecord())
		{
			SetGOODS_ID(msg.mGoodsID);
			SetPRICE(comp->nPRICE());
			SetAMOUNT(comp->nPRICE()*msg.mCount);
			SetCOUNT(msg.mCount);
		}
		else
			response.mError = eError_ProgramError;
	}

public:
	virtual void RegisterMsg() override
	{
		REG_COMP_MSG(DBGoodsBuyComponent, CG_RequestAddCart, CG_ReponseAddCart);
	}
};

//-------------------------------------------------------------------------
// 商品功能逻辑 (商品管理, 购买等)
//-------------------------------------------------------------------------
class GoodsLogicActor : public Actor
{
	AComponent		mGoodsComponent;

public:
	virtual void Init() override
	{
		mGoodsComponent = AddComponent("DBGoodsComponent");
		AddComponent("SaveRecordComponent");
		AddComponent("LoadRecordComponent");
	}

	virtual AComponent GetDBUserComponent(const AString &tableIndex) override
	{
		if (tableIndex == "t_goods")
			return mGoodsComponent;

		ERROR_LOG("No exist table %s component", tableIndex.c_str());
		return AComponent();
	}

	virtual ARecord LoadRecord(const char *szTableName, const char *szKey) override
	{
		Hand<DBUserComponent> comp = GetDBUserComponent(szTableName);
		if (comp && comp->GetTable())
			return comp->GetTable()->GetRecord(szKey);

		return ARecord();
	}

	virtual ARecord LoadRecord(const char *szTableName, Int64 nKey) override
	{
		Hand<DBUserComponent> comp = GetDBUserComponent(szTableName);
		if (comp && comp->GetTable())
			return comp->GetTable()->GetRecord(nKey);

		return ARecord();
	}

public:
	virtual void RegisterMsg() override
	{
		// 需要把这个单独放到一个进程里, 独立于DB
		REG_COMPONENT(DBGoodsComponent);
		REG_COMPONENT(SaveRecordComponent);
		REG_COMPONENT(LoadRecordComponent);
	}
};
//-------------------------------------------------------------------------

