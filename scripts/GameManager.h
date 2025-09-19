#pragma once
#include "Script.h"
class GameManager :
    public Script
{
    std::string m_TitleSceneName = "TitleScene";
    std::string m_GameSceneName = "GameScene";
    std::string m_ResultSceneName = "ResultScene";

public:
    GameManager() {
        REGISTER_PROPERTY(m_TitleSceneName, "TitleSceneName");
        REGISTER_PROPERTY(m_GameSceneName, "GameSceneName");
        REGISTER_PROPERTY(m_ResultSceneName, "ResultSceneName");
    }
    void Start() override;
    void Update() override;
    void Import(std::vector<std::string> tokens) override {
        m_TitleSceneName = tokens[5];
        m_GameSceneName = tokens[6];
        m_ResultSceneName = tokens[7];
    }
    void Export() override;
    const char* GetScriptName() const override;
};

