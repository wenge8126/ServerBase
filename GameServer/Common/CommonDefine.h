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

#define GOLBAL_CONFIG_KEY		"1"		// ȫ�����ñ���key, ���������ļ�����
#define GOLBAL_DBLIST_KEY			"0"		// ȫ��DB�����б�, ���ڲ������л���

#define SERVER_RUN_RESOURCE "1"		// �����ļ���Դ

// ʹ��DataBuffer �ظ���Ϣ������, �Ż�����, Login����תʱ���ط����лظ���Ϣ��
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
		// ���첽ִ�������, �ǻ��������лظ�, ���Կ���ʹ�þ�̬����
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
	WEB_SEND_BIG_MSG = 5,   // NOTE: �Է��Ľ���ID��������Ϊ��ID
	WEB_REV_BIG_MSG = 4,
	WEB_REQUST_DB = 6,
};

enum UNIT_TYPE
{
	Unit_Login = 100,
	Unit_DB = 101,
	Actor_WebLogin = 103,  // ֻ�����ṩ�ʺ�web http ��ȡ�������б�, �ʺ�web���� �ĵ�ԪID
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
	LC_EventMsg = 105,      // �������������͵��¼���Ϣ
	CL_EventMsg = 106,      // �ͻ����������͵�����������Ϣ
	CL_RequestResource = 107, // ������Դ
	CL_ReqeustMsg = 108, // �ͻ�������, Login�ظ�
	LC_RequestEventMsg = 109, // ����ͻ�����Ϣ�¼�, ��JS���߹��ܲ���(JSzip)
	CL_CodeActive = 110, // �����뼤��
	CL_TestLogin = 111, // ѹ�����Ե�½
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
	eStateBase = 1,     // ֻ���ػ���������״̬��Ϣ
	eStateAll = 2,      // ���е�, ����DB
	eStateOnly = 3,     // ֻ��������״̬(����, ����, ����)
	eStateDB = 4,       // ֻ����DB״̬
};

enum PayServerType
{
	ePayFormal = 1,	//��ʽ
	ePayTestServer = 2, //����
	ePayTiShen = 3, //ƻ������
};

#endif //_INCLUDE_COMMONDEFINE_H_