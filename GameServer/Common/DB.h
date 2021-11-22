// Auto generate by table tool
#ifndef _DB_H_
#define _DB_H_

#include "BaseTable.h"
#include "BaseMemoryDB.h"

#include "g_global_resourse.h" 
#include "t_account.h" 
#include "t_player.h" 
#include "g_global_config.h" 


using namespace NetCloud;

class DB
{
public:
	static bool mbInitSet;
	static bool InitReady(TableMap &mgr)
	{
		AutoTable t;
		bool bResult = true;
			t = mgr.find("g_global_resourse");
			if (t && g_global_resourse::InitCheck(t, mbInitSet)) { } else { ERROR_LOG("g_global_resourse config check fail"); bResult = false; }
			t = mgr.find("t_account");
			if (t && t_account::InitCheck(t, mbInitSet)) { } else { ERROR_LOG("t_account config check fail"); bResult = false; }
			t = mgr.find("t_player");
			if (t && t_player::InitCheck(t, mbInitSet)) { } else { ERROR_LOG("t_player config check fail"); bResult = false; }
			t = mgr.find("g_global_config");
			if (t && g_global_config::InitCheck(t, mbInitSet)) { } else { ERROR_LOG("g_global_config config check fail"); bResult = false; }
		return  bResult;
	}

	static void Clear()
	{
		g_global_resourse::table.setNull();
		t_account::table.setNull();
		t_player::table.setNull();
		g_global_config::table.setNull();

	}
public:
};



#endif
