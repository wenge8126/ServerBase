#ifndef _INCLUDE_WEBPROTOCOL_H_
#define _INCLUDE_WEBPROTOCOL_H_

#include "NetHead.h"
#include "EventProtocol.h"

enum
{
	WEB_MSG_RESPNSE_MSG = 1,
	CS_TEST_WEB_MSG = 2,
	SC_TEST_REQUEST_MSG = 3,
};

enum WS_FrameType
{
    WS_EMPTY_FRAME = 0xF0,
    WS_ERROR_FRAME = 0xF1,
    WS_TEXT_FRAME   = 0x01,
    WS_BINARY_FRAME = 0x02,
    WS_PING_FRAME = 0x09,
    WS_PONG_FRAME = 0x0A,
    WS_OPENING_FRAME = 0xF3,
    WS_CLOSING_FRAME = 0x08,
    WS_PART_FRAME = 0xFFFF,
};


class WebNetProtocal : public EventNetProtocol
{
public:
	WebNetProtocal()
	{
		
	}

	virtual int GetMsgHeadSize() const override { return 0; }

	virtual bool WritePacket(int packetID, const Packet* pPacket, DataStream *mSocketOutputStream) override
	{
		return pPacket->Write(*mSocketOutputStream) == TRUE;
	}
	virtual HandPacket ReadPacket(tNetConnect *pConnect, DataStream *mSocketInputStream) override { NOTE_LOG( "ReadPacket no override"); return HandPacket(); }
};


class Net_Export WebNetEventCenter : public Logic::EventCenter
{
public:
	void RegisterMsg(int eventIndex, Logic::AutoEventFactory factory) override;
	void RegisterMsg(Logic::AutoEventFactory hFactory);
	virtual AutoEvent StartEvent(int nEventNameIndex) override
	{
		return _Start(nEventNameIndex, 0);
	}

public:
	// 停止自动维护事件ID优化
	virtual void UpdateNetMsgIndex(AutoData msgIndexData) override {}
	// 生成消息事件索引数据，msgNameIndex = 0 表示生成所有的
	virtual bool GenerateMsgIndex(AutoData &destData, int msgNameIndex = 0) override { return false; }
	bool SerializeMsg(AutoEvent &hEvent, DataStream *destData);
	AutoEvent RestoreMsg(DataStream *scrData) override;

	AutoEvent _Start(ushort eventID, int eventNameIndex) override;

public:
	WebNetEventCenter();
};


#endif //_INCLUDE_WEBPROTOCOL_H_

