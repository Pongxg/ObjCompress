#include  "SettingManager.h"
#include  <iostream>
#include  <nlohmann/json.hpp>
#include  <fstream>
#include  "Util.h"
using json = nlohmann::json;

SettingManager* Singleton<SettingManager>::m_pSingleton = NULL;
SettingManager::SettingManager():
    m_strConfigName("")
{

}

SettingManager::~SettingManager() {

}

bool SettingManager::ParserCmdLine(int argc, char** argv)
{
    int config = 0;
    std::string arg = argv[0];
    if (arg == "-c")
    {
        if ( 1 < argc) {
            m_strConfigName = argv[1];
            return true;
        }
    } 
    return  false;
}

bool SettingManager::ParserConfig()
{
    nlohmann::json json_dto;
    std::ifstream jfile(m_strConfigName.c_str());
    if (!jfile.is_open())
    {
        std::cout << "open test.json failed" << std::endl;
        return false;
    }
    try
    {
        jfile >> json_dto;
        if (json_dto.is_discarded())
        {
            std::cout << "parse json data failed" << std::endl;
            return false;
        }
        json_setting(json_dto, m_tJosnSetting);
    }
    catch (std::exception& e)
    {
        std::cout << e.what() << std::endl;
        return false;
    }
    return  true;
}
