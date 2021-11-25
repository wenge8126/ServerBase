#pragma once

#include "NetConfig.h"

#include "ShareDBLib.h"

#if __LINUX__
#define  STATE_SHAREMEMCLOUDDB
#endif

#ifndef ShareCloudDB_Export
#	ifdef STATE_SHAREMEMCLOUDDB
#		define ShareCloudDB_Export
#		define ShareCloudDB_Export_H	
#	else
#		ifdef SHAREMEMCLOUDDB_EXPORTS
#			define ShareCloudDB_Export __declspec(dllexport)
#			define ShareCloudDB_Export_H	__declspec(dllexport)
#		else
#			define ShareCloudDB_Export __declspec(dllimport)
#			define ShareCloudDB_Export_H	
#		endif
#endif

#define RECORD_KEY		"_key_"			// 记录操作时, 参数对应的记录key值
#define UNIT_KEY			"_SCR_UNIT_ID_"

enum
{
	PACKET_REQUEST_DBOPERATE = PACKET_MAX + 1,
	PACKET_RESPONSE_DBOPERATE,
	PACKET_REQUEST_DBSAVE,
	PACKET_RESPONSE_DBSAVE,

	PACKET_CLOUD_REQUSTDB,
	PACKET_CLOUD_RESPONSEDB,
	PACKET_CLOUD_RESPONSE_RESULT,
	PACKET_CLOUD_RESPONSE_RESULT_PART,
	 
	PACKET_CLOUD_REQUEST_EVENT,
	PACKET_CLOUD_RESPONSE_EVENT,
	PACKET_CLOUD_RESULT_EVENT,
};

enum eDBResultType
{
	eNoneError,
	eRecordCreateFail,
	eRecordNotInRange,
	eRecordRestoreDataFail,
	eRecordKeyError,
	eRecordKeyRangeError,
	eRecordAlreadyExist,
	eRecordNoExist,
	eRecordNoExistInNode,
	eRecordNotNeedUpdate,
	eRecordDataNotEligible, //10
	eTableNoExist,
	eSourceCodeError,
	eRecordDeleteFail,
	eDBNodeNoExist,
	eSourceCodeNotFinish,
	eTableIndexNull,
	eCreateTableFail,
	eNoSetNewTable,
	eNoSetRequestRecord,
	eRecordSaveToUpdateDataFail, //20
	eFieldCheckFail,
	eFieldNoExist,
	eFieldNotSet,
	eDataTypeNoSame,		// 数据类型不一至
	eDBOperateResultFail,
	eDBOperateNoSet,
	eDBOperateNoExist,
	eDBOperateParamNoSet,
	eDBOperateParamError,
	eDefaultNotSet,				//30
	eRecordGrowthCreateFail,
	eIndexNoExist,
	eRequestOverTime,
	eWaitFinish,
	eRecordCountOverLimit,
	eUnknowError,
	eDBDisconnect,
	eDBWaitTimeOver,
	eOperateObjectNoExist,
	eSystemArgsError,			// 40
	eDBMaintain,  // DB维护, 更新中
	eDBErrorMax,
};

#endif
