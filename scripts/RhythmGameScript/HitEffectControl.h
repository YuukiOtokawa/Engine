#pragma once

#include "Script.h"

class HitEffectControl : public Script
{
    double effectRemainedTime = 1.0;
public:
    void Start() override;
    void Update() override;

    void Import(YAML::Node& node) override;
    void Export(YAML::Emitter& node) override;
    const char* GetScriptName() const override;
};
