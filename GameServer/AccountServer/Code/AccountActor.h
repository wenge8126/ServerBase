#pragma once

#include "ActorManager.h"
#include "HttpComponect.h"

#include "DefineMsgFactory.h"

#include "SCActor.h"
#include "ServerMsg.h"

using namespace NetCloud;

//-------------------------------------------------------------------------
// �ͻ��������ظ�����Ϣ
// ������LogicActor �յ���, ʹ���첽�ȴ� ClientActor �ٴ��첽������Ŀ��Actor
// ClientActor ���յ��ظ���, LogicAtor �����µĻظ���Ϣ, �ظ����ͻ���
// Client �ٸ��ݵȴ��ŵ�Actor ���ػظ����
//-------------------------------------------------------------------------


//-------------------------------------------------------------------------
// ���ڽ��ܴ���ͻ�������, ����Ӧ�ͻ������������Actor��Ϣ
//-------------------------------------------------------------------------
class AccountActor : public SCActor
{
public:
	AccountThread* GetLoginThread()
	{
		Auto<AccountActorManager> mgr = GetMgr();
		return mgr->mpThread;
	}

	virtual void Init() override
	{
		SCActor::Init();
		
		auto &config = CRunConfig<AccountServerConfig>::mConfig;

		Hand<HttpComponect> comp = AddComponent("HttpComponect");
		comp->mPort = config.web_net.port;
		comp->mCertFile = config.web_net.cert_file;
		comp->mIP = config.web_net.http_address;
		comp->mKeyFile = config.web_net.key_file;
		comp->mPassword = config.web_net.password;

	}

	void Analysis(NiceData &msg, const AString &requestData)
	{
		Array<AString> tempList;
		AString::Split(requestData.c_str(), tempList, "&", 100);
		for (int i = 0; i < tempList.size(); i++)
		{
			Array<AString> str;
			AString::Split(tempList[i].c_str(), str, "=", 2);

			if (str.size() == 2)
			{
				msg[str[0].c_str()] = str[1].c_str();
			}
		}
	}


	virtual void ResponseHttp(const AString &requestData, AString &response, bool bPost, const AString &requestAddress) override 
	{ 
		NiceData msg;
		Analysis(msg, requestData);

		NOTE_LOG("http request ********* %s", msg.dump().c_str());

		AString account = msg.get("ACCOUNT");
		if (account.length() > 0)
		{
			AC_RequestCreateAccount req;
			CA_ResponseCreateAccount resp;
			if (Await(UnitID(Actor_AccountCenter, 1), req, resp, 10000))
			{
				response = resp.dump();
				return;
			}
		}

		response = "7788--------1 Error"; 
	}


	void RegisterMsg(ActorManager *pActorMgr)
	{
		
	}
};

