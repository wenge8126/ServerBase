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
	// ���̿�ʼʱ, ����
	static bool Init(const char *szRunConfigName)
	{
		AString info, info2;
		AutoNice configData = RunConfig::LoadCheckConfig(szRunConfigName, info, mConfig.GetMsgName());
		if (!configData)
		{
			int result = ::MessageBox(NULL, info.ANIS().c_str(), "�������ô���, �Ƿ�������������", MB_OKCANCEL);
			if ( result == 1)
				if (RunConfig::GenerateRunConfigJson(info2))
					::MessageBox(NULL, "����������", "�ɹ�������������", MB_OK);
				else
					::MessageBox(NULL, info2.ANIS().c_str(), "������������ʧ��", MB_OK);
			return false;
		}
		AString info3;
		RunConfig::ShowConfig(szRunConfigName, info3);
		printf("��������:\r\n_________________________________________________________\r\n%s______________________________________________________\r\n", info3.ANIS().c_str());

		mConfig.Full(configData);

		return true;
	}

	// ���̿�ʼʱ, ����
	CRunConfig(const char *szRunConfigName)
	{
		AString info, info2;
		AutoNice configData = RunConfig::LoadCheckConfig(szRunConfigName, info, config.GetMsgName());
		if (!configData)
		{
			int result = ::MessageBox(NULL, info.ANIS().c_str(), "�������ô���, �Ƿ�������������", MB_OKCANCEL);
			if (result == 1)
				if (RunConfig::GenerateRunConfigJson(info2))
					::MessageBox(NULL, "����������", "�ɹ�������������", MB_OK);
				else
					::MessageBox(NULL, info2.ANIS().c_str(), "������������ʧ��", MB_OK);
			return;
		}

		config.Full(configData);
	}

public:
	static Config	mConfig;

	Config config;
};