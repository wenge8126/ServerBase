

#ifndef __PACKET_H__
#define __PACKET_H__

#include "EventCore.h"
#include "LoopDataStream.h"
#include "MemBase.h"
#include "Auto.h"

//-------------------------------------------------------------------------

#define CHECK_READ_STREAM(a) \
	if ( iStream.Read( (char*)&a, sizeof(a))!=sizeof(a)  ) return FALSE;

#define CHECK_WRITE_STREAM(a) \
	if (oStream.Write((char*)&a, sizeof(a) )!=sizeof(a) ) return FALSE;

//-------------------------------------------------------------------------*/
class Socket;
class Player;

typedef unsigned char	PacketID_t;


//Packet::Execute(...) 的返回值
enum PACKET_EXE
{
	PACKET_EXE_ERROR = 0 ,
	PACKET_EXE_BREAK ,
	PACKET_EXE_CONTINUE ,
	PACKET_EXE_NOTREMOVE ,
	PACKET_EXE_NOTREMOVE_ERROR ,
	PACKET_EXT_NO_EXESUNPRO,	//没有注册函数
};

class tNetConnect;
class PacketFactory;
class PacketFactoryPtr;
//-------------------------------------------------------------------------*/
class EventCoreDll_Export Packet : public AutoBase //Base<Packet>
{
public :
	Auto<PacketFactoryPtr>	mFactoryPtr;

public :
	Packet( )
	{}

	virtual ~Packet( ){}


	virtual	PacketID_t	GetPacketID() const = 0;

	virtual void InitData() {}

public:
	virtual UINT		GetState() const = 0;
	virtual VOID		SetState(UINT stateData) = 0;

	virtual BOOL		Read( DataStream& iStream, size_t packetSize, tNetConnect* pConnect) = 0 ;
	virtual BOOL		Write( DataStream& oStream ) const = 0;
	
	virtual bool ReadExt(DataStream& iStream, tNetConnect* pConnect) { return true; }
	virtual bool WriteExt(DataStream& oStream) { return true; }

	//返回值为：PACKET_EXE 中的内容；
	//PACKET_EXE_ERROR 表示出现严重错误，当前连接需要被强制断开
	//PACKET_EXE_BREAK 表示返回后剩下的消息将不在当前处理循环里处理
	//PACKET_EXE_CONTINUE 表示继续在当前循环里执行剩下的消息
	//PACKET_EXE_NOTREMOVE 表示继续在当前循环里执行剩下的消息,但是不回收当前消息
	virtual UINT		Execute( tNetConnect* pConnect ) = 0;	

	virtual BOOL		CheckPacket( ){ return TRUE ; }
	virtual bool		IsBigPacket() const { return false; }

	virtual void		SetNeedEncrypt(bool bNeed)const{ }

	virtual VOID		CleanUp( ){}
	virtual void		Release() override;

	Auto<Packet> GetSelf() { return Auto<Packet>(this); }
	int						_getID() const { return mID; }

	virtual const char* getName() const { return "None"; }
};
//----------------------------------------------------------------------------------
typedef Auto<Packet>	HandPacket;
//template<>
//void Hand<Packet>::FreeClass(Packet *p)
//{
//	p->Release();
//}
//typedef Hand<Packet>	HandPacket;
//----------------------------------------------------------------------------------

#endif
