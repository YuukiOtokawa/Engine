#pragma once

#include <string>
#include <d3d11.h>

//==========================================================================
// インターフェース定義
//==========================================================================
class IShader
{
protected:
    static HRESULT CompileShader(const std::string& filePath, const std::string& entryPoint, const std::string& shaderModel, ID3DBlob** ppBlobOut);

public:
    virtual ~IShader() = default;
};
