#pragma once
#include "Script.h"

class DefaultScript :
    public Script
{
public:
    DefaultScript() {
    }
    void Start() override;
    void Update() override;

    void Import(YAML::Node& node) override;
    void Export(YAML::Emitter& node) override;
    const char* GetScriptName() const override;
};

