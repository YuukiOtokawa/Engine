#include "VertexPixelShader.h"

#include "RenderCore.h"
#include "StringConverter.h"

#include "EngineConsole.h"

VertexPixelShader* VertexPixelShader::Load(std::string filePath)
{
	//if (RenderCore::GetInstance()->CheckShaderDuplicate(GetFileNameFromFilePath(filePath))) {
	//	return RenderCore::GetInstance()->GetVertexPixelShader(GetFileNameFromFilePath(filePath));
	//}

	VertexPixelShader* vspsShader = new VertexPixelShader();
	vspsShader->LoadShader(filePath);
	if (!vspsShader->m_isLoaded) {
		delete vspsShader;
		vspsShader = nullptr;
	}
	return vspsShader;
}

void VertexPixelShader::LoadShader(std::string filePath)
{
	ID3DBlob* pVSBlob = nullptr;
	if (CompileShader(filePath, "vtx", "vs_5_0", &pVSBlob) != S_OK) {
		if (pVSBlob) pVSBlob->Release();
		return;
	}
	ID3DBlob* pPSBlob = nullptr;
	if (CompileShader(filePath, "pix", "ps_5_0", &pPSBlob) != S_OK) {
		if (pVSBlob) pVSBlob->Release();
		if (pPSBlob) pPSBlob->Release();
		return;
	}
	auto d = RenderCore::GetInstance()->GetDevice();
	auto hr = d->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &m_pVertexShader);
	hr = d->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, &m_pPixelShader);
	m_pInputLayout = RenderCore::GetInstance()->CreateInputLayout(
		(unsigned char*)pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize()
	);

	//RenderCore::GetInstance()->AddVertexPixelShader(GetFileNameFromFilePath(filePath), this);
	pVSBlob->Release();
	pPSBlob->Release();
	m_isLoaded = true;
	return;
}

void VertexPixelShader::Bind()
{
	if (!m_pVertexShader || !m_pPixelShader) return;
	auto dc = RenderCore::GetInstance()->GetDeviceContext();
	dc->VSSetShader(m_pVertexShader, NULL, 0);
	dc->PSSetShader(m_pPixelShader, NULL, 0);
	dc->IASetInputLayout(m_pInputLayout);
}
