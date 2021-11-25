#pragma once

/*/-------------------------------------------------------------------------
DBNode �Ż�
1 DB ֮��, User��DB֮ǰ, ȫ��ʹ���첽�����������ʽ,
2 ȥ��DB User ������ع���(DBNodeManage), �¼��������
3 ȥ�� DataSoure
4 ȥ����ؼ��
5 ������, �� MemSQL�д���, DB����ʹ�ú�, ���ñ�ǩ, SQL����, �����ǩ�����õȴ�ʱ��
6 ���� �䴦������������, ���ټ���������
//------------------------------------------------------------------------*/

#ifndef _SHAREMEMCLOUDDBNODE_H_
#define _SHAREMEMCLOUDDBNODE_H_


#include "ShareMemAPI.h"
#include "ShareMemNet.h"

#include "TaskSystem.h"
#include "ShareMemCloudDB.h"

namespace NetCloud
{
	/*/-------------------------------------------------------------------------
// �ֲ�������ڵ��ϵ�DB
	�κ���������͵�Ԫ�����Կ��ٷ������DB, ��Ч�ṩ�����ƴ洢���� (��ؼ�����)
	1 ��(type)
	ѡ��һ���ڵ�, ���������������, ����ϣKEY�ֲ���������, �����ڵ㿪����, �ܺͼ��뵽����

	DBUint����DBʱ, ����������ò�����, ���ȵ� ���� TableTypeIndexUnit ������Ϣ, Ȼ���ٷ�������
//-------------------------------------------------------------------------*/
	class ShareCloudDB_Export ShareMemCloudDBNode : public Base<ShareMemCloudDBNode>
	{
	public:
		// �첽Э���ڻص�ִ��
		virtual void OnDBReadySucceed() {}


		virtual AutoAny GetAttachData() { return AutoAny(); }


		AutoNice DBRunInfo();

	public:		
		virtual AutoTable CreateTable(const char *szIndex, const char *szType);

		void AsyncRequestDBInfo();

		// �첽�������ڴ���Ϣ�¼�
		AutoEvent AwaitRequestMemDB(const char *szRequestEventName, tNiceData *pParamData, int waitMilSecond);


		AutoEvent StartShareMemMsg(const char *szRequestEventName);

		bool SendToShareMemory(Logic::tEvent *pEvent);
	
	public:
		virtual bool InitDB(tNiceData &initParam) ;


		virtual size_t AvailMemory() { return  90000000; }
		virtual size_t NowUseMemory() { return 100; }
		virtual void OnNotifyMemoryWarn(size_t availSize) {}


		virtual void Process() ;
		virtual void LowProcess(int spaceTime) ;

		virtual void Close() ;

		virtual bool ReadyClose() { mbReadyClose = true; return true; }

		// NOTE: ��Ҫ��Э�̺�����ʹ��
		virtual bool CreateDBTable(const char *szTableName, const char *szType, AutoTable table, AutoNice  extParam) ;

		/*/ �������ñ�񴴽�DB��		
		INT,STRING,STRING,INT,BOOL,STRING,STRING,BOOL
		ID,NAME,TYPE,MAX_LENGTH,IS_INDEX,INFO,SLOT,IS_ORDER
		-1,,,,,,0~99,1
		0,ID,INT64,,,����,,
		1,NAME,FIELD_CHAR_STRING,127,1,,,
		*/
		virtual bool CreateDBTable(const char *szTableName, AutoTable configTable) ;


	public:
		ShareMemCloudDBNode(int dbCode, const char *szCloudNodeIp, int nCloudNodePort, int nCloudSafeCode, bool bRegisterBaseOperate, int initBackTaskThreadNum);
		~ShareMemCloudDBNode()
		{
			mShareMemNet._free();

		}

		virtual void RegisterShareMemMsg(Logic::tEventCenter *pCenter);

		AutoTable GetTable(const AString &tableName) { return mTableList.find(tableName); }

	public:
		bool					mbReadyClose;
		

	protected:
		// �����ڴ�
		AutoNet														mShareMemNet;
		NiceData														mInitParam;
		FastHash<AString, AutoTable>						mTableList;
		
	};

	class ShareMemDBNet : public ShareMemNet
	{
	public:
		ShareMemCloudDBNode *mpDB;
	};
}
#endif //_SHAREMEMCLOUDDBNODE_H_