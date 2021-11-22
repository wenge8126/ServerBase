#pragma once

#ifndef _INCLUDE_DBDEFINE_H_
#define _INCLUDE_DBDEFINE_H_

//-------------------------------------------------------------------------
#define DB_CODE_VER							"2.00.00"

#	define DB_HASH_SLOT_COUNT								(30000)				// NOTE: short 最大65535/2, DB 集群哈希槽数量 (1个节点分配100, 整数内, 可以分配1000000, 可分配200个节点) 

#	define HASH_SLOT(x)			((ushort)(x % DB_HASH_SLOT_COUNT))
#	define STRHASH_SLOT(sz)		(CRC::crc16((unsigned char*)sz, strlen(sz)) % DB_HASH_SLOT_COUNT)
#	define STRHASH_SLOT(sz)		(CRC::crc16((unsigned char*)sz, strlen(sz)) % DB_HASH_SLOT_COUNT)
#	define ASTR_HASH_SLOT(str) (CRC::crc16((str.c_str(), str.length()) % DB_HASH_SLOT_COUNT)

#define DBIP				"DBIP"
#define DBPORT		"DBPORT"
#define DBUSER		"DBUSER"
#define DBPASSWORD	"DBPASSWORD"
#define DBBASE		"DBBASE"
#define TABLE_LIST	"TABLE_LIST"
#define BACK_SQL	"BACK_SQL"
#define DBPACKET_SIZE "DBPACKET_SIZE"

#define COOLTIME	"COOLTIME"
#define ISLOCALDB	"LOCAL_DB"
#define DB_CODE		"DB_CODE"
#define MAIN_NODE_IP "MAIN_NODE_IP"
#define MAIN_NODE_PORT  "MAIN_NODE_PORT"
#define SHARE_MEM_KEY   "SHARE_MEM_KEY"
#define SHARE_MEM_SIZE  "SHARE_MEM_SIZE"
#define CREATE_TABLE_UNIT_TYPE_INDEX			"CREATE_TABLE_UNIT_TYPE_INDEX"		// DB在网络云上创建唯一索引表格单元类型的单元(type=10000, ID=0), [表格类型从 10001开始]
#define USE_KEY_VALUDE_DB			"USE_KEY_VALUDE_DB"

#define SHARE_MEM_NET_KEY			"SHARE_MEM_NET_KEY"
#define SHARE_MEM_NET_BUFFER	"SHARE_MEM_NET_BUFFER"
#define SHARE_SQL_KEY					"SHARE_SQL_KEY"
#define SQL_COUNT						"SQL_COUNT"

#define INCREMENT						"INCREMENT"
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------

#define SHARE_MEMORY_TABLE			"ShareMem"
#define SHARE_MEM_INDEX_TABLE		"SHARE_MEM_INDEX_TABLE"
#define SHARE_MEM_DATA_TABLE		"SHARE_MEM_DATA_TABLE"
#define COOL_MEMORY_TABLE				"COOL_MEMORY_TABLE"
#define COOL_INDEX_TABLE					"COOL_INDEX_TABLE"
#define CLOUD_INDEX_TABLE				"CLOUD_INDEX_TABLE"

#define IS_SHAREMEM_TABLE(strType)  (strType == SHARE_MEMORY_TABLE || strType==SHARE_MEM_INDEX_TABLE || strType==SHARE_MEM_DATA_TABLE)

#define DBDATA_FIELD "DBDATA_FIELD"
#define DBDATA			-10

#define DBNICE_FIELD "DBNICE_FIELD"
#define DBNICE			-11

enum SHARE_MEM_USE_STATE
{
	eShareMemory_Free = 0,					// 未使用
	eShareMemory_Used = 1,					// 使用中
	eShareMemory_Updating = 2,			// 更新中(SQL线程)
	eShareMemory_Modifying = 3,			// 修改中(DB线程)
};

//-------------------------------------------------------------------------
struct ShareUpdateInfo
{
	uint mAlreadyUpdateCount = 0;		// 业务有修改时加1
	uint mShareUpdateCount = 0;		// 业务进程请求更新,完成落地后加1 , 最后根据这两个值是否一至,确定是否最后落地
};

enum BACK_DB_MSG_TASK
{
	eDBMsgTask_Serialize = 1,
	eDBMsgTask_Restore = 2,
	eDBMsgTask_ZipData = 3,
};

//-------------------------------------------------------------------------

#endif //_INCLUDE_DBDEFINE_H_