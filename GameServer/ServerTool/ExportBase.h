#pragma once

#include "base64.h"
#include "FileDataStream.h"

FieldInfo GetFieldInfoByTypeString(AString typeString)
{
	AString tempType = typeString;
	AString leftType = tempType.SplitLeft("<");
	leftType.toUpper();
	FieldInfo info = NULL;
	if (leftType.length() > 0)
		info = FieldInfoManager::getFieldInfo(leftType.c_str());
	else
	{
		tempType.toUpper();
		info = FieldInfoManager::getFieldInfo(tempType.c_str());
	}
	//if (info == NULL)		
	//	LOG("Can not use type > %s", typeString.c_str());

	return info;
}

AString splitTemplateType(AString type, bool bTs, bool *bIsBaseType = NULL)
{
	AString tempType = type.c_str();
	AString templateType = tempType.SplitBlock('<', '>');
	FieldInfo info = GetFieldInfoByTypeString(templateType);
	if (bTs)
	{
		if (info == NULL)
			return templateType;

		const char *szType = "";
		switch (info->getType())
		{

		case FIELD_STRING:
		case FIELD_CHAR_STRING:
			szType = "string";
			break;

		case FIELD_INT:
		case FIELD_SHORT:
		case FIELD_INT64:
		case FIELD_UINT64:
		case FIELD_FLOAT:
		case FIELD_DOUBLE:
			szType = "number";
			break;

		case  FIELD_BOOL:
			szType = "boolean";
			break;

		case FIELD_ARRAYDATA:
			szType = "[]";
			break;
		}

		if (szType == "")
		{
			LOG("Type > %s can not to TS", info->getTypeString());
		}

		return szType;
	}


	// c++
	if (info == NULL)
		return templateType;

	const char *szType = "";
	switch (info->getType())
	{

	case FIELD_STRING:
	case FIELD_CHAR_STRING:
		szType = "AString";
		break;

	case FIELD_INT:
		szType = "int";
		break;
	case FIELD_SHORT:
		szType = "short";
		break;
	case FIELD_INT64:
		szType = "Int64";
		break;
	case FIELD_UINT64:
		szType = "UInt64";
		break;
	case FIELD_FLOAT:
		szType = "float";
		break;
	case FIELD_DOUBLE:
		szType = "double";
		break;

	case  FIELD_BOOL:
		szType = "bool";
		break;

	case FIELD_ARRAYDATA:
		szType = "";		// 不允许嵌套
		LOG("C++ Array 不允许再次嵌套 数组");
		break;
	}

	if (szType == "")
	{
		LOG("Type > %s can not to CPP", info->getTypeString());
	}
	else if (bIsBaseType != NULL)
		*bIsBaseType = true;

	return szType;
}

AString GetNoteString(AString &scrString)
{
	AString noteString = scrString;
	bool bHave = strstr(scrString.c_str(), "//") != NULL;
	AString temp = noteString.SplitLeft("//");
	if (temp.length() > 0)
	{
		scrString = temp;
		scrString.replace(" ", "");
		scrString.replace("\t", "");
		return noteString;
	}
	if (bHave)
	{
		scrString.setNull();
		return noteString;
	}
	scrString.replace(" ", "");
	scrString.replace("\t", "");
	return AString();
}



AString GetMemberNote(AutoNice noteData, const AString &memberName)
{
	AString strNote = noteData[memberName];
	if (strNote.length() > 0)
	{
		AString temp = "// ";
		return temp + strNote;
	}
	return AString();
}

void ClearSpaceString(AString &infoStr)
{
	while (infoStr.length() > 0)
	{
		if (*infoStr.c_str() == ' ' || *infoStr.c_str() == '\t')
		{
			AString temp = infoStr.c_str() + 1;
			infoStr = temp;
		}
		else
			break;
	}
}
// 获取TS数据类型, 如 "int>", 或者 int , arraydata ...
AString GetTsType(AString typeString, AutoNice proList, bool bSetDefault)
{
	AString tempType = typeString;
	AString leftType = tempType.SplitLeft(">");
	FieldInfo info = NULL;
	if (leftType.length() > 0)
	{
		if (proList->exist(leftType.c_str()))
			return leftType;

		info = FieldInfoManager::getFieldInfo(leftType.c_str());
	}
	else
	{
		if (proList->exist(typeString.c_str()))
			return typeString;
		info = FieldInfoManager::getFieldInfo(typeString.c_str());
	}
	if (info == NULL)
	{
		LOG("Type > %s can not to TS", typeString.c_str());
		return "";
	}
	const char *szType = "";
	switch (info->getType())
	{

	case FIELD_STRING:
	case FIELD_CHAR_STRING:
		szType = bSetDefault ? ": string = \"\"" : "string";
		break;

	case FIELD_BYTE:
	case FIELD_INT:
	case FIELD_SHORT:
	case FIELD_INT64:
	case FIELD_UINT64:
	case FIELD_FLOAT:
	case FIELD_DOUBLE:
		szType = bSetDefault ? ": number = 0" : "number";
		break;

	case  FIELD_BOOL:
		szType = bSetDefault ? ": boolean = false" : "boolean";
		break;

	case FIELD_ARRAYDATA:
		szType = bSetDefault ? " = []" : "[]";
		break;
	}

	if (szType == "")
	{
		LOG("Type > %s can not to TS", info->getTypeString());
	}

	return szType;
}
//-------------------------------------------------------------------------

AString GenerateMsgProtocolCppCode(AutoNice proList, AutoNice proNotes, Array<AString> &structNameList, EasyHash<AString, bool> &exportRunHash, bool bFirst, bool bRunConfig, bool bIncludeMsgStruct)
{
	AString cppCode = "//Auto genereate msg data code		";
	//cppCode += TimeManager::GetMe().GetDateTime();
	cppCode += "\r\n#pragma once\r\n\r\n";

	cppCode += "#include \"BaseMsg.h\"\r\n#include \"Array.h\"\r\n#include \"ArrayList.h\"\r\n\r\n";

	if (!bFirst && !bRunConfig && bIncludeMsgStruct)
		cppCode += "#include \"MsgStruct.h\"\r\n\r\n";

	for (int i = 0; i < structNameList.size(); ++i)
	{
		AString name = structNameList[i];

		AutoNice d = (tNiceData*)proList[name];

		if (!d)
			continue;

		// 如果开头为 CC_ 则不导出C++, 表示客户端 TS专用
		if (name.c_str()[0] == 'C' && name.c_str()[1] == 'C')
			continue;

		AutoNice noteData = proNotes[name];

		AString strInfo = proNotes[name + "_info_"];
		if (strInfo.length() > 0)
		{
			cppCode += "// ";
			cppCode += strInfo;
			cppCode += "\r\n";
		}

		cppCode += "class ";
		cppCode += name;
		if (d->exist("mRequestID"))
			cppCode += " : public tRequestMsg";
		else
			cppCode += " : public tBaseMsg";
		AString structNote = GetMemberNote(noteData, "_struct_");
		if (structNote.length() > 0)
		{
			cppCode += "	";
			cppCode += structNote;
		}
		cppCode += "\r\n{ \r\npublic:\r\n";

		AString getCode;
		AString code;
		AString toDataCode;

		AString clearCode;
		AString saveCode;
		AString copyCode;

		AString getIntTypeCode;
		AString setIntTypeCode;

		bool bHaveArray = false;
		bool bOnlyOneArray = exportRunHash.exist(name) && !exportRunHash.find(name);
		if (bOnlyOneArray && d->count() != 1)
		{
			LOG("如果只导出为一个数组结构, 只能定义一个arraydata<>成员, 当前按一般结构处理");
			bOnlyOneArray = false;
		}

		for (auto m = d->begin(); m; ++m)
		{
			AString key = m.key();
			AString type = m.get();

			// 找不到类型, 即作为存在的结构处理
			FieldInfo info = GetFieldInfoByTypeString(type);
			if (info == NULL || proList->exist(type.c_str()))
			{
				if (type.sub(0, 3) == "map")
				{
					bool bIsBase = false;
					AString subType = splitTemplateType(type, false, &bIsBase);
					if (subType.length() > 0)
					{
					}
					LOG("Type error > %s ", type.c_str());
					continue;
				}

				code.Format("    %s %s;		%s\r\n", type.c_str(), m.key().c_str(), GetMemberNote(noteData, m.key()).c_str());
				cppCode += code;

				code.Format("        {\r\n            AutoNice d = (tNiceData*)scrData[\"%s\"];\r\n            if (d) %s.Full(d); else LOG(\"No exist data %s\");\r\n        }\r\n", m.key().c_str(), m.key().c_str(), m.key().c_str());
				getCode += code;

				code.Format("        {\r\n            AutoNice d = destData->NewNice(); d->msKey = \"%s\"; %s.ToData(d);\r\n            destData[\"%s\"] = d.getPtr();\r\n        }\r\n", type.c_str(), m.key().c_str(), m.key().c_str());
				toDataCode += code;

				code.Format("        %s.clear(false);\r\n", key.c_str());
				clearCode += code;

				code.Format("        SAVE_MSG_STRUCT(%s);\r\n", key.c_str());
				saveCode += code;

				code.Format("        %s.copy(other.%s);\r\n", key.c_str(), key.c_str());
				copyCode += code;

				continue;
			}

			//FieldInfo info = GetFieldInfoByTypeString(type);
			//if (info == NULL)
			//{
			//	LOG("Table %s > %s : %s can not to c++", name.c_str(), m.key().c_str(), type.c_str());
			//	continue;
			//}

			const char *szType = NULL;
			AString clearString;
			switch (info->getType())
			{
			case FIELD_STRING:
			case FIELD_CHAR_STRING:
				szType = "AString";
				clearString = ".setNull()";
				break;

			case FIELD_BYTE:
				szType = "byte";
				clearString = " = 0";
				break;

			case FIELD_INT:
				szType = "int";
				clearString = " = 0";
				break;

			case FIELD_SHORT:
				szType = "short";
				clearString = " = 0";
				break;

			case FIELD_INT64:
				szType = "Int64";
				clearString = " = 0";
				break;

			case FIELD_UINT64:
				szType = "UInt64";
				clearString = " = 0";
				break;

			case FIELD_FLOAT:
				szType = "float";
				clearString = " = 0";
				break;

			case FIELD_DOUBLE:
				szType = "double";
				clearString = " = 0";
				break;

			case  FIELD_BOOL:
				szType = "bool";
				clearString = " = false";
				break;

			case FIELD_ARRAYDATA:
				szType = "ArrayList";
				clearString = ".clear(false)";
				break;

			case  FIELD_NICEDATA:
			{
				szType = "AutoNice";
				clearString = ".setNull()";
				break;
			}
			case FIELD_DATA:
			{
				szType = "AutoData";
				clearString = ".setNull()";
				break;
			}
			}
			if (szType == NULL)
			{
				LOG("Table %s > %s : %s can not to c++", name.c_str(), m.key().c_str(), info->getTypeString());
				continue;
			}
			else
			{

				if (info->getType() == FIELD_NICEDATA || info->getType()==FIELD_DATA)
					code.Format("        if (%s) %s%s;\r\n", key.c_str(), key.c_str(), clearString.c_str());
				else
				{
					code.Format("        %s%s;\r\n", key.c_str(), clearString.c_str());
					if (info->getType() != FIELD_ARRAYDATA)
					{
						AString str;
						str.Format("        if (strcmp(szMember, \"%s\")==0) { AData value; value = %s; return value; }\r\n", key.c_str(), key.c_str());
						getIntTypeCode += str;

						str.Format("        if (strcmp(szMember, \"%s\")==0) { %s = value; return true; };\r\n", key.c_str(), key.c_str());
						setIntTypeCode += str;
					}
				}
				clearCode += code;

				if (info->getType() == FIELD_ARRAYDATA)
				{
					bHaveArray = true;
					bool bIsBase = false;
					code.Format("    %s<%s> %s;	%s\r\n", szType, splitTemplateType(type, false, &bIsBase).c_str(), m.key().c_str(), GetMemberNote(noteData, m.key()).c_str());
					cppCode += code;
					if (bIsBase)
					{
						if (bOnlyOneArray)
							code.Format("        FullArray(scrData, %s);\r\n", m.key().c_str());
						else
							code.Format("        FullArray(scrData, %s, \"%s\");\r\n", m.key().c_str(), m.key().c_str());
					}
					else
						code.Format("        FullStuctArray(scrData, %s, \"%s\");\r\n", m.key().c_str(), m.key().c_str());

					getCode += code;

					if (bIsBase)
					{
						if (bOnlyOneArray)
							code.Format("        ArrayToData(destData, %s);\r\n", m.key().c_str());
						else
							code.Format("        ArrayToData(destData, %s, \"%s\");\r\n", m.key().c_str(), m.key().c_str());
					}
					else
						code.Format("        StuctArrayToData(destData, %s, \"%s\");\r\n", m.key().c_str(), m.key().c_str());

					toDataCode += code;

					// ArrayNiceData:: serialize
					if (bIsBase)
					{
						if (bOnlyOneArray)
							code.Format("        SaveArray(%s, destData);\r\n", key.c_str());
						else
							code.Format("        SaveArray(\"%s\", %s, destData);\r\n", key.c_str(), key.c_str());
					}
					else
						code.Format("        SaveMsgArray(\"%s\", %s, destData);\r\n", key.c_str(), key.c_str());
					saveCode += code;

					if (bIsBase)
						code.Format("        %s = other.%s;\r\n", key.c_str(), key.c_str());
					else
						code.Format("        CopyMsgArray(other.%s, %s);\r\n", key.c_str(), key.c_str());
					copyCode += code;
				}
				else
				{
					code.Format("    %s %s;		%s\r\n", szType, m.key().c_str(), GetMemberNote(noteData, m.key()).c_str());
					cppCode += code;

					if (info->getType() == FIELD_NICEDATA)
					{
						code.Format("        %s = (tNiceData*)scrData[\"%s\"];\r\n", m.key().c_str(), m.key().c_str());
						getCode += code;

						code.Format("        destData[\"%s\"] = %s.getPtr();\r\n", m.key().c_str(), m.key().c_str());
						toDataCode += code;

						// AutoNiceFieldInfo::saveData
						code.Format("        SAVE_MSG_NICE(%s);\r\n", key.c_str());
						saveCode += code;

						code.Format("        COPY_MSG_NICE(other.%s, %s);\r\n", key.c_str(), key.c_str());
						copyCode += code;
					}
					else if (info->getType() == FIELD_DATA)
					{
						code.Format("        %s = (DataStream*)scrData[\"%s\"];\r\n", m.key().c_str(), m.key().c_str());
						getCode += code;

						code.Format("        destData[\"%s\"] = %s.getPtr();\r\n", m.key().c_str(), m.key().c_str());
						toDataCode += code;

						// AutoNiceFieldInfo::saveData
						code.Format("        SAVE_MSG_DATA(%s);\r\n", key.c_str());
						saveCode += code;

						code.Format("        COPY_MSG_DATA(other.%s, %s);\r\n", key.c_str(), key.c_str());
						copyCode += code;
					}
					else
					{
						code.Format("        CheckGet(scrData, %s);\r\n", m.key().c_str(), m.key().c_str());
						getCode += code;

						code.Format("        destData[\"%s\"] = %s;\r\n", m.key().c_str(), m.key().c_str());
						toDataCode += code;

						code.Format("        SAVE_MSG_VALUE(%s, %d);\r\n", key.c_str(), info->getType());
						saveCode += code;

						code.Format("        %s = other.%s;\r\n", key.c_str(), key.c_str());
						copyCode += code;
					}
				}
			}
		}

		code.Format("\r\npublic:\r\n    %s() { clear(false); };\r\n\r\n", name.c_str());
		cppCode += code;

		cppCode += "\r\n   virtual  void Full(AutoNice scrData) override\r\n    {\r\n        clear(false);\r\n";
		cppCode += getCode;
		cppCode += "    }\r\n\r\n";

		cppCode += "    virtual void ToData(AutoNice &destData) override\r\n    {\r\n";
		cppCode += toDataCode;
		cppCode += "    }\r\n\r\n";

		//cppCode += "\r\n    virtual void FullFromRecord(ARecord scrData) override\r\n    {\r\n        clear(false);\r\n";
		//cppCode += getCode;
		//cppCode += "    }\r\n\r\n";

		//cppCode += "    virtual void SaveToRecord(ARecord &destData) override\r\n    {\r\n";
		//cppCode += toDataCode;
		//cppCode += "    }\r\n\r\n";

		cppCode += "    bool serialize(DataStream *destData) const override\r\n    {\r\n";
		if (bOnlyOneArray)
		{
			code.setNull();
			if (!bHaveArray)
				LOG("只导出数组时, 必须定义为数据类型成员 arraydata<>");
		}
		else
			code.Format("        destData->write((short)%d);\r\n\r\n", (int)d->count());

		cppCode += code;
		cppCode += saveCode;
		cppCode += "        return true;\r\n    }\r\n\r\n";

		cppCode += "    void clear(bool bClearBuffer=false) override \r\n    {\r\n";
		cppCode += clearCode;
		cppCode += "    }\r\n\r\n";

		code.Format("    void copy(const tBaseMsg &otherMsg) override \r\n    {\r\n        if (strcmp(\"%s\", otherMsg.GetMsgName())!=0) { LOG(\"%%s is not %s\", otherMsg.GetMsgName()); return; }; const %s &other = *(const %s*)(&otherMsg);\r\n", name.c_str(), name.c_str(), name.c_str(), name.c_str());
		cppCode += code;
		cppCode += copyCode;
		cppCode += "    }\r\n\r\n";

		code.Format("    virtual const char* GetMsgName() const override { return \"%s\"; }\r\n\r\n", name.c_str());
		cppCode += code;

		if (d->exist("mRequestID"))
		{
			code.Format("    virtual void SetRequestID(MSG_ID id)  override { mRequestID = (int)id; }\r\n\r\n");
			cppCode += code;

			code.Format("    virtual MSG_ID GetRequestID()  override { return (MSG_ID)mRequestID; }\r\n\r\n");
			cppCode += code;
		}

		if (exportRunHash.find(name))
			cppCode += "    virtual int Run(ShareMemCloudDBNode *pNode, const AString &tableName, AutoNice &paramData, AutoNice &resultData) override;\r\n\r\n";

		cppCode += "    AData get(const char *szMember) const \r\n    {\r\n";
		cppCode += getIntTypeCode;
		cppCode += "        return AData();\r\n    }\r\n\r\n";

		cppCode += "    bool set(const char *szMember, AData value) \r\n    {\r\n";
		cppCode += setIntTypeCode;
		cppCode += "        LOG(\"No exist > %%s\", szMember);  return false;\r\n    }\r\n\r\n";

		cppCode += "    AData operator [] (const char *szMember) const \r\n    {\r\n";
		cppCode += "        return get(szMember);\r\n";
		cppCode += "    }\r\n\r\n";

		cppCode += "    AData operator [] (const AString &member) const \r\n    {\r\n";
		cppCode += "        return get(member.c_str());\r\n";
		cppCode += "    }\r\n\r\n";

		cppCode += "};\r\n\r\n";
	}

	if (bRunConfig)
	{
		AString infoString = proNotes->ToJSON();

		AString str = Base64Tool::encode(infoString.c_str(), infoString.length());

		cppCode += "class _comment {\r\npublic:\r\n    static const char* CommentString()\r\n     {\r\n        static const char *gCommentString = \r\n                \"";
		cppCode += str;
		cppCode += "\";\r\n        return gCommentString; \r\n    }\r\n };\r\n\r\n";
	}
	return cppCode;
}

AString GenerateMsgProtocolTSCode(AutoNice proList, AutoNice proNotes, Array<AString> &structNameList, bool bFirst)
{
	AString headCode = "//Auto genereate msg data code		";
	//headCode += TimeManager::GetMe().GetDateTime();
	headCode += "\r\n\r\n";

	headCode += "import BaseMsg from \"../Base/BaseMsg\"\r\n";
	headCode += "import NiceData, { AData } from \"../Base/NiceData\"\r\n\r\n";

	AString secordString;

	EasyHash<AString, bool>  tempImportList;

	AString tsCode;

	if (bFirst)
	{
		secordString = "export default class tBaseMsg extends BaseMsg\r\n"
			"{\r\n"
			"    public static CreateData(className) :tBaseMsg\r\n    {\r\n"
			"        switch (className)\r\n        {\r\n";
		//		case StudentProperty.name:
		//			return new StudentProperty();
		//		case BaseProperty.name:
		//			return new BaseProperty();
		//	}
		//	return null;
		//}
	}
	else
		tsCode += "import tBaseMsg from \"./MsgStruct\"\r\n\r\n";

	//for (auto it = proList->begin(); it; ++it)
	for (int i = 0; i < structNameList.size(); ++i)
	{
		AString name = structNameList[i];

		AutoNice d = (tNiceData*)proList[name];

		AString typeInfoCode;
		AString arrayElementCode;
		AString isMsgCode;
		AString resetClearCode;

		if (!d)
			continue;

		if (bFirst)
		{
			AString code;
			code.Format("            case \"%s\":\r\n                return new %s();\r\n", name.c_str(), name.c_str());
			secordString += code;
		}

		AString strInfo = proNotes[name + "_info_"];
		if (strInfo.length() > 0)
		{
			tsCode += "// ";
			tsCode += strInfo;
			tsCode += "\r\n";
		}

		tsCode += "export class ";
		tsCode += name;
		tsCode += "  extends tBaseMsg";

		AutoNice noteData = (tNiceData*)proNotes[name];
		AString structNote = GetMemberNote(noteData, "_struct_");
		if (structNote.length() > 0)
		{
			tsCode += "	";
			tsCode += structNote;
		}

		tsCode += "\r\n{\r\n";
		{
			AString code;
			code.Format("    public static msMsgName = \"%s\";\r\n\r\n", name.c_str());
			tsCode += code;
		}

		AString code;
		for (auto m = d->begin(); m; ++m)
		{
			AString type = m.get();

			// 找不到类型, 即作为存在的结构处理
			FieldInfo info = GetFieldInfoByTypeString(type);
			if (info == NULL || proList->exist(type.c_str()))
			{
				if (type == "any")
				{
					code.Format("    public %s :any = null;		%s\r\n", m.key().c_str(), GetMemberNote(noteData, m.key()).c_str());
					tsCode += code;
					continue;
				}
				if (type.sub(0, 3) == "map")
				{
					AString temp = type;
					AString subType = temp.SplitBlock('<', '>');
					if (subType.length() > 0)
					{
						AString tsType = GetTsType(subType, proList, false);
						if (tsType.length() > 0)
						{
							code.Format("    public %s :{[key:number]:%s} = {};		%s\r\n", m.key().c_str(), tsType.c_str(), GetMemberNote(noteData, m.key()).c_str());
							tsCode += code;
							continue;
						}
					}
					LOG(" type error > %s", type);
					continue;
				}

				code.Format("    public %s:{} = new %s();	// Msg	%s\r\n", m.key().c_str(), type.c_str(), GetMemberNote(noteData, m.key()).c_str());
				tsCode += code;

				code.Format("        this.%s = new %s();\r\n", m.key().c_str(), type.c_str());
				resetClearCode += code;

				// 加到是否为消息类型的函数代码
				AString tempIsMsg;
				tempIsMsg.Format("           case \"%s\": return true;\r\n", m.key().c_str());
				isMsgCode += tempIsMsg;

				// 加入到类型信息中
				AString tempTypeString;
				tempTypeString.Format("            case \"%s\":  return NiceData._getType(%d);		// Object\r\n", m.key().c_str(), FIELD_NICEDATA);
				typeInfoCode += tempTypeString;

				if (!proList->exist(type.c_str()))
				{
					code.Format("import {%s} from \"./MsgStruct\"\r\n", type.c_str());
					if (!tempImportList.exist(code))
					{
						tempImportList.insert(code, true);
						headCode += code;
					}
				}

				continue;
			}

			AString tempTypeString;
			tempTypeString.Format("            case \"%s\":  return NiceData._getType(%d);		// %s\r\n", m.key().c_str(), info->getType(), info->getTypeString());
			typeInfoCode += tempTypeString;

			//if (info == NULL)
			//{
			//	LOG("Table %s > %s : %s can not to TS", name.c_str(), m.key().c_str(), type.c_str());
			//	continue;
			//}
			AString szType = "";
			switch (info->getType())
			{
			case FIELD_STRING:
			case FIELD_CHAR_STRING:
				szType = ": string = \"\"";
				break;

			case FIELD_BYTE:
			case FIELD_INT:
			case FIELD_SHORT:
			case FIELD_INT64:
			case FIELD_UINT64:
			case FIELD_FLOAT:
			case FIELD_DOUBLE:
				szType = ": number = 0";
				break;

			case  FIELD_BOOL:
				szType = ": boolean = false";
				break;

			case FIELD_ARRAYDATA:
			{
				AString tempTemp = type;
				AString str = splitTemplateType(type, true);

				szType.Format(": %s[] = []", str.c_str());

				AString templateType = tempTemp.SplitBlock('<', '>');
				FieldInfo info = GetFieldInfoByTypeString(templateType);
				if (info == NULL && !proList->exist(str.c_str()))
				{
					code.Format("import {%s} from \"./MsgStruct\"\r\n", str.c_str());
					if (!tempImportList.exist(code))
					{
						tempImportList.insert(code, true);
						headCode += code;
					}
				}

				if (info == NULL)
				{
					// 追加上消息数组新建元素
					AString elementString;
					elementString.Format("            case \"%s\": return new  %s();\r\n", m.key().c_str(), str.c_str());
					arrayElementCode += elementString;
				}

				break;
			}
			case  FIELD_NICEDATA:
			{
				szType.Format(": Object = {}");
				break;
			}
			}
			if (szType == "")
			{
				LOG("Table %s > %s : %s can not to TS", name.c_str(), info->getName().c_str(), info->getTypeString());
				continue;
			}
			else
			{
				code.Format("    public %s %s;	// %s	%s\r\n", m.key().c_str(), szType.c_str(), info->getTypeString(), GetMemberNote(noteData, m.key()).c_str());
				tsCode += code;

				szType.SplitLeft("=");
				code.Format("        this.%s =%s;\r\n", m.key().c_str(), szType.c_str());
				resetClearCode += code;
			}
		}

		tsCode += "\r\n";
		tsCode += "    public GetType(memberName:string):AData\r\n    {\r\n";
		tsCode += "        switch (memberName)\r\n        {\r\n";
		tsCode += typeInfoCode;
		tsCode += "        }\r\n        return null;\r\n";
		tsCode += "    }\r\n";

		// 新建数组元素
		tsCode += "\r\n";
		tsCode += "    public NewArrayElement(arrayName:string): BaseMsg\r\n    {\r\n";
		tsCode += "        switch (arrayName)\r\n        {\r\n";
		tsCode += arrayElementCode;
		tsCode += "        }\r\n        return null;\r\n";
		tsCode += "    }\r\n";

		// 是否为子消息类型
		tsCode += "\r\n";
		tsCode += "    public IsMsg(memberName) :boolean\r\n    {\r\n";
		tsCode += "        switch (memberName)\r\n        {\r\n";
		tsCode += isMsgCode;
		tsCode += "        }\r\n        return false;\r\n";
		tsCode += "    }\r\n";

		// 初始清理
		tsCode += "\r\n";
		tsCode += "    public ResetClear()\r\n    {\r\n";
		tsCode += resetClearCode;
		tsCode += "    }\r\n";


		tsCode += "}\r\n\r\n";

	}
	headCode += "\r\n";
	if (bFirst)
		secordString += "        }\r\n        return null;\r\n    }\r\n}\r\n\r\n";
	AString comString = headCode + secordString;
	return comString + tsCode;
}

// 导出C# 消息代码
AString GenerateMsgProtocolC4Code(AutoNice proList, AutoNice proNotes, Array<AString> &structNameList, EasyHash<AString, bool> &exportRunHash, bool bFirst, bool bRunConfig, bool bIncludeMsgStruct)
{
	AString cppCode = "//Auto genereate msg data code		";
	//cppCode += TimeManager::GetMe().GetDateTime();
	cppCode += "\r\n"
		"using System;\r\n"
		"using System.Collections;\r\n"
		"using System.Collections.Generic;\r\n"
		"using Logic;\r\n"
		"\r\n\r\n";



	for (int i = 0; i < structNameList.size(); ++i)
	{
		AString name = structNameList[i];

		AutoNice d = (tNiceData*)proList[name];

		if (!d)
			continue;

		AutoNice noteData = proNotes[name];

		AString strInfo = proNotes[name + "_info_"];
		if (strInfo.length() > 0)
		{
			cppCode += "// ";
			cppCode += strInfo;
			cppCode += "\r\n";
		}

		cppCode += "public class ";
		cppCode += name;
		if (d->exist("mRequestID"))
			cppCode += " : RequestPacket";
		else
			cppCode += " : BasePacket";
		AString structNote = GetMemberNote(noteData, "_struct_");
		if (structNote.length() > 0)
		{
			cppCode += "	";
			cppCode += structNote;
		}
		cppCode += "\r\n{\r\n";

		AString getCode;
		AString code;
		//AString toDataCode;

		AString clearCode;
		//AString saveCode;
		//AString copyCode;

		//AString getIntTypeCode;
		//AString setIntTypeCode;

		bool bHaveArray = false;
		bool bOnlyOneArray = exportRunHash.exist(name) && !exportRunHash.find(name);
		if (bOnlyOneArray && d->count() != 1)
		{
			LOG("如果只导出为一个数组结构, 只能定义一个arraydata<>成员, 当前按一般结构处理");
			bOnlyOneArray = false;
		}

		for (auto m = d->begin(); m; ++m)
		{
			AString key = m.key();
			AString type = m.get();

			// 找不到类型, 即作为存在的结构处理
			FieldInfo info = GetFieldInfoByTypeString(type);
			if (info == NULL || proList->exist(type.c_str()))
			{
				if (type.sub(0, 3) == "map")
				{
					bool bIsBase = false;
					AString subType = splitTemplateType(type, false, &bIsBase);
					if (subType.length() > 0)
					{
					}
					LOG("Type error > %s ", type.c_str());
					continue;
				}

				code.Format("    %s %s;		%s\r\n", type.c_str(), m.key().c_str(), GetMemberNote(noteData, m.key()).c_str());
				cppCode += code;

				code.Format("        {\r\n            AutoNice d = (tNiceData*)scrData[\"%s\"];\r\n            if (d) %s.Full(d); else LOG(\"No exist data %s\");\r\n        }\r\n", m.key().c_str(), m.key().c_str(), m.key().c_str());
				getCode += code;

				//code.Format("        {\r\n            AutoNice d = destData->NewNice(); d->msKey = \"%s\"; %s.ToData(d);\r\n            destData[\"%s\"] = d.getPtr();\r\n        }\r\n", type.c_str(), m.key().c_str(), m.key().c_str());
				//toDataCode += code;

				code.Format("        %s.clear(false);\r\n", key.c_str());
				clearCode += code;

				//code.Format("        SAVE_MSG_STRUCT(%s);\r\n", key.c_str());
				//saveCode += code;

				//code.Format("        %s.copy(other.%s);\r\n", key.c_str(), key.c_str());
				//copyCode += code;

				continue;
			}

			//FieldInfo info = GetFieldInfoByTypeString(type);
			//if (info == NULL)
			//{
			//	LOG("Table %s > %s : %s can not to c++", name.c_str(), m.key().c_str(), type.c_str());
			//	continue;
			//}

			const char *szType = NULL;
			AString clearString;
			switch (info->getType())
			{
			case FIELD_STRING:
			case FIELD_CHAR_STRING:
				szType = "string";
				clearString = " = \"\"";
				break;

			case FIELD_BYTE:
				szType = "byte";
				clearString = " = 0";
				break;

			case FIELD_INT:
				szType = "int";
				clearString = " = 0";
				break;

			case FIELD_SHORT:
				szType = "short";
				clearString = " = 0";
				break;

			case FIELD_INT64:
				szType = "Int64";
				clearString = " = 0";
				break;

			case FIELD_UINT64:
				szType = "UInt64";
				clearString = " = 0";
				break;

			case FIELD_FLOAT:
				szType = "float";
				clearString = " = 0";
				break;

			case FIELD_DOUBLE:
				szType = "double";
				clearString = " = 0";
				break;

			case  FIELD_BOOL:
				szType = "bool";
				clearString = " = false";
				break;

			case FIELD_ARRAYDATA:
				szType = "ArrayList";
				clearString = ".clear(false)";
				break;

			case  FIELD_NICEDATA:
			{
				szType = "NiceData";
				clearString = ".clear()";
				break;
			}
			case FIELD_DATA:
			{
				szType = "DataBuffer";
				clearString = ".clear(true)";
				break;
			}
			}
			if (szType == NULL)
			{
				LOG("Table %s > %s : %s can not to c++", name.c_str(), m.key().c_str(), info->getTypeString());
				continue;
			}
			else
			{

				if (info->getType() == FIELD_NICEDATA || info->getType() == FIELD_DATA)
					code.Format("        if (%s!=null) %s%s;\r\n", key.c_str(), key.c_str(), clearString.c_str());
				else
				{
					code.Format("        %s%s;\r\n", key.c_str(), clearString.c_str());

				}
				clearCode += code;

				if (info->getType() == FIELD_ARRAYDATA)
				{
					bHaveArray = true;
					bool bIsBase = false;
					code.Format("    %s<%s> %s;	%s\r\n", szType, splitTemplateType(type, false, &bIsBase).c_str(), m.key().c_str(), GetMemberNote(noteData, m.key()).c_str());
					cppCode += code;
					if (bIsBase)
					{
						if (bOnlyOneArray)
							code.Format("        FullArray(scrData, %s);\r\n", m.key().c_str());
						else
							code.Format("        FullArray(scrData, %s, \"%s\");\r\n", m.key().c_str(), m.key().c_str());
					}
					else
						code.Format("        FullStuctArray(scrData, %s, \"%s\");\r\n", m.key().c_str(), m.key().c_str());

					getCode += code;

					//if (bIsBase)
					//{
					//	if (bOnlyOneArray)
					//		code.Format("        ArrayToData(destData, %s);\r\n", m.key().c_str());
					//	else
					//		code.Format("        ArrayToData(destData, %s, \"%s\");\r\n", m.key().c_str(), m.key().c_str());
					//}
					//else
					//	code.Format("        StuctArrayToData(destData, %s, \"%s\");\r\n", m.key().c_str(), m.key().c_str());

					//toDataCode += code;

					// ArrayNiceData:: serialize
					//if (bIsBase)
					//{
					//	if (bOnlyOneArray)
					//		code.Format("        SaveArray(%s, destData);\r\n", key.c_str());
					//	else
					//		code.Format("        SaveArray(\"%s\", %s, destData);\r\n", key.c_str(), key.c_str());
					//}
					//else
					//	code.Format("        SaveMsgArray(\"%s\", %s, destData);\r\n", key.c_str(), key.c_str());
					//saveCode += code;

					//if (bIsBase)
					//	code.Format("        %s = other.%s;\r\n", key.c_str(), key.c_str());
					//else
					//	code.Format("        CopyMsgArray(other.%s, %s);\r\n", key.c_str(), key.c_str());
					//copyCode += code;
				}
				else
				{
					code.Format("    public %s %s		%s\r\n", szType, m.key().c_str(), GetMemberNote(noteData, m.key()).c_str());
					cppCode += code;

					if (info->getType() == FIELD_NICEDATA || info->getType() == FIELD_DATA)
						code.Format("    {\r\n        set { mMsgData.set(\"%s\",  value); }\r\n        get { return mMsgData.getObject(\"%s\") as %s; }\r\n    }\r\n\r\n", m.key().c_str(), m.key().c_str(), szType);
					else
						code.Format("    {\r\n        set { mMsgData.set(\"%s\",  value); }\r\n        get { return (%s)mMsgData.getObject(\"%s\"); }\r\n    }\r\n\r\n", m.key().c_str(), szType, m.key().c_str());
					cppCode += code;

					//if (info->getType() == FIELD_NICEDATA)
					//{
					//	code.Format("        %s = (tNiceData*)scrData[\"%s\"];\r\n", m.key().c_str(), m.key().c_str());
					//	getCode += code;

					//	//code.Format("        destData[\"%s\"] = %s.getPtr();\r\n", m.key().c_str(), m.key().c_str());
					//	//toDataCode += code;

					//	//// AutoNiceFieldInfo::saveData
					//	//code.Format("        SAVE_MSG_NICE(%s);\r\n", key.c_str());
					//	//saveCode += code;

					//	//code.Format("        COPY_MSG_NICE(other.%s, %s);\r\n", key.c_str(), key.c_str());
					//	//copyCode += code;
					//}
					//else if (info->getType() == FIELD_DATA)
					//{
					//	code.Format("        %s = scrData.getObject(\"%s\") as DataBuffer;\r\n", m.key().c_str(), m.key().c_str());
					//	getCode += code;

					//	//code.Format("        destData[\"%s\"] = %s.getPtr();\r\n", m.key().c_str(), m.key().c_str());
					//	//toDataCode += code;

					//	//// AutoNiceFieldInfo::saveData
					//	//code.Format("        SAVE_MSG_DATA(%s);\r\n", key.c_str());
					//	//saveCode += code;

					//	//code.Format("        COPY_MSG_DATA(other.%s, %s);\r\n", key.c_str(), key.c_str());
					//	//copyCode += code;
					//}
					//else
					{
						//mMsgData.set("mRequestID", scrData.getObject("mRequestID");
						code.Format("        mMsgData.set(\"%s\", scrData.getObject(\"%s\"));\r\n", m.key().c_str(), m.key().c_str());
						getCode += code;

						//code.Format("        destData[\"%s\"] = %s;\r\n", m.key().c_str(), m.key().c_str());
						//toDataCode += code;

						//code.Format("        SAVE_MSG_VALUE(%s, %d);\r\n", key.c_str(), info->getType());
						//saveCode += code;

						//code.Format("        %s = other.%s;\r\n", key.c_str(), key.c_str());
						//copyCode += code;
					}
				}
			}
		}

		code.Format("\r\n\r\n    public %s() { InitData(); }\r\n\r\n", name.c_str());
		cppCode += code;

		cppCode += "\r\n   public override  void Full(NiceData scrData) \r\n    {\r\n        InitData();\r\n";
		cppCode += getCode;
		cppCode += "    }\r\n\r\n";

		//cppCode += "    virtual void ToData(AutoNice &destData) override\r\n    {\r\n";
		//cppCode += toDataCode;
		//cppCode += "    }\r\n\r\n";

		//cppCode += "\r\n    virtual void FullFromRecord(ARecord scrData) override\r\n    {\r\n        clear(false);\r\n";
		//cppCode += getCode;
		//cppCode += "    }\r\n\r\n";

		//cppCode += "    virtual void SaveToRecord(ARecord &destData) override\r\n    {\r\n";
		//cppCode += toDataCode;
		//cppCode += "    }\r\n\r\n";

		//cppCode += "    bool serialize(DataStream *destData)\r\n    {\r\n";
		//if (bOnlyOneArray)
		//{
		//	code.setNull();
		//	if (!bHaveArray)
		//		LOG("只导出数组时, 必须定义为数据类型成员 arraydata<>");
		//}
		//else
		//	code.Format("        destData->write((short)%d);\r\n\r\n", (int)d->count());

		//cppCode += code;
		//cppCode += saveCode;
		//cppCode += "        return true;\r\n    }\r\n\r\n";

		cppCode += "    public override void InitData() \r\n    {\r\n";
		cppCode += clearCode;
		cppCode += "    }\r\n\r\n";

		//code.Format("    void copy(const tBaseMsg &otherMsg) override \r\n    {\r\n        if (strcmp(\"%s\", otherMsg.GetMsgName())!=0) { LOG(\"%%s is not %s\", otherMsg.GetMsgName()); return; }; const %s &other = *(const %s*)(&otherMsg);\r\n", name.c_str(), name.c_str(), name.c_str(), name.c_str());
		//cppCode += code;
		//cppCode += copyCode;
		//cppCode += "    }\r\n\r\n";

		code.Format("    public override string MsgName()   { return \"%s\"; }\r\n\r\n", name.c_str());
		cppCode += code;

		//if (d->exist("mRequestID"))
		//{
		//	code.Format("    override void SetRequestID(MSG_ID id)   { mRequestID = (int)id; }\r\n\r\n");
		//	cppCode += code;

		//	code.Format("    override MSG_ID GetRequestID()  override { return (MSG_ID)mRequestID; }\r\n\r\n");
		//	cppCode += code;
		//}

		//if (exportRunHash.find(name))
		//	cppCode += "    virtual int Run(ShareMemCloudDBNode *pNode, const AString &tableName, AutoNice &paramData, AutoNice &resultData) override;\r\n\r\n";

		//cppCode += "    AData get(const char *szMember) const \r\n    {\r\n";
		//cppCode += getIntTypeCode;
		//cppCode += "        return AData();\r\n    }\r\n\r\n";

		//cppCode += "    bool set(const char *szMember, AData value) \r\n    {\r\n";
		//cppCode += setIntTypeCode;
		//cppCode += "        LOG(\"No exist > %%s\", szMember);  return false;\r\n    }\r\n\r\n";

		//cppCode += "    AData operator [] (const char *szMember) const \r\n    {\r\n";
		//cppCode += "        return get(szMember);\r\n";
		//cppCode += "    }\r\n\r\n";

		//cppCode += "    AData operator [] (const AString &member) const \r\n    {\r\n";
		//cppCode += "        return get(member.c_str());\r\n";
		//cppCode += "    }\r\n\r\n";

		cppCode += "};\r\n\r\n";
	}

	if (bRunConfig)
	{
		AString infoString = proNotes->ToJSON();

		AString str = Base64Tool::encode(infoString.c_str(), infoString.length());

		cppCode += "class _comment {\r\npublic:\r\n    static const char* CommentString()\r\n     {\r\n        static const char *gCommentString = \r\n                \"";
		cppCode += str;
		cppCode += "\";\r\n        return gCommentString; \r\n    }\r\n };\r\n\r\n";
	}
	return cppCode;
}

AutoNice GenerateProtocol(const AString &fileName, const AString tsPath, const AString cppPath, const AString &targetName, bool bTSCode, AString error, bool bExportComment = false, bool bIncludeMsgStruct = false)
{
	FileDataStream  f(fileName.c_str(), FILE_READ);
	AutoData  d = f.readAllData();
	if (!d || d->dataSize() <= 0)
	{	
		error.Format("%s 不存在或内容为空", fileName.c_str());
		MessageBox(NULL, error.c_str(), "错误", 0);
		return AutoNice();
	}
	d->resize(d->dataSize() + 3);
	d->data()[d->size() - 3] = '\r';
	d->data()[d->size() - 2] = '\n';
	d->data()[d->size() - 1] = '\0';

	Array<AString> structNameList;

	AutoNice  proList = MEM_NEW NiceData();

	AutoNice proNotes = MEM_NEW NiceData();

	AString  strPro = (const char*)d->data();
	//strPro.replace("\r\n", "");
	EasyHash<AString, bool> exportRunHash;

	EasyHash<AString, bool> runConfigList;

	AString lastStructInfo;

	while (true)
	{
		if (strPro.length() <= 0)
			break;

		AString lineString = strPro.SplitLeft("\r\n");
		if (lineString.length() <= 0)
		{
			if (strstr(strPro.c_str(), "\r\n") == NULL)
			{
				lineString = strPro;
				strPro.setNull();
			}
			else
				continue;
		}

		AString strInfo = GetNoteString(lineString);

		lineString.replace(" ", "");
		lineString.replace("\t", "");

		if (lineString.length() <= 0)
		{
			if (strInfo.length() > 0)
				lastStructInfo = strInfo;
			continue;
		}
		bool bIsRequest = false;

		AString name = lineString;
		if (strstr(name.c_str(), ":DB") != NULL)
		{
			name = lineString.SplitLeft(":DB");
			exportRunHash.insert(name, true);
		}
		else if (strstr(name.c_str(), ":ARRAY") != NULL)		// 分析是否直接导出为一个数组结构
		{
			name = lineString.SplitLeft(":ARRAY");
			exportRunHash.insert(name, false);				// 插入到需要导出 Run 列表, 但值为false
		}
		else if (strstr(name.c_str(), ":CONFIG") != NULL)		// 分析是否直接导出为一个数组结构
		{
			name = lineString.SplitLeft(":CONFIG");
			runConfigList.insert(name, true);				// 插入到需要导出 Run 列表, 但值为false
		}
		else if (strstr(name.c_str(), ":RQ") != NULL)
		{
			name = lineString.SplitLeft(":RQ");
			bIsRequest = true;
			//exportRunHash.insert(name, true);
		}

		AString dataString = strPro.SplitBlock();

		if (dataString.length() <= 0)
			continue;

		AutoNice noteData = MEM_NEW NiceData();
		proNotes[name] = noteData.getPtr();
		AutoNice orderNameList = MEM_NEW ArrayNiceData();
		proNotes[name + "__list__"] = orderNameList.getPtr();

		noteData["_struct_"] = strInfo;

		if (lineString.length() > 0)
			proNotes[name + "_info_"] = lastStructInfo;
		lastStructInfo.setNull();

		structNameList.push_back(name);


		AutoNice field = MEM_NEW NiceData();

		if (bIsRequest)
			field["mRequestID"] = "int";

		int i = 0;
		while (true)
		{
			if (dataString.length() <= 0)
				break;
			// int kkk;  //说明\r\n
			AString lineString = dataString.SplitLeft("\r\n");
			if (lineString.length() <= 0)
				continue;

			ClearSpaceString(lineString);

			AString noteStr;
			AString infoStr = lineString.SplitLeft("//");
			if (infoStr.length() <= 0)
				infoStr = lineString.SplitLeft(";");
			else
			{
				AString temp = infoStr.SplitLeft(";");
				infoStr = temp;
				noteStr = lineString;
			}

			if (infoStr.length() <= 0)
				continue;

			// 去掉前面的空格或Tab符号
			while (infoStr.length() > 0)
			{
				if (*infoStr.c_str() == ' ' || *infoStr.c_str() == '\t')
				{
					AString temp = infoStr.c_str() + 1;
					infoStr = temp;
				}
				else
					break;
			}

			infoStr.replace('\t', ' ');
			AString type = infoStr.SplitLeft(" ");
			if (type.length() <= 0 || infoStr.length() <= 0)
				break;

			orderNameList[orderNameList->count()] = infoStr;
			if (proList->exist(type.c_str()))
			{
				AutoNice existType = (tNiceData*)proList->get(type);
				if (existType)
				{
					field[infoStr] = type;
					if (noteStr.length() > 0)
					{
						noteData[infoStr] = noteStr;
					}
					noteData[infoStr + "__type__"] = type;
					continue;
				}
			}

			FieldInfo info = GetFieldInfoByTypeString(type);
			if (info != NULL)
				field[infoStr] = type;
			else
			{
				//LOG("Can not use type > %s, to %s", type.c_str(), infoStr.c_str());
				// 有可能是MsgStruct 中的协议
				field[infoStr] = type;
			}

			if (noteStr.length() > 0)
			{
				noteData[infoStr] = noteStr;
			}
		}

		proList[name] = field.getPtr();

	}

	//LOG("-------------------\r\n%s", proList->dump().c_str());
	// 生成TS

	AString tsFileName = tsPath;
	tsFileName += "/";
	tsFileName += targetName;
	tsFileName += ".ts";
	if (bTSCode)
	{
		AString tsCode = GenerateMsgProtocolTSCode(proList, proNotes, structNameList, false);

		//LOG(" code > \r\n%s", tsCode.c_str());
		FileDataStream  tsFile(tsFileName.c_str(), FILE_CREATE_UTF8);

		tsFile._write((void*)tsCode.c_str(), tsCode.length());
		tsFile.close();

		LOG("Genereate ts code > n%s", tsFileName.c_str());
	}
	//------------------------------------------------------------------------
	{
		// 生成C++
		AString cppCode = GenerateMsgProtocolCppCode(proList, proNotes, structNameList, exportRunHash, false, bExportComment, bIncludeMsgStruct);

		//LOG("C++ ----------------------\r\n%s", proNotes->dump().c_str());

		AString codeFileName = cppPath;
		codeFileName += "/";
		codeFileName += targetName;
		codeFileName += ".h";
		FileDataStream  cppCodeFile(codeFileName.c_str(), FILE_CREATE_UTF8);
		cppCodeFile._write((void*)cppCode.c_str(), cppCode.length());
		cppCodeFile.close();

		LOG("Genereate c++ code > n%s", codeFileName.c_str());
	}

	{
		// 生成C#
		AString cppCode = GenerateMsgProtocolC4Code(proList, proNotes, structNameList, exportRunHash, false, bExportComment, bIncludeMsgStruct);

		//LOG("C++ ----------------------\r\n%s", proNotes->dump().c_str());

		AString codeFileName = cppPath;
		codeFileName += "/";
		codeFileName += targetName;
		codeFileName += ".cs";
		FileDataStream  cppCodeFile(codeFileName.c_str(), FILE_CREATE_UTF8);
		cppCodeFile._write((void*)cppCode.c_str(), cppCode.length());
		cppCodeFile.close();

		LOG("Genereate c++ code > n%s", codeFileName.c_str());
	}

	return proNotes;
}
