#pragma once
#include "RunConfig.h"
#include "windows.h"

#define INIT_RUN_CONFIG(configClass, configName) { if (!CRunConfig<configClass>::Init(configName)) { TableTool::CleanUp(); return 0; } }
#define DEFINE_RUN_CONFIG(configClass) configClass CRunConfig<configClass>::mConfig;\
	configClass &config = CRunConfig<configClass>::mConfig;

#define RUN_CONFIG(configClass) configClass CRunConfig<configClass>::mConfig;

#define CHECK_RUN_CONFIG(configClass, configName) if (strstr(::GetCommandLine(), configName)!=NULL) INIT_RUN_CONFIG(configClass, configName)


template<typename Config>
class CRunConfig
{
public:
	// 进程开始时, 放入
	static bool Init(const char *szRunConfigName)
	{
		AString info, info2;
		AutoNice configData = RunConfig::LoadCheckConfig(szRunConfigName, info, mConfig.GetMsgName());
		if (!configData)
		{
			int result = ::MessageBox(NULL, info.ANIS().c_str(), "运行配置错误, 是否生成运行配置", MB_OKCANCEL);
			if ( result == 1)
				if (RunConfig::GenerateRunConfigJson(info2))
					::MessageBox(NULL, "请重启程序", "成功生成运行配置", MB_OK);
				else
					::MessageBox(NULL, info2.ANIS().c_str(), "生成运行配置失败", MB_OK);
			return false;
		}
		AString info3;
		RunConfig::ShowConfig(szRunConfigName, info3);
		printf("运行配置:\r\n_________________________________________________________\r\n%s______________________________________________________\r\n", info3.ANIS().c_str());

		mConfig.Full(configData);

		return true;
	}

	// 进程开始时, 放入
	CRunConfig(const char *szRunConfigName)
	{
		AString info, info2;
		AutoNice configData = RunConfig::LoadCheckConfig(szRunConfigName, info, config.GetMsgName());
		if (!configData)
		{
			int result = ::MessageBox(NULL, info.ANIS().c_str(), "运行配置错误, 是否生成运行配置", MB_OKCANCEL);
			if (result == 1)
				if (RunConfig::GenerateRunConfigJson(info2))
					::MessageBox(NULL, "请重启程序", "成功生成运行配置", MB_OK);
				else
					::MessageBox(NULL, info2.ANIS().c_str(), "生成运行配置失败", MB_OK);
			return;
		}

		config.Full(configData);
	}

public:
	static Config	mConfig;

	Config config;
};