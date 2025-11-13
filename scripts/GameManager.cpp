#include "GameManager.h"

#include <yaml-cpp/yaml.h>

REGISTERCLASS(GameManager);
REGISTER_SCRIPT(GameManager)

void GameManager::Start()
{
}

void GameManager::Update()
{
}

void GameManager::Import(YAML::Node& node)
{
    if (node["titleSceneName"]) {
        m_TitleSceneName = node["titleSceneName"].as<std::string>();
    }
    if (node["gameSceneName"]) {
        m_GameSceneName = node["gameSceneName"].as<std::string>();
    }
    if (node["resultSceneName"]) {
        m_ResultSceneName = node["resultSceneName"].as<std::string>();
    }
}

void GameManager::Export(YAML::Emitter& node)
{
    node << YAML::Key << "titleSceneName" << YAML::Value << m_TitleSceneName;
    node << YAML::Key << "gameSceneName" << YAML::Value << m_GameSceneName;
    node << YAML::Key << "resultSceneName" << YAML::Value << m_ResultSceneName;
}

