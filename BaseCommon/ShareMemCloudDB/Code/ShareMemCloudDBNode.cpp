#include "ShareMemCloudDBNode.h"

#include "MeshNetNode.h"


#include "ShareMemDBTable.h"
#include "DBDefine.h"
#include "ServerEvent.h"
#include "DBShareMemMsgEvent.h"

#include <process.h>
#include "ShareMemCloudDB.h"

using namespace Logic;

namespace NetCloud
{
	class D2M_ReqeustDBInfo : public tAsyncRequestEvent
	{
	public:
		virtual bool _DoEvent() override
		{
			WaitTime(5);
			return true;
		}
	}; 
	//-------------------------------------------------------------------------
	class _BaseDataOperate : public AutoBase
	{
	public:
		// paramData key : 字段名, value : 字段对应的值, resultData 未保存列表 key : 字段名, value : 错误码 (无字段或类型错误)
		int t_growth_create_record(ShareMemCloudDBNode * pNode, const AString & tableName, AutoNice & paramData, AutoNice & resultData)
		{
			AutoTable table = pNode->GetTable(tableName);
			if (!table)
				return eTableNoExist;

			ARecord re = table->GrowthNewRecord();
			if (!re)
			{
				return eRecordCreateFail;
			}

			resultData[RECORD_KEY] = re[0];

			for (auto it = paramData->begin(); it; ++it)
			{
				if (it.key() == RECORD_KEY)
					continue;
				Data d = re->get(it.key().c_str());
				if (d.empty())
				{
					resultData[it.key()] = eFieldNoExist;
					continue; 
				}
				if (!d.setData(Data(&it.get())))
				{
					resultData[it.key()] = eDataTypeNoSame;
				}
			}

			return eNoneError;
		}

		// paramData key : 字段名, value : 字段对应的值, resultData 未保存列表 key : 字段名, value : 错误码 (无字段或类型错误)
		int t_save_record(ShareMemCloudDBNode * pNode, const AString & tableName, AutoNice & paramData, AutoNice & resultData)
		{
			AutoTable table = pNode->GetTable(tableName);
			if (!table)
				return eTableNoExist;

			ARecord re = table->GetRecord(paramData[RECORD_KEY].string());
			if (!re)
			{
				re = table->CreateRecord(paramData[RECORD_KEY].string(), true);
			}
			for (auto it=paramData->begin(); it; ++it)
			{
				if (it.key()==RECORD_KEY || it.key()== UNIT_KEY)
					continue;
				Data d = re->get(it.key().c_str());
				if (d.empty())
				{
					resultData[it.key()] = eFieldNoExist;
					continue;
				}
				if (!d.setData(Data(&it.get())))
				{
					resultData[it.key()] = eDataTypeNoSame;
				}
			}

			return eNoneError;
		}

		// 获取 记录所有数据
		int t_load_record(ShareMemCloudDBNode * pNode, const AString & tableName, AutoNice & paramData, AutoNice & resultData)
		{
			AutoTable table = pNode->GetTable(tableName);
			if (!table)
				return eTableNoExist;

			ARecord re = table->GetRecord(paramData[RECORD_KEY].string());
			if (!re)
				return eRecordNoExist;

			re->ToNiceData(resultData);

			return eNoneError;
		}

		// paramData key 字段名, value : 保留), resultData key : 字段名, value : 值, 如果没结果, 表示不存在字段
		int t_load_record_data(ShareMemCloudDBNode * pNode, const AString & tableName, AutoNice & paramData, AutoNice & resultData)
		{
			AutoTable table = pNode->GetTable(tableName);
			if (!table)
				return eTableNoExist;

			ARecord re = table->GetRecord(paramData[RECORD_KEY].string());
			if (!re)
				return eRecordNoExist;

			for (auto it = paramData->begin(); it; ++it)
			{
				if (it.key() == RECORD_KEY || it.key()== UNIT_KEY)
					continue;
				Data d = re->get(it.key().c_str());
				if (d.empty())
				{
					continue;
				}
				resultData[it.key()].setData(d);
			}

			return eNoneError;
		}
	};


	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
	ShareMemCloudDBNode::ShareMemCloudDBNode(int dbCode, const char *szCloudNodeIp, int nCloudNodePort, int nCloudSafeCode, bool bRegisterBaseOperate, int initBackTaskThreadNum)
		: mbReadyClose(false)
	{


	}

	void ShareMemCloudDBNode::RegisterShareMemMsg(Logic::tEventCenter *pCenter)
	{
		EX_REGISTER_EVENT(pCenter, "MD_NotifyNewAppendShareMem", MD_NotifyNewAppendShareMem);
		EX_REGISTER_EVENT(pCenter, "DM_ReqeustDBInfo", D2M_ReqeustDBInfo);
		EX_REGISTER_EVENT(pCenter, "DM_CreateRecord", tAsyncRequestEvent);
		EX_REGISTER_EVENT(pCenter, "DM_CreateTable", tAsyncRequestEvent);
		EX_REGISTER_EVENT(pCenter, "DM_NotifyDeleteRecord", Logic::tBaseNetEvent);
		EX_REGISTER_EVENT(pCenter, "DM_NotifyCreateUpdateShareMemory", Logic::tBaseNetEvent);
	}

	void ShareMemCloudDBNode::Process()
	{

		if (mShareMemNet)
			mShareMemNet->Process();

	
	}

	void ShareMemCloudDBNode::LowProcess(int spaceTime)
	{

	}

	void ShareMemCloudDBNode::Close()
	{
		if (mShareMemNet)
			mShareMemNet->StopNet();
		mShareMemNet._free();

		mInitParam.clear();
	}

	bool ShareMemCloudDBNode::CreateDBTable(const char *szTableName, const char *szType, AutoTable table, AutoNice  extParam)
	{

		AutoData fieldData = MEM_NEW DataBuffer(512);
		table->GetField()->saveToData(fieldData.getPtr());

		AutoNice  paramData = MEM_NEW NiceData();
		paramData["TABLE_NAME"] = szTableName;
		paramData["FIELD_DATA"] = fieldData.getPtr();
		paramData["FIELD_CODE"] = (int)table->GetField()->GetCheckCode();

		paramData["TYPE"] = szType;

		paramData["EXT_PARAM"] = extParam.getPtr();

		AutoEvent result = AwaitRequestMemDB("DM_CreateTable", paramData.getPtr(), 10000);
		if (result)
		{
			if (result["RESULT"])
			{
				AutoData memListData = (DataStream*)result["MEM_LIST_DATA"];
				if (memListData)
				{
					Auto<CloudDBTable> newTable = CreateTable(szTableName, szType);
					fieldData->seek(0);
					if (!newTable->GetField()->restoreFromData(fieldData.getPtr()))
					{
						ERROR_LOG("Restore field fail %s", szTableName);
						return false;
					}
					newTable->SetMainIndex(0, !extParam["IS_ORDER"]);
					newTable->ApplyExt(extParam);
					memListData->seek(0);
					Array<int> memArray;
					if (memListData->readArray(memArray))
					{
						if (newTable->RestoreFromShareMem(memArray))
						{
							//ReadyDBTableToCloud(szTableName);
						}
						else
							ERROR_LOG(" %s RestoreFromShareMem fail", szTableName);
					}
					else
						ERROR_LOG("Restore mem list data fail");
					// NOTE_LOG("Succeed create DB table > %s", szTableName);
					return true;
				}
			}
			ERROR_LOG("Create table %s  fail > %s", szTableName, result["ERROR"].c_str());
		}
		else
			ERROR_LOG("Create table %s  request fail > %s", szTableName);

		return false;
	}


	bool ShareMemCloudDBNode::CreateDBTable(const char *szTableName, AutoTable configTable)
	{
		ARecord  infoRe = configTable->GetRecord(-1);

		AutoNice extParam = MEM_NEW NiceData();
		extParam["SLOT"] = infoRe["SLOT"];
		extParam["IS_ORDER"] = infoRe["IS_ORDER"];
		extParam["COOL_SECOND"] = infoRe["COOL_SECOND"];		// 扩展数据冷却时长
		AString type = infoRe["TYPE"];

		AutoTable table = tBaseTable::NewBaseTable();

		AString indexFieldList;

		int i = 0;
		while (true)
		{
			ARecord fieldInfoRe = configTable->GetRecord(i++);
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
				ERROR_LOG("%s Field %s type [%s] error", szTableName, fieldInfoRe["NAME"].c_str(), fieldInfoRe["TYPE"].c_str());
				return false;
			}
		}

		extParam["INDEX_FIELD"] = indexFieldList;

		if (type.length() <= 0)
			type = indexFieldList.empty() ? SHARE_MEMORY_TABLE : SHARE_MEM_INDEX_TABLE;

		return CreateDBTable(szTableName, type.c_str(), table, extParam);

		return false;
	}




	AutoNice ShareMemCloudDBNode::DBRunInfo()
	{
		AutoNice info = MEM_NEW NiceData();

		for (auto it = mTableList.begin(); it; ++it)
		{
			Auto<CloudDBTable>t = it.get();
			if (t)
			{
				info[t->GetTableName()] = t->RunInfo();				
			}
			else
			{
				AutoTable baseTable = it.get();
				if (baseTable)
					info[baseTable->GetTableName()] = baseTable->GetTableType();
			}
		}
		return info;
	}

	AutoTable ShareMemCloudDBNode::CreateTable(const char *szIndex, const char *szType)
	{


		CloudDBTable *p = NULL;

			p = MEM_NEW ShareMemDBTable(this);


		if (p != NULL)
		{
			p->SetTableName(szIndex);
			mTableList.insert(szIndex, p);
		}
		return p;
	}

	void ShareMemCloudDBNode::AsyncRequestDBInfo()
	{


		// 一直尝试初始共享内存网络
		int key = mInitParam[SHARE_MEM_NET_KEY];
		int bufferSize = mInitParam[SHARE_MEM_NET_BUFFER];

		AutoEvent resp;

		// 共享内存进程, 一旦被连接, 就只能被同一个进程使用, 不再接受其他进程ID, 直到重启
		while (true)
		{
			while (true)
			{
				// 连接共享进程
				if (mShareMemNet->StartNet(STRING((int)key), bufferSize))
					break;

				NOTE_LOG("Wait try init share memory msg net [key %d : %d], after 3 second ...\r\n !!! May be need restart ShareMemDB !!!", key, bufferSize);
				tTimer::AWaitTime(3000);

				if (mbReadyClose)
					return;
			}
			 
			//请求获取DB信息, 初始所有DB表格
			NiceData requestParam;

			//NOTE: 使用进程ID, 作为更新KEY, 避免在重启时, KEY重复
			int proID = _getpid();
			requestParam[SHARE_SQL_KEY] = proID; // (int)mInitParam[SHARE_SQL_KEY];


			NOTE_LOG("Wait request DB info from share memory, Try 5 second ...");
			resp = AwaitRequestMemDB("DM_ReqeustDBInfo", &requestParam, 6000);
			if (resp && resp["RESULT"])
				break;
			
			if (resp)
				ERROR_LOG("DM_ReqeustDBInfo error : %s", resp["ERROR"].string().c_str());

			LOG("Wait init share memory 3 second ...");
			tTimer::AWaitTime(3000);
			if (mbReadyClose)
				return;
		}

		AutoNice sqlParam = (tNiceData*)resp["_SQL_PARAM_"];
		if (!sqlParam)
		{
			ERROR_LOG("Fail get sql param from share memory");
			AssertNote(0, "Fail get sql param from share memory");
			return;
		}
		NOTE_LOG("get SQL param >\r\n%s", sqlParam->dump().c_str());
		mInitParam.append(*sqlParam, true);


		AutoData  listData = (DataStream*)resp["MEM_LIST_DATA"];
		if (!listData)
		{
			ERROR_LOG("%s response MEM_LIST_DATA is NULL", resp->GetEventName());
			return;
		}

		AString info = "\r\n-------------------------------------------------------------";

		int tableCount = 0;
		listData->seek(0);
		listData->read(tableCount);
		for (int i = 0; i < tableCount; ++i)
		{
			AString tableName, type;
			listData->readString(tableName);
			listData->readString(type);
			Auto<CloudDBTable> table = CreateTable(tableName.c_str(), type.c_str());
			if (!table)
			{
				ERROR_LOG("Create table fail :  %s, type %s", tableName.c_str(), type.c_str());
				continue;
			}
			if (!table->GetField()->restoreFromData(listData.getPtr()))
			{
				ERROR_LOG("%s Restore field fail", tableName.c_str());
				break;
			}

			AutoNice extData = MEM_NEW NiceData();
			if (!extData->restore(listData.getPtr()))
			{
				ERROR_LOG("%s Restore ext param fail", tableName.c_str());
				break;
			}
			table->SetMainIndex(0, !extData["IS_ORDER"]);
			table->ApplyExt(extData);

			Array<int> memArray;
			if (listData->readArray(memArray))
			{
				AString str;
				if (table->RestoreFromShareMem(memArray))
				{
					str.Format("\r\nDB [%s] succeed init load count %d", tableName.c_str(), table->GetRecordCount());
					//ReadyDBTableToCloud(tableName.c_str());
				}
				else
				{
					str.Format("\r\nX: DB [%s] load record fail", tableName.c_str());
				}
				info += str;
			}
			else
			{
				ERROR_LOG("%s Read memory key array fail", tableName.c_str());
				break;
			}
		}
		info += "\r\n-------------------------------------------------------------\r\n";
		NOTE_LOG(info.c_str());

		// 不使用KEY整数方式优化, DBNice字段直接绑在导出的结构, 直接以NiceData序列保存 2020.11.2
		//// 使用IDNiceData进行优化, DBNice字段, 默念就是IDNiceData
		//// ID string key index table
		//AString idIndexTableName = "_id_index_table_";
		//AutoTable idIndexTable = GetTable(idIndexTableName);
		//if (!idIndexTable)
		//{
		//	idIndexTable = tBaseTable::NewBaseTable(false);
		//	idIndexTable->AppendField("ID", FIELD_INT);
		//	idIndexTable->AppendField("KEY", FIELD_CHAR_STRING)->setMaxLength(127);
		//	idIndexTable->GetField()->_updateInfo();
		//	idIndexTable->SetMainIndex(0, true, false);
		//					
		//	CreateDBTable( idIndexTableName.c_str(), SHARE_MEM_INDEX_TABLE, idIndexTable, AutoNice());
		//	idIndexTable = GetTable(idIndexTableName);
		//}
		//if (idIndexTable)
		//	TableManager::getSingleton().mStringKeyIndex.InitKeyTable(idIndexTable);

		OnDBReadySucceed();

	}

	AutoEvent ShareMemCloudDBNode::AwaitRequestMemDB(const char *szRequestEventName, tNiceData *pParamData, int waitMilSecond)
	{
		Hand< tAsyncRequestEvent> requestEvt = mShareMemNet->GetClientConnect()->StartEvent(szRequestEventName);
		if (!requestEvt)
		{
			ERROR_LOG("%s No register request event", szRequestEventName);
			return AutoEvent();
		}

		if (pParamData != NULL)
			requestEvt->AppendData(*pParamData, true);
		return requestEvt->AwaitRequest(waitMilSecond);
	}

	AutoEvent ShareMemCloudDBNode::StartShareMemMsg(const char *szRequestEventName)
	{
		return mShareMemNet->GetClientConnect()->StartEvent(szRequestEventName);
	}

	bool ShareMemCloudDBNode::SendToShareMemory(Logic::tEvent *pEvent)
	{
		return mShareMemNet->GetClientConnect()->SendEvent(pEvent);
	}

	bool ShareMemCloudDBNode::InitDB(tNiceData &initParam)
	{
		mInitParam.append(initParam, true);

		// 初始共享内存网络
		ShareMemDBNet *pNet = MEM_NEW ShareMemDBNet();
		mShareMemNet = pNet;
		pNet->mpDB = this;

		RegisterShareMemMsg(mShareMemNet->GetEventCenter());

		// 异步连接共享内存进程并获取DB信息
		ASYNC(&ShareMemCloudDBNode::AsyncRequestDBInfo, this);

		return true;
	}






}

