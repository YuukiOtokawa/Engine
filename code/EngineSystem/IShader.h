#pragma once

#include <string>
#include <d3d11.h>

#include "EngineMetaFile.h"

//==========================================================================
// インターフェース定義
//==========================================================================
class IShader : public EngineMetaFile
{
protected:
    static HRESULT CompileShader(const std::string& filePath, const std::string& entryPoint, const std::string& shaderModel, ID3DBlob** ppBlobOut);
    bool m_isLoaded = false;
    std::string m_HLSLFilePath = "";
public:
    IShader() = default;
    virtual ~IShader() = default;

    void Load(std::string filepath) {
        m_HLSLFilePath = filepath;
        LoadShader(filepath);
    }

    virtual void LoadShader(std::string filePath) = 0;
};
