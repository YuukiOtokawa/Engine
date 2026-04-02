#pragma once

#include "Script.h"

class HitEffectControl : public Script
{
    double effectRemainedTime = 1.0;
public:
    void Start() override;
    void Update() override;

    const char* GetScriptName() const override;
};
