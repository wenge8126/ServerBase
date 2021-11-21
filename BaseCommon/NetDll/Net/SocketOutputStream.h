
//
//�ļ����ƣ�	SocketOutputStream.h
//����������	��Ϣ���ݵ�������棬�ṩ���ݵĸ�ʽ��д��ͷ��͹���
//				
//				
//
//

#ifndef __SOCKETOUTPUTSTREAM_H__
#define __SOCKETOUTPUTSTREAM_H__

#include "BaseCommon.h"
#include "NetHead.h"
#include "NetConfig.h"
#include "AutoString.h"
#include "LoopDataStream.h"

class Socket;
class Packet;
class tNetObject;

class Net_Export SocketOutputStream : public LoopDataStream
{
public :
	SocketOutputStream(UINT BufferSize = DEFAULTSOCKETINPUTBUFFERSIZE,
		UINT MaxBufferSize = DISCONNECTSOCKETINPUTSIZE );

	//SocketOutputStream( Socket* sock, 
	//					UINT BufferSize = DEFAULTSOCKETOUTPUTBUFFERSIZE,
	//					UINT MaxBufferSize = DISCONNECTSOCKETOUTPUTSIZE ) ;
	virtual ~SocketOutputStream( ) ;


public :
	INT							Flush( Socket *m_pSocket ) ;
 
	UINT						GetFreeLength();


//protected :	
//	CHAR*		m_Buffer ;
//	
//	UINT		m_BufferLen ;
//	UINT		m_MaxBufferLen ;
//	
//	UINT		m_Head ;
//	UINT		m_Tail ;
};



#endif
