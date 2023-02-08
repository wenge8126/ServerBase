#pragma once

//-------------------------------------------------------------------------
// HTTP post 二进制数据消息
// 1 顺序请求
// 2 通过ResponseBytesHttp 与 Actor消息交互 (HttpReqeustActorMsg  )
//-------------------------------------------------------------------------

#include "CommonLib.h"
#include "TaskManager.h"
#include "NiceData.h"
#include "Packet.h"
#include "EasyList.h"
#include "BaseMsg.h"

class CommonLib_Export HttpPostNet : public TaskManager
{
public:
	AutoNice AwaitRequest(AutoData token, const char *szUrl, int actorType, Int64 actorID, tBaseMsg *packet, int connectOverSecond, int overSecond, const char *pCaPath = NULL);

	AString AwaitRequestGet(const char *szUrl, const AString &paramString, int connectOverSecond, int overSecond, const char *pCaPath);

public:
	HttpPostNet();

	enum 
	{
		eTaskRequestString,		
		eTaskReqeustPostBytesMsg,
	};

};


