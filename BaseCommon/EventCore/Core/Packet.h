

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


//Packet::Execute(...) �ķ���ֵ
enum PACKET_EXE
{
	PACKET_EXE_ERROR = 0 ,
	PACKET_EXE_BREAK ,
	PACKET_EXE_CONTINUE ,
	PACKET_EXE_NOTREMOVE ,
	PACKET_EXE_NOTREMOVE_ERROR ,
	PACKET_EXT_NO_EXESUNPRO,	//û��ע�ắ��
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

	//����ֵΪ��PACKET_EXE �е����ݣ�
	//PACKET_EXE_ERROR ��ʾ�������ش��󣬵�ǰ������Ҫ��ǿ�ƶϿ�
	//PACKET_EXE_BREAK ��ʾ���غ�ʣ�µ���Ϣ�����ڵ�ǰ����ѭ���ﴦ��
	//PACKET_EXE_CONTINUE ��ʾ�����ڵ�ǰѭ����ִ��ʣ�µ���Ϣ
	//PACKET_EXE_NOTREMOVE ��ʾ�����ڵ�ǰѭ����ִ��ʣ�µ���Ϣ,���ǲ����յ�ǰ��Ϣ
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
