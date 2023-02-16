#pragma once

#include "ExportToolDlg.h"
#include "XlsxTableLoader.h"
#include "FileDataStream.h"
#include "Md5Tool.h"
#include "ResoursePack.h"
#include "ExportBase.h"
#include "GenerateCodeTool.h"
#include "IndexBaseTable.h"
#include "GenerateCodeTS.h"

void CExportToolDlg::GenerateConfigPacketAndCode(bool bGenerateCode)
{
	ClearLog();

	CString configPath;

	mEditConfigPath.GetWindowText(configPath);

	AString listFileName = (LPCTSTR)configPath;
	listFileName += "/";
	listFileName += "ConfigList.xlsx";

	// 打开ConfigList
	AutoTable  configListTable = XlsxTableLoader::LoadXlsx(listFileName, "ConfigList");
	if (!configListTable)
	{
		MessageBox("配置目录内不存在 ConfigTable");
		return;
	}

	CString logFileName = configPath;
	logFileName += "/Warn.log";


	::DeleteFile((LPCSTR)logFileName);

	XlsxTableLoader::msLogFileName = (LPCSTR)logFileName;
	XlsxTableLoader::mbInitLogFile = true;

	//-------------------------------------------------------------------------
	// 读取本地导出MD5记录
	AutoTable recordTable;
	FileDataStream localRecordFile("RunConfig/_ExportRecord.table", FILE_READ);
	AutoData recordData = localRecordFile.readAllData();
	localRecordFile.close();
	if (recordData)
	{
		recordTable = tBaseTable::NewBaseTable();
		recordData->seek(0);
		if (!recordTable->LoadFromData(recordData.getPtr()))
			recordTable.setNull();
	}

	if (!recordTable)
	{
		recordTable = tBaseTable::NewBaseTable();
		recordTable->AppendField("INDEX", FIELD_STRING);
		recordTable->AppendField("MD5", FIELD_STRING);
		recordTable->AppendField("DATA", FIELD_DATA);
	}

	AutoTable xlsxFileIndexTable;

	AutoData listData = recordTable->GetValue("__xlsx_list__", "DATA");
	if (listData)
	{
		xlsxFileIndexTable = tBaseTable::NewBaseTable();
		listData->seek(0);
		if (!xlsxFileIndexTable->LoadFromData(listData.getPtr()))
			xlsxFileIndexTable.setNull();
	}
	if (!xlsxFileIndexTable)
	{
		xlsxFileIndexTable = tBaseTable::NewBaseTable();
		xlsxFileIndexTable->AppendField("INDEX", FIELD_STRING);
		xlsxFileIndexTable->AppendField("MD5", FIELD_STRING);
	}

	EasyHash<AString, bool> checkList; // 已经检查过的列表
	int reloadCount = 0;
	int loadLocalCount = 0;
	UInt64 now = TimeManager::NowTick();
	//-------------------------------------------------------------------------

	mMakeConfigButton.EnableWindow(FALSE);
	mMakeConfigButton.SetWindowText("正在生成中...");

	CString makeTablePath;
	mEditMakeConfigPath.GetWindowText(makeTablePath);

	AString targetPath;
	targetPath.Format("%s/", (LPCTSTR)makeTablePath);
	FileDataStream::CreateMultiDirectory(targetPath.c_str());


	AString makeCSVPath;
	makeCSVPath.Format("%sMake/", targetPath.c_str());
	FileDataStream::CreateMultiDirectory(makeCSVPath.c_str());

	EasyGenerateResourcesPack  packetTool;

	DataTableManager  tempMgr;

	EasyMap<AString, AutoTable> enumTableList;

	// 导出协议的数据
	Array<AString> structNameList;
	AutoNice  proList = MEM_NEW NiceData();
	AutoNice proNotes = MEM_NEW NiceData();

	FastHash<AString, AutoTable> structTableList;

	Array<ARecord>  configArray;
	for (auto it = configListTable->GetRecordIt(); *it; ++(*it))
	{
		ARecord record = it->Record();
		configArray[(int)record[0]] = record;
	}

	for (int i = 0; i < configArray.size(); ++i)
	{
		ARecord record = configArray[i];
		if (!record)
			continue;

		const char *szTableName = record["INDEX"].c_str();
		AString xlsxFile = record["XLSX"];
		xlsxFile = xlsxFile.ANIS();
		AString fileName;
		fileName.Format("%s/%s.xlsx", (LPCTSTR)configPath, xlsxFile.c_str());

		AutoTable table;
		bool bSameLocal = false;
		if (checkList.exist(fileName))
		{
			bSameLocal = checkList.find(fileName);
		}
		else
		{
			FileDataStream xlsxFile(fileName.c_str(), FILE_READ);
			AutoData d = xlsxFile.readAllData();
			if (!d)
			{
				LOG("Read open fail > %s", fileName.c_str());
				continue;
			}

			MD5 md(d->data(), d->dataSize());
			AString md5String = md.toString();
			Data data = xlsxFileIndexTable->GetValue(fileName.c_str(), "MD5");
			AString existMD5;
			if (!data.empty())
				existMD5 = data;
			if (md5String == existMD5)
			{
				checkList.insert(fileName, true);
				bSameLocal = true;
			}
			else
			{
				ARecord re = xlsxFileIndexTable->CreateRecord(fileName, true);
				re["MD5"] = md5String;
				checkList.insert(fileName, false);
			}
		}
		if (bSameLocal)
		{
			Data d = recordTable->GetValue(szTableName, "DATA");
			AutoData tableData;
			if (!d.empty())
				tableData = (DataStream*)recordTable->GetValue(szTableName, "DATA");
			if (tableData)
			{
				table = tBaseTable::NewBaseTable();
				tableData->seek(0);
				if (!table->LoadFromData(tableData.getPtr()))
				{
					LOG("%s Load local data table fail", szTableName);
					table.setNull();
				}
				else
					loadLocalCount++;
			}
		}

		if (!table)
		{
			AutoTable xlxtable = XlsxTableLoader::LoadXlsx(fileName, record["SHEET"].string());
			if (xlxtable)
			{
				table = MEM_NEW SkipBaseTable(INIT_FIELD_MODE::eInitCreateField);
				// 移除掉注释字段 *开关的
				bool bHave = false;
				for (int i = 0; i < xlxtable->GetField()->getCount(); ++i)
				{
					FieldInfo info = xlxtable->GetField()->getFieldInfo(i);
					if (info->getName().c_str()[0] != '*')
					{
						table->AppendField(info->getName().c_str(), (FIELD_TYPE)info->getType());
					}
					else
						bHave = true;
				}

				if (bHave)
				{
					for (auto it = xlxtable->GetRecordIt(); *it; ++(*it))
					{
						ARecord re = it->Record();
						ARecord newRe = table->CreateRecord(re[0].string(), true);
						for (int i = 1; i < table->GetField()->getCount(); ++i)
						{
							newRe[i] = re[table->GetField()->getFieldInfo(i)->getName()];
						}
					}
				}
				else
					table = xlxtable;

				table->SetTableName(szTableName);
				reloadCount++;
			}
			else
			{
				LOG("%s Load xlsx %s , sheet %s fail", szTableName, fileName.c_str(), record["SHEET"].string().ANIS().c_str());
				continue;
			}
		}

		if (table)
		{
			// 打包
			AutoData tableData = MEM_NEW DataBuffer(1024);
			if (table->SaveData(tableData.getPtr()))
			{
				packetTool.AppendResourceData(szTableName, tableData);
				MD5 md(tableData->data(), tableData->dataSize());
				record["MD5"] = md.toString();

				ARecord re = recordTable->CreateRecord(szTableName, true);
				re["MD5"] = record["MD5"];
				re["DATA"] = tableData.getPtr();

				// 上传到DB
				//NiceData param;
				//param["NAME"] = szTableName;
				//param["DATA"] = tableData.getPtr();
				//param["MD5"] = record["MD5"];
				//int nResult = 0;
				//bool b = mDBUnit->RequestDB("t_resource", "", "m_update_resource", &param, 10000, CloudDBCall(&ToolInfo::OnUploadResourceResponse, msTool.getPtr()) );
				//if (!b)
				//	LOG("Update config %s Upload fail : %d", szTableName, nResult);
			}
			else
			{
				LOG("%s save data fail", szTableName);
				continue;
			}

			AString name;
			name.Format("%s%s.csv", makeCSVPath.c_str(), szTableName);
			table->SaveCSV(name.c_str());

			// 导出协议结构(MSG_NAME 配置后, 不再导出配置表格
			AString msgClassName = record["STRUCT_NAME"];
			if (msgClassName.length() > 0)
			{
				if (record["STRUCT_CONFIG"])
				{
					if (!tempMgr.InsertTable(record["INDEX"].c_str(), table, false))
						LOG("Code error, no repeat %s", record["INDEX"].c_str());
				}

				if (table->GetField()->existField("TYPE") && table->GetField()->existField("MEMBER"))
				{
					structNameList.push_back(msgClassName);
					if (table->GetField()->existField("ENUM"))
						structTableList.insert(msgClassName, table);

					AutoNice structInfo = MEM_NEW NiceData();
					proList[msgClassName] = structInfo.getPtr();

					AutoNice noteInfo = MEM_NEW NiceData();
					proNotes[msgClassName] = noteInfo;

					int col = 0;
					for (auto it = table->GetRecordIt(); *it; ++(*it))
					{
						ARecord  memberRe = it->Record();
						if (!memberRe)
							break;
						AString memName = memberRe["MEMBER"];
						AString type = memberRe["TYPE"];
						if (memName.length() > 0 && type.length() > 0)
						{
							structInfo[memName] = type;
							AString infoString = memberRe["DESC"];

							if (memName == "eTask_GoWC")
							{
								col++;
							}
							if (infoString.length() <= 0)
								infoString = memberRe["*DESC"];

							infoString.replace("\t", "");
							infoString.replace("\r", "");
							infoString.replace("\n", "");
							noteInfo[memName] = infoString;

						}
						else
							LOG("%s record %s no set MEMBER or TYPE", szTableName, record[0].c_str());
					}
				}
				else
					LOG("定义协议结构, 必须设置TYPE, ENUM字段, 定义成员的类型, 请正确配置后重新生成");
			}
			else if (record["ONLY_ENUM"])
			{
				enumTableList.insert(record["INDEX"].c_str(), table);
			}
			else	if (!tempMgr.InsertTable(record["INDEX"].c_str(), table, false))
				LOG("Code error, no repeat %s", record["INDEX"].c_str());
		}
		//else
			//LOG("Fail load xlsx  %s, sheet %s, index %s", record["XLSX"].c_str(), record["SHEET"].c_str(), record["INDEX"].c_str());
	}

	configListTable->SaveCSV((targetPath + "ConfigList.csv").c_str());
	LOG("Succeed make list csv table > %s", (targetPath + "ConfigList.csv").c_str());

	AutoData tableData = MEM_NEW DataBuffer(1024);
	if (configListTable->SaveData(tableData.getPtr()))
	{
		packetTool.AppendResourceData("ConfigList", tableData);
	}
	else
		LOG("ConfigList save data fail");

	AString err;
	if (packetTool.GeneratePack((targetPath + "t_configtable.pak").c_str(), err))
		LOG("Succeed generate config table packet >%s", (targetPath + "t_configtable.pak").c_str())
	else
		LOG("File generate config > %s", err.c_str());

	if (bGenerateCode)
	{
		CString path;
		mEditMakeCPath.GetWindowText(path);

		AString codePath; // = targetPath + "Code/";
		//FileDataStream::CreateMultiDirectory(codePath.c_str());
		codePath.Format("%s/", (LPCTSTR)path);

		AString info = GenerateCodeTool::generateDBManager(tempMgr.GetTableMap(), codePath, "ConfigDB");
		//LOG("Generate C++ config code > %s", info.c_str());

		mEditMakeJSPath.GetWindowText(path);
		codePath.Format("%s/", (LPCTSTR)path);

		AString infoTS = GenerateCodeTS::GenerateCode(tempMgr.GetTableMap(), codePath, "ConfigDB");
		//LOG("Generate config TS code > %s", infoTS.c_str());

		// 导出枚举
		for (int i = 0; i < enumTableList.size(); ++i)
		{
			tempMgr.InsertTable(enumTableList.getKey(i).c_str(), enumTableList.get(i), true);
		}
		AString enumCppCode;
		AString enumTsCode = GenerateConfigEnumTsCode(tempMgr.GetTableMap(), enumCppCode);

		// ts 枚举文件
		{
			AString enumFileName = codePath;
			enumFileName += "ConfigEnum.ts";

			FileDataStream enumFile(enumFileName.c_str(), FILE_CREATE_UTF8);
			enumFile._write((void*)enumTsCode.c_str(), enumTsCode.length());
			enumFile.close();

			LOG("Succeed generate ts enum > %s", enumFileName.c_str());
		}
		{
			mEditMakeCPath.GetWindowText(path);
			codePath.Format("%s/", (LPCTSTR)path);
			AString enumFileName = codePath;
			enumFileName += "ConfigEnum.h";

			FileDataStream enumFile(enumFileName.c_str(), FILE_CREATE_UTF8);
			enumFile._write((void*)enumCppCode.c_str(), enumCppCode.length());
			enumFile.close();

			LOG("Succeed generate ts enum > %s", enumFileName.c_str());
		}

		// 导出协议
		{
			AString tsCode = GenerateMsgProtocolTSCode(proList, proNotes, structNameList, true);

			mEditMakeJSPath.GetWindowText(path);
			codePath.Format("%s/MsgStruct.ts", (LPCTSTR)path);

			FileDataStream tsCodeFile(codePath.c_str(), FILE_CREATE_UTF8);
			tsCodeFile._write((void*)tsCode.c_str(), tsCode.length());
			tsCodeFile.close();

			LOG("Succend generate ts msg struct > %s", codePath.c_str());

			EasyHash<AString, bool>  temp;
			AString cppCode = GenerateMsgProtocolCppCode(proList, proNotes, structNameList, temp, true, false, false); // structTableList);

			mEditMakeCPath.GetWindowText(path);
			codePath.Format("%s/MsgStruct.h", (LPCTSTR)path);

			FileDataStream cppCodeFile(codePath.c_str(), FILE_CREATE_UTF8);
			cppCodeFile._write((void*)cppCode.c_str(), cppCode.length());
			cppCodeFile.close();

			LOG("Succend generate c++ msg struct > %s", codePath.c_str());
		}
	}

	// 保存到本地
	{
		AutoData d = MEM_NEW DataBuffer(1024);
		xlsxFileIndexTable->SaveData(d.getPtr());
		ARecord re = recordTable->CreateRecord("__xlsx_list__", true);
		re["DATA"] = d.getPtr();

		DataBuffer allData(16 * 1024 * 1024);
		if (!recordTable->SaveData(&allData))
			LOG("Save all table data  fail");
		FileDataStream localRecordFile("RunConfig/_ExportRecord.table", FILE_CREATE);
		localRecordFile._write(allData.data(), allData.dataSize());
		localRecordFile.close();

		CString path;
		mEditMakeJSPath.GetWindowText(path);
		AString tsPath = (LPCSTR)path;
		tsPath.replace("scripts/Game", "resources/config_table.bin");
		FileDataStream configResourceFile(tsPath.c_str(), FILE_CREATE);

		// 完全压缩
		AutoData zipData = MEM_NEW DataBuffer(1024 * 1024 * 16);
		int x = allData.ZipData(zipData, 0, 0, 0, MAX_SPEED);

		configResourceFile._write(zipData->data(), zipData->dataSize());
		configResourceFile.close();
		LOG("*** Generate config resource file > %s", tsPath.c_str());
	}
	mMakeConfigButton.EnableWindow(TRUE);
	mMakeConfigButton.SetWindowText("生成配置");

	LOG("%s Finish export update reload xlsx %d + local %d = %d, use time %llu", bGenerateCode ? "Generate code : " : "Only make packet :", reloadCount, loadLocalCount, reloadCount + loadLocalCount, TimeManager::NowTick() - now);
}


AString CExportToolDlg::GenerateConfigEnumTsCode(TableMap &tableMap, AString &cppEnumString)
{
	AString tsCode = "//Auto generate from ExcelTool ";
	tsCode += VERSION;
	tsCode += "\r\n//";
	// tsCode += TimeManager::GetMe().GetDateTime();

	cppEnumString = tsCode;
	cppEnumString += "\r\n#pragma once\r\n\r\n";

	tsCode += "\r\n\r\n";

	AString code;
	for (auto it = tableMap.begin(); it; ++it)
	{
		AString configIndex = it.key();
		AutoTable t = it.get();
		if (!t->GetField()->existField("ENUM"))
			continue;
		FieldInfo info = t->GetField()->getFieldInfo("ENUM");
		if (info != NULL)
		{
			int descCol = -1;
			if (t->GetField()->existField("DESC"))
				descCol = t->GetField()->getFieldCol("DESC");
			code.Format("enum Enum%s\r\n{\r\n", configIndex.c_str());
			tsCode += "export ";
			tsCode += code;
			cppEnumString += code;

			EasyHash<AString, bool>  memberList;
			EasyMap<AString, bool> repeatList;
			int repeatCount = 0;
			for (auto recordIt = t->GetRecordIt(); *recordIt; ++(*recordIt))
			{
				ARecord re = recordIt->Record();
				AString enumString = re["ENUM"];
				if (enumString.length() > 0)
				{
					if (memberList.exist(enumString))
					{
						repeatList.insert(enumString, true);
						++repeatCount;
						continue;
					}
					memberList.insert(enumString, true);

					code.Format("    %s = %d,", enumString.c_str(), (int)re[0]);
					tsCode += code;
					cppEnumString += code;

					code.setNull();
					if (descCol >= 0)
					{
						AString desc = re[descCol];
						if (desc.length() > 0)
						{
							code.Format("    //%s", desc.c_str());
						}
					}
					code += "\r\n";

					tsCode += code;
					cppEnumString += code;
				}
			}
			if (repeatCount > 0)
			{
				AString info;
				for (int i = 0; i < repeatList.size(); ++i)
				{
					info + " ";
					info += repeatList.getKey(i);
				}
				//WARN_LOG("%s  repeat member %s : %d", configIndex.c_str(), info.c_str(), repeatCount);
			}
			tsCode += "};\r\n\r\n";
			cppEnumString += "};\r\n\r\n";
		}
	}
	return tsCode;
}
