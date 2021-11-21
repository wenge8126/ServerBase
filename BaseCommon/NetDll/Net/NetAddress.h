
#ifndef _INCLUDE_NETADDRESS_H_
#define _INCLUDE_NETADDRESS_H_

#include "AutoString.h"
#include "NetHead.h"
#include "BaseCommon.h"


class Net_Export NetAddress
{

public:
	static UInt64 IP2Num(uint ipNum, int port);

	static AString Num2IP(uint nNumIp);
	static uint IP2Num(const char *szIP);

	static AString DNS2IP(const char *szDNSName);

	static AString GetAddrInfo(UInt64 netKey);

	static bool IsIP(const char *szIp);

	static AString AddressToIP(const char *szDNSAddress);

	static AString GetLocalIP();

	static bool IsLan(const char *szIP);

	static uint IPv6ToIPv4Num(const char *szIPv6);

	static AString IPv6ToIPv4(const char *szIPv6)
	{
		return Num2IP(IPv6ToIPv4Num(szIPv6));
	}


public:
	NetAddress()
		: mIpPort(0)
	{

	}

	NetAddress(const char* szIp, int port);
	NetAddress(uint ip, int port);
	NetAddress(UInt64 other)
		:mIpPort(other) {}

	int GetIp() const { return (int)(mIpPort>>32 & 0xFFFFFFFF); }
	int GetPort() const { return (int)(mIpPort & 0xFFFFFFFF); }

	AString Ip() const;
	AString dump() const;

	operator UInt64 ()
	{
		return mIpPort;
	}
	NetAddress& operator = (UInt64 value) { mIpPort = value; return *this; }

	// for hash
	operator size_t () const
	{
		return (size_t)mIpPort;
	}
	bool operator == (const NetAddress &other) const { return mIpPort == other.mIpPort; }

protected:
	UInt64	mIpPort;
};



#endif //_INCLUDE_NETADDRESS_H_