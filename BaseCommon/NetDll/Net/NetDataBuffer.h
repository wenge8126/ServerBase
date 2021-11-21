#ifndef _INCLUDE_NETDATABUFFER_H_
#define _INCLUDE_NETDATABUFFER_H_


#include "DataBuffer.h"

#include "Common.h"
#include "NetHandle.h"

#include "NetHandle.h"

typedef DataBuffer NetDataBuffer;

// ��Ҫ����ת���ַ�����������ݻ��棬Ŀǰ�����������ݴ��ݣ�ͳһ�����ַ�����ΪUTF8

//class NetDataBuffer : public DataBuffer
//{
//public:
//	NetDataBuffer(DSIZE initSize, tNetHandle *pNet)
//		: DataBuffer(initSize)
//		, mNet(pNet->GetSelf())
//	{
//
//	}
//
//public:
//	virtual bool writeString(const char* scrString)
//	{
//		AssertEx(mNet, "����Ӧ�������� NetDataBuffer ");
//		AString resultString = mNet->OnSendStringConvertCode(scrString);
//		if (resultString)
//		{
//			if ( write((unsigned short)(resultString.length())) )
//				return _write((void*)resultString.c_str(), resultString.length());					
//		}
//		else
//		{
//			unsigned short length = 0;
//			if (scrString!=NULL)
//				length = strlen(scrString);
//			if ( write((unsigned short)(length) ) )
//			{
//				if (length>0)
//					return _write((void*)scrString, strlen(scrString));
//				else
//					return true;
//			}
//		}
//		AssertEx (0, "write to data buffer fail");
//		return false;
//	}
//
//	inline virtual AString readString(void)
//	{
//		AString str;
//		readString(str);
//		return str;
//	}
//
//	inline virtual bool readString(AString &resultString)
//	{
//		AString str;
//		if (readString(str))
//		{
//			resultString = str.c_str();
//			return true;
//		}
//
//		return false;
//	}
//
//
//	//virtual const char* readString(DSIZE &resultLength)
//	//{
//	//	AssertEx(0, "����ֱ��ʹ�ô˹���");
//	//	return NULL;
//	//}
//
//	virtual bool readString(AString &resultString)
//	{
//		unsigned short length = 0;
//		if (!read(length))
//			return false;
//
//		if (length<=0)
//		{
//			resultString.setNull();
//			return true;
//		}
//		
//		AssertEx(mNet, "����Ӧ�������� NetDataBuffer ");
//
//		resultString.resize(length+1);
//		if (!_read(resultString._str(), length))
//			return false;
//
//		AString result = mNet->OnReceiveStringConverCode(resultString.c_str());
//		if (result)
//			resultString = result;
//
//		return true;
//	}
//
//public:
//	AutoNet		mNet;
//};


#endif //_INCLUDE_NETDATABUFFER_H_

