#pragma once

//-------------------------------------------------------------------------
// 以远程客户端与服务器Actor交互
// 具有账号验证安全流程
// LoginNetActor > CS_ClientRequest, 返回 SC_ResponseMsg
//-------------------------------------------------------------------------


class CommonLib_Export UserClientNet : public tNetProcess
{
public:
	virtual bool ProcessPacket(tNetConnect* pConnect, Packet *pPacket) override
	{
		Auto<SC_ResponseMsg> resp = pPacket;

	}

public:
	AutoNet		mClientNet;
};

//-------------------------------------------------------------------------