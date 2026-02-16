#pragma once

#include "IShader.h"

class VertexPixelShader : public IShader {
    friend class RenderCore;

    ID3D11VertexShader* m_pVertexShader = nullptr;
    ID3D11PixelShader* m_pPixelShader = nullptr;
    ID3D11InputLayout* m_pInputLayout = nullptr;

    VertexPixelShader(const VertexPixelShader&) = delete;
    void operator=(const VertexPixelShader&) = delete;
public:
    VertexPixelShader() { m_ClassID = CID_VertexPixelShader; }
    virtual ~VertexPixelShader() {
        SAFE_RELEASE(m_pVertexShader);
        SAFE_RELEASE(m_pPixelShader);
        SAFE_RELEASE(m_pInputLayout);
    }
    static VertexPixelShader* Load(std::string filePath);
    void LoadShader(std::string filePath) override;
    void Bind();
};
