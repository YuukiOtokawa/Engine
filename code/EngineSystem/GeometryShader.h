#pragma once

#include "IShader.h"

class GeometryShader : public IShader {
    friend class RenderCore;

    ID3D11GeometryShader* m_pGeometryShader = nullptr;

    GeometryShader(const GeometryShader&) = delete;
    GeometryShader& operator=(const GeometryShader&) = delete;
public:
    GeometryShader() { m_ClassID = CID_GeometryShader;}
    virtual ~GeometryShader() {
        SAFE_RELEASE(m_pGeometryShader);
    }

    static GeometryShader* Load(std::string filePath);
    void LoadShader(std::string filePath) override;
    void Bind();

    static void Unbind();

};
