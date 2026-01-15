#pragma once

#include "IShader.h"

using namespace EngineCoreSystem;

class VertexPixelShader : public IShader {
    ID3D11VertexShader* m_pVertexShader = nullptr;
    ID3D11PixelShader* m_pPixelShader = nullptr;
public:
    VertexPixelShader() = default;
    virtual ~VertexPixelShader() = default;
    bool Load(std::string filePath, std::string vsEntryPoint = "vtx", std::string psEntryPoint = "pix");
    void Bind();
};
