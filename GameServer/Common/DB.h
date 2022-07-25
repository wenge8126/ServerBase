// Auto generate by table tool
#ifndef _DB_H_
#define _DB_H_

#include "BaseTable.h"
#include "BaseMemoryDB.h"

#include "t_account.h" 
#include "t_serverlist.h" 


using namespace NetCloud;

class DB
{
public:
	static bool mbInitSet;
	static bool InitReady(TableMap &mgr)
	{
		AutoTable t;
		bool bResult = true;
			t = mgr.find("t_account");
			if (t && t_account::InitCheck(t, mbInitSet)) { } else { ERROR_LOG("t_account config check fail"); bResult = false; }
			t = mgr.find("t_serverlist");
			if (t && t_serverlist::InitCheck(t, mbInitSet)) { } else { ERROR_LOG("t_serverlist config check fail"); bResult = false; }
		return  bResult;
	}

	static void Clear()
	{
		t_account::table.setNull();
		t_serverlist::table.setNull();

	}
public:
};



#endif
