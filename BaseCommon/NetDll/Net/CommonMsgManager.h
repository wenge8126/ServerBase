/********************************************************************
	created:	2010/06/03
	created:	3:6:2010   13:23
	filename: 	d:\Work\client_trunk\Client\WXClient\Network\CommonMsgManager.h
	file path:	d:\Work\client_trunk\Client\WXClient\Network
	file base:	CommonMsgManager
	file ext:	h
	author:		���ĸ�
	
	purpose:	�����Ǵ���ͨ����Ϣ���¼�,�������Ϣ,�ʺ�������Щ��Ƶ�������¼�,�������
*********************************************************************/
#pragma once

#include <map>
#include <string>
#include <list>
#include "DataBuffer.h"

#include "NetHead.h"

using namespace std;

class Player;
class Packet;
struct HUMANLIST;

//�ȴ����͵�����
struct  SendEventData
{
	AString		eventName;
	AutoData	data;
	size_t		mSize;
	unsigned int upTime;
	Player		*player;
	HUMANLIST	*listOutHuman;	//�������õķ�������б�

	SendEventData(const AString &evt,AutoData hData,size_t size)
	{
		eventName = evt;
		mSize = size;
		data = hData;
		upTime = true;
		player = NULL;
		listOutHuman = NULL;
	}
};


class Net_Export CommonMsgManager
{

public:
	CommonMsgManager();
	
	unsigned int OnCommonMsg( Player *pPlayer, Packet *pPacket );

	virtual	unsigned int ExeCommonPacket( Player *pPlayer, Packet *pPacket,
		const AString &eventName, AutoData hTable ) = 0;
	// ����һ����ķְ�,����ȫ������
	virtual bool __SendData( SendEventData &sendData ) = 0;
	virtual bool SendData( const char* eventName, AutoData hData, Player *player ) = 0;

protected:
	typedef std::map<int, AutoData>		AutoDataMap;
	AutoDataMap							mRevDataMap;
	
	std::list<SendEventData>			m_waitSendTableList;	


public:
	void Tick();

};
//----------------------------------------------------------------------------------------------

