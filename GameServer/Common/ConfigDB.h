// Auto generate by table tool
#ifndef _CONFIGDB_H_
#define _CONFIGDB_H_

#include "BaseTable.h"
#include "BaseMemoryDB.h"

#include "ResourceIndex.h" 
#include "WindowConfig.h" 
#include "ErrorCode.h" 


using namespace NetCloud;

class ConfigDB
{
public:
	static bool mbInitSet;
	static bool InitReady(TableMap &mgr)
	{
		AutoTable t;
		bool bResult = true;
			t = mgr.find("ResourceIndex");
			if (t && ResourceIndex::InitCheck(t, mbInitSet)) { } else { ERROR_LOG("ResourceIndex config check fail"); bResult = false; }
			t = mgr.find("WindowConfig");
			if (t && WindowConfig::InitCheck(t, mbInitSet)) { } else { ERROR_LOG("WindowConfig config check fail"); bResult = false; }
			t = mgr.find("ErrorCode");
			if (t && ErrorCode::InitCheck(t, mbInitSet)) { } else { ERROR_LOG("ErrorCode config check fail"); bResult = false; }
		return  bResult;
	}

	static void Clear()
	{
		ResourceIndex::table.setNull();
		WindowConfig::table.setNull();
		ErrorCode::table.setNull();

	}
public:
};



#endif
