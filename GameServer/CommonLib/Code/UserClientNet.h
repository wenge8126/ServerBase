#pragma once

//-------------------------------------------------------------------------
// ��Զ�̿ͻ����������Actor����
// �����˺���֤��ȫ����
// LoginNetActor > CS_ClientRequest, ���� SC_ResponseMsg
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