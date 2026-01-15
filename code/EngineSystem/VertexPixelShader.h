#pragma once

#include "IShader.h"
#include "RenderCore.h"

using namespace EngineCoreSystem;

class VertexPixelShader : public IShader {
    friend class RenderCore;

    ID3D11VertexShader* m_pVertexShader = nullptr;
    ID3D11PixelShader* m_pPixelShader = nullptr;

    VertexPixelShader() = default;
    VertexPixelShader(const VertexPixelShader&) = delete;
    void operator=(const VertexPixelShader&) = delete;
public:
    virtual ~VertexPixelShader() = default;
    VertexPixelShader* Load(std::string filePath, std::string vsEntryPoint = "vtx", std::string psEntryPoint = "pix");
    bool LoadShader(std::string filePath, std::string vsEntryPoint = "vtx", std::string psEntryPoint = "pix");
    void Bind();
};
