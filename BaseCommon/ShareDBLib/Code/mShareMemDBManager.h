#pragma once

#ifndef _INCLUDE_SHAREMEMDBMANAGER_H_
#define _INCLUDE_SHAREMEMDBMANAGER_H_

#include "IndexDBRecord.h"
#include "ShareDBLib.h"
#include "ArrayIndex.h"
#include "ShareMemAO.h"
#include "ShareDBLib.h"
#include "mShareMemTableData.h"
#include "DBTool.h"
#include "NetHandle.h"

#include "AutoPtr.h"
//-------------------------------------------------------------------------
// 1 ��SQL�ж�ȡ��¼
// 2 �½���¼
// 3 �����ڴ�DB������
// �����ڴ��¼��DB�����޸�, �˴���ȡ���
// ʹ�ü�¼������ֵ ����ͬ�� (eShareMemory_Used:�޲���, eShareMemory_Updating : ��, eShareMemory_Modifying : д)
// DBʹ��Э���첽��������ģʽ (ֻ����DB����Э�̺����н���DB�������޸�)
// ����߳�ʹ�ó��Լ���(��ǰ������, ���Թ�),  Ϊ��, �����ڴ��ʱ��Ӱ����������, ÿ���ڴ������һ���߳̽������(�ʺ�CPU�����϶�Ļ���)
// ʹ�ü�¼�ڴ�ָ��ǰһ���ֽڱ�ʾ ��״̬
//-------------------------------------------------------------------------

namespace Logic
{
	class tEventCenter;
}

namespace NetCloud
{
	//-------------------------------------------------------------------------
	// ���е�DB����ڴ����
	// ��DB���̽���, ������б���ڴ���Ϣ����, �����½���¼�ڴ�
	//-------------------------------------------------------------------------
	class ShareDBLib_Export ShareMemDBManager : public Base<ShareMemDBManager>
	{
	public:
		AString InitLoad(NiceData &initParam);

		// ���������ڴ��������
		bool StartNet(SM_KEY key, DSIZE bufferSize)
		{
			return mShareMemNet->StartNet(STRING((int)key), bufferSize);
		}

		void Close()
		{
			mShareMemNet->StopNet();
			
			mTableInfoMap.clear();
		}

		void RegisterShareMemMsg(Logic::tEventCenter *pCenter);

	public:
		void Process();

		void LowProcess();

		AString LoadAllDBTable();

		// ����DB��Ϣ tablelist �е��ֶ���Ϣ, ��ʼ����DB������
		AShareMemTableData InitCreateTableData(const AString &tableName, const AString &strFieldInfo, AString &resultInfo);

		SHARE_PTR CreateRecord(const AString &tableName, const AString &key);

		SHARE_PTR GrowthNewRecord(const AString &tableName);

		bool DeleteRecord(const AString &tableName, UInt64 recordDataPtr, const AString &recordKey);

		bool CreateDBTable(const AString &tableName, const AString &tableType, AutoField tableField, AutoNice extParam, AString &info);

		void NotifyNewAppendShareMem(const AString &tableName, int key, int arrayIndex);

		AShareMemTableData GetTableData(const AString &tableName)
		{
			return mTableInfoMap.find(tableName);
		}

		bool InitDBUpdate(int shareKey);


	public:
		ShareMemDBManager();
		~ShareMemDBManager()
		{
			
		}


	public:
		typedef AutoPtr<ShareMemAO> AShareMemAO;
		Array<AShareMemAO>									mTempAttchShareMemList;		// ��ʱ���ӷ��Ͷ˶�̬�����Ĺ����ڴ�
		EasyHash<AString, AShareMemTableData>		mTableInfoMap;
		AutoTable															mTableListDBTable;
		AutoTable															mBackTableListTable;				// ���ݿ��DB�б�

		AutoDBTool														mMySQLTool;
		AutoDBTool														mBackDBMySQL;							// ���ݿ��MYSQL
		NiceData															mInitParam;

		AutoNet															mShareMemNet;

		Buffer																mRecordStateData;
		AString																mUpdateTableName;
		AString																mUpdateRecordKey;
		int																	mSQLKey = 0;
	};

}

#endif
