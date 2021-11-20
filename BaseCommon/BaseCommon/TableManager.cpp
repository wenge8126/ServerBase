/********************************************************************
	created:	2010/04/29
	created:	29:4:2010   10:38
	filename: 	d:\Work\common\DataBase\TableManager.cpp
	file path:	d:\Work\common\DataBase
	file base:	TableManager
	file ext:	cpp
	author:		杨文鸽
	
	purpose:	表格全局管理对象,可以保存各种表格及创建,且可提供配置读取
*********************************************************************/
//#include "stdafx.h"
#include "BaseTable.h"
#include "TableManager.h"
#include "IndexBaseTable.h"
#include "Common.h"

#ifdef __SERVER__
//#	include "DBTable.h"
#endif

//#ifndef __LINUX__
//	#	include "CSVTable.h"
//#endif

#include <stdarg.h>

#include "FileDataStream.h"
#include "FieldInfo.h"
#include "ConfigTable.h"
#include "ResoursePack.h"
//------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------

TableManager*			TableManager::ms_Singleton = NULL;
//--------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
DataTableManager::DataTableManager()
{

}

AutoTable DataTableManager::GetTable( const char* tableIndex, const char* errorInfo/* = NULL*/ )
{
	auto it = mTableMap.find(tableIndex);
	if (!it)
	{
		DEBUG_LOG("Error :>>> [%s] table no exist", tableIndex);
	}
	return it;
}
//-----------------------------------------------------------------------------------------------------------

AutoTable DataTableManager::CreateNewTable( const char* tableIndex,const char* tableType /*= "NiceTable" */ )
{
	if (NULL==tableType)
		return AutoTable();

	AutoTable	newTable = CreateNewObject(tableType);
	mTableMap.insert(tableIndex,  newTable);

	return newTable;
}
//-----------------------------------------------------------------------------------------------------------

bool DataTableManager::ReleaseTable( const char* tableIndex, bool nowFree )
{
	if (NULL==tableIndex)
		return false;

	AutoTable hT = GetTable(tableIndex);
	if (hT)
	{
		if (nowFree) 
			hT._free();
		mTableMap.erase( tableIndex );
		return true;
	}
	else
		return false;
}


void DataTableManager::LoadTable( AutoTable configTable )
{
	DEBUG_LOG( "开始加载配置表格...\n"
		"====================================================================","" );
	int count = 0;
	//for ( TableIt temp(configTable);temp._Have();temp._Next())
	for (ARecordIt it = configTable->GetRecordIt(); (*it); ++(*it))
	{
		++count;
		AString tableIndex, tableFile, type;
		ARecord configRe = *it;
		tableIndex = configRe->get(1).string();
		//if(!configRe->get(1, tableIndex))
		if (tableIndex=="")
		{
			ERROR_LOG("行[%d]表格索引设置不正确!",count );  
			continue;
		}
		//if(!configRe->get(2,tableFile))
		tableFile = configRe->get(2).string();
		if (tableFile=="")
		{
			ERROR_LOG("[%s]表格类型没有设置!",tableIndex.c_str() );
			continue;
		}
		type = configRe->get(3).string();
		//if(!configRe->get(3,type))
		//{
		//	DEBUG_LOG("[%s]表格类型没有设置!",tableIndex.c_str() );
		//}

		//AutoTable existTable = GetTable(tableIndex.c_str());
		AutoTable currTable;
		if ( IsExists(tableIndex.c_str()) )
			WARN_LOG("[%s]>[%s]配置表格已经存在, 当前覆盖处理", tableIndex.c_str(), tableFile.c_str());
		
		currTable = CreateNewTable( tableIndex.c_str(), type.c_str() );
		
		if ( currTable )
		{			
			AString pathFile = TableManager::GetDefaultPath().c_str();
			pathFile += tableFile;
				
			if (currTable->LoadCSV(pathFile.c_str()))
			{												
				DEBUG_LOG( "成功加载表格[%s][%s], Record count [%d]!", tableIndex.c_str(), tableFile.c_str(), currTable->GetMainIndex()->GetCount() );
			}
			else
				DEBUG_LOG( "加载表格[%s]失败, file [%s]!", tableIndex.c_str(), tableFile.c_str() );
			currTable->SetTableName(tableIndex.c_str());
//			AutoData  fileData = ReadFile(pathFile.c_str());
//			if (!fileData)
//			{
//				DEBUG_LOG( "加载表格[%s]>[%s]失败!",pathFile.c_str() );
//				continue;				
//			}
//			else
//			{
//				fileData->seek(0);
//				if ( currTable->LoadFromData(fileData.getPtr()) )
//				{
//					DEBUG_LOG( "成功加载表格[%s][%s], Record count [%d]!", tableIndex.c_str(), tableFile.c_str(), currTable->GetRecordNum() );
//
//#if DEBUG_SAVE_CONFIG_TABLE
//					AString test = SAVE_DEBUG_PATH;
//					test += tableIndex;
//					test += ".csv";
//					currTable->_SaveToCSV(test.c_str());
//#endif
//				}
//			}
		}
		else
		{
			DEBUG_LOG( "加载表格[%s]失败!",tableFile.c_str() );
		}
		//if (existTable && !existTable->AppendFromTable(currTable))
		//{
		//	DEBUG_LOG( "追加表格失败-->[%s]",tableFile.c_str() );
		//}



		//DEBUG_LOG( "完成加载表格[%s]...\n"
			//"---------------------------------------------------------------------",
			//tableFile.c_str() );
	}
	//ReadyTable(false);
	DEBUG_LOG( "====================================================================\n"
		"完成加载配置表格...\n", "" );
}

bool DataTableManager::LoadTable( const char* configTableFile,const char* type /*= "CSV" */ )
{
	AutoTable tempConfig = CreateNewObject(type);
	if (tempConfig)
	{
		AString pathFile = TableManager::GetDefaultPath().c_str();
		pathFile += configTableFile;

		if ( tempConfig->LoadCSV(pathFile.c_str()) )
		{
			InsertTable(configTableFile, tempConfig, true);
			LoadTable(tempConfig);
			return true;
		}
		ERROR_LOG("Load table [%s] fail", configTableFile);
		//AutoData  fileData = ReadFile(pathFile.c_str());
		//if (!fileData)
		//	return false;
  //      fileData->seek(0);
		//if ( tempConfig->LoadFromData(fileData.getPtr()) )
		//{
		//	LoadTable(tempConfig);
		//	return true;
		//}
	}
	return false;
}

bool DataTableManager::LoadTable(EasyResourcesPack *tablePak, const char *szListIndex, const char *szPakName)
{
	AutoData listData = tablePak->loadResource(szListIndex);
	if (!listData)
	{
		ERROR_LOG("No exist %s table list in table pak %s", szListIndex, szPakName);
		return false;
	}
	listData->seek(0);

	AutoTable listTable = CreateNewTable(szListIndex);
	if (!listTable->LoadFromData(listData.getPtr()))
	{
		ERROR_LOG("Fail load %s table list in table pak %s", szListIndex, szPakName);
		return false;
	}
	bool bOk = true;
	for (auto it = listTable->GetRecordIt(); *it; ++(*it))
	{
		ARecord record = it->Record();
		AString tableName = record["INDEX"];

		AutoData listData = tablePak->loadResource(tableName.c_str());
		if (!listData)
		{
			bOk = false;
			ERROR_LOG("No exist config table data >%s in table pak %s", tableName.c_str(), szPakName);
			continue;
		}
		listData->seek(0);

		AutoTable table = CreateNewTable(tableName.c_str());
		if (!table->LoadFromData(listData.getPtr()))
		{
			bOk = false;
			ERROR_LOG("Config %s load fail in table pak %s", tableName.c_str(), szPakName);
		}
	}
	return bOk;
}

bool DataTableManager::InsertTable( const char* tableIndex,AutoTable hTable,bool bReplace )
{
	auto it = mTableMap.find(tableIndex);
	if (it)
	{	
		if ( !bReplace )
			return false;
	}

	mTableMap.insert(tableIndex, hTable);
	//mTableMap.OptimizeSpeed(3);	
	return true;
}

void DataTableManager::ReadyTable(bool bChangeIDIndex)
{
	//mTableMap.OptimizeSpeed(11.5f);
	
	if (bChangeIDIndex)
	{
		for (auto it=mTableMap.begin(); it; ++it)
		{
			AutoTable currTable = it.get();
			if (currTable)
			{
				// 优化字段
				currTable->GetField()->_optimizeNameHash(4);
				//if (currTable->GetField()->getFieldInfo(currTable->GetMainIndexCol())->getType()!=FIELD_STRING)
				//{
				//	// 转为使用数组ID索引，快速查找记录
				//	int maxID = 0;
				//	if (!currTable->ModifyIDIndex(CONFIG_LIMIT_MAX_ID, maxID))
				//		WARN_LOG("[%s] Config table change id index fail, may be max id [%d] >%d", currTable->GetTableName(), maxID, CONFIG_LIMIT_MAX_ID);
				//}	
			}
		}
	}
	//for (size_t i=0; i<mTableMap.size(); ++i)
	//{
	//	AutoTable hT = mTableMap.get(i);
	//	if (hT)
	//		hT->Ready(this);
	//}
}
//-----------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------
void TableManager::SetLog( SaveTableLog *tableLog )
{
	TableTool::SetLog(tableLog);
}
//-----------------------------------------------------------------------------------------------------------

void TableManager::SetDefaultPath( const char* path )
{
	TableManager::getSingleton().mDefaultPath = path;
}
//-----------------------------------------------------------------------------------------------------------

const AString& TableManager::GetDefaultPath(void)
{
	return TableManager::getSingleton().mDefaultPath;
}

void TableManager::FreeAll()
{
	delete TableManager::getSingletonPtr();
	//NiceDataField::FreeAllFieldInfo();
	FieldInfoManager::FreeAllFieldInfo();
}

TableManager::TableManager()
{
	AssertEx( !ms_Singleton, "already exist singleton object");
	ms_Singleton = this;

	//FieldInfoManager::GetMe().InitFieldInfo();
	//NiceDataField::getMe().InitFieldInfo();
}

TableManager::~TableManager()
{
	AssertEx( ms_Singleton, "no exist singleton object");
	ms_Singleton = NULL;
	
}

//-----------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------

void DataTableManager::Upate( void* param )
{
	//for (size_t i=0; i<mTableMap.size(); ++i)
	//{
	//	AutoTable hT = mTableMap.get(i);
	//	if (hT)
	//		hT->Update(param);
	//}
}

ARecord DataTableManager::GetRecord( const char* tableIndex, const char* recordIndex )
{
	AutoTable hT = GetTable(tableIndex);
	if (hT)
	{
		return hT->GetRecord(recordIndex);
	}
	WARN_LOG("WARN: [%s] record no exist int table [%s]", tableIndex, tableIndex);
	return ARecord();
}

ARecord DataTableManager::GetRecord( const char* tableIndex, int nRecordIndex )
{
	AutoTable hT = GetTable(tableIndex);
	if (hT)
		return hT->GetRecord(nRecordIndex);
	WARN_LOG("WARN: [%d] record no exist int table [%s]", nRecordIndex, tableIndex);
	return ARecord();
}

Data DataTableManager::GetData( const char* tableIndex, const char* recordIndex, const char* fieldName )
{
	ARecord hR = GetRecord(tableIndex, recordIndex);
	if (hR)
	{
		Data d = hR->get(fieldName);
		if (!d.empty())
			return d;
		else
			ERROR_LOG("WARN: [%s] field no exist int table [%s]", fieldName, tableIndex);
	}

	return Data();
}

Data DataTableManager::GetData( const char* tableIndex, int recordIndex, const char* fieldName )
{
	ARecord hR = GetRecord(tableIndex, recordIndex);
	if (hR)
	{
		Data d = hR->get(fieldName);
		if (!d.empty())
			return d;
		else
			ERROR_LOG("WARN: [%s] field no exist int table [%s]", fieldName, tableIndex);
	}
	return Data();
}

Data DataTableManager::GetData(const char* tableIndex, const char* recordIndex, int field)
{
	ARecord hR = GetRecord(tableIndex, recordIndex);
	if (hR)
	{
		Data d = hR->get(field);
		if (!d.empty())
			return d;
		else
			ERROR_LOG("WARN: [%d] field no exist int table [%s]", field, tableIndex);
	}

	return Data();
}

Data DataTableManager::GetData(const char* tableIndex, int recordIndex, int field)
{
	ARecord hR = GetRecord(tableIndex, recordIndex);
	if (hR)
	{
		Data d = hR->get(field);
		if (!d.empty())
			return d;
		else
			ERROR_LOG("WARN: [%d] field no exist int table [%s]", field, tableIndex);
	}
	return Data();
}

void DataTableManager::Log( const char *szInfo, ... )
{
#if DEVELOP_MODE
	va_list va;
	va_start(va, szInfo);
	TableTool::Log(va, szInfo);
#endif
}

AutoData DataTableManager::ReadFile( const char *szPathFileName )
{
	AutoData  tempFile = MEM_NEW FileDataStream(szPathFileName, FILE_READ);
	if (!tempFile->empty())
	{
		return tempFile;
	}
	return AutoData();
}

//-------------------------------------------------------------------------
enum TableModifyDataMode
{
	eModifyFieldValue = 0,							// 修改指定索引的表格对应字段的数值
	eApplyAffectParamToIndexTableFieldValue = 1,	// 修改指定索引的表格中对应字段的数值乘以影响系数
	eApplyAffectParamToAllTableFieldValue = 2,		// 修改管理中所有表格中指定字段的数值乘以影响系数
	eApplyIndexTableDeleteRecord = 3,
};
//-------------------------------------------------------------------------
bool DataTableManager::ModifyTableData( AutoTable modifySetTable )
{
	AssertEx(0, "未实现");
//	if (!modifySetTable)
//		return false;
//
//	AString checkFieldNameList = "INDEX MODIFYMODE TABLEINDEX CONFIGINDEX FIELDNAME MODIFYVALUE AFFECTPARAM MINVALUE MAXVALUE INFO";
//
//	if (!modifySetTable->CheckFieldName(checkFieldNameList))
//	{
//		Log("Error: 修改描述设置表格字段不正确 INDEX	MODIFYMODE	TABLEINDEX	CONFIGINDEX	FIELDNAME	MODIFYVALUE	AFFECTPARAM MAXVALUE MAXVALUE INFO");
//		return false;
//	}
//
//	Array<AString> fieldNameInfo;
//	AString::Split(checkFieldNameList.c_str(), fieldNameInfo, " ", FIELD_MAX_COUNT);
//
//	for (TableIt tIt(modifySetTable); tIt._Have(); tIt._Next())
//	{
//		ARecord hRe = tIt.getCurrRecord();
//
//		int modifyMode = hRe->get(fieldNameInfo[1]);
//		if (modifyMode==eModifyFieldValue)
//		{
//			AString tableIndex, configIndex, fieldName, modifyValue;
//			tableIndex = hRe->get(fieldNameInfo[2]).c_str();
//			configIndex = hRe->get(fieldNameInfo[3]).c_str();
//			fieldName = hRe->get(fieldNameInfo[4]).c_str();
//			modifyValue = hRe->get(fieldNameInfo[5]).c_str();
//			float fAffectParam = hRe->get(fieldNameInfo[6]);
//			if (tableIndex.length()>0 
//				&& configIndex.length()>0
//				&& fieldName.length()>0
//				&& modifyValue.length()>0
//				)
//			{
//				AutoTable hConfigTable = TableManager::getSingleton().GetTable(tableIndex.c_str());
//				if (!hConfigTable)
//				{
//					DEBUG_LOG("Error: [%s] config table no exist", tableIndex.c_str());
//					continue;
//				}
//				ARecord configRecord = hConfigTable->GetRecord(configIndex);
//				if (!configRecord)
//				{
//					configRecord = hConfigTable->CreateRecord(configIndex.c_str());
//					DEBUG_LOG("WARN: New config record [%s] into table [%s]", configIndex.c_str(), tableIndex.c_str());
//				}
//				AString oldValue = configRecord->get(fieldName.c_str()).c_str();
//				if (configRecord->set(fieldName.c_str(), modifyValue.c_str()))
//				{
//					DEBUG_LOG("WARN: Succeed modify config table [%s] > record [%s] > field [%s] > value [%s] to new [%s]", 
//						tableIndex.c_str(), configIndex.c_str(), fieldName.c_str(), oldValue.c_str(), modifyValue.c_str());
//				}
//				else
//				{
//					DEBUG_LOG("Error: modify record field [%s] value fail, may be field no exist", fieldName.c_str());
//				}
//#if DEBUG_SAVE_CONFIG_TABLE
//				AString strFile = SAVE_DEBUG_PATH;
//				strFile += tableIndex;
//				strFile += ".csv";
//				hConfigTable->_SaveToCSV(strFile.c_str());
//#endif
//			}
//		}
//		else if (modifyMode==eApplyAffectParamToAllTableFieldValue)
//		{
//			AString fieldName;
//			fieldName = hRe->get(fieldNameInfo[4]).c_str();
//			float fAffectParam = hRe->get(fieldNameInfo[6]);
//
//			AString minString = hRe->get(fieldNameInfo[7]);
//			AString maxString = hRe->get(fieldNameInfo[8]);
//
//			for (size_t i=0; i<mTableMap.size(); ++i)
//			{
//				AutoTable hConfigTable = mTableMap.get(i);
//				if (!hConfigTable || !hConfigTable->GetField() || !hConfigTable->ExistField(fieldName.c_str()))
//					continue;
//				FieldInfo info = hConfigTable->GetField()->getFieldInfo(fieldName.c_str());
//				if (info!=NULL 
//					&& (info->getType()==FIELD_INT || info->getType()==FIELD_FLOAT)
//					)
//				{
//					AString fileIndexName = hConfigTable->GetIndexName().c_str();
//
//					AutoData  fileData = ReadFile(fileIndexName);
//					if (!fileData)
//					{
//						DEBUG_LOG( "Error: Reload config table [%s] fail, read file data error", fileIndexName.c_str() );
//						continue;				
//					}
//					else
//					{
//						fileData->seek(0);
//						if ( hConfigTable->LoadFromData(fileData.getPtr()) )
//						{
//							DEBUG_LOG("WARN: Succeed reload config table [%s]", fileIndexName.c_str());
//						}
//						else
//						{
//							DEBUG_LOG( "Error: Reload config table [%s] fail, load data error", fileIndexName.c_str() );
//							continue;				
//						}
//					}			
//					
//					for (TableIt configTIt(hConfigTable); configTIt._Have(); configTIt._Next())
//					{
//						ARecord re = configTIt.getCurrRecord();
//						if (re)
//						{
//							float old = re->get(fieldName.c_str());
//
//							float nowValue = old * fAffectParam;
//							if (minString!="" && nowValue<atof(minString.c_str()))
//							{
//								nowValue = (float)atof(minString.c_str());
//							}
//							if (maxString!="" && nowValue>atof(maxString.c_str()))
//								nowValue = (float)atof(maxString.c_str());
//
//							re->set(fieldName.c_str(), nowValue);
//						}
//					}
//#if DEBUG_SAVE_CONFIG_TABLE
//					static int index = 0;
//					AString strFile = SAVE_DEBUG_PATH;
//					strFile += mTableMap.getKey(i);
//					strFile += "_";
//					strFile += ++index;
//					strFile += ".csv";
//					hConfigTable->_SaveToCSV(strFile.c_str());
//#endif
//				}
//			}
//		}
//		else if (modifyMode==eApplyIndexTableDeleteRecord)
//		{
//			AString tableIndex = hRe->get(fieldNameInfo[2]);
//			AString configIndex = hRe->get(fieldNameInfo[3]);
//
//			AutoTable hConfigTable = TableManager::getSingleton().GetTable(tableIndex.c_str());
//			if (!hConfigTable)
//			{
//				DEBUG_LOG("Error: [%s] config table no exist", tableIndex.c_str());
//				continue;
//			}
//			bool bDeleted = hConfigTable->DeleteRecord(configIndex);
//			if (bDeleted)
//			{
//				DEBUG_LOG("WARN: Deleted config record [%s] from table [%s]", configIndex.c_str(), tableIndex.c_str());
//			}
//		}
//	}

	return true;
}

AutoTable DataTableManager::GenerateInitNewModifyTable()
{
	//INDEX MODIFYMODE TABLEINDEX CONFIGINDEX FIELDNAME MODIFYVALUE AFFECTPARAM MINVALUE MAXVALUE INFO
	AutoTable hNewTable = CreateNewObject("Nice");
	int i = 0;
	hNewTable->SetField("INDEX", FIELD_INT, i++);
	hNewTable->SetField("MODIFYMODE", FIELD_INT, i++);
	hNewTable->SetField("TABLEINDEX", FIELD_STRING, i++);
	hNewTable->SetField("CONFIGINDEX", FIELD_STRING, i++);
	hNewTable->SetField("FIELDNAME", FIELD_STRING, i++);
	hNewTable->SetField("MODIFYVALUE", FIELD_STRING, i++);
	hNewTable->SetField("AFFECTPARAM", FIELD_FLOAT, i++);
	hNewTable->SetField("MINVALUE", FIELD_FLOAT, i++);
	hNewTable->SetField("MAXVALUE", FIELD_FLOAT, i++);
	hNewTable->SetField("INFO", FIELD_STRING, i++);
	if (!hNewTable->GetField()->check())
	{
		DEBUG_LOG("ERROR: 初始创建修改表格结构失败, 验证字段信息失败");
		return AutoTable();
	}
	return hNewTable;
}

DataTableManager::~DataTableManager()
{
	for (auto it=mTableMap.begin(); it; ++it)
	{
		if (it.get())
			it.get()->ClearAll();
	}
	mTableMap.clear();
}

//-----------------------------------------------------------------------------------------------------------

AutoTable ConfigManager::CreateNewObject(const char* tableIndex)
{
	return MEM_NEW ConfigTable();
}
//-------------------------------------------------------------------------*/
