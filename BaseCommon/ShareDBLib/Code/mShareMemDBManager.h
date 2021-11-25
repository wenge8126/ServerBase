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
// 1 从SQL中读取记录
// 2 新建记录
// 3 共享内存DB表格落地
// 共享内存记录在DB进程修改, 此处读取落地
// 使用记录锁数据值 进行同步 (eShareMemory_Used:无操作, eShareMemory_Updating : 读, eShareMemory_Modifying : 写)
// DB使用协程异步阻塞加锁模式 (只有在DB操作协程函数中进行DB操作及修改)
// 落地线程使用尝试加锁(当前锁不了, 即略过),  为了, 增加内存块时不影响其他操作, 每个内存块启动一个线程进行落地(适合CPU核数较多的环境)
// 使用记录内存指针前一个字节表示 锁状态
//-------------------------------------------------------------------------

namespace Logic
{
	class tEventCenter;
}

namespace NetCloud
{
	//-------------------------------------------------------------------------
	// 所有的DB表格内存管理
	// 与DB进程交互, 完成现有表格内存信息交付, 分配新建记录内存
	//-------------------------------------------------------------------------
	class ShareDBLib_Export ShareMemDBManager : public Base<ShareMemDBManager>
	{
	public:
		AString InitLoad(NiceData &initParam);

		// 开启共享内存网络服务
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

		// 根据DB信息 tablelist 中的字段信息, 初始创建DB表数据
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
		Array<AShareMemAO>									mTempAttchShareMemList;		// 临时附加发送端动态增长的共享内存
		EasyHash<AString, AShareMemTableData>		mTableInfoMap;
		AutoTable															mTableListDBTable;
		AutoTable															mBackTableListTable;				// 备份库的DB列表

		AutoDBTool														mMySQLTool;
		AutoDBTool														mBackDBMySQL;							// 备份库的MYSQL
		NiceData															mInitParam;

		AutoNet															mShareMemNet;

		Buffer																mRecordStateData;
		AString																mUpdateTableName;
		AString																mUpdateRecordKey;
		int																	mSQLKey = 0;
	};

}

#endif
