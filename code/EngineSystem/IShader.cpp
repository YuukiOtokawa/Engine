#include "IShader.h"

#include <d3dcompiler.h>
#include "StringConverter.h"

HRESULT IShader::CompileShader(const std::string& filePath, const std::string& entryPoint, const std::string& shaderModel, ID3DBlob** ppBlobOut)
{
	DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(_DEBUG) || defined(DEBUG)
	shaderFlags |= D3DCOMPILE_DEBUG;
	shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	ID3DBlob* pErrorBlob = nullptr;
	HRESULT hr = D3DCompileFromFile(
		ToWString(filePath).c_str(),
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entryPoint.c_str(),
		shaderModel.c_str(),
		shaderFlags,
		0,
		ppBlobOut,
		&pErrorBlob
	);
	if (FAILED(hr))
	{
		if (pErrorBlob)
		{
			// TODO:ƒGƒ‰[ˆ—
			OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
			pErrorBlob->Release();
		}
		return hr;
	}
	if (pErrorBlob) pErrorBlob->Release();
	return S_OK;
}
