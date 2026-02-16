#include "GeometryShader.h"

#include "RenderCore.h"
#include "StringConverter.h"

GeometryShader* GeometryShader::Load(std::string filePath) {
    //if (RenderCore::GetInstance()->CheckShaderDuplicate(GetFileNameFromFilePath(filePath))) {
    //    return RenderCore::GetInstance()->GetGeometryShader(GetFileNameFromFilePath(filePath));
    //}

    GeometryShader* gShader = new GeometryShader();
    gShader->LoadShader(filePath);
    if (!gShader->m_isLoaded) {
        delete gShader;
        gShader = nullptr;
    }
    return gShader;
}

void GeometryShader::LoadShader(std::string filePath) {
    ID3DBlob* pGSBlob = nullptr;
    if (CompileShader(filePath, "geo","gs_5_0",&pGSBlob)) {
        if (pGSBlob) pGSBlob->Release();
        return;
    }
    
    auto d = RenderCore::GetInstance()->GetDevice();
    auto hr = d->CreateGeometryShader(pGSBlob->GetBufferPointer(),pGSBlob->GetBufferSize(),NULL,&m_pGeometryShader);
    
    pGSBlob->Release();
    m_isLoaded = true;
}

void GeometryShader::Bind() {
    if (!m_pGeometryShader) return;
    
    auto dc = RenderCore::GetInstance()->GetDeviceContext();
    dc->GSSetShader(m_pGeometryShader,nullptr,0);
}

void GeometryShader::Unbind() {
    auto dc = RenderCore::GetInstance()->GetDeviceContext();
    dc->GSSetShader(NULL, NULL,0);
}
