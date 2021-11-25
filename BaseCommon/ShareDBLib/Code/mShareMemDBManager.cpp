#include "mShareMemDBManager.h"
#include "MySqlDBTool.h"
#include "DBTable.h"
#include "mNewCreateShareMemFieldIndex.h"
#include "mShareMemMsgEvent.h"
#include "DBDefine.h"

namespace NetCloud
{

	//-------------------------------------------------------------------------

	void ShareMemDBManager::RegisterShareMemMsg(Logic::tEventCenter *pCenter)
	{
		EX_REGISTER_EVENT(pCenter, "DM_ReqeustDBInfo", DM_ReqeustDBInfo);
		EX_REGISTER_EVENT(pCenter, "DM_CreateRecord", DM_CreateRecord);
		EX_REGISTER_EVENT(pCenter, "MD_NotifyNewAppendShareMem", Logic::tBaseNetEvent);
		EX_REGISTER_EVENT(pCenter, "DM_CreateTable", DM_CreateTable);
		EX_REGISTER_EVENT(pCenter, "DM_NotifyDeleteRecord", DM_NotifyDeleteRecord);
		REGISTER_EVENT(pCenter, DM_NotifyCreateUpdateShareMemory);
	}

	void InitTableList(AutoTable listTable)
	{
		listTable->AppendField("ID", FIELD_STRING)->setMaxLength(64);
		listTable->AppendField("DATA", FIELD_STRING)->setMaxLength(6 * 1024);
		listTable->AppendField("SLOT", FIELD_DATA);
		listTable->AppendField("DB_INFO", FIELD_STRING)->setMaxLength(6 * 1024);
		listTable->AppendField("LAST_KEY", FIELD_UINT64);   // 用于Nice记录自增
		listTable->ReadyField();
	}

	AString NetCloud::ShareMemDBManager::InitLoad(NiceData &initParam)
	{
		mInitParam.clear(true);
		mInitParam.append(initParam, true);
		
		if (!mMySQLTool->InitStart(initParam))
		{ 
			AString error;
			error.Format("DB ready fail >%s\r\n%s", mMySQLTool->getErrorMsg(), initParam.dump().c_str());
			return error;
		}

		AString tableList = initParam.get(TABLE_LIST);
		if (tableList.empty())
		{
			AString error;
			error.Format("DB 启动失败: 需要配置TABLE_LIST, 列表名称, 用于工作的数据表格列表,DB数据库");
			return error;
		}

		DBTable *pDBTable = MEM_NEW DBTable();
		mTableListDBTable = pDBTable;
		mTableListDBTable->SetTableName(tableList.c_str());
		// 调取列表
		pDBTable->Init(mMySQLTool, tableList.c_str());
		if (!pDBTable->LoadDB(true, true))
		{			
			WARN_LOG("[%s] Load table list fail >%s, now new create", tableList.c_str(), mMySQLTool->getErrorMsg());
			
			InitTableList(mTableListDBTable);

			if (!mTableListDBTable->GetField()->check() || !pDBTable->CreateToDB())
			{
				AString err;
				err.Format("List field error or Create table list fail >%s", mMySQLTool->getErrorMsg());
				return err;
			}
		}

		// 初始备份库的列表
		AutoNice backSQL = (tNiceData*)initParam.get(BACK_SQL);
		if (backSQL)
		{
			mBackDBMySQL = MEM_NEW MySqlDBTool();

			if (!mBackDBMySQL->InitStart(*backSQL))
			{
				AString info;
				info.Format("Init back sql fail : %s\r\n%s", mBackDBMySQL->getErrorMsg(), backSQL->dump().c_str());
				return info;
			}

			DBTable *pDBTable = MEM_NEW DBTable();
			mBackTableListTable = pDBTable;
			mBackTableListTable->SetTableName(tableList.c_str());
			// 调取列表
			pDBTable->Init(mBackDBMySQL, tableList.c_str());
			if (!pDBTable->LoadDB(true, true))
			{
				WARN_LOG("[%s] Load table list fail >%s, now new create", tableList.c_str(), mMySQLTool->getErrorMsg());

				InitTableList(mBackTableListTable);

				if (!mBackTableListTable->GetField()->check() || !pDBTable->CreateToDB())
				{
					AString err;
					err.Format("List field error or Create table list fail >%s", mBackDBMySQL->getErrorMsg());
					return err;
				}
			}
		}

		AString info = LoadAllDBTable();
		int key = initParam[SHARE_MEM_NET_KEY];
		int buffSize = initParam[SHARE_MEM_NET_BUFFER];
		if (!StartNet(key, buffSize))
		{
			AString err;
			err.Format("\r\nStart share mem net fail");
			info += err;
		}
		return info;
	}

	AString NetCloud::ShareMemDBManager::LoadAllDBTable()
	{
		AString returnInfo;
		for (auto it = mTableListDBTable->GetRecordIt(); *it; ++(*it))
		{
			ARecord tableInfoRe = it->Record();
			AString tableName = tableInfoRe->getIndexData();
			AShareMemTableData tableData = InitCreateTableData(tableName, tableInfoRe["DATA"].string(), returnInfo);
			if (tableData)
			{				
				returnInfo += "\r\n";
				returnInfo += tableData->LoadAllRecord(mMySQLTool.getPtr());
			}
			else
			{
				AString info;
				info.Format("\r\n[%s] Create DB table data fail", tableName.c_str());
				returnInfo += info;
			}
		}
		if (returnInfo.length() <= 0)
			returnInfo = "Succeed : none DB table";
		return returnInfo;
	}

	NetCloud::AShareMemTableData NetCloud::ShareMemDBManager::InitCreateTableData(const AString &tableName, const AString &strTableInfoData, AString &resultInfo)
	{
		NiceData tableInfo;
		if (!tableInfo.FullJSON(strTableInfoData))
		{
			ERROR_LOG("Restore table [%s] info json data fail > %s", tableName.c_str(), strTableInfoData.c_str());
			return AShareMemTableData();
		}

		AShareMemTableData tableData = MEM_NEW ShareMemTableData(tableName, this);
		tableData->mType = tableInfo["TABLE_TYPE"];		
		if (!tableData->InitField(tableInfo["FIELD_INFO"].string(), resultInfo) )
		{
			ERROR_LOG("[%s] Init field fail > \r\n%s", tableName.c_str(), tableInfo["FIELD_INFO"].c_str());
			return AShareMemTableData();
		}
		tableData->InitExtParam((tNiceData*)tableInfo["EXT_PARAM"]);
		mTableInfoMap.insert(tableName, tableData);
		return tableData;
	}

	SHARE_PTR NetCloud::ShareMemDBManager::CreateRecord(const AString &tableName, const AString &key)
	{
		AShareMemTableData tableData = mTableInfoMap.find(tableName);
		if (tableData)
		{
			return tableData->NewCreateRecord(key);
		}

		return SHARE_PTR();
	}


	void ShareMemDBManager::Process()
	{
		// 最后关闭再执行一次
		//for (auto it = mTableInfoMap.begin(); it.have(); ++it)
		//{
		//	auto tableData = it.get();
		//	Auto<NewCreateShareMemFieldIndex> pool = tableData->mField;
		//	pool->mRecordMemPool.ProcessRecordDataSave(dynamic_cast<MySqlDBTool*>(mMySQLTool.getPtr()));
		//}
		mShareMemNet->Process();
		


	}

	void ShareMemDBManager::LowProcess()
	{

	}

	SHARE_PTR ShareMemDBManager::GrowthNewRecord(const AString &tableName)
	{
		AShareMemTableData tableData = mTableInfoMap.find(tableName);
		if (tableData)
		{
			return tableData->GrowthNewRecord();
		}

		return SHARE_PTR();
	}

	bool ShareMemDBManager::DeleteRecord(const AString &tableName, UInt64 recordDataPtr, const AString &recordKey)
	{
		AShareMemTableData tableData = mTableInfoMap.find(tableName);
		if (tableData)
		{
			return tableData->DeleteRecord(recordDataPtr, recordKey);
		}
		return false;
	}

	bool ShareMemDBManager::CreateDBTable(const AString &tableName, const AString &tableType, AutoField tableField, AutoNice extParam, AString &info)
	{
		if (mTableListDBTable->GetRecord(tableName))
		{
			info.Format("[%s]表格已经存在, 请先确认是否清除, 或备份后清理", tableName.c_str());
			return false;
		}
		// 检查字段是否符合要求
		tableField->_updateInfo();
		if (!tableField->check())
		{
			info.Format("%s Field check fail", tableName.c_str());
			return false;
		}
		if (IS_SHAREMEM_TABLE(tableType))
		{
			bool bCheck = true;
			for (int i = 0; i < tableField->getCount(); ++i)
			{
				FieldInfo fieldInfo = tableField->getFieldInfo(i);
				if (!fieldInfo->CanUseShareMemory())
				{
					AString err;
					err.Format("\r\n[%s] field %s : type [%s]  can not use share memory", tableName.c_str(), fieldInfo->getName().c_str(), fieldInfo->getTypeString());
					info += err;
					bCheck = false;
				}
			}
			if (!bCheck)
				return false;
		}
		AString fieldData = tableField->ToString();
		if (fieldData.empty())
		{
			info.Format("序列字段数据失败[%s]", tableName.c_str());
			return false;
		}

		// 表格信息组成结构{节点ID分段信息}{字段校验码}{字段信息序列字符串}
		AString keyIDRangeInfo;
		AString indexType = "INT";

		FieldInfo indexField = tableField->getFieldInfo(0);
		if (!indexField)
		{
			AString info;
			info.Format("DB %s init field error, may be has not set field", tableName.c_str());
			ERROR_LOG(info.c_str());
			return info;
		}
		if (indexField->getType() == FIELD_CHAR_STRING)
			indexType = "HASH";

		NiceData tableInfo;
		tableInfo["TABLE_TYPE"] = tableType;
		tableInfo["KEY_TYPE"] = indexType.c_str();		
		tableInfo["FIELD_CODE"] = (int)tableField->GetCheckCode();
		tableInfo["FIELD_INFO"] = fieldData.c_str();
		tableInfo["VER"] = DB_CODE_VER;

		tableInfo["EXT_PARAM"] = extParam.getPtr();

		//if (extParam)
		//{
		//	tableInfo["LOAD_LIMIT_FIELD"] = extParam["LOAD_LIMIT_FIELD"].string();
		//	tableInfo["LOAD_LIMIT_RANGE"] = (int)extParam["LOAD_LIMIT_RANGE"];
		//	tableInfo["ORDER_INDEX"] = (bool)extParam["ORDER_INDEX"];
		//}

		AString tableInfoData = tableInfo.ToJSON();

		if (tableInfoData.empty() || tableInfoData == "")
		{
			AString error;
			error.Format("[%s] 信息转换至JSON字符串失败 >%s", tableInfo.dump().c_str());
			ERROR_LOG(error.c_str());
			return error;
		}

		tableInfo["FIELD_INFO"] = "";
		bool bOk = false;

		// NOTE: 修改使用DBTable直接保存					
		Auto<MySqlDBTool> tool = mMySQLTool;
		if (tableType == SHARE_MEM_DATA_TABLE)
		{
			AutoField tempField = MEM_NEW BaseFieldIndex();
			tempField->FullFromString(fieldData);
			tempField->setField(DBNICE_FIELD, FIELD_NICEDATA, tempField->getCount())->setMaxLength(8*1024*1024);	// 设置为最大16M长度
			tempField->setField(DBDATA_FIELD, FIELD_DATA, tempField->getCount())->setMaxLength(16*1024*1000);
			tempField->_updateInfo();

			bOk = tool->CreateDBTable(tableName.c_str(), tempField, tableInfo.ToJSON().c_str());
			if (mBackDBMySQL)
			{
				Auto<MySqlDBTool> t = mBackDBMySQL;
				if (!t->CreateDBTable(tableName.c_str(), tempField, tableInfo.ToJSON().c_str()))
					ERROR_LOG("Back DB create table fail %s : %s", tableName.c_str(), t->getErrorMsg());
			}
		}
		else
		{
			bOk = tool->CreateDBTable(tableName.c_str(), tableField, tableInfo.ToJSON().c_str());
			if (mBackDBMySQL)
			{
				Auto<MySqlDBTool> t = mBackDBMySQL;
				if (!t->CreateDBTable(tableName.c_str(), tableField, tableInfo.ToJSON().c_str()) )
					ERROR_LOG("Back DB create table fail %s : %s", tableName.c_str(), t->getErrorMsg());
			}
		}

		if (!bOk)
		{
			info.Format("[%s] Create DB table fail", tableName.c_str());
			return false;
		}
		// 创建MYSQL索引
		AString strIndexList;
		if (extParam)
			strIndexList = extParam["INDEX_FIELD"];
		if (strIndexList.length() > 0)
		{
			StringArray strList;
			AString::Split(strIndexList.c_str(), strList, " ", 1000);
			AString sqlList;
			for (int i = 0; i < strList.size(); ++i)
			{
				sqlList.Format("CREATE INDEX _index_%s ON `%s` (%s)", strList[i].c_str(), tableName.c_str(), strList[i].c_str());
				if (!tool->exeSql(sqlList, false))
					ERROR_LOG("MYSQL [%s] create index fail >%s", tableName.c_str(), tool->getErrorMsg());
			}
		}

		ARecord re = mTableListDBTable->CreateRecord(tableName, false);
		if (!re)
		{
			info.Format("Append %s to list DB fail", tableName.c_str());
			return false;
		}
		re["DATA"] = tableInfoData;

		re->FullAllUpdate(true);
		Auto<DBTable> t = mTableListDBTable;
		if (!t->SaveTable(true))
		{
			info.Format("ERROR: [%s] Save db list fail >%s", tableName.c_str(), t->GetDBTool()->getErrorMsg());
			NOTE_LOG(info.c_str());
			return false;
		}

		if (mBackTableListTable)
		{
			ARecord re = mBackTableListTable->CreateRecord(tableName, true);
			if (!re)
			{
				info.Format("Append %s to back list DB fail", tableName.c_str());
				return false;
			}
			re["DATA"] = tableInfoData;

			re->FullAllUpdate(true);
			Auto<DBTable> t = mBackTableListTable;
			if (!t->SaveTable(true))
			{
				info.Format("ERROR: [%s] Save back db list fail >%s", tableName.c_str(), t->GetDBTool()->getErrorMsg());
				NOTE_LOG(info.c_str());
				return false;
			}
		}

		AShareMemTableData tableData = InitCreateTableData(tableName, tableInfoData, info);
		if (tableData && tableData->mField->CheckSame(tableField->GetCheckCode()))
		{
			info += "\r\n";
			info += tableData->LoadAllRecord(mMySQLTool.getPtr());
			return true;
		}
		else
		{
			info.Format("\r\n[%s] Create DB table data fail, or field restor fail", tableName.c_str());
			return false;
		}
	}

	void ShareMemDBManager::NotifyNewAppendShareMem(const AString &tableName, int key, int arrayIndex)
	{
		if (mShareMemNet->GetClientConnect()->IsDisconnect())
		{
			WARN_LOG("当前无连接DB，忽略通知增加共享内存");
			return;
		}

		AutoEvent evt = mShareMemNet->GetClientConnect()->StartEvent("MD_NotifyNewAppendShareMem");
		evt["TABLE_NAME"] = tableName;
		evt["SHARE_KEY"] = key;
		evt["ARRAY_INDEX"] = arrayIndex;

		evt->Start();
	}

	bool ShareMemDBManager::InitDBUpdate(int shareKey)
	{
		if (mSQLKey != 0)
		{
			ERROR_LOG("SQL update already init %d, can not init agin by %d", mSQLKey, shareKey);
			return false;
		}



		mSQLKey = shareKey;

		return true;
	}


	ShareMemDBManager::ShareMemDBManager()
	{
		mMySQLTool = MEM_NEW MySqlDBTool();
		ShareMemSQLNet *pNet = MEM_NEW ShareMemSQLNet();
		mShareMemNet = pNet;
		pNet->mpDBMgr = this;

		RegisterShareMemMsg(mShareMemNet->GetEventCenter());
	}



	 

}