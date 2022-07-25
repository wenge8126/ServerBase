#pragma once

#include "HttpClient.h"
#include "FileDataStream.h"
#include "CommonLib.h"

#define ONLY_BAD_SEND		1
#define TEST_SERVER				1

class CommonLib_Export_H DuanXin
{
public:
	static bool Send(const AString &state, int lineCount, int serverID)
	{
		FileDataStream addrConfig("./RunConfig/DuanXin.json", FILE_READ);
		AutoData d = addrConfig.readAllData();
		if (!d || d->dataSize()<=0)
		{
			ERROR_LOG("./RunConfig/DuanXin.json no exist or is null");
			return false;
		}

		AString configString;
		configString.set(d->data(), d->dataSize());

		NiceData config;
		config.FullJSON(configString);

		AString addr = config["http"];
		if (addr.length() <= 0)
		{
			ERROR_LOG("./RunConfig/DuanXin.json no config [http]");
			return false;
		}

		CHttpClient  client;
		std::string response;

		AString strRequest;
		if (lineCount > 0)
			strRequest.Format("http://%s?code=server%d_%s_line%d", addr.c_str(), serverID, state.c_str(), lineCount);
		else
			strRequest.Format("http://%s?code=server%d_%s", addr.c_str(), serverID, state.c_str());

		client.Get(strRequest.c_str(), response, 10);
		if (response == "OK")
		{
			LOG("Send succeed : %s", strRequest.c_str());
			return true;
		}
		else
		{
			ERROR_LOG("%s\r\nResponse: %s", strRequest.c_str(), AString::getANIS(response.c_str()).c_str());
			return false;
		}

		//int i = 0;
		//bool b = true;
		//AString key = "phone";
		//while (++i<100)
		//{
		//	AString phoneNum = config[key + i];
		//	if (phoneNum.length() <= 0)
		//		break;

		//	AString strRequest;
		//	if (lineCount>0)
		//		strRequest.Format("http://%s?phone=%s&code=server%d_%s_line%d", addr.c_str(), phoneNum.c_str(), serverID, state.c_str(), lineCount);
		//	else
		//		strRequest.Format("http://%s?phone=%s&code=server%d_%s", addr.c_str(), phoneNum.c_str(), serverID, state.c_str());

		//	client.Get(strRequest.c_str(), response, 10);
		//	if (response == "OK")
		//		LOG("Send succeed : %s", strRequest.c_str())
		//	else
		//	{
		//		ERROR_LOG("%s\r\nResponse: %s", strRequest.c_str(), AString::getANIS(response.c_str()).c_str());
		//		b = false;
		//	}
		//} 

		//return b;		
	}

	static AString GetState(StateDataType stateValue)
	{
		if (stateValue <= 0)
		{
			return "ok";			
		}
		else
		{
			StateData  stateData(stateValue);

			if (stateData.isOpen(EBASE_RUN_STATE::eStartError) || stateData.isOpen(EBASE_RUN_STATE::eLogicError))
				return "error";
			else if (stateData.isOpen(EBASE_RUN_STATE::eLogicMaintain))
				return "maintain";
			else if (stateData.isOpen(EBASE_RUN_STATE::eMemoryTooBad))
				return "bad";
			else if (stateData.isOpen(EBASE_RUN_STATE::eDiskTooBad))
				return "bad";
			else if (stateData.isOpen(EBASE_RUN_STATE::eLoopTooBad))
				return "bad";
			else if (stateData.isOpen(EBASE_RUN_STATE::eLowLoopTooBad))
				return "bad";
			else if ((stateValue & 0xFF) > 0)
				return "warn";
		}
		return "unkwon";
	}
};