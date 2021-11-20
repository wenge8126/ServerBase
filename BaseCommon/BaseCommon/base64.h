#ifndef _INCLUDE_BASE64_H_
#define _INCLUDE_BASE64_H_

#include <string>
#include "BaseCommon.h"
#include "AutoString.h"
#include "DataStream.h"

std::string BaseCommon_Export base64_encode(unsigned char const* , unsigned int len);
std::string BaseCommon_Export base64_decode(std::string const& s);

class BaseCommon_Export Base64Tool
{
public:
	static AString encode(const char *szString, int len);
	static AString decode(const char *szBase64);
	static void decode(const char *szBase64, DataStream &destData);

	static std::string UrlDecode(const std::string & str);
};

#endif //_INCLUDE_BASE64_H_