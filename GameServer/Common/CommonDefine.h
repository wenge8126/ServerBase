#pragma once

#include "NiceData.h"
#include "AnyData.h"


#ifndef _INCLUDE_COMMONDEFINE_H_
#define _INCLUDE_COMMONDEFINE_H_


typedef Int64  ID;

#define TABLE_PLAYER					"t_player"
#define TABLE_ACCOUNT				"t_account"
#define CONFIG_LIST_INDEX			"ConfigList"
#define TEMP_CONFIG_PACKET		"_config_packet_data_"

#define GOLBAL_CONFIG_KEY		"1"		// 全局配置保存key, 用于运行文件更新
#define GOLBAL_DBLIST_KEY			"0"		// 全局DB创建列表, 用于部署运行环境

#define SERVER_RUN_RESOURCE "1"		// 运行文件资源

// 使用DataBuffer 回复消息体数据, 优化序列, Login在中转时不必反序列回复消息体
template<typename T>
struct _ResponseTool_
{
	T  mMsg;
	AutoNice  mResultData;

	_ResponseTool_(AutoNice &resultData)
	{
		mResultData = resultData;
	}

	~_ResponseTool_()
	{
		// 在异步执行这里后, 是会立即序列回复, 所以可以使用静态缓存
		thread_local static AutoData respData = MEM_NEW DataBuffer(1024);
		respData->clear(false);
		if (mMsg.serialize(respData.getPtr()))
			mResultData["DATA"] = respData.getPtr();
		else
			ERROR_LOG("Save response data fail > %s", mMsg.dump().c_str());
	}
};
//#define DEFINE_param_result(req, resp) 	req param; param.Full(paramData); resp *resultMsg = MEM_NEW resp(); resp &result = *resultMsg; AutoNice _resultData = resultMsg; resultData["DATA"] = (tNiceData*)_resultData.getPtr();
//#define DEFINE_result(resp) resp *resultMsg = MEM_NEW resp(); resp &result = *resultMsg; resultData["DATA"] = dynamic_cast<tNiceData*>(resultMsg);
#define DEFINE_param_result(req, resp) 	req param; param.Full(paramData);  _ResponseTool_<resp> _tempTool(resultData); resp &result = _tempTool.mMsg;
//#define DEFINE_result(resp) _ResponseTool_<resp> _tempTool(resultData); resp &result = _tempTool.mMsg;

#define DEFINE_result(resp)	Auto<resp> _resp = MEM_NEW resp(); resultData["__MSG__"] = (tNiceData*)_resp.getPtr(); resp &result = *_resp;


enum LOGIN_RESULT
{
	eOk = 0,
	eUnknowLoginResult = 100,
	eCreateAccountSucceed,
	eLoginSucceed,
	eAccountIsNull,
	ePasswordIsNull,
	eAccountExist,
	eAccountNoExist,
	ePasswordError,
	eDBQueryError,
	eNoLoginState,
	eNullData,
	eMailFull,
	eMailNoPass,
	eIsNotManager,
	eAccountCreateFail,

	eCheckStateSuceed,
	eCheckLoginTimeFail,
	eLoginStateOverTime,
	eCreateDataRecordFail,
};

enum WEB_BASE_MSG
{
	WEB_MSG_RESPONSE_MSG = 1,
	WEB_SEND_BIG_MSG = 5,   // NOTE: 对方的接收ID必须设置为此ID
	WEB_REV_BIG_MSG = 4,
	WEB_REQUST_DB = 6,
};

enum UNIT_TYPE
{
	Unit_Login = 100,
	Unit_DB = 101,
	Actor_WebLogin = 103,  // 只用来提供帐号web http 获取服务器列表, 帐号web分配 的单元ID
	Actor_DBWorker = 104,
	Actor_ToolWorker = 105,
	Actor_ManagerTool = 1000,
	Actor_Account = 1001,
	Actor_GameServer = 1002,
	Actor_Player = 1003,
	Actor_Client = 1004,
};

enum MSG_TYPE
{
	MS_Bigen = 100,
	CL_Login = 101,
	CL_Reginster = 102,
	LC_Notify =104,
	LC_EventMsg = 105,      // 服务器主动发送的事件消息
	CL_EventMsg = 106,      // 客户端主动发送到服务器的消息
	CL_RequestResource = 107, // 请求资源
	CL_ReqeustMsg = 108, // 客户端请求, Login回复
	LC_RequestEventMsg = 109, // 请求客户端消息事件, 如JS工具功能操作(JSzip)
	CL_CodeActive = 110, // 激活码激活
	CL_TestLogin = 111, // 压力测试登陆
};

enum NotifyMsgType
{
	eNotify_UpdateConfig = 1,
};

class ExServerInfoData : public AnyData
{
public:
	int mServerID;
	AString mServerName;
	AString mOpenTime;
};

struct BackDBHead
{
	int mListBegin = 0;
	int mTableInfoBegin = 0;
	int mTableDataBegin = 0;
	UInt64 mAllSize = 0;
};

enum RequestStateType
{
	eStateBase = 1,     // 只返回基本的运行状态信息
	eStateAll = 2,      // 所有的, 包括DB
	eStateOnly = 3,     // 只返回运行状态(正常, 错误, 警告)
	eStateDB = 4,       // 只返回DB状态
};

enum PayServerType
{
	ePayFormal = 1,	//正式
	ePayTestServer = 2, //测试
	ePayTiShen = 3, //苹果提审
};

#endif //_INCLUDE_COMMONDEFINE_H_