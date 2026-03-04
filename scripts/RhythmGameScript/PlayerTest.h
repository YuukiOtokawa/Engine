#pragma once
#include "Script.h"

// これがスクリプトのヘッダー
class Transform;
class PlayerTest :
    public Script
{
    float speed = 1.0f;
    Transform* cameraTransform;
    Transform* playerTransform;
public:
    PlayerTest() {
        REGISTER_PROPERTY(speed, "speed");
    }

    void Start() override;
    void Update() override;

    void Import(YAML::Node& node) override;
    void Export(YAML::Emitter& node) override;
    const char* GetScriptName() const override;
};

