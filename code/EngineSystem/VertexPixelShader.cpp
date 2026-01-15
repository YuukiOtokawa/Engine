#include "VertexPixelShader.h"

#include "RenderCore.h"
#include "StringConverter.h"

#include "EngineConsole.h"

using namespace EngineCoreSystem;

VertexPixelShader* VertexPixelShader::Load(std::string filePath, std::string vsEntryPoint, std::string psEntryPoint)
{
	if (RenderCore::GetInstance()->CheckVertexPixelShaderDuplicate(GetFileNameFromFilePath(filePath))) {
		return RenderCore::GetInstance()->GetVertexPixelShader(GetFileNameFromFilePath(filePath));
	}

	VertexPixelShader* vspsShader = new VertexPixelShader();
	if (!vspsShader->LoadShader(filePath, vsEntryPoint, psEntryPoint)) {
		EngineConsole::LogError("Failed to load compute shader: %s", filePath.c_str());
		delete vspsShader;
		return nullptr;
	}

	return vspsShader;
}

bool VertexPixelShader::LoadShader(std::string filePath, std::string vsEntryPoint, std::string psEntryPoint)
{
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
