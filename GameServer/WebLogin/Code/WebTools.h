#pragma once

#include "ServerEvent.h"

//-------------------------------------------------------------------------
// ���������߸�������, ����, �������, ʹ��JSzip ѹ���������ݰ�
// CoCos Web ���� ���߽�ɫ WebLogin, �������ͨ��Gate���ӵ�WebLogin, 
// Ȼ��, ��WebLogin����Web���ؽ��, �ظ�������
//-------------------------------------------------------------------------

// ����JSZIPѹ������
class LC_RequestJSZipData : Logic::tServerEvent
{
public:
	virtual bool _DoEvent() override
	{
		WaitTime(20);
		return true;
	}

	virtual void _OnResp(AutoEvent &respEvent) override
	{
		NOTE_LOG("JS zip result > %s\r\n%s", respEvent["RESULT"].c_str(), respEvent->GetData().dump().c_str());

	}
};