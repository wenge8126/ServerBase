/********************************************************************
	created:	2010/04/29
	created:	29:4:2010   10:37
	filename: 	d:\Work\common\DataBase\TableManager.h
	file path:	d:\Work\common\DataBase
	file base:	TableManager
	file ext:	h
	author:		杨文鸽
	
	purpose:	表格全局管理对象,可以保存各种表格及创建,且可提供配置读取
*********************************************************************/
#ifndef _TABLEMANAGER_H_
#define _TABLEMANAGER_H_

#include "BaseCommon.h"
#include "BaseTable.h"
#include "EasyMap.h"
#include "FactoryTemplate.h"
#include "FastHash.h"


#define GET_KEY_ID(name)  TableManager::getSingleton().mStringKeyIndex.KeyID(name)

//#include <hash_map>
//-------------------------------------------------------------------------
// 用于全局索引字符串KEY
// NOTE: 目前使用全局静态方式
// 启用共享内存方式时, 此实例分配在共享内存
// DB 表格中, 此实例保存在 tablelist 的 EXT_DATA 字段中, 以NiceData方式保存, 运行中可动态修改
//-------------------------------------------------------------------------
typedef int ID_TYPE;

class StringKeyIndex
{
public:
	StringKeyIndex()
	{
		Init();
	}

	void Init()
	{
		mKeyDBTable = tBaseTable::NewBaseTable();
		mKeyDBTable->AppendField("ID", FIELD_INT);
		mKeyDBTable->AppendField("KEY", FIELD_STRING);
		mKeyDBTable->GetField()->_updateInfo();
		mKeyDBTable->SetMainIndex(0, true, false);
	}

public:
	ID_TYPE KeyID(const AString &strKey)
	{
		ID_TYPE *pID = mKeyHashList.findPtr(strKey);
		if (pID != NULL)
			return *pID;

		ID_TYPE id = MAKE_INDEX_ID(strKey.c_str());
		if (mKeyDBTable)
		{
			ARecord r = mKeyDBTable->GetRecord(id);
			if (!r)
			{
				r = mKeyDBTable->CreateRecord(id, true);
				if (r)
					r->set(1, strKey);
			}
		}
		mKeyHashList.insert(strKey, id);

		return id;
	}

	AString KeyString(ID_TYPE keyID)
	{
		ARecord r = mKeyDBTable->GetRecord(keyID);
		if (r)
			return r->get(1).string();
		ERROR_LOG("%u key no exist", keyID);
		return AString();
	}

	void InitKeyTable(AutoTable keyIndexTable)
	{
		mKeyHashList.clear(true);
		mKeyDBTable = keyIndexTable;
		for (auto it = mKeyDBTable->GetRecordIt(); *it; ++(*it))
		{
			auto re = it->Record();
			int keyID = re->get(0);
			AString key = re->getString(1);
			if (key.length() <= 0)
				mKeyDBTable->DeleteRecord(re);
			else
				mKeyHashList.insert(key, keyID);
		}
	}

protected:
	FastHash<AString, ID_TYPE>		mKeyHashList;

	AutoTable									mKeyDBTable;
};

//-------------------------------------------------------------------------
#if __WINDOWS__ || __SERVER__
#	define DEBUG_SAVE_CONFIG_TABLE		0
#	define SAVE_DEBUG_PATH				"d:/"
#else
#	define DEBUG_SAVE_CONFIG_TABLE		0
#endif

#define DEBUG_FIELD_INFO			0
//-------------------------------------------------------------------------
typedef FastHash<AString, AutoTable> TableMap;

class EasyResourcesPack;
//-------------------------------------------------------------------------
class BaseCommon_Export DataTableManager
{
public:
	DataTableManager();
	virtual ~DataTableManager();;

public:
	void Release(void) { delete this; }

	void Upate(void* param);

	virtual void Log(const char *szInfo, ...);

	virtual AutoData ReadFile(const char *szPathFileName );

public:

	bool LoadTable( const char* configTableFile,const char* type = "CSV" );
	// 从一个配置表中,加载所有资源表
	void LoadTable( AutoTable configTable );

	bool LoadTable(EasyResourcesPack *tablePak, const char *szListIndex, const char *szPakName = "");

	void ReadyTable(bool bChangeIDIndex);

	// 得到配置表
	AutoTable GetTable( const char* tableIndex, const char* errorInfo = NULL );

	ARecord GetRecord(const char* tableIndex, const char* recordIndex);
	ARecord GetRecord(const char* tableIndex, int nRecordIndex);

	Data GetData(const char* tableIndex, const char* recordIndex, const char* fieldName);
	Data GetData(const char* tableIndex, int recordIndex, const char* fieldName);

	Data GetData(const char* tableIndex, const char* recordIndex, const AString &fieldName)
	{
		return GetData(tableIndex, recordIndex, fieldName.c_str());
	}
	Data GetData(const char* tableIndex, int recordIndex, const AString &fieldName)
	{
		return GetData(tableIndex, recordIndex, fieldName.c_str());
	}

	Data GetData( const char* tableIndex, const char* recordIndex, int field );
	Data GetData( const char* tableIndex, int recordIndex, int field );

	// 修改数据, 根据修改配置表格 
	//INT	INT	STRING	STRING	STRING	STRING	FLOAT	STRING
	//INDEX	MODIFYMODE	TABLEINDEX	CONFIGINDEX	FIELDNAME	MODIFYVALUE	AFFECTPARAM	INFO
	bool ModifyTableData(AutoTable modifySetTable);

	AutoTable GenerateInitNewModifyTable();

public:
	// 创建一个新的表格,如果当前存在则删除当前的
	virtual AutoTable CreateNewTable( const char* tableIndex,const char* tableType = "Nice" );

	virtual AutoTable CreateNewObject(const char *szType)
	{
		return tBaseTable::NewBaseTable();
	}

	// 释放一类表格
	bool ReleaseTable( const char* tableIndex,bool nowFree = false );

	//是否存在某种类型的表
	bool IsExists( const char* tableIndex ){ return mTableMap.find(tableIndex); }

	// 清空所有加载的数据表格
	void ReleaseAllTable(void){ mTableMap.clear(); }

	bool InsertTable( const char* tableIndex,AutoTable hTable,bool bReplace );

	TableMap& GetTableMap(void){ return mTableMap; }

protected:
	TableMap            mTableMap;
};

#ifndef __LINUX__
template<>
void AutoPtr<DataTableManager>::FreeClass(DataTableManager *pPtr)
{
	pPtr->Release();
}
template<>
void AutoPtr<DataTableManager>::FreeUse()
{
	mUse->Release();
}

#endif
//------------------------------------------------------------------------------------
class BaseCommon_Export ConfigManager : public DataTableManager
{
public:
	virtual AutoTable CreateNewObject( const char* tableIndex ) override;
};
//------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------
class BaseCommon_Export TableManager : public ConfigManager
{
protected:
	static TableManager      *ms_Singleton;

public:
	TableManager();
	~TableManager();

	static bool Exist() { return ms_Singleton != NULL; }

	static TableManager* getSingletonPtr(void)
	{
		AssertEx( ms_Singleton , "no exist singleton object");
		return ms_Singleton;
	}
	static TableManager& getSingleton(void)
	{  
#pragma warning(push)
#pragma warning(disable:6011)
		//AssertEx( ms_Singleton , "no exist singleton object");
		if (ms_Singleton==NULL)
			new TableManager();

		return ( *ms_Singleton );

#pragma warning(pop)
	}  

public:
	static void	SetLog( SaveTableLog *tableLog );
	static void SetDefaultPath( const char* path );
	static const AString& GetDefaultPath(void);

	static void FreeAll();

protected:
	AString		mDefaultPath;

public:
	StringKeyIndex			mStringKeyIndex;
};
//------------------------------------------------------------------------------------

#endif	//_TABLEMANAGER_H_
