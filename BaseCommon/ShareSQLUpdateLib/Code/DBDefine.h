#pragma once

#ifndef _INCLUDE_DBDEFINE_H_
#define _INCLUDE_DBDEFINE_H_

//-------------------------------------------------------------------------
#define DB_CODE_VER							"2.00.00"

#	define DB_HASH_SLOT_COUNT								(30000)				// NOTE: short ���65535/2, DB ��Ⱥ��ϣ������ (1���ڵ����100, ������, ���Է���1000000, �ɷ���200���ڵ�) 

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
#define CREATE_TABLE_UNIT_TYPE_INDEX			"CREATE_TABLE_UNIT_TYPE_INDEX"		// DB���������ϴ���Ψһ�������Ԫ���͵ĵ�Ԫ(type=10000, ID=0), [������ʹ� 10001��ʼ]
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
	eShareMemory_Free = 0,					// δʹ��
	eShareMemory_Used = 1,					// ʹ����
	eShareMemory_Updating = 2,			// ������(SQL�߳�)
	eShareMemory_Modifying = 3,			// �޸���(DB�߳�)
};

//-------------------------------------------------------------------------
struct ShareUpdateInfo
{
	uint mAlreadyUpdateCount = 0;		// ҵ�����޸�ʱ��1
	uint mShareUpdateCount = 0;		// ҵ������������,�����غ��1 , ������������ֵ�Ƿ�һ��,ȷ���Ƿ�������
};

enum BACK_DB_MSG_TASK
{
	eDBMsgTask_Serialize = 1,
	eDBMsgTask_Restore = 2,
	eDBMsgTask_ZipData = 3,
};

//-------------------------------------------------------------------------

#endif //_INCLUDE_DBDEFINE_H_