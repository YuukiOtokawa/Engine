#include "VertexPixelShader.h"

#include "RenderCore.h"
#include "StringConverter.h"

#include "EngineConsole.h"

VertexPixelShader* VertexPixelShader::Load(std::string filePath)
{
	if (RenderCore::GetInstance()->CheckVertexPixelShaderDuplicate(GetFileNameFromFilePath(filePath))) {
		return RenderCore::GetInstance()->GetVertexPixelShader(GetFileNameFromFilePath(filePath));
	}

	VertexPixelShader* vspsShader = new VertexPixelShader();
	vspsShader->LoadShader(filePath);
}

void VertexPixelShader::LoadShader(std::string filePath)
{
	ID3DBlob* pVSBlob = nullptr;
	if (CompileShader(filePath, "vtx", "vs_5_0", &pVSBlob) != S_OK) {
		pVSBlob->Release();
		return;
	}
	ID3DBlob* pPSBlob = nullptr;
	if (CompileShader(filePath, "pix", "ps_5_0", &pPSBlob) != S_OK) {
		pVSBlob->Release();
		pPSBlob->Release();
		return;
	}
	auto d = RenderCore::GetInstance()->GetDevice();
	auto hr = d->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &m_pVertexShader);
	hr = d->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, &m_pPixelShader);

	RenderCore::GetInstance()->AddVertexPixelShader(GetFileNameFromFilePath(filePath), this);
	pVSBlob->Release();
	pPSBlob->Release();
	return;
}

void VertexPixelShader::Bind()
{
	auto dc = RenderCore::GetInstance()->GetDeviceContext();
	dc->VSSetShader(m_pVertexShader, NULL, 0);
	dc->PSSetShader(m_pPixelShader, NULL, 0);
	dc->IASetInputLayout(RenderCore::GetInstance()->GetInputLayout());
}
