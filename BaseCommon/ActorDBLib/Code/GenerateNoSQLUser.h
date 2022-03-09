
#ifndef _INCLUDE_GENERATENOSQLUSER_H_
#define _INCLUDE_GENERATENOSQLUSER_H_

#include "FileDataStream.h"
#include "FastHash.h"
#include "BaseTable.h"
#include "TableManager.h"

class GenerateNoSQLUser
{
public:
	// szPath 生成目录, 结尾需要'/'
	static AString generate(const AString &tableName, AutoTable table, const char *szPath, bool bConst)
	{
		AutoTable mTable = table;
		AString mPath = szPath;

		AString name = tableName;
		//name += "_NOSQLUSER_COMPONENT"
		name.toUpper();
		const char *szTableName = tableName.c_str();

		AString mCodeString;

		mCodeString.Format("// Auto generate by table tool\r\n"
			"#ifndef _%s_H_\r\n"
			"#define _%s_H_\r\n"
			"\r\n"
			"#include \"NoSQLComponent.h\" \r\n\r\n"
			"class %s : public RecordNoSQLUserComponent\r\n{\r\npublic:\r\n"       
			"	%s(){}\r\n\r\n"
			, name.c_str()
			, name.c_str()
			, szTableName
			, szTableName
			);

		AString temp;
		
		//temp.Format(
		//	"	virtual bool InitRecord(ARecord re) override\r\n	{\r\n"
		//	"       if (re)\r\n        {\r\n"
		//	"           if (re->GetTable()==NULL)\r\n            {\r\n                ERROR(\"Reocrd table is NULL\"); return *this;\r\n            }\r\n"
		//	"           if (re->getField()->GetCheckCode()!=%d)\r\n            {\r\n                ERROR(\"Reocrd field check fail\"); return *this;\r\n            }\r\n"
		//	"        }\r\n"
		//	"       InitRecord(re);\r\n"
		//	"		if (mDataRecord) mKey = mDataRecord[0]; else mKey = '';\r\n"
		//	"		return mDataRecord;\r\n	}\r\n\r\n"
		//	, szTableName
		//	, mTable->GetField()->GetCheckCode()
		//);
		//mCodeString += temp;



		AString fieldNameList;
		AutoField f = mTable->GetField();
		for (int i=0; i<f->getCount(); ++i)
		{
			FieldInfo info = f->getFieldInfo(i);
            AString name = info->getName();
            if (name=="TEXT")
                name +="_";

			if (bConst)
				temp.Format("	const Data %s(/*%s*/) const { if (mDataRecord) return mDataRecord->get(%d); ERROR_LOG(\"No exist record %%s\", mKey.c_str()); return Data(); }\r\n", name.c_str(), info->getTypeString(), i);
			else if (info->getType()==FIELD_NICEDATA || info->getType()==FIELD_DATA || info->getType()==FIELD_SQLDATA || info->getType()==FIELD_ARRAYDATA)
				temp.Format("	Data %s(bool bSetUpdate=false) /*%s*/ { if (mDataRecord){ if (bSetUpdate) mDataRecord->NotifyChanged(%d); return mDataRecord->get(%d); } ERROR_LOG(\"No exist record %%s\", mKey.c_str()); return Data(); }\r\n", name.c_str(), info->getTypeString(), i, i);
			else if (info->getType()==FIELD_DBNICE)
                temp.Format("	DBNice %s(/*%s*/) { if (mDataRecord) return mDataRecord->getDBNice(%d); ERROR_LOG(\"No exist record %%s\", mKey.c_str()); return DBNice(); }\r\n", name.c_str(), info->getTypeString(), i);
            else
				temp.Format("	Data %s(/*%s*/) { if (mDataRecord) return mDataRecord->get(%d); ERROR_LOG(\"No exist record %%s\", mKey.c_str()); return Data(); }\r\n", name.c_str(), info->getTypeString(), i);
			mCodeString += temp;
			AString str;
			str.Format("fieldNameList[%d] = \"%s\"; ", i, info->getName().c_str());
			fieldNameList += str;
		}
		mCodeString += "\r\n\r\n";

		AString createTableString;

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
				temp.Format("	int n%s() const { int v = 0; if (mDataRecord) {  mDataRecord->get(%d, v); return v; } ERROR_LOG(\"No exist record %%s\", mKey.c_str()); return v; }\r\n", name.c_str(), i);
				break;

			case FIELD_UINT64:
				temp.Format("	UInt64 u%s() const { UInt64 v = 0; if (mDataRecord) {  mDataRecord->get(%d, v); return v; } ERROR_LOG(\"No exist record %%s\", mKey.c_str()); return v; }\r\n", name.c_str(), i);
				break;

			case FIELD_INT64:
				temp.Format("	Int64 w%s() const { Int64 v = 0; if (mDataRecord) {  mDataRecord->get(%d, v); return v; } ERROR_LOG(\"No exist record %%s\", mKey.c_str()); return v; }\r\n", name.c_str(), i);
				break;

			case FIELD_FLOAT:
				temp.Format("	float f%s() const { float v = 0; if (mDataRecord) {  mDataRecord->get(%d, v); return v; } ERROR_LOG(\"No exist record %%s\", mKey.c_str()); return v; }\r\n", name.c_str(), i);
				break;

			case FIELD_STRING:
            case FIELD_CHAR_STRING:
				temp.Format("	const char* sz%s() const { if (mDataRecord)  return  mDataRecord->getString(%d); ERROR_LOG(\"No exist record %%s\", mKey.c_str()); return \"\"; }\r\n", name.c_str(), i);
				break;

			case FIELD_BOOL:
				temp.Format("	bool b%s() const { int v = 0; if (mDataRecord) {  mDataRecord->get(%d, v); return v!=0; } ERROR_LOG(\"No exist record %%s\", mKey.c_str()); return false; }\r\n", name.c_str(), i);
				break;

			case FIELD_NICEDATA:
				temp.Format("	AutoNice nice%s(bool bSetUpdate=false) {  AutoNice v; if (mDataRecord) { mDataRecord->get(%d, v);  if (bSetUpdate){ if (!v) {v=MEM_NEW NiceData(); mDataRecord->set(%d, v); } else mDataRecord->NotifyChanged(%d); } return v; } ERROR_LOG(\"No exist record %%s\", mKey.c_str()); return v; }\r\n", name.c_str(), i, i, i);
				break;

			case FIELD_ARRAYDATA:
				temp.Format("	AutoNice array%s(bool bSetUpdate=false) {  AutoNice v; if (mDataRecord) { mDataRecord->get(%d, v);  if (bSetUpdate){ if (!v) {v=MEM_NEW ArrayNiceData(); mDataRecord->set(%d, v); } else mDataRecord->NotifyChanged(%d); } return v; } ERROR_LOG(\"No exist record %%s\", mKey.c_str()); return v; }\r\n", name.c_str(), i, i, i);
				break;

            case FIELD_DBNICE:
                temp.Format("	DBNice nice%s(/*%s*/) { if (mDataRecord) return mDataRecord->getDBNice(%d); ERROR_LOG(\"No exist record %%s\", mKey.c_str()); return DBNice(); }\r\n", name.c_str(), info->getTypeString(), i);
                break;
			case FIELD_DATA:
			case FIELD_SQLDATA:
				temp.Format("	AutoData data%s(bool bSetUpdate=false) {  AutoData v; if (mDataRecord) {  mDataRecord->get(%d, v);   if (bSetUpdate){ if (!v) {v=MEM_NEW DataBuffer(); mDataRecord->set(%d, v); } else mDataRecord->NotifyChanged(%d); }   return v; } ERROR_LOG(\"No exist record %%s\", mKey.c_str()); return v; }\r\n", name.c_str(), i, i, i);
				break;

			case FIELD_TABLE:
			case FIELD_DB_TABLE:
				temp.Format("	AutoTable table%s() const {  if (mDataRecord) return  ((BaseRecord*)mDataRecord.getPtr())->GetFieldTable(%d); ERROR_LOG(\"No exist record %%s\", mKey.c_str()); return AutoTable(); }\r\n", name.c_str(), i);
				break;
		

			default:
				ERROR_LOG("不支持直接导出的值函数 >%d : %s, %s", info->getType(), info->getTypeString(),  info->getName().c_str());
				continue;
			}

			mCodeString += temp;
			temp.Format("        table->AppendField(\"%s\", \"%s\");\r\n", info->getName(), info->getTypeString());
			createTableString += temp;

			if (info->getType()==FIELD_STRING)
			{			
				temp.Format("	const AString& str%s() const { if (mDataRecord)  return  mDataRecord->getAString(%d); ERROR_LOG(\"No exist record %%s\", mKey.c_str()); static AString s; return s; }\r\n", name.c_str(), i);
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

		temp.Format("\r\n	virtual bool InitRecord(ARecord record) override\r\n	{\r\n"
			"		AutoField f = record->getField();\r\n"
            "		if (!f || !f->CheckSame(%d))\r\n"
            "		{\r\n"
            "			ERROR_LOG(\"No same check code of %%s\", record->GetTable()->GetTableName());\r\n"
            "           return false;\r\n"
            "       }\r\n\r\n"
			"		Array<AString> fieldNameList(f->getCount());\r\n"
			"		%s\r\n"
			"		if (fieldNameList.size()!=f->getCount()) { ERROR_LOG(\"%%s field count is error.\",  record->GetTable()->GetTableName()); return false; }\r\n"
			"		for (int i=0; i<f->getCount(); ++i)\r\n		{\r\n"
			"			FieldInfo info = f->getFieldInfo(i);\r\n"
			"			if (info==NULL || info->getName()!=fieldNameList[i]) { ERROR_LOG(\"%%s %%d field is not same %%s\",  record->GetTable()->GetTableName(), i, fieldNameList[i].c_str()); return false; }\r\n"
			"		}\r\n"
			"		return RecordNoSQLUserComponent::InitRecord(record);\r\n	}\r\n"
            , f->GetCheckCode()
			, fieldNameList.c_str()
			);

		mCodeString += temp;
		
		temp.Format("\r\n    static AutoTable CreateTable()\r\n    {\r\n"
			"        AutoTable table = MEM_NEW StructBaseTable();\r\n"
			"%s"
			"        return table;\r\n    }\r\n\r\n"
			, createTableString.c_str()
		);
		mCodeString += temp;

		mCodeString += (
			"};\r\n\r\n#endif\r\n"
			);

		AString fileName = mPath + tableName;
		fileName += ".h";
		FileDataStream  file(fileName.c_str(), FILE_CREATE_UTF8);
		file._write((void*)mCodeString.c_str(), mCodeString.length());

		temp.Format("Succeed generate code %s", fileName.c_str());
		return temp;
	}

	

	static  AString generateNoSQLUser(TableMap &tableList, AString path)
	{
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
			GenerateNoSQLUser t; // 
			info += "\r\n";
			info+= t.generate(indexName, it.get(), path.c_str(), false);
		}

		return info;
	}


};



#endif //_INCLUDE_GENERATECODETOOL_H_

