#pragma once

#ifndef _INCLUDE_SYSYTEMINFO_H_
#define _INCLUDE_SYSYTEMINFO_H_

#include "NetHead.h"
#include <wtypes.h>
#include "NiceData.h"

class Net_Export  SystemInfo
{
public:
	static int CpuProcess();
	static bool SystemMem(size_t &totalSize, size_t &activeSize);
	static size_t ProcessMemory();

	static bool DiskSpaceInfo(const char *szDiskDrive, size_t &totalSize, size_t &freeSize);

	// ��ȡ���������в���, ��ʽ param_name=string �м��Կո����
	static int ReadCommandLineParam(const char *szCommandLine, tNiceData &paramList);

	// �Ƴ����������в���	
	static AString RemoveCommandLineParam(const char *szCommandLine);

	// ׷�������в���
	static AString AppendCommandLineParam(const char *szCommonLine, tNiceData &paramList);
};



#endif //_INCLUDE_SYSYTEMINFO_H_
