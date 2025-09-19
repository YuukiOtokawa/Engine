#pragma once
#include "Script.h"
class PlayerControl :
    public Script
{
    float m_Speed = 5.0f;
public:
    PlayerControl() {
        REGISTER_PROPERTY(m_Speed, "Speed");
    }
    void Start() override;
    void Update() override;

    void Import(std::vector<std::string> tokens) override {
        m_Speed = std::stof(tokens[5]);
    }

    void Export() override;
    const char* GetScriptName() const override;
};

