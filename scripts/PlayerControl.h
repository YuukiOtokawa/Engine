#pragma once
#include "Script.h"

class GameManager;

class PlayerControl :
    public Script
{
    // 他スクリプトのポインタも持てる
    GameManager* gameManager = nullptr;
    float m_Speed = 1.0f;
    float m_JumpForce = 1.0f;
public:
    PlayerControl() {
        REGISTER_PROPERTY(m_Speed, "Speed");
        REGISTER_PROPERTY(m_JumpForce, "JumpForce");
    }
    void Start() override;
    void Update() override;

    void Import(YAML::Node& node) override;
    void Export(YAML::Emitter& node) override;
    const char* GetScriptName() const override;

    void Hit();
};

