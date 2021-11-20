
#ifndef _INCLUDE_GENERATECODETOOL_H_
#define _INCLUDE_GENERATECODETOOL_H_

#include "FileDataStream.h"
#include "FastHash.h"
#include "BaseTable.h"
#include "TableManager.h"

class GenerateCodeTool
{
public:
	// szPath 生成目录, 结尾需要'/'
	GenerateCodeTool(AutoTable table, const char *szPath, const char *szDBClassName)
	{
		mTable = table;
		mPath = szPath;
		mDBClassName = szDBClassName;

		//generate();
	}

	AString generate(bool bConst)
	{
		AString name = mTable->GetTableName();
		name.toUpper();
		const char *szTableName = mTable->GetTableName();
		mCodeString.Format("// Auto generate by table tool\r\n"
			"#ifndef _%s_H_\r\n"
			"#define _%s_H_\r\n"
			"\r\n"
			"class %s\r\n{\r\npublic:\r\n"
            "	static AutoTable table;\r\n\r\n"
            "	inline operator bool () const   {  return mRecord; }\r\n\r\n"
			"	static ARecordIt GetRecordIt()  { if (table) return table->GetRecordIt(); return ARecordIt(); }\r\n\r\n"
			"	%s(){}\r\n\r\n"
			, name.c_str()
			, name.c_str()
			, szTableName
			, szTableName
			);

		AString temp;
		temp.Format("	%s(int key)\r\n	{\r\n"
            "       AutoTable t = %s::table;\r\n"
			"		mKey = key;\r\n"
			"		if ( t )\r\n"
			"		{\r\n"
			"			mRecord = t->GetRecord(key);\r\n"
			"			if (!mRecord)	 DEBUG_LOG(\"No exist record %%d in table %%s\", key, t->GetTableName());\r\n"
			"		}\r\n		else\r\n"
			"			ERROR_LOG(\"No exist table %s\");\r\n	}\r\n\r\n"
			, szTableName
            , szTableName
			, szTableName
			);
		mCodeString += temp;

        temp.Format("	%s(Int64 key)\r\n	{\r\n"
            "       AutoTable t = %s::table;\r\n"
            "		mKey = key;\r\n"
            "		if ( t )\r\n"
            "		{\r\n"
            "			mRecord = t->GetRecord(key);\r\n"
            "			if (!mRecord)	 DEBUG_LOG(\"No exist record %%d in table %%s\", key, t->GetTableName());\r\n"
            "		}\r\n		else\r\n"
            "			ERROR_LOG(\"No exist table %s\");\r\n	}\r\n\r\n"
            , szTableName
            , szTableName
			, szTableName
            );
        mCodeString += temp;

        temp.Format("	%s(UInt64 key)\r\n	{\r\n"
            "       AutoTable t = %s::table;\r\n"
            "		mKey = (Int64)(key);\r\n"
            "		if ( t )\r\n"
            "		{\r\n"
            "			mRecord = t->GetRecord(key);\r\n"
           // "			if (!mRecord)	 DEBUG_LOG(\"No exist record %%d in table %%s\", key, t->GetTableName());\r\n"
            "		}\r\n		else\r\n"
            "			ERROR_LOG(\"No exist table %s\");\r\n	}\r\n\r\n"
            , szTableName
            , szTableName
			, szTableName
            );
        mCodeString += temp;
		
		temp.Format(
			"	%s(const char *key)\r\n	{\r\n"
            "       AutoTable t = %s::table;\r\n"
			"		mKey = TOINT64(key);\r\n"
			"		if ( t )\r\n"
			"		{\r\n"
			"			mRecord = t->GetRecord(key);\r\n"
			//"			if (!mRecord)	 ERROR_LOG(\"No exist record %%s in table %%s\", key, t->GetTableName());\r\n"
			"		}\r\n		else\r\n"
			"			ERROR_LOG(\"No exist table %s\");\r\n	}\r\n\r\n"
			, szTableName
            , szTableName
			, szTableName
			);
		mCodeString += temp;
		
		temp.Format(
			"	%s& operator = (ARecord re)\r\n	{\r\n"
			"       if (re)\r\n        {\r\n"
			"           if (re->GetTable()==NULL)\r\n            {\r\n                ERROR(\"Reocrd table is NULL\"); return *this;\r\n            }\r\n"
			"           if (re->getField()->GetCheckCode()!=%d)\r\n            {\r\n                ERROR(\"Reocrd field check fail\"); return *this;\r\n            }\r\n"
			"        }\r\n"
			"       mRecord = re;\r\n"
			"		if (mRecord) mKey = mRecord[0]; else mKey = 0;\r\n"
			"		return *this;\r\n	}\r\n\r\n"
			, szTableName
			, mTable->GetField()->GetCheckCode()
		);
		mCodeString += temp;

		temp.Format("	%s(bool bNewGrowth) { if (bNewGrowth) GrowthNew(); }\r\n\r\n"
			, szTableName			
		);
		mCodeString += temp;

        temp.Format("   bool NewCreate(const char *szKey, bool bReplace) { mRecord = table->CreateRecord(szKey, bReplace); return mRecord; }\r\n");
        mCodeString += temp;
        temp.Format("   void GrowthNew() { mRecord = table->GrowthNewRecord(); }\r\n\r\n");
        mCodeString += temp;

		temp.Format(
			"	%s(const AString &key)\r\n	{\r\n"
			"       AutoTable t = %s::table;\r\n"
			"		mKey = TOINT(key.c_str());\r\n"
			"		if ( t )\r\n"
			"		{\r\n"
			"			mRecord = t->GetRecord(key);\r\n"
			//"			if (!mRecord)	 ERROR_LOG(\"No exist record %%s in table %%s\", key.c_str(), t->GetTableName());\r\n"
			"		}\r\n		else\r\n"
			"			ERROR_LOG(\"No exist table %s\");\r\n	}\r\n\r\n"
			, szTableName
			, szTableName
			, szTableName
			);
		mCodeString += temp;

		temp.Format("	%s(ARecord record)\r\n	{\r\n"
			"       *this = record;\r\n"			
			"	}\r\n\r\n"
			, szTableName
			);
		mCodeString += temp;

		temp.Format("    void Save()\r\n	{\r\n"
			"       if (mRecord) mRecord->SaveUpdate();\r\n"
			"	}\r\n\r\n"
			, szTableName
		);
		mCodeString += temp;

		AString fieldNameList;
		AutoField f = mTable->GetField();
		for (int i=0; i<f->getCount(); ++i)
		{
			FieldInfo info = f->getFieldInfo(i);
            AString name = info->getName();
            if (name=="TEXT")
                name +="_";
			if (bConst)
				temp.Format("	const Data %s(/*%s*/) const { if (mRecord) return mRecord->get(%d); ERROR_LOG(\"No exist record %%d\", mKey); return Data(); }\r\n", name.c_str(), info->getTypeString(), i);
			else if (info->getType()==FIELD_NICEDATA || info->getType()==FIELD_DATA || info->getType()==FIELD_SQLDATA || info->getType()==FIELD_ARRAYDATA)
				temp.Format("	Data %s(bool bSetUpdate=false) /*%s*/ { if (mRecord){ if (bSetUpdate) mRecord->NotifyChanged(%d); return mRecord->get(%d); } ERROR_LOG(\"No exist record %%d\", mKey); return Data(); }\r\n", name.c_str(), info->getTypeString(), i, i);
			else if (info->getType()==FIELD_DBNICE)
                temp.Format("	DBNice %s(/*%s*/) { if (mRecord) return mRecord->getDBNice(%d); ERROR_LOG(\"No exist record %%d\", mKey); return DBNice(); }\r\n", name.c_str(), info->getTypeString(), i);
            else
				temp.Format("	Data %s(/*%s*/) { if (mRecord) return mRecord->get(%d); ERROR_LOG(\"No exist record %%d\", mKey); return Data(); }\r\n", name.c_str(), info->getTypeString(), i);
			mCodeString += temp;
			AString str;
			str.Format("fieldNameList[%d] = \"%s\"; ", i, info->getName().c_str());
			fieldNameList += str;
		}
		mCodeString += "\r\n\r\n";
		// 产生直接取值代码
		for (int i=0; i<f->getCount(); ++i)
		{
			FieldInfo info = f->getFieldInfo(i);
			AString name = info->getName();

			switch (info->getType())
			{
			case FIELD_BYTE:
			case FIELD_SHORT:
			case FIELD_INT:
				temp.Format("	int n%s() const { int v = 0; if (mRecord) {  mRecord->get(%d, v); return v; } ERROR_LOG(\"No exist record %%d\", mKey); return v; }\r\n", name.c_str(), i);
				break;

			case FIELD_UINT64:
				temp.Format("	UInt64 u%s() const { UInt64 v = 0; if (mRecord) {  mRecord->get(%d, v); return v; } ERROR_LOG(\"No exist record %%d\", mKey); return v; }\r\n", name.c_str(), i);
				break;

			case FIELD_INT64:
				temp.Format("	Int64 w%s() const { Int64 v = 0; if (mRecord) {  mRecord->get(%d, v); return v; } ERROR_LOG(\"No exist record %%d\", mKey); return v; }\r\n", name.c_str(), i);
				break;

			case FIELD_FLOAT:
				temp.Format("	float f%s() const { float v = 0; if (mRecord) {  mRecord->get(%d, v); return v; } ERROR_LOG(\"No exist record %%d\", mKey); return v; }\r\n", name.c_str(), i);
				break;

			case FIELD_STRING:
            case FIELD_CHAR_STRING:
				temp.Format("	const char* sz%s() const { if (mRecord)  return  mRecord->getString(%d); ERROR_LOG(\"No exist record %%d\", mKey); return \"\"; }\r\n", name.c_str(), i);
				break;

			case FIELD_BOOL:
				temp.Format("	bool b%s() const { int v = 0; if (mRecord) {  mRecord->get(%d, v); return v!=0; } ERROR_LOG(\"No exist record %%d\", mKey); return false; }\r\n", name.c_str(), i);
				break;

			case FIELD_NICEDATA:
				temp.Format("	AutoNice nice%s(bool bSetUpdate=false) {  AutoNice v; if (mRecord) { mRecord->get(%d, v);  if (bSetUpdate){ if (!v) {v=MEM_NEW NiceData(); mRecord->set(%d, v); } else mRecord->NotifyChanged(%d); } return v; } ERROR_LOG(\"No exist record %%d\", mKey); return v; }\r\n", name.c_str(), i, i, i);
				break;

			case FIELD_ARRAYDATA:
				temp.Format("	AutoNice array%s(bool bSetUpdate=false) {  AutoNice v; if (mRecord) { mRecord->get(%d, v);  if (bSetUpdate){ if (!v) {v=MEM_NEW ArrayNiceData(); mRecord->set(%d, v); } else mRecord->NotifyChanged(%d); } return v; } ERROR_LOG(\"No exist record %%d\", mKey); return v; }\r\n", name.c_str(), i, i, i);
				break;

            case FIELD_DBNICE:
                temp.Format("	DBNice nice%s(/*%s*/) { if (mRecord) return mRecord->getDBNice(%d); ERROR_LOG(\"No exist record %%d\", mKey); return DBNice(); }\r\n", name.c_str(), info->getTypeString(), i);
                break;
			case FIELD_DATA:
			case FIELD_SQLDATA:
				temp.Format("	AutoData data%s(bool bSetUpdate=false) {  AutoData v; if (mRecord) {  mRecord->get(%d, v);   if (bSetUpdate){ if (!v) {v=MEM_NEW DataBuffer(); mRecord->set(%d, v); } else mRecord->NotifyChanged(%d); }   return v; } ERROR_LOG(\"No exist record %%d\", mKey); return v; }\r\n", name.c_str(), i, i, i);
				break;

			case FIELD_TABLE:
			case FIELD_DB_TABLE:
				temp.Format("	AutoTable table%s() const {  if (mRecord) return  ((BaseRecord*)mRecord.getPtr())->GetFieldTable(%d); ERROR_LOG(\"No exist record %%d\", mKey); return AutoTable(); }\r\n", name.c_str(), i);
				break;
		

			default:
				ERROR_LOG("不支持直接导出的值函数 >%d : %s, %s", info->getType(), info->getTypeString(),  info->getName().c_str());
				continue;
			}

			mCodeString += temp;

			if (info->getType()==FIELD_STRING)
			{			
				temp.Format("	const AString& str%s() const { if (mRecord)  return  mRecord->getAString(%d); ERROR_LOG(\"No exist record %%d\", mKey); static AString s; return s; }\r\n", name.c_str(), i);
				mCodeString += temp;
			}
		}

		mTable->OnGenerateCode(mCodeString);

		//temp.Format("\r\n	bool InitCheck(AutoTable t)\r\n	{\r\n"
		//	"		int fieldCode = %d;\r\n"
		//	"		AutoField f = t->GetField();\r\n"
		//	"		if (!f->CheckSame(fieldCode)) { ERROR_LOG(\"Field code %%d != %%d\", f->GetCheckCode(), fieldCode); return false; } \r\n		return true;\r\n"
		//	"	}\r\n"
		//	, f->GetCheckCode()			
		//	);

		temp.Format("\r\n	static bool InitCheck(AutoTable t, bool bInitSet)\r\n	{\r\n"
            "       if (bInitSet) table = t;\r\n"
			"		AutoField f = t->GetField();\r\n"
            "		if (!f || !f->CheckSame(%d))\r\n"
            "		{\r\n"
            "			ERROR_LOG(\"No same check code of %%s\", t->GetTableName());\r\n"
            "           return false;\r\n"
            "       }\r\n\r\n"
			"		Array<AString> fieldNameList(f->getCount());\r\n"
			"		%s\r\n"
			"		if (fieldNameList.size()!=f->getCount()) { ERROR_LOG(\"%%s field count is error.\", t->GetTableName()); return false; }\r\n"
			"		for (int i=0; i<f->getCount(); ++i)\r\n		{\r\n"
			"			FieldInfo info = f->getFieldInfo(i);\r\n"
			"			if (info==NULL || info->getName()!=fieldNameList[i]) { ERROR_LOG(\"%%s %%d field is not same %%s\", t->GetTableName(), i, fieldNameList[i].c_str()); return false; }\r\n"
			"		}\r\n"
			"		return true;\r\n	}\r\n"
            , f->GetCheckCode()
			, fieldNameList.c_str()
			);

		mCodeString += temp;

		mCodeString += ("public:\r\n"
			"	ARecord	mRecord;\r\n"			
			"	Int64			mKey;\r\n"
			"};\r\n\r\n#endif\r\n"
			);

		AString fileName = mPath + mTable->GetTableName();
		fileName += ".h";
		FileDataStream  file(fileName.c_str(), FILE_CREATE_UTF8);
		file._write((void*)mCodeString.c_str(), mCodeString.length());

		temp.Format("Succeed generate code %s", fileName.c_str());
		return temp;
	}

	static  AString generateManager(DataTableManager &mgr, AString path)
	{
		AString temp;
		AString mCodeString;

		AString includeList;
		for (auto it=mgr.GetTableMap().begin(); it; ++it)
		{
			AString indexName = it.key();
			temp.Format("#include \"%s.h\" \r\n", indexName.c_str());
			includeList += temp;
		}
		
		mCodeString.Format("// Auto generate by table tool\r\n"
			"#ifndef _CONFIG_DATA_H_\r\n"
			"#define _CONFIG_DATA_H_\r\n"
			"\r\n"
			"%s\r\n"
			"class ConfigData\r\n{\r\npublic:\r\n"						
			, includeList.c_str()
			);

		AString valList;
		AString checkList;
		for (auto it=mgr.GetTableMap().begin(); it; ++it)
		{
			AString indexName = it.key();
			temp.Format("	%s Get%s(int key) { return %s(m%s, key); }\r\n"
				, indexName.c_str()
				, indexName.c_str()
				, indexName.c_str()
				, indexName.c_str()
				);
			mCodeString += temp;

			temp.Format("	%s Get%s(const char *key) { return %s(m%s, key); }\r\n"
				, indexName.c_str()
				, indexName.c_str()
				, indexName.c_str()
				, indexName.c_str()
				);
			mCodeString += temp;

			temp.Format(
				"	AutoTable m%s;\r\n", indexName.c_str()
				);
			valList += temp;

			temp.Format("			t = mgr.GetTable(\"%s\");\r\n"
				"			if (t && %s::InitCheck(t)) m%s = t; else { ERROR_LOG(\"%s config check fail\"); return false; }\r\n"
				, indexName.c_str()
				, indexName.c_str()
				, indexName.c_str()
				, indexName.c_str()
				);
			checkList += temp;
		}

		temp.Format("\r\n	bool InitReady(DataTableManager &mgr)\r\n"
			"	{\r\n"
			"			AutoTable t;\r\n"
			"%s"
			"		return true;\r\n"
			"	}\r\n"
			, checkList.c_str()
			);
		mCodeString += temp;

		mCodeString += "public:\r\n";
		mCodeString += valList;

		mCodeString +="};\r\n\r\n#endif\r\n";

		AString fileName = path;		
		fileName += "ConfigData.h";
		FileDataStream  file(fileName.c_str(), FILE_CREATE_UTF8);
		file._write((void*)mCodeString.c_str(), mCodeString.length());

		temp.Format("Succeed generate code %s", fileName.c_str());

		return temp;
	}

	static  AString generateDBManager(TableMap &tableList, AString path, const char *szDBClassName)
	{
		AString name = szDBClassName;
		name.toUpper();

		// path 生成目录, 结尾需要'/', 如果没有, 则会自动加上
		if (path.length() > 1)
		{
			if (path.c_str()[path.length() - 1] != '/' || path.c_str()[path.length() - 1] != '\\')
				path += "/";
		}

		AString info;
		for (auto it=tableList.begin(); it; ++it)
		{
			AString indexName = it.key();
			if (strstr(indexName.c_str(), "/")!=NULL)
			{
				NOTE_LOG("WARN: 不可生成带有 / 路径符号的表格代码 %s", indexName.c_str());
				continue;
			}
			GenerateCodeTool t(it.get(), path.c_str(), szDBClassName);			
			info += "\r\n";
			info+= t.generate(false);
		}

		AString temp;
		AString mCodeString;
		AString cppVarString;
		cppVarString.Format("	bool %s::mbInitSet = true;\r\n", szDBClassName);

		AString includeList;
		for (auto it=tableList.begin(); it; ++it)
		{
			AString indexName = it.key();
			if (strstr(indexName.c_str(), "/")!=NULL)
			{
				NOTE_LOG("WARN: 不可生成带有 / 路径符号的表格代码 %s", indexName.c_str());
				continue;
			}
			temp.Format("#include \"%s.h\" \r\n", indexName.c_str());
			includeList += temp;
		}

		mCodeString.Format("// Auto generate by table tool\r\n"
			"#ifndef _%s_H_\r\n"
			"#define _%s_H_\r\n"
			"\r\n"
            "#include \"BaseTable.h\"\r\n"
			"#include \"BaseMemoryDB.h\"\r\n\r\n"
			"%s\r\n"
			"\r\nusing namespace NetCloud;\r\n\r\n"
			"class %s\r\n{\r\npublic:\r\n	static bool mbInitSet;"						
			, name.c_str()
			, name.c_str()
			, includeList.c_str()
			, szDBClassName
			);

		AString valList;
		AString checkList;
		AString clearFunString;
		for (auto it=tableList.begin(); it; ++it)
		{
			AString indexName = it.key();
			if (strstr(indexName.c_str(), "/")!=NULL)
			{
				NOTE_LOG("WARN: 不可生成带有 / 路径符号的表格代码 %s", indexName.c_str());
				continue;
			}

			//temp.Format("	static %s get_%s(int key) { return %s(ms_%s, key); }\r\n"
			//	, indexName.c_str()
			//	, indexName.c_str()
			//	, indexName.c_str()
			//	, indexName.c_str()
			//	);
			//mCodeString += temp;

			//temp.Format("	static %s get_%s(const char *key) { return %s(ms_%s, key); }\r\n"
			//	, indexName.c_str()
			//	, indexName.c_str()
			//	, indexName.c_str()
			//	, indexName.c_str()
			//	);
			//mCodeString += temp;

			//temp.Format(
			//	"	static AutoTable ms_%s;\r\n", indexName.c_str()
			//	);
			//valList += temp;

            temp.Format(
                "	AutoTable %s::table;\r\n", indexName.c_str()
                );
            cppVarString += temp;

			temp.Format("			t = mgr.find(\"%s\");\r\n"
                "			if (t && %s::InitCheck(t, mbInitSet)) { } else { ERROR_LOG(\"%s config check fail\"); bResult = false; }\r\n"
				, indexName.c_str()
				, indexName.c_str()
				//, indexName.c_str()
				, indexName.c_str()
				);
			checkList += temp;

			temp.Format("		%s::table.setNull();\r\n", indexName.c_str());
			clearFunString += temp;
		}

		temp.Format("\r\n	static bool InitReady(TableMap &mgr)\r\n"
			"	{\r\n"
			"		AutoTable t;\r\n"
			"		bool bResult = true;\r\n"
			"%s"
			"		return  bResult;\r\n"
			"	}\r\n\r\n"
			"	static void Clear()\r\n"
			"	{\r\n"
			"%s\r\n	}\r\n"
			, checkList.c_str()
			, clearFunString.c_str()
			);
		mCodeString += temp;

		mCodeString += "public:\r\n";
		mCodeString += valList;

		mCodeString +="};\r\n\r\n";
        //mCodeString += cppVarString;
        mCodeString +="\r\n\r\n#endif\r\n";

		AString fileName = path;		
		fileName += szDBClassName;
		fileName += ".h";
		FileDataStream  file(fileName.c_str(), FILE_CREATE_UTF8);
		file._write((void*)mCodeString.c_str(), mCodeString.length());

        AString cppfileName = path;		
        cppfileName += szDBClassName;
        cppfileName += ".cpp";
        FileDataStream  cppfile(cppfileName.c_str(), FILE_CREATE_UTF8);
        AString head;
        head.Format("#include \"%s.h\"\r\n\r\n", szDBClassName);
        cppfile._write((void*)head.c_str(), head.length());
        cppfile._write((void*)cppVarString.c_str(), cppVarString.length());

		temp.Format("Succeed generate code %s", fileName.c_str());

		return info + temp;
	}

protected:
	AutoTable		mTable;
	AString			mPath;
	AString			mCodeString;
	AString			mDBClassName;
};



#endif //_INCLUDE_GENERATECODETOOL_H_

/* 生成示例
// Auto generate by table tool
#ifndef _WORKERCONFIG_H_
#define _WORKERCONFIG_H_

class WorkerConfig
{
public:
	WorkerConfig(AutoTable t, int key)
	{
		mKey += key;
		if (t->GetField()->CheckSame(-1436943822))
		{
			mRecord = t->GetRecord(key);
			if (!mRecord)	 ERROR_LOG("No exist record %s in table %s", mKey.c_str(), t->GetTableName());
		}
		else
			ERROR_LOG("No same check code of %s", t->GetTableName());
	}

	WorkerConfig(AutoTable t, const char *key)
	{
		mKey += key;
		if (t->GetField()->CheckSame(-1436943822))
		{
			mRecord = t->GetRecord(key);
			if (!mRecord)	 ERROR_LOG("No exist record %s in table %s", mKey.c_str(), t->GetTableName());
		}
		else
			ERROR_LOG("No same check code of %s", t->GetTableName());
	}

	const Data INDEX() const { if (mRecord) return mRecord->get(0); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	const Data NAME() const { if (mRecord) return mRecord->get(1); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	const Data COST_FOOD() const { if (mRecord) return mRecord->get(2); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	const Data INFO() const { if (mRecord) return mRecord->get(3); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	const Data MODEL() const { if (mRecord) return mRecord->get(4); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	const Data HEAD_ATLAS_NAME() const { if (mRecord) return mRecord->get(5); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	const Data HEAD_SPRITE_NAME() const { if (mRecord) return mRecord->get(6); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	const Data SCALE() const { if (mRecord) return mRecord->get(7); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	const Data UI_SCALE() const { if (mRecord) return mRecord->get(8); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	const Data GOLD_YIELD() const { if (mRecord) return mRecord->get(9); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	const Data MOVE_SPEED() const { if (mRecord) return mRecord->get(10); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	const Data LOOKBOUND() const { if (mRecord) return mRecord->get(11); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	const Data AI_TIME() const { if (mRecord) return mRecord->get(12); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	const Data ATTACK_SKILL() const { if (mRecord) return mRecord->get(13); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	const Data BASE_ATTACK() const { if (mRecord) return mRecord->get(14); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	const Data BASE_HP() const { if (mRecord) return mRecord->get(15); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	const Data BASE_MP() const { if (mRecord) return mRecord->get(16); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	const Data ATTACKSPEED() const { if (mRecord) return mRecord->get(17); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	const Data ATTACK_STATE_1() const { if (mRecord) return mRecord->get(18); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	const Data AFFECT_BUFFER_1() const { if (mRecord) return mRecord->get(19); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	const Data GROUP_ID() const { if (mRecord) return mRecord->get(20); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	const Data DROP_EXP() const { if (mRecord) return mRecord->get(21); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	const Data ATTACK_DAMAGE() const { if (mRecord) return mRecord->get(22); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }
	const Data EFFECT() const { if (mRecord) return mRecord->get(23); ERROR_LOG("No exist record %s", mKey.c_str()); return Data(); }

	static bool InitCheck(AutoTable t)
	{
		AutoField f = t->GetField();
		Array<AString> fieldNameList(f->getCount());
		fieldNameList[0] = "INDEX"; fieldNameList[1] = "NAME"; fieldNameList[2] = "COST_FOOD"; fieldNameList[3] = "INFO"; fieldNameList[4] = "MODEL"; fieldNameList[5] = "HEAD_ATLAS_NAME"; fieldNameList[6] = "HEAD_SPRITE_NAME"; fieldNameList[7] = "SCALE"; fieldNameList[8] = "UI_SCALE"; fieldNameList[9] = "GOLD_YIELD"; fieldNameList[10] = "MOVE_SPEED"; fieldNameList[11] = "LOOKBOUND"; fieldNameList[12] = "AI_TIME"; fieldNameList[13] = "ATTACK_SKILL"; fieldNameList[14] = "BASE_ATTACK"; fieldNameList[15] = "BASE_HP"; fieldNameList[16] = "BASE_MP"; fieldNameList[17] = "ATTACKSPEED"; fieldNameList[18] = "ATTACK_STATE_1"; fieldNameList[19] = "AFFECT_BUFFER_1"; fieldNameList[20] = "GROUP_ID"; fieldNameList[21] = "DROP_EXP"; fieldNameList[22] = "ATTACK_DAMAGE"; fieldNameList[23] = "EFFECT"; 
		if (fieldNameList.size()!=f->getCount()) { ERROR_LOG("%s field count is error.", t->GetTableName()); return false; }
		for (int i=0; i<f->getCount(); ++i)
		{
			FieldInfo info = f->getFieldInfo(i);
			if (info==NULL || info->getName()!=fieldNameList[i]) { ERROR_LOG("%s %d field is not same %s", t->GetTableName(), i, fieldNameList[i].c_str()); return false; }
		}
		return true;
	}
public:
	ARecord	mRecord;
	AString			mKey;
};

#endif
*/