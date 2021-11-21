#include "SystemInfo.h"
#include "Common.h"

#if __LINUX__

bool SystemInfo::CpuProcess()
{
	return false;
}

bool SystemInfo::SystemMem(size_t &useMem, size_t &activePhys)
{
	return false;
}
#elif __WINDOWS__
#	include <Windows.h>
#	include "psapi.h"

#include <winternl.h>

#pragma  comment(lib,"Psapi.lib")

typedef struct _UINT64_DELTA
{
	ULONG64 Value;
	ULONG64 Delta;
} UINT64_DELTA, *PUINT64_DELTA;

typedef struct _UINTPTR_DELTA
{
	ULONG_PTR Value;
	ULONG_PTR Delta;
} UINTPTR_DELTA, *PUINTPTR_DELTA;

typedef struct {
	LARGE_INTEGER liKeBootTime;
	LARGE_INTEGER liKeSystemTime;
	LARGE_INTEGER liExpTimeZoneBias;
	ULONG uCurrentTimeZoneId;
	DWORD dwReserved;
} SYSTEM_TIME_INFORMATION;

typedef long(__stdcall *fnNtQuerySystemInformation)(
	IN  UINT SystemInformationClass,
	OUT PVOID SystemInformation,
	IN  ULONG SystemInformationLength,
	OUT PULONG ReturnLength OPTIONAL);

//static fnNtQuerySystemInformation _NtQuerySystemInformation = NULL;

#define InitializeDelta(DltMgr) \
	((DltMgr)->Value = 0, (DltMgr)->Delta = 0)

#define UpdateDelta(DltMgr, NewValue) \
	((DltMgr)->Delta = (NewValue) - (DltMgr)->Value, \
	(DltMgr)->Value = (NewValue), (DltMgr)->Delta)

int GetCpuUsage()
{
	ULONG64 total_time = 0;
	ULONG64 sys_time = 0;
	static SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION CpuInformation[1024] = { 0 };
	static SYSTEM_INFO sys_info;

	static UINT64_DELTA cpu_kernel_delta;
	static UINT64_DELTA cpu_user_delta;
	static UINT64_DELTA cpu_idle_delta;
	static SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION cpu_totals;
	memset(&cpu_totals, 0, sizeof(SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION));

	GetSystemInfo(&sys_info);

	static fnNtQuerySystemInformation _NtQuerySystemInformation = (fnNtQuerySystemInformation)GetProcAddress(LoadLibrary("ntdll.dll"), "NtQuerySystemInformation");

	_NtQuerySystemInformation(
		SystemProcessorPerformanceInformation,
		&CpuInformation,
		sizeof(SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION) * (ULONG)sys_info.dwNumberOfProcessors,
		NULL
	);
	NOTE_LOG(" cpu %d ------------------------------------", (int)sys_info.dwNumberOfProcessors);
	for (int i = 0; i < (int)sys_info.dwNumberOfProcessors; i++)
	{
		SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION& cpu_info = CpuInformation[i];

		// KernelTime includes idle time.
		LONGLONG dpc_time = cpu_info.Reserved1[0].QuadPart;
		LONGLONG interrupt_time = cpu_info.Reserved1[i].QuadPart;
		cpu_info.KernelTime.QuadPart -= cpu_info.IdleTime.QuadPart;
		cpu_info.KernelTime.QuadPart += dpc_time + interrupt_time;

		cpu_totals.Reserved1[0].QuadPart += dpc_time;
		cpu_totals.IdleTime.QuadPart += cpu_info.IdleTime.QuadPart;
		cpu_totals.Reserved2 += cpu_info.Reserved2;
		cpu_totals.Reserved1[1].QuadPart += cpu_info.Reserved1[1].QuadPart;
		cpu_totals.KernelTime.QuadPart += cpu_info.KernelTime.QuadPart;
		cpu_totals.UserTime.QuadPart += cpu_info.UserTime.QuadPart;

		LONGLONG total = cpu_info.KernelTime.QuadPart + cpu_info.UserTime.QuadPart + cpu_info.IdleTime.QuadPart;
		NOTE_LOG(" cpu %d : %.2f", (int)sys_info.dwNumberOfProcessors, (total - cpu_info.IdleTime.QuadPart) * 100 / total);
	}
	NOTE_LOG("-------------------------------------------");

	UpdateDelta(&cpu_kernel_delta, cpu_totals.KernelTime.QuadPart);
	UpdateDelta(&cpu_user_delta, cpu_totals.UserTime.QuadPart);
	UpdateDelta(&cpu_idle_delta, cpu_totals.IdleTime.QuadPart);

	total_time = cpu_kernel_delta.Delta + cpu_user_delta.Delta + cpu_idle_delta.Delta;
	sys_time = cpu_idle_delta.Delta;
	int cpu = 0;
	if (total_time)
	{
		cpu = (int)(100 - sys_time * 100 / total_time);//注意此处使用 的110为CPU上线的数值为 110
		if (cpu < 0) {
			cpu = 0;
		}
		if (cpu > 100) {
			cpu = 100;
		}
		return cpu;
	}
	else
	{
		return 0;
	}
	return 0;
}

int SystemInfo::CpuProcess()
{	
	return GetCpuUsage();
}


bool SystemInfo::SystemMem(size_t &totalSize, size_t &activePhys)
{
	MEMORYSTATUSEX sysMemory;
	sysMemory.dwLength = sizeof(sysMemory);
	if (::GlobalMemoryStatusEx(&sysMemory) == FALSE)
		return false;
	activePhys = sysMemory.ullAvailPhys;
	
	totalSize = sysMemory.ullTotalPhys;

	return true;
}

size_t SystemInfo::ProcessMemory()
{
	PROCESS_MEMORY_COUNTERS memoryUseInfo;

	if (GetProcessMemoryInfo(GetCurrentProcess(), &memoryUseInfo, sizeof(memoryUseInfo)) == FALSE)
		return false;

	return memoryUseInfo.WorkingSetSize;
}

bool SystemInfo::DiskSpaceInfo(const char *szDiskDrive, size_t &totalSize, size_t &freeSize)
{
	ULARGE_INTEGER nFreeBytesAvailable;

	ULARGE_INTEGER nTotalNumberOfBytes;

	ULARGE_INTEGER nTotalNumberOfFreeBytes;


	if (GetDiskFreeSpaceEx(szDiskDrive, &nFreeBytesAvailable, &nTotalNumberOfBytes, &nTotalNumberOfFreeBytes)==TRUE)
	{

		freeSize =   nFreeBytesAvailable.HighPart;
		freeSize = (freeSize << 32) + nFreeBytesAvailable.LowPart;

		totalSize = nTotalNumberOfBytes.HighPart;
		totalSize = (totalSize << 32) + nTotalNumberOfBytes.LowPart;

		return true;
	}
	return false;
}

int SystemInfo::ReadCommandLineParam(const char *szCommandLine, tNiceData &paramList)
{
	StringArray  strList;
	AString::Split(szCommandLine, strList, " ", 100);
	for (int i=1; i<strList.size(); ++i)
	{
		AString str = strList[i];
		AString key = str.SplitLeft("=");
		// 去掉多余的空格
		key.replace(" ", "");
		str.replace(" ", "");
		if (key.length() > 0 && str.length() > 0)
			paramList[key] = str;
	}
	return paramList.count();
}

AString SystemInfo::RemoveCommandLineParam(const char *szCommandLine)
{
	StringArray  strList;
	AString::Split(szCommandLine, strList, " ", 100);
	for (int i = 1; i < strList.size(); ++i)
	{
		AString str = strList[i];
		AString key = str.SplitLeft("=");
		// 找到第一个参数开始的位置, 返回开始位置之前的字符串
		if (key.length() > 0 && str.length() > 0)
		{
			key += "=";
			const char *sz = strstr(szCommandLine, key.c_str());
			if (sz != NULL)
			{
				AString result;
				result.set(szCommandLine, (DSIZE)(sz - szCommandLine-1));
				return result;
			}
		}
	}
	return szCommandLine;
}

AString SystemInfo::AppendCommandLineParam(const char *szCommonLine, tNiceData &paramList)
{
	AString strCommand = szCommonLine;
	for (auto it = paramList.begin(); it; ++it)
	{
		AString info;
		info.Format(" %s=%s", it.key().c_str(), ((AString)it.get()).c_str());
		strCommand += info;
	}
	return strCommand;
}

//-------------------------------------------------------------------------
#endif