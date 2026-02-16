#pragma once

#include "ComputeBuffer.h"
#include "ComputeShader.h"
#include "Script.h"

class TestCompute : public Script {
public:
    TestCompute() {
    }

    void Start() override;
    void Update() override;


    const char* GetScriptName() const override;
};
