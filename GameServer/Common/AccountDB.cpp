#include "AccountDB.h"

	bool AccountDB::mbInitSet = true;
	AutoTable g_global_resourse::table;
	AutoTable t_account::table;
	AutoTable t_rmb_wx::table;
	AutoTable g_server_list::table;
	AutoTable g_global_config::table;
	AutoTable t_black_list::table;
	AutoTable t_rmb_feed_back::table;
	AutoTable t_white_list::table;
	AutoTable t_code::table;
	AutoTable t_level_feed_back::table;
