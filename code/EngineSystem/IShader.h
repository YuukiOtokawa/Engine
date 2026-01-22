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

public:
    IShader() = default;
    virtual ~IShader() = default;

    virtual void LoadShader(std::string filePath) = 0;
};
