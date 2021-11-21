#include "NetAddress.h"

#define _USE_SOCKET_API_		1

#ifdef __LINUX__
#	include <netdb.h>
#	include <arpa/inet.h>
#else 
#	include "winsock2.h"
#endif

//AString NetAddress::Num2IP(UInt64 scrNum, int &port, int &gsPort)
//{
//#if _USE_SOCKET_API_
//	in_addr scr;
//#if __LINUX__
//	scr.s_addr = (scrNum >> 32) & 0xFFFFFFFF;
//#else
//	scr.S_un.S_addr = (scrNum>>32) & 0xFFFFFFFF;	
//#endif
//
//	AString re = inet_ntoa(scr);
//
//#else
//	AString re = Num2IP((scrNum>>32) & 0xFFFFFFFF);
//#endif
//	port  = (scrNum & 0xFFFF0000)>>16;	
//	gsPort = scrNum & 0xFFFF;
//
//	return re;
//}

AString NetAddress::Num2IP( uint nNumIp )
{
#if _USE_SOCKET_API_
	in_addr scr;
#if __LINUX__
	scr.s_addr = nNumIp;
#else
	scr.S_un.S_addr = nNumIp;
#endif
	AString re = inet_ntoa(scr);
	return re;
#else
	uint s0 = (nNumIp & 0x000000FF);
	uint s1 = (nNumIp & 0x0000FF00)>>8;
	uint s2 = (nNumIp & 0X00FF0000)>>16;	
	uint s3 = (nNumIp & 0xFF000000)>>24;	

	AString strIp;
	strIp.Format("%u.%u.%u.%u", s0, s1, s2, s3);
	return strIp;
#endif
}

//UInt64 NetAddress::IP2Num(const char *szIP, int port, int gsPort)
//{
//	unsigned int t = port;
//	t = (t<<16) + gsPort;
//#if _USE_SOCKET_API_
//	UInt64 reNum =  inet_addr(szIP);
//#else
//	UInt64 reNum =  IP2Num(szIP); 
//#endif
//	reNum = (reNum<<32) + t;
//	return reNum;
//}

UInt64 NetAddress::IP2Num( uint ipNum, int port )
{

	UInt64 reNum = ipNum;

	reNum = (reNum<<32) + port; 
	return reNum;
}

uint NetAddress::IP2Num( const char *szIP )
{
#if _USE_SOCKET_API_
	return inet_addr(szIP);
#else
	AString ip = szIP;

	Array<AString> ipPart;

	int n = AString::Split(szIP, ipPart, ".", 8);
	if (n==4)
	{
		uint s0 = atoi(ipPart[0].c_str());

		uint s1 = atoi(ipPart[1].c_str());
		s1 = s1 << 8;

		uint s2 = atoi(ipPart[2].c_str());
		s2 = s2 << 16;

		uint s3 = atoi(ipPart[3].c_str());
		s3 = s3 << 24;

		uint resultNum = s0 + s1 + s2 + s3;
		return resultNum;
	}
	else
		return 0;
#endif
}


NetAddress::NetAddress(const char* szIp, int port)
{
	mIpPort = IP2Num(szIp);  
	mIpPort = (mIpPort<<32) + port;
}


NetAddress::NetAddress(uint ip, int port)
{
	mIpPort = ip;
	mIpPort = (mIpPort << 32) + port;
}

AString NetAddress::Ip() const
{
	return Num2IP((mIpPort>>32) & 0xFFFFFFFF);
}


AString NetAddress::dump() const
{
	AString info = GetAddrInfo(mIpPort);
	return info;
}

AString NetAddress::DNS2IP( const char *szDNSName )
{
	AString result;
	struct hostent *hp;
	struct in_addr in;
	struct sockaddr_in local_addr;
	if(!(hp=gethostbyname(szDNSName)))
	{
		result = ("Error dns name");	
	}
	else
	{
		memcpy(&local_addr.sin_addr.s_addr, hp->h_addr, 4);
		in.s_addr=local_addr.sin_addr.s_addr;
		result = inet_ntoa(in);
	}
	return result;
}

AString NetAddress::GetAddrInfo( UInt64 netKey )
{	
	NetAddress addr(netKey);
	AString ip = addr.Ip();
	AString info;
	info.Format("[%s:%d]", ip.c_str(), addr.GetPort());
	return info;
}

bool NetAddress::IsIP(const char *szIp)
{
	int a, b, c, d;
	char temp[100];
	if ((sscanf(szIp, "%d.%d.%d.%d", &a, &b, &c, &d)) != 4)
		return false;
	sprintf(temp, "%d.%d.%d.%d", a, b, c, d);
	if (strcmp(temp, szIp) != 0)
		return false;
	return (a <= 255 && a >= 0) && (b <= 255 && b >= 0) && (c <= 255 && c >= 0) && (d <= 255 && d >= 0);
}

AString NetAddress::AddressToIP(const char *szDNSAddress)
{
	HOSTENT* host = gethostbyname(szDNSAddress);
	if (host == NULL)
	{
		return AString();
	}

	return inet_ntoa(*(in_addr*)*host->h_addr_list);
}

AString NetAddress::GetLocalIP()
{
	char hostname[128];
	int ret = gethostname(hostname, sizeof(hostname));
	if (ret == -1) {
		return "0.0.0.0";
	}
	struct hostent *hent;
	hent = gethostbyname(hostname);
	if (NULL == hent) {
		return "0.0.0.0";
	}
	//直接取h_addr_list列表中的第一个地址h_addr
	return inet_ntoa(*(struct in_addr*)hent->h_addr);
	//int ip = ntohl(((struct in_addr*)hent->h_addr)->s_addr);
	//return Num2IP((uint)ip);
	//int i;
	//for(i=0; hent->h_addr_list[i]; i++) {
	//    uint32_t u = ntohl(((struct in_addr*)hent->h_addr_list[i])->s_addr);
	//    std::cout << u << std::endl;
	//}
	//return 0;
}

bool NetAddress::IsLan(const char *szIP)
{
	int x = TOINT(szIP);
	return x == 192 || x == 10 || x == 127;
}

uint NetAddress::IPv6ToIPv4Num(const char *szIPv6)
{
	StringArray	strList;
	if (AString::Split(szIPv6, strList, ":", 10) >= 8)
	{
		char *str;
		uint x = (uint)strtol(strList[7].c_str(), &str, 16);
		uint b1 = x & 0xFF;
		b1 = b1 << 8;
		b1 += x >> 8;
		uint low = (uint)strtol(strList[6].c_str(), &str, 16);
		uint b2 = low & 0xFF;
		b2 = b2 << 8;
		b2 += low >> 8;

		uint v = (b1 << 16) + b2;

		return v;
	}
	return 0;
}
