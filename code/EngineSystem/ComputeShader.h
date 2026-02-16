#pragma once

#include "IShader.h"

struct PARTICLE {
    Vector4O Position;
    Vector4O Acceleration;
    Vector4O Velocity;
    Vector4O Parameter;
};

class ComputeShader : public IShader {


    ID3D11ComputeShader* m_pComputeShader = nullptr;

    ComputeShader(const ComputeShader&) = delete;
    void operator=(const ComputeShader&) = delete;


public:
    ComputeShader() { m_ClassID = CID_ComputeShader; }
    ~ComputeShader() {
        SAFE_RELEASE(m_pComputeShader);
    }
    static ComputeShader* Load(std::string filePath);
    void LoadShader(std::string filePath) override;
    void Dispatch(int x, int y, int z);

};
