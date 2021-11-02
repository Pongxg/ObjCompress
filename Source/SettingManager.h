#pragma once
#include "Singleton.h"
#include <string>
#include "nlohmann/json.hpp"
#include "Util.h"


class SettingManager :public Singleton<SettingManager>
{
public:
	SettingManager();
	~SettingManager();

	bool ParserCmdLine(int argc,char** argv);
	
	bool ParserConfig();
public:
	std::string		m_strConfigName;
	JSON_SETTING    m_tJosnSetting;
};

#define  gSettingInstance SettingManager::GetInstance()