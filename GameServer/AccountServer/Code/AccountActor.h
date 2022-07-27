#pragma once

#include "ActorManager.h"
#include "HttpComponect.h"

#include "DefineMsgFactory.h"

#include "SCActor.h"
#include "ServerMsg.h"
#include "GameCommon.h"

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


	//����ʾ��: http://127.0.0.1:1080/?CMD=CREATE&ACCOUNT=wenge3&PASSWORD=999888
	//����ʾ��: http://127.0.0.1:1080/?CMD=CHECK&ACCOUNT=wenge3&PASSWORD=999888
	//{ �ظ�
	//	"DBID" : "10004",
	//		"LOGIN_IP" : "127.0.0.1",
	//		"PORT" : 4001,
	//		"TOKEN" : "OK"
	//}
	virtual void ResponseHttp(const AString &requestData, AString &response, bool bPost, const AString &requestAddress) override 
	{ 
		NiceData msg;
		GameCommon::Analysis(msg, requestData);

		NOTE_LOG("http request ********* %s", msg.dump().c_str());
		AString cmd = msg.get("CMD");
		
		AString account = msg.get("ACCOUNT");
		AString passWord = msg.get("PASSWORD");
		
		if (account.length() >= 6 && passWord.length() >= 6)
		{
			
			if (cmd == "CREATE")
			{
				AC_RequestCreateAccount req;
				req.mAccount = account;
				req.mPassword = passWord;
				CA_ResponseCreateAccount resp;
				if (Await(UnitID(Actor_AccountCenter, 1), req, resp, 10000))
				{
					//response = resp.dump();
					response = resp.ToJSON();
					return;
				}
				else
				{
					NiceData respNice;
					respNice["error"] = eErrorCode_DBCreateFail;
					response = respNice.ToJSON();
				}
			}
			else if (cmd == "CHECK")
			{
				NiceData respNice;
				AC_RequestAccountData req;
				req.mAccount = account;

				CA_ResponseAccountData resp;
				if (Await(UnitID(Actor_AccountCenter, 1), req, resp, 10000))
				{

					if (true) //???  resp.mPassword==passWord
					{
						// ����token, �������������������LoginServer
						respNice["TOKEN"] = "OK";
						respNice["LOGIN_IP"] = "127.0.0.1";
						respNice["PORT"] = 4001;
						respNice["DBID"] = resp.mDBID;
						respNice["error"] = 0;
						response = respNice.ToJSON();
					}
					else
					{
						
						respNice["error"] = eErrorCode_PasswordError;
						response = respNice.ToJSON();
					}
					return;
				}
				else
				{
					
					respNice["error"] = eErrorCode_DBQeuryFail;
					response = respNice.ToJSON();
				}				
			}
			else
			{
				NiceData respNice;
				respNice["error"] = eErrorCode_HttpCmdError;
				respNice["cmd"] = cmd;
				response = respNice.ToJSON();
			}				
		}
		else
		{
			NiceData respNice;
			respNice["error"] = eErrorCode_AccountOrPasswordLengthLess;		
			response = respNice.ToJSON();
		}
		
		//response = requestData; // "7788--------1 Error";
	}


	void RegisterMsg(ActorManager *pActorMgr)
	{
		
	}
};

