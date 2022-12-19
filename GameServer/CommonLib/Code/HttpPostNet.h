#pragma once

//-------------------------------------------------------------------------
// HTTP post ������������Ϣ
// 1 ˳������
// 2 ͨ��ResponseBytesHttp �� Actor��Ϣ���� (HttpReqeustActorMsg  )
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
	AutoNice AwaitRequest(const char *szUrl, int actorType, Int64 actorID, tBaseMsg *packet, int connectOverSecond, int overSecond, const char *pCaPath = NULL);

public:
	HttpPostNet();

	enum 
	{
		eTaskRequestString,		
		eTaskReqeustPostBytesMsg,
	};

};


