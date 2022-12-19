#include "GenerateCodeTS.h"

#include "FileDataStream.h"
#include "TimeManager.h"


AString GenerateCodeTS::GenerateCode(TableMap &tableList, AString path, const char *szDBClassName)
{
	AString tsCode = "//Auto generate from ExcelTool ";
	tsCode += VERSION;
	tsCode += "\r\n//";
	// tsCode += TimeManager::GetMe().GetDateTime();
	tsCode += "\r\n\r\n";

	tsCode += "import {BaseRecord, BaseTable } from \"../Base/BaseTable\"\r\n\r\n";	
	tsCode += "import DataBuffer from \"../Base/Base\"\r\n";
	tsCode += "import NiceData from \"../Base/NiceData\"\r\n";	
	tsCode += "import { tEvent, tEventFactory, CEvent, EventCenter, tServerEvent, tClientEvent, AsyncServerEvent, AsyncServerEventFactory } from \"../Base/Event\"\r\n";
	tsCode += "import {BaseManager, BaseFactory} from \"../Base/BaseManager\"\r\n";
	tsCode += "import {UIManager, RegisterWindow} from \"./UIManager\"\r\n";

	tsCode += "\r\n";

	tsCode += "export class RegisterBase\r\n{\r\n";
	tsCode += "	public constructor(baseName:string, baseClass)\r\n	{\r\n";		
	//tsCode += "		//let baseName = baseClass[\"name\"];\r\n";
	tsCode += "        if ((baseName as string) == null || baseClass == null) { LOG.LogError(\" Register fail > \" + baseName.toString()); return; }\r\n";
	tsCode += "		com.msMgr.Register(baseName, new BaseFactory(baseClass));\r\n	}\r\n}\r\n\r\n";

	AString code;
	AString staticDefine;

	for (auto it = tableList.begin(); it; ++it)
	{
		AutoTable table = it.get();
		tsCode += GenerateConfigTS(table);

		code.Format("	public static %s = %s;\r\n", table->GetTableName(), table->GetTableName());
		staticDefine += code;
	}

	code.Format("export default class com \r\n{\r\n", szDBClassName);
	tsCode += code; 

	tsCode += "	public static msMgr:BaseManager = new BaseManager();\r\n";

	tsCode += "	public static RootNode:cc.Node;\r\n";
	tsCode += "	public static UINode:cc.Node;\r\n";
	tsCode += "	public static SceneNode:cc.Node;\r\n";
	tsCode += "	public static CameraNode:cc.Node;\r\n\r\n";
	
	tsCode += "	public static NiceData = NiceData\r\n";
	tsCode += "	public static DataBuffer = DataBuffer\r\n";
	tsCode += "	public static BaseRecord = BaseRecord\r\n";
	tsCode += "	public static BaseTable = BaseTable\r\n";
	tsCode += "	public static tEvent = tEvent\r\n";
	tsCode += "	public static CEvent = CEvent\r\n";
	tsCode += "	public static tEventFactory = tEventFactory\r\n"; 
	tsCode += "	public static EventCenter = EventCenter\r\n";	
	tsCode += "	public static RegisterBase = RegisterBase\r\n";
	tsCode += "	public static BaseManager = BaseManager;\r\n";
	tsCode += "	public static UIManager = UIManager;\r\n";
	tsCode += "	public static RegisterWindow = RegisterWindow;\r\n\r\n";
	
	tsCode += "	// All Config\r\n";
	tsCode += staticDefine;
	tsCode += "\r\n";

	tsCode += "	public static Init(tableList: Object)\r\n	{\r\n";
	tsCode += "		 let bReadyCheck = true\r\n";

	//AString exportString;

	for (auto it = tableList.begin(); it; ++it)
	{
		AutoTable table = it.get();
		code.Format("			&& %s.Init(tableList[\"%s\"])\r\n", table->GetTableName(), table->GetTableName());
		tsCode += code;
		
		//exportString += ", ";
		//exportString += table->GetTableName();
	}

	tsCode += "			;\r\n";

	tsCode += "		LOG.LogError(\"Config ready > \"+bReadyCheck?\"succeed\":\"fail\");\r\n";
	tsCode += "	}\r\n\r\n";

	tsCode += "	 // 注意使用 await 调用\r\n";
	tsCode += "	public static async Await(waitSce:number): Promise<void> {\r\n";
	tsCode += "		var promise = new Promise<void>(resolve => {\r\n";
	tsCode += "			setTimeout(()=>{ resolve();}, waitSce*1000);\r\n";
	tsCode += "			});\r\n";
	tsCode += "		return promise;\r\n	}\r\n\r\n";
	 
	tsCode += "	 // Load (resources)\r\n";
	tsCode += "	public static async Load(res: string, type?:any) : Promise<any>{\r\n";
	tsCode += "	 	var promise = new Promise<any>(resolve => {\r\n";
	tsCode += "	 		var onResourceLoaded = function(errorMessage, loadedResource) {\r\n";
	tsCode += "	 			if (errorMessage) {\r\n";
	tsCode += "	 				LOG.Log('Prefab error:' + errorMessage);\r\n";
	tsCode += "	 				resolve(null);\r\n";
	tsCode += "	 				return;\r\n";
	tsCode += "	 			}\r\n";
	tsCode += "	 			else\r\n";
	tsCode += "	 				resolve(loadedResource);\r\n";
	tsCode += "	 		};\r\n";
	tsCode += "         if (type!=null)\r\n";
	tsCode += "             cc.resources.load(res, type, onResourceLoaded);\r\n"; 
	tsCode += "        else\r\n";
	tsCode += "            cc.resources.load(res, onResourceLoaded);\r\n";
	tsCode += "	 	});\r\n";
	tsCode += "	 	return promise;\r\n";
	tsCode += "	 }\r\n";

	tsCode += "}\r\n\r\n";

	tsCode += "export { com }\r\n";
	//tsCode += exportString;
	//tsCode += " }\r\n";

	FileDataStream::CreateMultiDirectory(path.c_str());

	AString file = path;
	//file += szDBClassName;
	file += "com.ts";
	FileDataStream tsFile(file.c_str(), FILE_CREATE_UTF8);
	tsFile._write((void*)tsCode.c_str(), tsCode.length());
	tsFile.close();

	AString result = "Succeed generate TS code > ";
	result += file;
	return result;
}

AString GenerateCodeTS::GenerateConfigTS(AutoTable table, bool bDefault)
{
	const char *szConfigName = table->GetTableName();
	AString tsCode;
	if (bDefault)
		tsCode.Format("export default class %s\r\n{\r\n", szConfigName);
	else
		tsCode.Format("export class %s\r\n{\r\n", szConfigName);
	for (int i = 0; i < table->GetField()->getCount(); ++i)
	{
		FieldInfo info = table->GetField()->getFieldInfo(i);
		AString code;
		const char *szType = NULL;
		switch (info->getType())
		{
			case FIELD_STRING:
			case FIELD_CHAR_STRING:
				szType = ": string = \"\"";
				break;

			case FIELD_NICEDATA:
				szType = ": Object = {}";
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
				szType = " = []";
				break;

			case FIELD_DATA:
				szType = ": egret.ByteArray = null;";
				break;				
		}
		if (szType == NULL)
		{
			ERROR_LOG("Table %s > %s : %s can not to TS", szConfigName, info->getName().c_str(), info->getTypeString());
		}
		code.Format("	public %s %s;\r\n", info->getName().c_str(), szType);
		tsCode += code;
	}

	if (bDefault)
	{
		tsCode += "}\r\n";
		return tsCode;
	}

	tsCode += "\r\n	public static msTable:BaseTable = null;   \r\n\r\n";

	AString code;

	tsCode += "	public constructor(key:any=null)\r\n	{ \r\n";
	tsCode += "		if (key==null)  return;\r\n";
	code.Format("		if (%s.msTable==null)\r\n		{\r\n			LOG.LogError(\"%s config no exist >\"+key.toString());\r\n			return;\r\n		}\r\n", szConfigName, szConfigName);
	tsCode += code;

	code.Format("		let record:BaseRecord = %s.msTable.GetRecord(key);\r\n", szConfigName);
	tsCode += code;

	tsCode += "		if (record!=null)\r\n			record.FullConfig(this);\r\n";
	code.Format("		else\r\n			LOG.LogError(\"%s config table no exist record > \"+key.toString());\r\n", szConfigName);
	tsCode += code;
	tsCode += "	}\r\n\r\n";


	code.Format("	public static GetConfig(key:any) : %s\r\n	{\r\n", szConfigName);
	tsCode += code;
	code.Format("		return new %s(key);\r\n	}\r\n\r\n", szConfigName);
	tsCode += code;

	code.Format("	public static Create(record:BaseRecord) : %s\r\n	{\r\n", szConfigName);
	tsCode += code;
	code.Format("		let re = new %s();\r\n        record.FullConfig(re);\r\n        return re;\r\n	}\r\n\r\n", szConfigName);
	tsCode += code;
			
	code.Format("	public static Init(table:BaseTable)\r\n	{\r\n		%s.msTable = table;\r\n", szConfigName);
	tsCode += code;

	code.Format("		if (table==null)\r\n			LOG.LogError(\"%s table is null\");\r\n		return table!=null;\r\n", szConfigName);
	tsCode += code;

	tsCode += "	}\r\n\r\n}\r\n\r\n";

	return tsCode;
}

//AString GetMemberNote(AutoNice noteData, const AString &memberName)
//{
//	AString strNote = noteData[memberName];
//	if (strNote.length() > 0)
//	{
//		AString temp = "// ";
//		return temp + strNote;
//	}
//	return AString();
//}
//
//void ClearSpaceString(AString &infoStr)
//{
//	while (infoStr.length() > 0)
//	{
//		if (*infoStr.c_str() == ' ' || *infoStr.c_str() == '\t')
//		{
//			AString temp = infoStr.c_str() + 1;
//			infoStr = temp;
//		}
//		else
//			break;
//	}
//}
//
//FieldInfo GetFieldInfoByTypeString(AString typeString)
//{
//	AString tempType = typeString;
//	AString leftType = tempType.SplitLeft("<");
//	leftType.toUpper();
//	FieldInfo info = NULL;
//	if (leftType.length() > 0)
//		info = FieldInfoManager::getFieldInfo(leftType.c_str());
//	else
//	{
//		tempType.toUpper();
//		info = FieldInfoManager::getFieldInfo(tempType.c_str());
//	}
//	//if (info == NULL)		
//	//	LOG("Can not use type > %s", typeString.c_str());
//
//	return info;
//}
//
//AString splitTemplateType(AString type, bool bTs, bool *bIsBaseType = NULL)
//{
//	AString tempType = type.c_str();
//	AString templateType = tempType.SplitBlock('<', '>');
//	FieldInfo info = GetFieldInfoByTypeString(templateType);
//	if (bTs)
//	{
//		if (info == NULL)
//			return templateType;
//
//		const char *szType = "";
//		switch (info->getType())
//		{
//
//		case FIELD_STRING:
//		case FIELD_CHAR_STRING:
//			szType = "string";
//			break;
//
//		case FIELD_INT:
//		case FIELD_SHORT:
//		case FIELD_INT64:
//		case FIELD_UINT64:
//		case FIELD_FLOAT:
//		case FIELD_DOUBLE:
//			szType = "number";
//			break;
//
//		case  FIELD_BOOL:
//			szType = "boolean";
//			break;
//
//		case FIELD_ARRAYDATA:
//			szType = "[]";
//			break;
//		}
//
//		if (szType == "")
//		{
//			LOG("Type > %s can not to TS", info->getTypeString());
//		}
//
//		return szType;
//	}
//
//
//	// c++
//	if (info == NULL)
//		return templateType;
//
//	const char *szType = "";
//	switch (info->getType())
//	{
//
//	case FIELD_STRING:
//	case FIELD_CHAR_STRING:
//		szType = "AString";
//		break;
//
//	case FIELD_INT:
//		szType = "int";
//		break;
//	case FIELD_SHORT:
//		szType = "short";
//		break;
//	case FIELD_INT64:
//		szType = "Int64";
//		break;
//	case FIELD_UINT64:
//		szType = "UInt64";
//		break;
//	case FIELD_FLOAT:
//		szType = "float";
//		break;
//	case FIELD_DOUBLE:
//		szType = "double";
//		break;
//
//	case  FIELD_BOOL:
//		szType = "bool";
//		break;
//
//	case FIELD_ARRAYDATA:
//		szType = "";		// 不允许嵌套
//		LOG("C++ Array 不允许再次嵌套 数组");
//		break;
//	}
//
//	if (szType == "")
//	{
//		LOG("Type > %s can not to CPP", info->getTypeString());
//	}
//	else if (bIsBaseType != NULL)
//		*bIsBaseType = true;
//
//	return szType;
//}
//
//AString GetNoteString(AString &scrString)
//{
//	AString noteString = scrString;
//	bool bHave = strstr(scrString.c_str(), "//") != NULL;
//	AString temp = noteString.SplitLeft("//");
//	if (temp.length() > 0)
//	{
//		scrString = temp;
//		scrString.replace(" ", "");
//		scrString.replace("\t", "");
//		return noteString;
//	}
//	if (bHave)
//	{
//		scrString.setNull();
//		return noteString;
//	}
//	scrString.replace(" ", "");
//	scrString.replace("\t", "");
//	return AString();
//}
//
//
//// 获取TS数据类型, 如 "int>", 或者 int , arraydata ...
//AString GetTsType(AString typeString, AutoNice proList, bool bSetDefault)
//{
//	AString tempType = typeString;
//	AString leftType = tempType.SplitLeft(">");
//	FieldInfo info = NULL;
//	if (leftType.length() > 0)
//	{
//		if (proList->exist(leftType.c_str()))
//			return leftType;
//
//		info = FieldInfoManager::getFieldInfo(leftType.c_str());
//	}
//	else
//	{
//		if (proList->exist(typeString.c_str()))
//			return typeString;
//		info = FieldInfoManager::getFieldInfo(typeString.c_str());
//	}
//	if (info == NULL)
//	{
//		LOG("Type > %s can not to TS", typeString.c_str());
//		return "";
//	}
//	const char *szType = "";
//	switch (info->getType())
//	{
//
//	case FIELD_STRING:
//	case FIELD_CHAR_STRING:
//		szType = bSetDefault ? ": string = \"\"" : "string";
//		break;
//
//	case FIELD_BYTE:
//	case FIELD_INT:
//	case FIELD_SHORT:
//	case FIELD_INT64:
//	case FIELD_UINT64:
//	case FIELD_FLOAT:
//	case FIELD_DOUBLE:
//		szType = bSetDefault ? ": number = 0" : "number";
//		break;
//
//	case  FIELD_BOOL:
//		szType = bSetDefault ? ": boolean = false" : "boolean";
//		break;
//
//	case FIELD_ARRAYDATA:
//		szType = bSetDefault ? " = []" : "[]";
//		break;
//	}
//
//	if (szType == "")
//	{
//		LOG("Type > %s can not to TS", info->getTypeString());
//	}
//
//	return szType;
//}
//
//AString GenerateCodeTS::GenerateMsgProtocolTSCode(AutoNice proList, AutoNice proNotes, Array<AString> &structNameList, bool bFirst)
//{
//	AString headCode = "//Auto genereate msg data code		";
//	//headCode += TimeManager::GetMe().GetDateTime();
//	headCode += "\r\n\r\n";
//
//	headCode += "import BaseMsg from \"../Base/BaseMsg\"\r\n";
//	headCode += "import NiceData, { AData } from \"../Base/NiceData\"\r\n\r\n";
//
//	AString secordString;
//
//	EasyHash<AString, bool>  tempImportList;
//
//	AString tsCode;
//
//	if (bFirst)
//	{
//		secordString = "export default class tBaseMsg extends BaseMsg\r\n"
//			"{\r\n"
//			"    public static CreateData(className) :tBaseMsg\r\n    {\r\n"
//			"        switch (className)\r\n        {\r\n";
//		//		case StudentProperty.name:
//		//			return new StudentProperty();
//		//		case BaseProperty.name:
//		//			return new BaseProperty();
//		//	}
//		//	return null;
//		//}
//	}
//	else
//		tsCode += "import tBaseMsg from \"./MsgStruct\"\r\n\r\n";
//
//	//for (auto it = proList->begin(); it; ++it)
//	for (int i = 0; i < structNameList.size(); ++i)
//	{
//		AString name = structNameList[i];
//
//		AutoNice d = (tNiceData*)proList[name];
//
//		AString typeInfoCode;
//		AString arrayElementCode;
//		AString isMsgCode;
//		AString resetClearCode;
//
//		if (!d)
//			continue;
//
//		if (bFirst)
//		{
//			AString code;
//			code.Format("            case \"%s\":\r\n                return new %s();\r\n", name.c_str(), name.c_str());
//			secordString += code;
//		}
//
//		AString strInfo = proNotes[name + "_info_"];
//		if (strInfo.length() > 0)
//		{
//			tsCode += "// ";
//			tsCode += strInfo;
//			tsCode += "\r\n";
//		}
//
//		tsCode += "export class ";
//		tsCode += name;
//		tsCode += "  extends tBaseMsg";
//
//		AutoNice noteData = (tNiceData*)proNotes[name];
//		AString structNote = GetMemberNote(noteData, "_struct_");
//		if (structNote.length() > 0)
//		{
//			tsCode += "	";
//			tsCode += structNote;
//		}
//
//		tsCode += "\r\n{\r\n";
//		{
//			AString code;
//			code.Format("    public static msMsgName = \"%s\";\r\n\r\n", name.c_str());
//			tsCode += code;
//		}
//
//		AString code;
//		for (auto m = d->begin(); m; ++m)
//		{
//			AString type = m.get();
//
//			// 找不到类型, 即作为存在的结构处理
//			FieldInfo info = GetFieldInfoByTypeString(type);
//			if (info == NULL || proList->exist(type.c_str()))
//			{
//				if (type == "any")
//				{
//					code.Format("    public %s :any = null;		%s\r\n", m.key().c_str(), GetMemberNote(noteData, m.key()).c_str());
//					tsCode += code;
//					continue;
//				}
//				if (type.sub(0, 3) == "map")
//				{
//					AString temp = type;
//					AString subType = temp.SplitBlock('<', '>');
//					if (subType.length() > 0)
//					{
//						AString tsType = GetTsType(subType, proList, false);
//						if (tsType.length() > 0)
//						{
//							code.Format("    public %s :{[key:number]:%s} = {};		%s\r\n", m.key().c_str(), tsType.c_str(), GetMemberNote(noteData, m.key()).c_str());
//							tsCode += code;
//							continue;
//						}
//					}
//					LOG(" type error > %s", type);
//					continue;
//				}
//
//				code.Format("    public %s:{} = new %s();	// Msg	%s\r\n", m.key().c_str(), type.c_str(), GetMemberNote(noteData, m.key()).c_str());
//				tsCode += code;
//
//				code.Format("        this.%s = new %s();\r\n", m.key().c_str(), type.c_str());
//				resetClearCode += code;
//
//				// 加到是否为消息类型的函数代码
//				AString tempIsMsg;
//				tempIsMsg.Format("           case \"%s\": return true;\r\n", m.key().c_str());
//				isMsgCode += tempIsMsg;
//
//				// 加入到类型信息中
//				AString tempTypeString;
//				tempTypeString.Format("            case \"%s\":  return NiceData._getType(%d);		// Object\r\n", m.key().c_str(), FIELD_NICEDATA);
//				typeInfoCode += tempTypeString;
//
//				if (!proList->exist(type.c_str()))
//				{
//					code.Format("import {%s} from \"./MsgStruct\"\r\n", type.c_str());
//					if (!tempImportList.exist(code))
//					{
//						tempImportList.insert(code, true);
//						headCode += code;
//					}
//				}
//
//				continue;
//			}
//
//			AString tempTypeString;
//			tempTypeString.Format("            case \"%s\":  return NiceData._getType(%d);		// %s\r\n", m.key().c_str(), info->getType(), info->getTypeString());
//			typeInfoCode += tempTypeString;
//
//			//if (info == NULL)
//			//{
//			//	LOG("Table %s > %s : %s can not to TS", name.c_str(), m.key().c_str(), type.c_str());
//			//	continue;
//			//}
//			AString szType = "";
//			switch (info->getType())
//			{
//			case FIELD_STRING:
//			case FIELD_CHAR_STRING:
//				szType = ": string = \"\"";
//				break;
//
//			case FIELD_BYTE:
//			case FIELD_INT:
//			case FIELD_SHORT:
//			case FIELD_INT64:
//			case FIELD_UINT64:
//			case FIELD_FLOAT:
//			case FIELD_DOUBLE:
//				szType = ": number = 0";
//				break;
//
//			case  FIELD_BOOL:
//				szType = ": boolean = false";
//				break;
//
//			case FIELD_ARRAYDATA:
//			{
//				AString tempTemp = type;
//				AString str = splitTemplateType(type, true);
//
//				szType.Format(": %s[] = []", str.c_str());
//
//				AString templateType = tempTemp.SplitBlock('<', '>');
//				FieldInfo info = GetFieldInfoByTypeString(templateType);
//				if (info == NULL && !proList->exist(str.c_str()))
//				{
//					code.Format("import {%s} from \"./MsgStruct\"\r\n", str.c_str());
//					if (!tempImportList.exist(code))
//					{
//						tempImportList.insert(code, true);
//						headCode += code;
//					}
//				}
//
//				if (info == NULL)
//				{
//					// 追加上消息数组新建元素
//					AString elementString;
//					elementString.Format("            case \"%s\": return new  %s();\r\n", m.key().c_str(), str.c_str());
//					arrayElementCode += elementString;
//				}
//
//				break;
//			}
//			case  FIELD_NICEDATA:
//			{
//				szType.Format(": Object = {}");
//				break;
//			}
//			}
//			if (szType == "")
//			{
//				LOG("Table %s > %s : %s can not to TS", name.c_str(), info->getName().c_str(), info->getTypeString());
//				continue;
//			}
//			else
//			{
//				code.Format("    public %s %s;	// %s	%s\r\n", m.key().c_str(), szType.c_str(), info->getTypeString(), GetMemberNote(noteData, m.key()).c_str());
//				tsCode += code;
//
//				szType.SplitLeft("=");
//				code.Format("        this.%s =%s;\r\n", m.key().c_str(), szType.c_str());
//				resetClearCode += code;
//			}
//		}
//
//		tsCode += "\r\n";
//		tsCode += "    public GetType(memberName:string):AData\r\n    {\r\n";
//		tsCode += "        switch (memberName)\r\n        {\r\n";
//		tsCode += typeInfoCode;
//		tsCode += "        }\r\n        return null;\r\n";
//		tsCode += "    }\r\n";
//
//		// 新建数组元素
//		tsCode += "\r\n";
//		tsCode += "    public NewArrayElement(arrayName:string): BaseMsg\r\n    {\r\n";
//		tsCode += "        switch (arrayName)\r\n        {\r\n";
//		tsCode += arrayElementCode;
//		tsCode += "        }\r\n        return null;\r\n";
//		tsCode += "    }\r\n";
//
//		// 是否为子消息类型
//		tsCode += "\r\n";
//		tsCode += "    public IsMsg(memberName) :boolean\r\n    {\r\n";
//		tsCode += "        switch (memberName)\r\n        {\r\n";
//		tsCode += isMsgCode;
//		tsCode += "        }\r\n        return false;\r\n";
//		tsCode += "    }\r\n";
//
//		// 初始清理
//		tsCode += "\r\n";
//		tsCode += "    public ResetClear()\r\n    {\r\n";
//		tsCode += resetClearCode;
//		tsCode += "    }\r\n";
//
//
//		tsCode += "}\r\n\r\n";
//
//	}
//	headCode += "\r\n";
//	if (bFirst)
//		secordString += "        }\r\n        return null;\r\n    }\r\n}\r\n\r\n";
//	AString comString = headCode + secordString;
//	return comString + tsCode;
//}
