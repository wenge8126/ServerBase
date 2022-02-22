/********************************************************************
	created:	2013/05/14
	created:	14:5:2013   17:32
	filename: 	C:\Work\Game\Mushroom\BaseCommon\ServerBase\Base\EventProtocol.h
	file path:	C:\Work\Game\Mushroom\BaseCommon\ServerBase\Base
	file base:	EventProtocol
	file ext:	h
	author:		Yang WenGe
	
	purpose:	
*********************************************************************/
#ifndef _INCLUDE_EVENTPROTOCOL_H_
#define _INCLUDE_EVENTPROTOCOL_H_

#include "NetHandle.h"
#include "NetHead.h"
#include "Packet.h"
//#include "TableRecord.h"
#include "Event.h"
#include "EventPacket.h"

//----------------------------------------------------------------------
enum
{
	ID_INDEX_COL	= 0,
	VALUENAME_COL	= 1,
	TYPE_COL		= 2,
	DEFAULT_COL		= 3,
	CHECK_COL_MAX
};

const char gMsgTableFieldName[4][16] = { "INDEX", "VALUENAME", "TYPE", "DEFAULT" };

#define TABLE_TYPE_STRING_FLAT	"TABLE"
//----------------------------------------------------------------------------------------------
// 事件网络消息，用来解析事件，序列化事件， 主要读写包功能
class EventPacket;
class EventData;
struct PacketData;
class LoopData;

class Net_Export EventNetProtocol : public tNetProtocol
{
	friend class BaseNetHandle;

public:
	EventNetProtocol();

public:
	// 消息数据定义
	//ARecord GetStructRecord( AutoEvent evt, const AString &structDefineIndex, AutoEvent parentEvent, int nIndex, bool bCreate = true );
	virtual int GetMsgHeadSize() const  { return PACKET_HEADER_SIZE; }

public:
	virtual bool RegisterNetPacket(AutoPacketFactory f, bool bRespace = true);
	virtual AutoPacketFactory FindPacketFactory(PacketID_t packetID) override { return mNetPacketFactoryList.Find(packetID); }
	virtual int AppendNetPacketFrom(tNetProtocol *other, bool bReplace);

	virtual HandPacket	CreatePacket (PacketID packetID);

	virtual HandPacket GenerateEventPacket(tNetConnect *pConnect, Logic::tEvent *pSendEvent, bool bNeedZip = true);

	virtual bool WritePacket(int packetID, const Packet* pPacket, DataStream *mSocketOutputStream ) override;
	virtual HandPacket ReadPacket( tNetConnect *pConnect, DataStream *mSocketInputStream ) override;

	virtual void OnWritePacketError(AString strErrorInfo);
	virtual void OnReadPacketError(AString strErrorInfo);
	// 为了网络安全, 如果发生协议恢复失败时, 全部清除连接处理
	virtual void OnPacketExecuteError(tNetConnect *pConnect, Packet *pPacket)
	{
		WARN_LOG("Net packet excute error >[%s], then now remove connect >[%s:%d]", pPacket->GetMsgName(), pConnect->GetIp(), pConnect->GetPort());
		pConnect->SetRemove(true); 
	}
	
    virtual void OnBeforeSendEvent(tNetConnect *pConnect, Logic::tEvent *pEvent);

    virtual bool UseNiceDataProtocol() const { return true; }

public:
	virtual bool CheckPacketInfo(tNetConnect *pConnect, PacketID_t packetID, UINT packetSize);

protected:
	ArrayMap<AutoPacketFactory> mNetPacketFactoryList;
	DataBuffer	mTempWriteBuffer;

public:
	static bool SendEvent(tNetConnect *connect, Logic::tEvent *sendEvent);

public: 
    static const uint  _MAX_SIZE;
};

//----------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------
#endif //_INCLUDE_EVENTPROTOCOL_H_