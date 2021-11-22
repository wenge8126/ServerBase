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
			
	code.Format("	public static Init(table:BaseTable)\r\n	{\r\n		%s.msTable = table;\r\n", szConfigName);
	tsCode += code;

	code.Format("		if (table==null)\r\n			LOG.LogError(\"%s table is null\");\r\n		return table!=null;\r\n", szConfigName);
	tsCode += code;

	tsCode += "	}\r\n\r\n}\r\n\r\n";

	return tsCode;
}
