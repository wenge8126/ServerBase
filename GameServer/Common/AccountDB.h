// Auto generate by table tool
#ifndef _ACCOUNTDB_H_
#define _ACCOUNTDB_H_

#include "BaseTable.h"
#include "BaseMemoryDB.h"

#include "g_global_resourse.h" 
#include "t_account.h" 
#include "t_rmb_wx.h" 
#include "g_server_list.h" 
#include "g_global_config.h" 
#include "t_black_list.h" 
#include "t_rmb_feed_back.h" 
#include "t_white_list.h" 
#include "t_code.h" 
#include "t_level_feed_back.h" 


using namespace NetCloud;

class AccountDB
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
			t = mgr.find("t_rmb_wx");
			if (t && t_rmb_wx::InitCheck(t, mbInitSet)) { } else { ERROR_LOG("t_rmb_wx config check fail"); bResult = false; }
			t = mgr.find("g_server_list");
			if (t && g_server_list::InitCheck(t, mbInitSet)) { } else { ERROR_LOG("g_server_list config check fail"); bResult = false; }
			t = mgr.find("g_global_config");
			if (t && g_global_config::InitCheck(t, mbInitSet)) { } else { ERROR_LOG("g_global_config config check fail"); bResult = false; }
			t = mgr.find("t_black_list");
			if (t && t_black_list::InitCheck(t, mbInitSet)) { } else { ERROR_LOG("t_black_list config check fail"); bResult = false; }
			t = mgr.find("t_rmb_feed_back");
			if (t && t_rmb_feed_back::InitCheck(t, mbInitSet)) { } else { ERROR_LOG("t_rmb_feed_back config check fail"); bResult = false; }
			t = mgr.find("t_white_list");
			if (t && t_white_list::InitCheck(t, mbInitSet)) { } else { ERROR_LOG("t_white_list config check fail"); bResult = false; }
			t = mgr.find("t_code");
			if (t && t_code::InitCheck(t, mbInitSet)) { } else { ERROR_LOG("t_code config check fail"); bResult = false; }
			t = mgr.find("t_level_feed_back");
			if (t && t_level_feed_back::InitCheck(t, mbInitSet)) { } else { ERROR_LOG("t_level_feed_back config check fail"); bResult = false; }
		return  bResult;
	}

	static void Clear()
	{
		g_global_resourse::table.setNull();
		t_account::table.setNull();
		t_rmb_wx::table.setNull();
		g_server_list::table.setNull();
		g_global_config::table.setNull();
		t_black_list::table.setNull();
		t_rmb_feed_back::table.setNull();
		t_white_list::table.setNull();
		t_code::table.setNull();
		t_level_feed_back::table.setNull();

	}
public:
};



#endif
