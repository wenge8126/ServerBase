#ifndef _INCLUDE_WEBPROTOCOL_H_
#define _INCLUDE_WEBPROTOCOL_H_

#include "sha1.h"
#include <string>
#include "NetHead.h"
#include "EventProtocol.h"

#define DEFALUT_WEB_SEND_PACKET_ID	100

//using namespace std;

enum
{
	WEB_MSG_RESPNSE_MSG = 1,
	CS_TEST_WEB_MSG = 2,
	SC_TEST_REQUEST_MSG = 3,
};

enum WS_Status
{
    WS_STATUS_CONNECT = 0,
    WS_STATUS_UNCONNECT = 1,
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
class DataStream;
class Net_Export WebProtocol
{
public:
    static int wsHandshake(std::string &request, std::string &response);

    //NOTE: 未判断源数据长度是否 >= 接收数据长度, 在使用后, 需要进行检查长度信息是否正确
    static int wsDecodeFrame(DataStream *pScrMsgFrameData, DataStream *destData, WS_FrameType &frameType, bool &bLastFrame, bool &bMask);

    static int wsEncodeFrame(DataStream *pScrSendData, DataStream *pDestData, enum WS_FrameType frameType);

};


class WebSendPacket : public Packet
{
public:
	WebSendPacket()
		: mData(128){}

public:
	virtual	PacketID_t	GetPacketID( ) const { return DEFALUT_WEB_SEND_PACKET_ID; }
	virtual UINT		GetState() const { return 0; }
	virtual VOID		SetState(UINT stateData) {}

	virtual BOOL		Read(DataStream& iStream, size_t packetSize, tNetConnect *pConnect ) { return FALSE; }
	virtual BOOL		Write( DataStream& oStream ) const 
	{ 
		return oStream._write((void*)mData.data(), mData.dataSize()); 
	}

	//返回值为：PACKET_EXE 中的内容；
	//PACKET_EXE_ERROR 表示出现严重错误，当前连接需要被强制断开
	//PACKET_EXE_BREAK 表示返回后剩下的消息将不在当前处理循环里处理
	//PACKET_EXE_CONTINUE 表示继续在当前循环里执行剩下的消息
	//PACKET_EXE_NOTREMOVE 表示继续在当前循环里执行剩下的消息,但是不回收当前消息
	virtual UINT		Execute( tNetConnect* pConnect ) { return 0; } 

public:
	DataBuffer	mData;
};

class WebNetProtocal : public EventNetProtocol
{
public:
	WebNetProtocal()
	{
		RegisterNetPacket(MEM_NEW DefinePacketFactory<WebSendPacket, DEFALUT_WEB_SEND_PACKET_ID>());
	}

	virtual int GetMsgHeadSize() const override { return 0; }

	virtual bool WritePacket( const Packet* pPacket, DataStream *mSocketOutputStream ) override
	{
		return pPacket->Write(*mSocketOutputStream)==TRUE;
	}
	virtual HandPacket ReadPacket( tNetConnect *pConnect, DataStream *mSocketInputStream ) override { AssertNote(0, "Can not use"); return false; }
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

