
#ifndef _INCLUDE_EVENTPACKET_H_
#define _INCLUDE_EVENTPACKET_H_

#include "Packet.h"
#include "Event.h"
#include "NiceData.h"
#include "EventCenter.h"

#include "NetHead.h"
#include "DefinePacketFactory.h"



//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
class Net_Export DataPacket : public Packet
{
public:
	DataPacket()
		: mbAlreadyEncrypt(false)
	{

	}

	virtual void InitData() override 
	{ 
		mStateData.init(true);
		//mbNeedEncrypt = false; 
		mbAlreadyEncrypt = false; 
		//if (mData)
		//	mData->clear(false);
	}

public:
	virtual bool SetData(AutoData hData, size_t size)
	{
		mData = hData;
		if (mData)
			mData->seek(size);
		return (bool)mData;
	}

	AutoData& GetData() { return mData; }

	void _SetSendData(AutoData hData, size_t size){ mData = hData; if (mData) mData->seek(size); }	

	virtual	VOID		SetPacketID( PacketID_t id )  {};

	//公用继承接口
	virtual BOOL Read( DataStream& iStream, size_t packetSize ) 
	{
		size_t size = packetSize;
		if (!mData)
			mData = MEM_NEW DataBuffer(size);
		else if (mData->size()<(DSIZE)size)
			mData->resize(size);
		if (iStream.Read(mData->data(), (UINT)size)==size)
		{
			mData->seek(size);
			mData->setDataSize(size);

			_OnReceiveData(mData->data(), size);

			return TRUE;
		}
		return FALSE;
	}
	virtual BOOL Write( DataStream& oStream )const
	{
		if (!mData)
			return FALSE;
		size_t size = mData->dataSize();

		const char *szData = ((DataStream*)mData.getPtr())->data();
		_OnSendData((void*)szData, size);

		if (oStream.Write( szData, (UINT)size)==size)
			return TRUE;

		return FALSE;
	}

	virtual UINT GetPacketSize() const {  if (mData) return (UINT)mData->dataSize(); else return (UINT)0; }

protected:
	bool _OnSendData(void *szSendData, size_t size) const;
	bool _OnReceiveData(void *szRevData, size_t size);

	virtual void SetNeedEncrypt(bool bNeed)const{ mStateData.set(ePacketStateEncrypt, bNeed); }
	bool _EncryptData( void *scrData, DSIZE length ) const;
	bool _DecryptData( void *scrData, DSIZE length ) const;

	virtual UINT		GetState() const { return ((StateDataType)mStateData) & 0xFF; }
	virtual VOID		SetState(UINT stateData) { mStateData = (StateDataType)stateData; }

	bool HasState(StateDataType state) const { return mStateData.isOpen(state); }

protected:
	AutoData				mData;
	mutable StateData		mStateData;
	mutable bool			mbAlreadyEncrypt;
};
//-----------------------------------------------------------------------------
class tNetHandle;


class Net_Export EventPacket : public DataPacket
{
public:
	EventPacket();

	~EventPacket()
	{
	}

public:
	virtual int SetEvent(AutoEvent &hEvent);
	virtual AutoData GetData(void){ return mData; }

	virtual UINT Execute( tNetConnect* pConnect );

	virtual	PacketID_t	GetPacketID( ) const { return PACKET_EVENT; }

	virtual AutoData& GetEventData(){ return mData; }
	virtual void SwapEventData( AutoData &eventData );

	virtual bool ReadySendData(){ return true; }

};
//-----------------------------------------------------------------------------
// 优化读取恢复消息事件
class Net_Export EventProcessPacket : public EventPacket
{
public:
	virtual	PacketID_t	GetPacketID( ) const { return PACKET_EVENT_PROCESS; }
	virtual BOOL		Read(DataStream& iStream, size_t packetSize )
	{ AssertEx(0, "不再直接使用Read"); return FALSE; }
	// 接收数据
	virtual BOOL ReadEvent(tNetConnect *pConnect,  DataStream& iStream, size_t packetSize );

	virtual UINT Execute( tNetConnect* pConnect ) override;

	virtual void InitData() override 
	{ 
		EventPacket::InitData();
		mStateData.init(true);
		mMsgEvent.setNull();
	}

public:
	AutoEvent mMsgEvent;
};

//-----------------------------------------------------------------------------
//class CheckRequestPacket : public DataPacket
//{
//public:
//	CheckRequestPacket()
//	{
//		mData = MEM_NEW DataBuffer();
//	}
//	bool SetNiceData(const NiceData &niceData)
//	{		
//		return niceData.serialize(mData.getPtr());
//	}
//
//	virtual UINT Execute( tNetConnect* pConnect );
//
//	virtual	PacketID_t	GetPacketID( ) const { return PACKET_CHECK_REQUEST; }
//
//};
//
////-----------------------------------------------------------------------------
//class CheckResponsePacket : public DataPacket
//{
//public:
//	CheckResponsePacket()
//	{
//		mData = MEM_NEW DataBuffer();
//	}
//	bool SetNiceData(const NiceData &niceData)
//	{		
//		return niceData.serialize(mData.getPtr());
//	}
//
//	virtual UINT Execute( tNetConnect* pConnect );
//
//	virtual	PacketID_t	GetPacketID( ) const { return PACKET_CHECK_RESPONSE; }
//
//};
//-----------------------------------------------------------------------------
class CompressPacket : public EventPacket
{
public:
	CompressPacket();

	virtual void InitData() override 
	{
		EventPacket::InitData();
		//if (mZipBuffer)
		//	mZipBuffer->clear(false);
	}

public:
	//virtual bool SetEvent(AutoEvent &hEvent);

	//virtual UINT Execute( tNetConnect* pConnect );

	virtual	PacketID_t	GetPacketID( ) const { return PACKET_COMPRESS_EVENT; }

	virtual BOOL Read(DataStream& iStream, size_t packetSize );
	virtual bool FullZipData(AutoData zipData);

	virtual int SetEvent(AutoEvent &hEvent);

	virtual AutoData& GetEventData(){ return mZipBuffer; }

	virtual bool ReadySendData();

	virtual void SwapEventData( AutoData &eventData );

	virtual bool SetData(AutoData hData, size_t size){ return FullZipData(hData); }

public:
	static bool UnZip(AutoData zipBuffer, AutoData &resultData);

protected:
	AutoData		mZipBuffer;

};

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

#endif //_INCLUDE_EVENTPACKET_H_