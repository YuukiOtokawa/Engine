#pragma once
#include "Script.h"

#include <random>

class Fractal :
    public Script
{
    int size = 17;
    std::vector<float> heightMap;

    bool isActive = true;
public:
    Fractal() {
        REGISTER_PROPERTY(isActive, "isActive");
    }

    void Start() override;
    void Update() override;

    void Generate(float roughness = 1.0f);

    void DiamondStep(int x, int y, int stepSize, float scale);
    void SquareStep(int x, int y, int stepSize, float scale);

    void Normalize();

    float GetRandomValue();

    const char* GetScriptName() const override;
};

