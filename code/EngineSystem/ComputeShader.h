#pragma once

#include "IShader.h"

class ComputeShader : public IShader {


    ID3D11ComputeShader* m_pComputeShader = nullptr;

    ComputeShader() = default;
    ComputeShader(const ComputeShader&) = delete;
    void operator=(const ComputeShader&) = delete;
public:
    static ComputeShader* Load(std::string filePath, std::string entryPoint = "com");
    bool LoadShader(std::string filePath, std::string entryPoint = "com");
    void Dispatch(int x, int y, int z);

};
