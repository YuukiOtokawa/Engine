#pragma once
#include "Script.h"
#include "ScriptFactory.h"

class GameManager :
    public Script
{
    std::string m_TitleSceneName = "TitleScene";
    std::string m_GameSceneName = "GameScene";
    std::string m_ResultSceneName = "ResultScene";

public:
    GameManager() {
        //REGISTER_PROPERTY(m_TitleSceneName, "TitleSceneName");
        //REGISTER_PROPERTY(m_GameSceneName, "GameSceneName");
        //REGISTER_PROPERTY(m_ResultSceneName, "ResultSceneName");
    }
    void Start() override;
    void Update() override;
    void Import(YAML::Node& node) override;
    void Export(YAML::Emitter& node) override;

    DECLARE_SCRIPT(GameManager)
};

