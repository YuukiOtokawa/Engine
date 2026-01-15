#pragma once

#include "IShader.h"

using namespace EngineCoreSystem;

class ComputeShader : public IShader {
    ID3D11ComputeShader* m_pComputeShader = nullptr;
public:
    bool Load(std::string filePath, std::string entryPoint = "com");
    void Dispatch(int x, int y, int z);
};
