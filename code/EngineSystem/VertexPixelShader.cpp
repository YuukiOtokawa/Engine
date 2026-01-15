#include "VertexPixelShader.h"

#include "RenderCore.h"
#include "StringConverter.h"

using namespace EngineCoreSystem;

bool VertexPixelShader::Load(std::string filePath, std::string vsEntryPoint, std::string psEntryPoint)
{
	if (RenderCore::GetInstance()->CheckVertexPixelShaderDuplicate(GetFileNameFromFilePath(filePath))) {
		//TODO:キーによって既存のシェーダーを取得してセットする処理

		return true;
	}

	ID3DBlob* pVSBlob = nullptr;
	if (CompileShader(filePath, vsEntryPoint, "vs_5_0", &pVSBlob) != S_OK) {
		pVSBlob->Release();
		return false;
	}
	ID3DBlob* pPSBlob = nullptr;
	if (CompileShader(filePath, psEntryPoint, "ps_5_0", &pPSBlob) != S_OK) {
		pVSBlob->Release();
		pPSBlob->Release();
		return false;
	}
	auto d = RenderCore::GetInstance()->GetDevice();
	auto hr = d->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &m_pVertexShader);
	hr = d->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, &m_pPixelShader);

	RenderCore::GetInstance()->AddVertexPixelShader(GetFileNameFromFilePath(filePath), this);
	pVSBlob->Release();
	pPSBlob->Release();
	return true;
}

void VertexPixelShader::Bind()
{
	auto dc = RenderCore::GetInstance()->GetDeviceContext();
	dc->VSSetShader(m_pVertexShader, NULL, 0);
	dc->PSSetShader(m_pPixelShader, NULL, 0);
	dc->IASetInputLayout(RenderCore::GetInstance()->GetInputLayout());
}
