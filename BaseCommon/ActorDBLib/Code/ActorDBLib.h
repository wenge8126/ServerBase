#pragma once

#ifndef ActorDBLib_Export
#ifdef STATE_ACTORDB_LIB
#define ActorDBLib_Export
#define ActorDBLib_Export_H	
#else
#		ifdef ACTORDBLIB_EXPORTS
#			define ActorDBLib_Export __declspec(dllexport)
#			define ActorDBLib_Export_H	__declspec(dllexport)
#		else
#			define ActorDBLib_Export __declspec(dllimport)
#			define ActorDBLib_Export_H	
#		endif
#endif


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