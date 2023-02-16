#include "DBTableManager.h"
#include "ServerEvent.h"
#include "IOCPServerNet.h"
#include "DBTable.h"
#include "Timer.h"
#include "CacheDBTable.h"
#include "DBIndexTable.h"

using namespace Logic;

namespace NetCloud
{

	//-------------------------------------------------------------------------


	void DBTableManager::InitTableList(AutoTable listTable)
	{
		listTable->AppendField("ID", FIELD_STRING)->setMaxLength(64);
		listTable->AppendField("DATA", FIELD_STRING)->setMaxLength(6 * 1024);
		listTable->AppendField("SLOT", FIELD_DATA);
		listTable->AppendField("DB_INFO", FIELD_STRING)->setMaxLength(6 * 1024);
		listTable->AppendField("LAST_KEY", FIELD_UINT64);   // ����Nice��¼����
		listTable->ReadyField();
	}

	AutoTable DBTableManager::InitCreateTable(const AString &tableName, const AString &strTableInfoData, AString &resultInfo)
	{
		NiceData tableInfo;
		if (!tableInfo.FullJSON(strTableInfoData))
		{
			ERROR_LOG("Restore table [%s] info json data fail > %s", tableName.c_str(), strTableInfoData.c_str());
			return AutoTable();
		}

		Auto<LogicDBTable> table;
		int second = NeedCacheTable(tableName);
		if (second > 0)
			table = MEM_NEW CacheDBTable(mbUseShareSQL, second);
		else if (second < 0)
			table = MEM_NEW DBIndexTable(mbUseShareSQL);
		else
			table = MEM_NEW LogicDBTable(mbUseShareSQL);

		table->SetTableName(tableName.c_str());
		bool b = table->GetField()->FullFromString(tableInfo["FIELD_INFO"].string());
		table->GetField()->_updateInfo();
		if (b && table->GetField()->check())
		{
			table->mSQLParam = mInitParam;
			table->ApplyExt((tNiceData*)tableInfo["EXT_PARAM"]);

			table->mpDB = this;

			mTableList.insert(tableName, table);
			return table;
		}
		else
		{
			ERROR_LOG("[%s] Init field fail > \r\n%s", tableName.c_str(), tableInfo["FIELD_INFO"].c_str());
			return AutoTable();
		}
	}

	AString DBTableManager::LoadAllDBTable()
	{
		LOG("----------------------------------------");
		AString returnInfo;
		for (auto it = mTableListDBTable->GetRecordIt(); *it; ++(*it))
		{
			ARecord tableInfoRe = it->Record();
			AString tableName = tableInfoRe->getIndexData();
			AutoTable table = InitCreateTable(tableName, tableInfoRe["DATA"].string(), returnInfo);
			if (table)
			{
				LOG("Init table %s", tableName.c_str());
			}  
			else
			{
				AString info;
				info.Format("\r\n[%s] Create DB table data fail", tableName.c_str());
				returnInfo += info;
			}
		}
		LOG("----------------------------------------");
		return returnInfo;
	}

	AString DBTableManager::InitLoadDB()
	{
		AString tableList = mInitParam->get(TABLE_LIST);
		if (tableList.empty())
		{
			AString error;
			error.Format("DB ����ʧ��: ��Ҫ����TABLE_LIST, �б�����, ���ڹ��������ݱ���б�,DB���ݿ�");
			return error;
		}

		AutoDBTool sqlTool = MEM_NEW MySqlDBTool();
		if (!sqlTool->InitStart(*mInitParam))
		{
			AString error;
			error.Format("Mysql init fail :\r\n %s\r\n%s", sqlTool->getErrorMsg(), mInitParam->dump().c_str());
			return error;
		}

		DBTable *pDBTable = MEM_NEW DBTable();
		mTableListDBTable = pDBTable;
		mTableListDBTable->SetTableName(tableList.c_str());
		// ��ȡ�б�
		pDBTable->Init(sqlTool, tableList.c_str());
		if (!pDBTable->LoadDB(true, true))
		{
			WARN_LOG("[%s] Load table list fail >%s, now new create", tableList.c_str(), sqlTool->getErrorMsg());

			InitTableList(mTableListDBTable);

			if (!mTableListDBTable->GetField()->check() || !pDBTable->CreateToDB())
			{
				AString err;
				err.Format("List field error or Create table list fail >%s", sqlTool->getErrorMsg());
				return err;
			}
		}
		else
		{
			AutoField field = mTableListDBTable->GetField();
			field->getFieldInfo("ID")->setMaxLength(64);
			field->getFieldInfo("DATA")->setMaxLength(6 * 1024);
	
			field->getFieldInfo("DB_INFO")->setMaxLength(6 * 1024);
		}

		return LoadAllDBTable();
	}

	bool DBTableManager::CreateDBTable(const char *szTableName, AutoTable configTable, AString &errorInfo)
	{
		AutoNice extParam;
		AutoTable table = GenerateDBTable(szTableName, configTable, extParam, errorInfo);
		if (!table)
		{
			AString info;
			info.Format("New create DB table [%s] fail >%s", szTableName, errorInfo.c_str());
			errorInfo = info;
			return false;
		}

		if (!CreateDBTable(szTableName, table, extParam))
		{
			AString info;
			info.Format("New create DB table [%s] fail >%s", szTableName, errorInfo.c_str());
			errorInfo = info;
			return false;
		}
		return true;
	}

	bool DBTableManager::CreateDBTable(const AString &tableName, AutoTable table, AutoNice extParam)
	{
		AString info;
		if (mTableListDBTable->GetRecord(tableName))
		{
			info.Format("[%s]����Ѿ�����, ����ȷ���Ƿ����, �򱸷ݺ�����", tableName.c_str());
			return false;
		}
		AutoField tableField = table->GetField();
		// ����ֶ��Ƿ����Ҫ��
		tableField->_updateInfo();
		if (!tableField->check())
		{
			info.Format("%s Field check fail", tableName.c_str());
			return false;
		}

		AString fieldData = tableField->ToString();
		if (fieldData.empty())
		{
			info.Format("�����ֶ�����ʧ��[%s]", tableName.c_str());
			return false;
		}

		// �����Ϣ��ɽṹ{�ڵ�ID�ֶ���Ϣ}{�ֶ�У����}{�ֶ���Ϣ�����ַ���}
		NiceData tableInfo;
		tableInfo["FIELD_CODE"] = (int)tableField->GetCheckCode();
		tableInfo["FIELD_INFO"] = fieldData.c_str();
		tableInfo["VER"] = DB_CODE_VER;

		tableInfo["EXT_PARAM"] = extParam.getPtr();


		AString tableInfoData = tableInfo.ToJSON();

		if (tableInfoData.empty() || tableInfoData == "")
		{
			AString error;
			error.Format("[%s] ��Ϣת����JSON�ַ���ʧ�� >%s", tableInfo.dump().c_str());
			ERROR_LOG(error.c_str());
			return error;
		}

		tableInfo["FIELD_INFO"] = "";


		// NOTE: �޸�ʹ��DBTableֱ�ӱ���					
		Auto<MySqlDBTool> tool = MEM_NEW MySqlDBTool();
		if (!tool->InitStart(*mInitParam))
		{
			ERROR_LOG("SQL init fail : %s\r\n%s", tool->getErrorMsg(), mInitParam->dump().c_str());
			return "SQL init fail";
		}
		
		bool bOk = tool->CreateDBTable(tableName.c_str(), tableField, tableInfo.ToJSON().c_str());		
		if (!bOk)
		{
			info.Format("[%s] Create DB table fail", tableName.c_str());
			return false;
		}

		// ���������ֶ�
		if (extParam->exist(INCREMENT))
		{
			AString keyName = table->GetField()->getFieldInfo(0)->getName();
			AString sql;
			int keyType = table->GetField()->getFieldInfo(0)->getType();
			if (keyType == FIELD_INT)
			{
				sql.Format("alter table `%s`  change `%s` `%s` int auto_increment;  alter table `%s` auto_increment = %s;", tableName.c_str(), keyName.c_str(), keyName.c_str(), tableName.c_str(), extParam[INCREMENT].c_str());
			}
			else
				sql.Format("alter table `%s`  change `%s` `%s` bigint auto_increment;  alter table `%s` auto_increment = %s;", tableName.c_str(), keyName.c_str(), keyName.c_str(), tableName.c_str(), extParam[INCREMENT].c_str());
			
			if (!tool->exeMultiSql(sql, false, NULL))
			{
				info.Format("Set auto_increment key fail :%s\r\n%s", tool->getErrorMsg(), sql.c_str());
				return false;
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
		
		t->SetDBTool(tool);
		if (!t->SaveTable(true))
		{
			info.Format("ERROR: [%s] Save db list fail >%s", tableName.c_str(), t->GetDBTool()->getErrorMsg());
			NOTE_LOG(info.c_str());
			return false;
		}

		AutoTable newTable = InitCreateTable(tableName, tableInfoData, info);
		
		if (!newTable)
		{
			info.Format("\r\n[%s] Create DB table data fail, or field restor fail : %s", tableName.c_str(), info.c_str());
			return false;
		}
		return true;
	}

	AutoTable DBTableManager::ModifyDBTable(const char *szTableName, AutoTable structConfigTable, AString &errorInfo)
	{
		AutoNice extParam;

		AutoTable resultTable = GenerateDBTable(szTableName, structConfigTable, extParam, errorInfo);
		if (!resultTable)
		{
			return AutoTable();
		}

		Auto<LogicDBTable> existTable = GetTable(szTableName);
		if (!existTable)
		{
			errorInfo.Format("Modify table struct fail : No exist table %s", szTableName);
			return AutoTable();
		}
		{
			bool bHaveNeedDelete = false;
			// �Զ�����
			AutoField field = existTable->GetField();

			for (int i = 0; i < field->getCount(); ++i)
			{
				FieldInfo pField = field->getFieldInfo(i);
				if (!resultTable->GetField()->existField(pField->getName().c_str()))
				{
					AString error;
					error.Format("\r\nDB %s find need remove field %s , but now can delete field", szTableName, pField->getName().c_str());
					errorInfo += error;
					bHaveNeedDelete = true;
				}
			}
			if (bHaveNeedDelete)
				return AutoTable();
		}

		{
			bool bHaveAddField = false;
			// �ҳ���Ҫ�����ֶ�
			AutoField field = resultTable->GetField();
			
			for (int i = 0; i < field->getCount(); ++i)
			{
				FieldInfo pField = field->getFieldInfo(i);
				if (!existTable->GetField()->existField(pField->getName().c_str()))
				{
					AString info;
					info.Format("\r\nDB %s find need add field %s ,  Now add field : ", szTableName, pField->getName().c_str());
					errorInfo += info;

					// ִ������
					info.setNull();
					if (!existTable->mDBDataLoadSQL->mMainThreadSQLTool.DBTableAddField(szTableName, field, pField->getName().c_str(), info))
					{
						errorInfo += "\r\n";
						errorInfo += info;
						return AutoTable();
					}
					errorInfo += "\r\n";
					errorInfo += info;

					bHaveAddField = true;
				}
			}

			if (!bHaveAddField)
			{
				errorInfo.Format("[%s] not need add field", szTableName);
				return AutoTable();
			}
			AutoField tableField = resultTable->GetField();
			AString fieldData = tableField->ToString();
			if (fieldData.empty())
			{
				AString info;
				info.Format("\r\n�����ֶ�����ʧ��[%s]", szTableName);
				errorInfo += info;
				return AutoTable();
			}

			// �����Ϣ��ɽṹ{�ڵ�ID�ֶ���Ϣ}{�ֶ�У����}{�ֶ���Ϣ�����ַ���}
			NiceData tableInfo;
			tableInfo["FIELD_CODE"] = (int)tableField->GetCheckCode();
			tableInfo["FIELD_INFO"] = fieldData.c_str();
			tableInfo["VER"] = DB_CODE_VER;

			tableInfo["EXT_PARAM"] = extParam.getPtr();


			AString tableInfoData = tableInfo.ToJSON();

			if (tableInfoData.empty() || tableInfoData == "")
			{
				AString error;
				error.Format("\r\n[%s] ��Ϣת����JSON�ַ���ʧ�� >%s", tableInfo.dump().c_str());
				ERROR_LOG(error.c_str());
				errorInfo += error;
				return AutoTable();
			}			

			// NOTE: �޸�ʹ��DBTableֱ�ӱ���					
			Auto<MySqlDBTool> tool = MEM_NEW MySqlDBTool();
			if (!tool->InitStart(*mInitParam))
			{				
				errorInfo.Format("SQL init fail : %s\r\n%s", tool->getErrorMsg(), mInitParam->dump().c_str());
				return AutoTable();
			}			

			ARecord re = mTableListDBTable->GetRecord(szTableName);
			if (!re)
			{
				AString info;
				info.Format("Append %s to list DB fail", szTableName);
				errorInfo += info;
				return AutoTable();
			}
			re["DATA"] = tableInfoData;

			//re->FullAllUpdate(true);

			Auto<DBTable> t = mTableListDBTable;

			t->SetDBTool(tool);
			if (!t->SaveTable(true))
			{
				AString info;
				info.Format("ERROR: [%s] Save db list fail >%s", szTableName, t->GetDBTool()->getErrorMsg());
				NOTE_LOG(info.c_str());
				errorInfo += info;
				return AutoTable();
			}

		}
		return resultTable;
	}

	AutoTable DBTableManager::GenerateDBTable(const char *szTableName, AutoTable structConfigTable, AutoNice &extParam, AString &errorInfo)
	{
		ARecord  infoRe = structConfigTable->GetRecord(-1);

		extParam = MEM_NEW NiceData();
		extParam["SLOT"] = infoRe["SLOT"];
		extParam["IS_ORDER"] = infoRe["IS_ORDER"];
		extParam["COOL_SECOND"] = infoRe["COOL_SECOND"];		// ��չ������ȴʱ��

		AString type = infoRe["TYPE"];

		AutoTable table = tBaseTable::NewBaseTable();

		AString indexFieldList;

		int i = 0;
		while (true)
		{
			ARecord fieldInfoRe = structConfigTable->GetRecord(i++);
			if (!fieldInfoRe)
				break;

			FieldInfo info = table->AppendField(fieldInfoRe["NAME"].c_str(), fieldInfoRe["TYPE"].c_str());
			if (info != NULL)
			{
				int maxLen = fieldInfoRe["MAX_LENGTH"];
				if (maxLen > 0)
					info->setMaxLength(maxLen);
				if (fieldInfoRe["IS_INDEX"])
				{
					if (!indexFieldList.empty())
						indexFieldList += " ";
					indexFieldList += fieldInfoRe["NAME"].string();
				}
			}
			else
			{
				errorInfo.Format("%s Field %s type [%s] error", szTableName, fieldInfoRe["NAME"].c_str(), fieldInfoRe["TYPE"].c_str());
				return AutoTable();
			}
		}

		extParam["INDEX_FIELD"] = indexFieldList;

		// �����ֶ�
		if (structConfigTable->GetField()->existField(INCREMENT))
		{
			if (infoRe[INCREMENT].string() == "YES")
			{
				ARecord fieldInfoRe = structConfigTable->GetRecord(0);
				if (fieldInfoRe)
				{
					int keyType = table->GetField()->getFieldInfo(0)->getType();
					if (keyType != FIELD_INT && keyType != FIELD_INT64)
					{
						errorInfo.Format("No set 0 Field  is not int or Int64: %s, can not use auto INCREMENT", szTableName);
						return AutoTable();
					}
					extParam[INCREMENT] = fieldInfoRe[INCREMENT];
				}
				else
				{
					errorInfo.Format("No set 0 Field : %s", szTableName);
					return AutoTable();
				}
			}
		}

		if (type.length() <= 0)
			type = indexFieldList.empty() ? SHARE_MEMORY_TABLE : SHARE_MEM_INDEX_TABLE;

		AutoField tableField = table->GetField();
		// ����ֶ��Ƿ����Ҫ��
		tableField->_updateInfo();
		if (tableField->getCount()<=0 || !tableField->check())
		{
			errorInfo.Format("Generate DB table %s Field check fail", szTableName);
			return AutoTable();
		}
		table->SetTableName(szTableName);
		return table;
	}

	//-------------------------------------------------------------------------
	DBTableManager::DBTableManager()
	{
		mShareDBClientNet = MEM_NEW ShareDBClientNet(this);
	}

	void DBTableManager::Process()
	{
		for (auto it = mTableList.begin(); it; ++it)
		{
			Auto<LogicDBTable> t = it.get();
			t->Process();
		}

		mShareDBClientNet->Process();
	}

	void DBTableManager::LowProcess()
	{
		for (auto it = mTableList.begin(); it; ++it)
		{
			Auto<LogicDBTable> t = it.get();
			t->LowProcess();
		}
		mShareDBClientNet->LowProcess(10);
	}

	bool DBTableManager::AsyncInitSQLUpdate(AutoNice sqlParam, const char *szShareDBServerIP, int nShareDBServerPort, int nShareNetSaftCode, int updateShareKey, bool bUseShareSQL)
	{
		mbUseShareSQL = bUseShareSQL;
		mInitParam = sqlParam;

		// �������е�DB���
		AString resultInfo = InitLoadDB();
		if (resultInfo.length() > 0)
		{
			ERROR_LOG("Init load DB fail : %s", resultInfo.c_str());
			return false;
		}

		if (!mbUseShareSQL)
		{
			LOG("Now without share SQL mode");
			return true;
		}


		if (!mSQLUpdate.Init(updateShareKey))
		{
			ERROR_LOG("Init sql update share fail, may be key : %d already used", updateShareKey);
			return false;
		}		
			

		Hand<ShareDBClientNet> net = mShareDBClientNet;
		net->mSafeCode = nShareNetSaftCode;
		
		while (!net->IsConnected() && !mbReadyClose)
		{
			// ���ӹ������
			net->AsyncConnect(szShareDBServerIP, nShareDBServerPort);

			NOTE_LOG("Wait try init share memory msg net [ %s : %d], after 3 second ...\r\n !!! May be need restart ShareMemDB !!!", szShareDBServerIP, nShareDBServerPort);
			tTimer::AWaitTime(3000);
		}

		bool bOk = false;

		while (net->IsConnected() && !mbReadyClose)
		{
			AutoNice requestParam = MEM_NEW NiceData();
			requestParam["SQL"] = mInitParam.getPtr();
			requestParam[SHARE_SQL_KEY] = updateShareKey;

			//�����ȡDB��Ϣ, ��ʼ����DB���
			NOTE_LOG("Wait request DB info from share memory, Try 5 second ...");
			AutoEvent resp = AwaitRequestMemDB(mShareDBClientNet, "DM_ReqeustReadySQLThread", requestParam.getPtr(), 6000);
			if (resp && resp["RESULT"])
			{
				bOk = true;
				break;
			}

			if (resp)
				ERROR_LOG("DM_ReqeustReadySQLThread error : %s", resp["ERROR"].string().c_str());

			LOG("Wait init share memory 3 second ...");
			tTimer::AWaitTime(3000);

		}

		return bOk;
	}

	AutoEvent DBTableManager::AwaitRequestMemDB(AutoNet mShareSQLNet, const char *szRequestEventName, tNiceData *pParamData, int waitMilSecond)
	{
		Hand< tAsyncRequestEvent> requestEvt = mShareSQLNet->GetClientConnect()->StartEvent(szRequestEventName);
		if (!requestEvt)
		{
			ERROR_LOG("%s No register request event", szRequestEventName);
			return AutoEvent();
		}

		if (pParamData != NULL)
			requestEvt->AppendData(*pParamData, true);
		return requestEvt->AwaitRequest(waitMilSecond);
	}
	//-------------------------------------------------------------------------
	ShareDBClientNet::ShareDBClientNet(DBTableManager *pDB) : mpDBManager(pDB)
	{
		mEventCenter = MEM_NEW Logic::EventCenter();

		EX_REGISTER_EVENT(mEventCenter, "DM_ReqeustReadySQLThread", tAsyncRequestEvent);
	}
	//-------------------------------------------------------------------------
}