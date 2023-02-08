// Auto generate by table tool
#ifndef _CONFIGDB_H_
#define _CONFIGDB_H_

#include "TableManager.h"

#include "GiftConfig.h" 
#include "ResourceIndex.h" 
#include "WindowConfig.h" 
#include "ErrorCode.h" 
#include "GlobalValue.h" 

class ConfigDB
{
public:
	static bool mbInitSet;
	static bool InitReady(TableMap &mgr)
	{
		AutoTable t;
		bool bResult = true;
			t = mgr.find("GiftConfig");
			if (!t) ERROR_LOG("GiftConfig config table no exist");
			if (t && GiftConfig::InitCheck(t, mbInitSet)) { } else { ERROR_LOG("GiftConfig config check fail"); bResult = false; }
			t = mgr.find("ResourceIndex");
			if (!t) ERROR_LOG("ResourceIndex config table no exist");
			if (t && ResourceIndex::InitCheck(t, mbInitSet)) { } else { ERROR_LOG("ResourceIndex config check fail"); bResult = false; }
			t = mgr.find("WindowConfig");
			if (!t) ERROR_LOG("WindowConfig config table no exist");
			if (t && WindowConfig::InitCheck(t, mbInitSet)) { } else { ERROR_LOG("WindowConfig config check fail"); bResult = false; }
			t = mgr.find("ErrorCode");
			if (!t) ERROR_LOG("ErrorCode config table no exist");
			if (t && ErrorCode::InitCheck(t, mbInitSet)) { } else { ERROR_LOG("ErrorCode config check fail"); bResult = false; }
			t = mgr.find("GlobalValue");
			if (!t) ERROR_LOG("GlobalValue config table no exist");
			if (t && GlobalValue::InitCheck(t, mbInitSet)) { } else { ERROR_LOG("GlobalValue config check fail"); bResult = false; }
		return  bResult;
	}

	static void Clear()
	{
		GiftConfig::table.setNull();
		ResourceIndex::table.setNull();
		WindowConfig::table.setNull();
		ErrorCode::table.setNull();
		GlobalValue::table.setNull();

	}
public:
};



#endif
