// Auto generate by table tool
#ifndef _MEMDB_H_
#define _MEMDB_H_

#include "BaseTable.h"
#include "BaseMemoryDB.h"

#include "t_account.h" 
#include "t_data.h" 


using namespace NetCloud;

class MemDB
{
public:

	static bool InitReady(EasyHash<AString, AutoTable> &mgr)
	{
		AutoTable t;
		bool bResult = true;
			t = mgr.find("t_account");
			if (t && t_account::InitCheck(t)) { } else { ERROR_LOG("t_account config check fail"); bResult = false; }
			t = mgr.find("t_data");
			if (t && t_data::InitCheck(t)) { } else { ERROR_LOG("t_data config check fail"); bResult = false; }
		return  bResult;
	}

	static void Clear()
	{
		t_account::table.setNull();
		t_data::table.setNull();

	}
public:
};



#endif
