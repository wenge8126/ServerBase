//#include "stdafx.h"
//////////////////////////////////////////////////////////////////////
// 
// Socket.cpp
// 
// 
//////////////////////////////////////////////////////////////////////

#include "Socket.h"
#include "Assertx.h"

#include "stdio.h"
#include <string.h>
#include "Common.h"

#ifdef __LINUX__
	#include <sys/select.h>
#endif

#if DEVELOP_MODE
CsLock				Socket::mDebugLock;
int					Socket::mSocketCount = 0;
#endif

Socket::Socket () 
	: m_SocketID(INVALID_SOCKET)
{ 
	__ENTER_FUNCTION_FOXNET
#if DEVELOP_MODE
		mDebugLock.lock();
	++mSocketCount;
	DEBUG_LOG("++ socket count > [%d]", mSocketCount);
	mDebugLock.unlock();
#endif
	m_SocketID = INVALID_SOCKET ;
	memset( &m_SockAddr, 0, sizeof(SOCKADDR_IN) ) ;
	memset( m_Host, 0, IP_SIZE ) ;
	m_Port = 0 ;
	
	__LEAVE_FUNCTION_FOXNET
}

Socket::Socket ( const CHAR* host , UINT port ) 
	: m_SocketID(INVALID_SOCKET)
{ 
	__ENTER_FUNCTION_FOXNET
#if DEVELOP_MODE
		mDebugLock.lock();
	++mSocketCount;
	DEBUG_LOG("++ socket count > [%d]", mSocketCount);
	mDebugLock.unlock();
#endif
	strncpy( m_Host, host, IP_SIZE-1 ) ;
	m_Port = port ;

	create() ;	

	__LEAVE_FUNCTION_FOXNET
}

Socket::~Socket () 
{ 
	__ENTER_FUNCTION_FOXNET

	close() ;
#if DEVELOP_MODE
	mDebugLock.lock();
	--mSocketCount;
	DEBUG_LOG("-- socket count > [%d]", mSocketCount);
	mDebugLock.unlock();
#endif
	__LEAVE_FUNCTION_FOXNET
}

BOOL Socket::create( )
{
	__ENTER_FUNCTION_FOXNET

	m_SocketID = SocketAPI::socket_ex( AF_INET , SOCK_STREAM , 0 );
    
	memset( &m_SockAddr , 0 , sizeof(m_SockAddr) );
	
	m_SockAddr.sin_family = AF_INET;

	if( isValid() ) 
	{
		return TRUE ;
	}
	else 
		return FALSE ;

	__LEAVE_FUNCTION_FOXNET

	return FALSE ;
}

// close previous connection and connect to another server socket
BOOL Socket::reconnect ( const CHAR* host , UINT port )
{
	__ENTER_FUNCTION_FOXNET

	// delete old socket impl object
	close();

	// create new socket impl object
	strncpy( m_Host, host, IP_SIZE-1 ) ;
	m_Port = port ;

	create() ;

	// try to connect
	return connect();	

	__LEAVE_FUNCTION_FOXNET

	return FALSE ;
}

VOID Socket::close () 
{ 
	__ENTER_FUNCTION_FOXNET

//	if( !isSockError() ) 
	if( isValid() && !isSockError() ) 
	{
		SocketAPI::closesocket_ex( m_SocketID );
	}

	m_SocketID = INVALID_SOCKET ;
	memset( &m_SockAddr, 0, sizeof(SOCKADDR_IN) ) ;
	memset( m_Host, 0, IP_SIZE ) ;
	m_Port = 0 ;

	__LEAVE_FUNCTION_FOXNET
}
	
BOOL Socket::connect () 
{ 
	__ENTER_FUNCTION_FOXNET

	m_SockAddr.sin_addr.s_addr = inet_addr( m_Host );
	
	// set sockaddr's port
	m_SockAddr.sin_port = htons(m_Port);
	
	// try to connect to peer host
	return SocketAPI::connect_ex( m_SocketID , (const struct sockaddr *)&m_SockAddr , sizeof(m_SockAddr) );


	__LEAVE_FUNCTION_FOXNET

	return 0 ;
}

BOOL Socket::connect (const CHAR* host, UINT port) 
{ 
	__ENTER_FUNCTION_FOXNET

	strncpy( m_Host, host, IP_SIZE-1 ) ;
	m_Port = port ;

	return connect() ;

	__LEAVE_FUNCTION_FOXNET

	return 0 ;
}

UINT Socket::send (const VOID* buf, UINT len, UINT flags) 
{ 
	__ENTER_FUNCTION_FOXNET

	return SocketAPI::send_ex( m_SocketID, buf, len, flags );

	__LEAVE_FUNCTION_FOXNET

	return 0 ;
}

UINT Socket::receive (VOID* buf, UINT len, UINT flags) 
{ 
	__ENTER_FUNCTION_FOXNET

	return SocketAPI::recv_ex( m_SocketID , buf , len , flags );

	__LEAVE_FUNCTION_FOXNET

	return 0 ;
}

UINT Socket::available ()const
{ 
	__ENTER_FUNCTION_FOXNET

	return SocketAPI::availablesocket_ex( m_SocketID );

	__LEAVE_FUNCTION_FOXNET

	return 0 ;
}

UINT Socket::getLinger ()const 
{ 
	__ENTER_FUNCTION_FOXNET

	struct linger ling;
	UINT len = sizeof(ling);
	
	SocketAPI::getsockopt_ex( m_SocketID , SOL_SOCKET , SO_LINGER , &ling , &len );
	
	return ling.l_linger;

	__LEAVE_FUNCTION_FOXNET

	return 0 ;
}

SOCKET Socket::accept( struct sockaddr* addr, UINT* addrlen )
{
	__ENTER_FUNCTION_FOXNET

	return SocketAPI::accept_ex( m_SocketID, addr, addrlen ) ;

	__LEAVE_FUNCTION_FOXNET

	return INVALID_SOCKET ;
}

BOOL Socket::setLinger (UINT lingertime) 
{
	__ENTER_FUNCTION_FOXNET

	struct linger ling;
	
	ling.l_onoff = lingertime > 0 ? 1 : 0;
	ling.l_linger = lingertime;
	
	return SocketAPI::setsockopt_ex( m_SocketID , SOL_SOCKET , SO_LINGER , &ling , sizeof(ling) );

	__LEAVE_FUNCTION_FOXNET

	return FALSE ;
}

UINT Socket::getSockError()const 
{ 
	__ENTER_FUNCTION_FOXNET

	return isSockError(); 

	__LEAVE_FUNCTION_FOXNET

	return 0 ;
}

BOOL Socket::isNonBlocking ()const 
{ 
	__ENTER_FUNCTION_FOXNET

	return SocketAPI::getsocketnonblocking_ex( m_SocketID );

	__LEAVE_FUNCTION_FOXNET

	return FALSE ;
}

BOOL Socket::setNonBlocking (BOOL on ) 
{ 
	__ENTER_FUNCTION_FOXNET

	return SocketAPI::setsocketnonblocking_ex( m_SocketID , on );

	__LEAVE_FUNCTION_FOXNET

	return FALSE ;
}

UINT Socket::getReceiveBufferSize ()const 
{ 
	__ENTER_FUNCTION_FOXNET

	UINT ReceiveBufferSize;
	UINT size = sizeof(ReceiveBufferSize);

	SocketAPI::getsockopt_ex( m_SocketID , SOL_SOCKET , SO_RCVBUF , &ReceiveBufferSize, &size );
	
	return ReceiveBufferSize;

	__LEAVE_FUNCTION_FOXNET

	return 0 ;
}

BOOL Socket::setReceiveBufferSize (UINT size)
{ 
	__ENTER_FUNCTION_FOXNET

	return (BOOL)(SocketAPI::setsockopt_ex( m_SocketID , SOL_SOCKET , SO_RCVBUF , &size, sizeof(UINT) ) ) ;

	__LEAVE_FUNCTION_FOXNET

	return FALSE ;
}

UINT Socket::getSendBufferSize ()const 
{ 
	__ENTER_FUNCTION_FOXNET

	UINT SendBufferSize;
	UINT size = sizeof(SendBufferSize);

	SocketAPI::getsockopt_ex( m_SocketID , SOL_SOCKET , SO_SNDBUF , &SendBufferSize, &size );
	
	return SendBufferSize;

	__LEAVE_FUNCTION_FOXNET

	return 0 ;
}

BOOL Socket::setSendBufferSize (UINT size)
{ 
	__ENTER_FUNCTION_FOXNET

	return (BOOL)(SocketAPI::setsockopt_ex( m_SocketID , SOL_SOCKET , SO_SNDBUF , &size, sizeof(UINT) ) );

	__LEAVE_FUNCTION_FOXNET

	return FALSE ;
}

UINT Socket::getPort ()const 
{ 
	__ENTER_FUNCTION_FOXNET

	return m_Port; 

	__LEAVE_FUNCTION_FOXNET
}

IP_t Socket::getHostIP ()const 
{ 
	__ENTER_FUNCTION_FOXNET

	return (IP_t)(m_SockAddr.sin_addr.s_addr);; 

	__LEAVE_FUNCTION_FOXNET
}

LPCSTR Socket::getHost() const
{
	return m_Host;
}

BOOL Socket::isValid ()const 
{
	__ENTER_FUNCTION_FOXNET

	return m_SocketID!=INVALID_SOCKET; 

	__LEAVE_FUNCTION_FOXNET
}

SOCKET Socket::getSOCKET ()const 
{ 
	__ENTER_FUNCTION_FOXNET

	return m_SocketID; 

	__LEAVE_FUNCTION_FOXNET
}

BOOL Socket::isSockError()const
{
	__ENTER_FUNCTION_FOXNET

	INT error;
	UINT len = sizeof(error);
	
	INT Result = SocketAPI::getsockopt_ex2( m_SocketID , SOL_SOCKET , SO_ERROR , &error, &len );

	if( Result == 0 ) 
		return FALSE;
	else 			  
		return TRUE;

	__LEAVE_FUNCTION_FOXNET

	return FALSE ;
}

BOOL Socket::bind( )
{
	__ENTER_FUNCTION_FOXNET

	m_SockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    m_SockAddr.sin_port        = htons(m_Port);

	BOOL result = SocketAPI::bind_ex( m_SocketID , (const struct sockaddr *)&m_SockAddr , sizeof(m_SockAddr) ) ;
	if( result )
		return TRUE ; 
	else
		return FALSE ;

	__LEAVE_FUNCTION_FOXNET

	return 0 ;
}

BOOL Socket::bind( UINT port )
{
	__ENTER_FUNCTION_FOXNET

	m_Port = port ;

	m_SockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	m_SockAddr.sin_port        = htons( m_Port);

	BOOL result = SocketAPI::bind_ex( m_SocketID , (const struct sockaddr *)&m_SockAddr , sizeof(m_SockAddr) );
	if( result )
		return TRUE ;
	else
		return FALSE ;

	__LEAVE_FUNCTION_FOXNET

	return 0 ;
}

BOOL Socket::listen( INT backlog )
{
	__ENTER_FUNCTION_FOXNET
		
	return SocketAPI::listen_ex( m_SocketID , backlog );

	__LEAVE_FUNCTION_FOXNET

	return FALSE ;
}

BOOL Socket::isReuseAddr ()const
{
	__ENTER_FUNCTION_FOXNET 
		
	INT reuse;
	UINT len = sizeof(reuse);
	
	SocketAPI::getsockopt_ex( m_SocketID , SOL_SOCKET , SO_REUSEADDR , &reuse , &len );
	
	return reuse == 1;
	
	__LEAVE_FUNCTION_FOXNET

	return 0 ;
}

BOOL Socket::setReuseAddr ( BOOL on )
{
	__ENTER_FUNCTION_FOXNET 

	INT opt = on == TRUE ? 1 : 0;
	
	return SocketAPI::setsockopt_ex( m_SocketID , SOL_SOCKET , SO_REUSEADDR , &opt , sizeof(opt) );

	__LEAVE_FUNCTION_FOXNET

	return TRUE ;
}

void Socket::setHost( const char* szIp )
{
	strncpy( m_Host, szIp, IP_SIZE-1 ) ;
}

bool Socket::ReadyConnect()
{
	return setNonBlocking()==TRUE 
		&& getSockError()==0 
		&& setLinger(0)==TRUE;
}



void Socket::log( const char* szInfo )
{
	PRINT("%s",szInfo);
}

void Socket::setSOCKET( SOCKET sock )
{
	m_SocketID = sock;
}

//BOOL Socket::canWrite( int checkTime )
//{    
//	fd_set		mFdSet;     
//	timeval				mTimeVal;  
//
//	mTimeVal.tv_sec = 0;
//	mTimeVal.tv_usec = checkTime;
//
//	FD_ZERO(&mFdSet);
//	FD_SET(m_SocketID, &mFdSet);
//
//	int num = select((int)m_SocketID+1, NULL, &mFdSet, NULL, &mTimeVal);
//	return num>0 && FD_ISSET(m_SocketID, &mFdSet);
//
//	//struct timeval timeout; 
//	//fd_set r;          
//	//FD_ZERO(&r);
//	//FD_SET(m_SocketID,&r); 
//	//timeout.tv_sec=0;    
//	//timeout.tv_usec=100;
//	//int retval = select(m_SocketID+1,NULL, &r, NULL,&timeout);
//	//if(retval==-1)
//	//{
//	//	perror("select");
//	//	return 0;
//	//}
//	//else if(retval == 0)
//	//{
//	//	fprintf(stderr,"timeout/n");
//	//	return 0;
//	//}
//	//return TRUE;
//}


