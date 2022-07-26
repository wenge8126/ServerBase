#pragma once

#include "ActorManager.h"
#include "HttpComponect.h"

#include "DefineMsgFactory.h"

#include "SCActor.h"
#include "MsgData.h"
#include "DBUser_t_account.h"
#include "ServerMsg.h"
#include "t_account.h"

using namespace NetCloud;

//-------------------------------------------------------------------------
// �ͻ��������ظ�����Ϣ
// ������LogicActor �յ���, ʹ���첽�ȴ� ClientActor �ٴ��첽������Ŀ��Actor
// ClientActor ���յ��ظ���, LogicAtor �����µĻظ���Ϣ, �ظ����ͻ���
// Client �ٸ��ݵȴ��ŵ�Actor ���ػظ����
//-------------------------------------------------------------------------

class CenterThread;

//-------------------------------------------------------------------------
class AccountDBComponent : public DBUser_t_account
{
public:
	virtual void OnLoadRecord(ARecord record) override;
};

//-------------------------------------------------------------------------
// ���ڽ��ܴ���ͻ�������, ����Ӧ�ͻ������������Actor��Ϣ
//-------------------------------------------------------------------------
class AccountCenterActor : public SCActor
{
	struct AccountData 
	{
		Int64 mDBID;
		AString  mPassword;
	};

public:
	Hand< DBUser_t_account> mAccountDBUser;
	FastHash<AString, AccountData> mAccountHash;

public:
	// �����ʺ�
	void On(AC_RequestCreateAccount &req, CA_ResponseCreateAccount &response, UnitID sender, int)
	{
		AccountData *pExistAccount = mAccountHash.findPtr(req.mAccount);
		if (pExistAccount != NULL)
		{
			response.mErrorCode = eErrorCode_ExistAccount;
			response.mDBID = pExistAccount->mDBID;
			return;
		}

		ARecord record = mAccountDBUser->GetTable()->GrowthNewRecord();
		if (record)
		{
			t_account tRecord(record);
			tRecord.ACCOUNT() = req.mAccount;
			//??? tRecord.PASSWORD() = req.mPassword;
			tRecord.INFO(true) = req.mInfoData.getPtr();
			record->SaveUpdate();
			response.mErrorCode = eErrorCodeNone;
			response.mDBID = record->get(0);
			AccountData d;
			d.mDBID = response.mDBID;
			//??? d.mPassword = req.mPassword;
			mAccountHash.insert(req.mAccount, d);
		}
		else
		{
			response.mErrorCode = eErrorCode_DBCreateFail;
		}

	}

	void On(AC_RequestAccountData &req, CA_ResponseAccountData &response, UnitID sender, int)
	{
		AccountData *pExistAccount = mAccountHash.findPtr(req.mAccount);
		if (pExistAccount != NULL)
		{
			//mAccountDBUser->LoadRecord(*pDBID);
			response.mErrorCode = eErrorCodeNone;
			response.mDBID = pExistAccount->mDBID;
			response.mPassword = pExistAccount->mPassword;
			return;
		}
		response.mErrorCode = eErrorCode_NoExistAccount;
	}

public:
	CenterThread* GetLoginThread();


	virtual void Init() override
	{
		SCActor::Init();

		mAccountDBUser = AddComponent("AccountDBComponent");

		if (!mAccountDBUser->InitTable("t_account"))
		{
			ERROR_LOG("Load t_account table fail");
			return;
		}

		// �������е�
		Auto<LogicDBTable> t = mAccountDBUser->GetTable();
		t->LoadAllRecord(mAccountDBUser.getPtr());
		NOTE_LOG("Load total account : %d", mAccountHash.size());
	}

	virtual void OnLoadRecord(ARecord record)
	{
		AccountData d;
		d.mDBID = record->get(0);
		d.mPassword = record->get(2);
		AString account = record->get(1);
		mAccountHash.insert(account, d);
	}

public:
	void On(RQ_CreateDBTable &msg, RS_CreateDBTable &resp, UnitID senderID, int);

	void RegisterMsg(ActorManager *pActorMgr)
	{
		REG_ACTOR_MSG(AccountCenterActor, RQ_CreateDBTable, RS_CreateDBTable);
		REG_COMPONENT(AccountDBComponent);

		REG_ACTOR_MSG(AccountCenterActor, AC_RequestCreateAccount, CA_ResponseCreateAccount);
		REG_ACTOR_MSG(AccountCenterActor, AC_RequestAccountData, CA_ResponseAccountData);
	}
};

