#pragma once

#include "IShader.h"

class VertexPixelShader : public IShader {
    friend class RenderCore;

    ID3D11VertexShader* m_pVertexShader = nullptr;
    ID3D11PixelShader* m_pPixelShader = nullptr;

    VertexPixelShader(const VertexPixelShader&) = delete;
    void operator=(const VertexPixelShader&) = delete;
public:
    VertexPixelShader() { m_ClassID = CID_VertexPixelShader; }
    virtual ~VertexPixelShader() = default;
    static VertexPixelShader* Load(std::string filePath);
    void LoadShader(std::string filePath) override;
    void Bind();
};
