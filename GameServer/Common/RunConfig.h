#pragma once

#include "RunConfigStruct.h"
#include "FileDataStream.h"
#include "base64.h"

class RunConfig
{
public:
	static RunConfigData& GetConfig()
	{
		thread_local static RunConfigData	msConfig;
		return msConfig;
	}

public:
	static bool InitLoad()
	{
		AutoNice configNice = ReadJson("./RunConfig/Run.json");

		if (!configNice)
			return false;

		// 读取当前的
		GetConfig().Full(configNice);

		return true;
	}

	static AutoNice ReadJson(const AString &fileName)
	{
		FileDataStream  f(fileName.c_str(), FILE_READ);
		if (f)
		{
			AutoData d = f.readAllData();

			AutoNice  existConfig = MEM_NEW NiceData();
			AString json;
			json.set(d->data(), d->dataSize());
			if (existConfig->FullJSON(json))
				return existConfig;
		}
		return AutoNice();
	}

	static AutoNice LoadCheckConfig(const char *szConfigIndex, AString &info, const char *szTypeName)
	{
		if (!InitLoad())
		{
			info.Format("./RunConfig/Run.json no exist or JSON format error, please check");
			return false;
		}
		RunConfigData  configData;
		AutoNice d = MEM_NEW NiceData();
		configData.ToData(d);

		if (!d->exist(szConfigIndex))
		{
			info.Format("No exist config template > %s", szConfigIndex);
			return AutoNice();
		}

		AutoNice templateConfig = (tNiceData*)d->get(szConfigIndex);
		if (!templateConfig)
		{
			info.Format("No exist config template > %s", szConfigIndex);
			return AutoNice();
		}

		AutoNice configNice = ReadJson("./RunConfig/Run.json");
		if (!configNice)
		{
			info.Format("./RunConfig/Run.json no exist or JSON format error, please check");
			return AutoNice();
		}

		if (!configNice ->exist(szConfigIndex))
		{
			info.Format("No exist config > %s", szConfigIndex);
			return AutoNice();
		}

		AutoNice config = (tNiceData*) configNice->get(szConfigIndex);
		if (!config)
		{
			info.Format("No exist config > %s", szConfigIndex);
			return AutoNice();
		}

		if (!CheckSame(templateConfig, config, info, szConfigIndex))
		{
			//info.Format("Now config is not same of template > %s", szConfigIndex);
			return AutoNice();
		}

		// 检测类型是否一至
		AutoNice commInfo = MEM_NEW NiceData();
		AString str = Base64Tool::decode(_comment::CommentString());
		commInfo->FullJSON(str);
		//NOTE_LOG("%s", commInfo->dump().c_str());

		AutoNice childComm = (tNiceData*)commInfo->get("RunConfigData");
		if (childComm)
		{
			AString key = szConfigIndex;
			AString typeName = childComm->get(key + "__type__");
			if (typeName != szTypeName)
			{
				info.Format("Config %s is not same of template type  %s !=  type %s", szConfigIndex, szTypeName, typeName.c_str());
				return AutoNice();
			}
		}
		else
		{
			info.Format("No exist template for type > %s : type %s", szConfigIndex, szTypeName);
			return AutoNice();
		}

		return config;
	}

	static bool CheckSame(AutoNice templateConfig, AutoNice config, AString &info, AString name)
	{
		bool bResult = true;
		for (auto it = templateConfig->begin(); it; ++it)
		{
			AString key = it.key();
				if (!config->exist(key.c_str()))
				{
					AString temp;
					temp.Format("No exist config [\"%s\"] ::  %s\r\n", key.c_str(), name.c_str());
					info += temp;
						bResult = false;
					continue;
				}
			AutoNice d = (tNiceData*)it.get();
			if (d)
			{
				AutoNice existConfig = (tNiceData*)config->get(key);
				if (!existConfig)
				{
					AString temp;
					temp.Format("Now [\"%s\"] is not  config struct ::  %s\r\n", key.c_str(), name.c_str());
					info += temp;
					bResult = false;
				}
				else
				{
					AString nextName = name;
					nextName += ":";
					nextName += key;
					bool b = CheckSame(d, existConfig, info, nextName);
					if (!b)
						bResult = false;
				}
			}

			AData existData = config->get(key);
			if (it.get().getType() != existData.getType())
			{
				tFieldInfo *p = FieldInfoManager::GetMe().getFieldInfo(existData.getType());
				tFieldInfo *pT = FieldInfoManager::GetMe().getFieldInfo(it.get().getType());
				AString temp;
				temp.Format("Now [\"%s\"] is not  same type %s != template %s  ::  %s\r\n", key.c_str(), p!=NULL ? p->getTypeString():"Null", pT!=NULL ? pT->getTypeString():"Null", name);
				info += temp;
				bResult = false;
			}
		}
		return bResult;
	}

	static bool GenerateRunConfigJson(AString &info)
	{
		AString configPath = "./RunConfig";

		// 读取注释
		//AString commPath = configPath;
		//commPath += "/RunInfo.json";

		//AutoNice commInfo = RunConfig::ReadJson(commPath.c_str());
		AutoNice commInfo = MEM_NEW NiceData();
		AString str = Base64Tool::decode(_comment::CommentString());
		commInfo->FullJSON(str);

		AutoNice childComm = (tNiceData*)commInfo->get("RunConfigData");
		if (!childComm)
			childComm = commInfo;

		AString configFileName = configPath;
		configFileName += "/Run.json";

		RunConfigData	config;

		// 读取当前的
		AutoNice existConfig = RunConfig::ReadJson(configFileName.c_str());
		if (existConfig)
			config.Full(existConfig);

		Auto<NiceData> resultData = MEM_NEW NiceData();
		AutoNice d = resultData;
		config.ToData(d);
		d->msKey = "RunConfigData";
		
		AString resultJson = resultData->ToJSONEx(commInfo.getPtr(), childComm.getPtr());
		FileDataStream  wf(configFileName.c_str(), FILE_CREATE);
		if (wf)
		{
			wf._write((void*)resultJson.c_str(), resultJson.length());
			info.Format("Generate run config > %s", configFileName.c_str());
			return true;
		}
		else
		{
			info.Format("Fail create run config > %s", configFileName.c_str());
			return false;
		}
	}

	static bool ShowConfig(const char *szConfigName, AString &info)
	{
		AutoNice commInfo = MEM_NEW NiceData();
		AString str = Base64Tool::decode(_comment::CommentString());
		commInfo->FullJSON(str);
		//NOTE_LOG("%s", commInfo->dump().c_str());

		AutoNice childComm = (tNiceData*)commInfo->get("RunConfigData");
		if (childComm)
		{
			AString key = szConfigName;
			AString typeName = childComm->get(key + "__type__");
			childComm = (tNiceData*)commInfo->get(typeName);
		}
		AString configFileName = "./RunConfig/Run.json";

		// 读取当前的
		AutoNice existConfig = RunConfig::ReadJson(configFileName.c_str());
		if (existConfig)
		{
			Auto<NiceData> resultData = (tNiceData*)existConfig->get(szConfigName);

			AString typeKey = szConfigName;

			//resultData->msKey = szConfigName;
			AutoNice comm = (tNiceData*)commInfo->get("RunConfigData");
			AString typeName;
			if (comm)
				typeName = comm->get(typeKey + "__type__");
			resultData->msKey = typeName;

			//printf("%s", childComm->dump().c_str());

			AString resultJson = resultData->ToJSONEx(commInfo.getPtr(), childComm.getPtr());
			if (resultJson.length() > 0)
			{
				info = szConfigName;
				info += "\r\n";
				info += resultJson;
				return true;
			}
		}
		
		{
			info.Format("Now no exist config > %s", szConfigName);
			return false;
		}
	}

	static bool ShowConfig(AutoNice config, const char *szConfigName, AString &info)
	{
		AutoNice commInfo = MEM_NEW NiceData();
		AString str = Base64Tool::decode(_comment::CommentString());
		commInfo->FullJSON(str);
		//NOTE_LOG("%s", commInfo->dump().c_str());

		AutoNice childComm = (tNiceData*)commInfo->get("RunConfigData");
		if (childComm)
		{
			AString key = szConfigName;
			AString typeName = childComm->get(key + "__type__");
			childComm = (tNiceData*)commInfo->get(typeName);
		}
		AString configFileName = "./RunConfig/Run.json";

		// 读取当前的
		Auto<NiceData> resultData = config;

		AString typeKey = szConfigName;

		//resultData->msKey = szConfigName;
		AutoNice comm = (tNiceData*)commInfo->get("RunConfigData");
		AString typeName;
		if (comm)
			typeName = comm->get(typeKey + "__type__");
		resultData->msKey = typeName;

		//printf("%s", childComm->dump().c_str());

		AString resultJson = resultData->ToJSONEx(commInfo.getPtr(), childComm.getPtr());
		if (resultJson.length() > 0)
		{
			info = szConfigName;
			info += "\r\n";
			info += resultJson;
			return true;
		}

		{
			info.Format("Now no exist config > %s", szConfigName);
			return false;
		}
	}
};
