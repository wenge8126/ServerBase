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

	// 读取命令行所有参数, 格式 param_name=string 中间以空格隔开
	static int ReadCommandLineParam(const char *szCommandLine, tNiceData &paramList);

	// 移除命令行所有参数	
	static AString RemoveCommandLineParam(const char *szCommandLine);

	// 追加命令行参数
	static AString AppendCommandLineParam(const char *szCommonLine, tNiceData &paramList);
};



#endif //_INCLUDE_SYSYTEMINFO_H_
