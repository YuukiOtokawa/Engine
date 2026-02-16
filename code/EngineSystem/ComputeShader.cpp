#include "ComputeShader.h"

#include "RenderCore.h"
#include "StringConverter.h"

#include "EngineConsole.h"

ComputeShader* ComputeShader::Load(std::string filePath)
{
	if (RenderCore::GetInstance()->CheckShaderDuplicate(GetFileNameFromFilePath(filePath))) {
		return RenderCore::GetInstance()->GetComputeShader(GetFileNameFromFilePath(filePath));
	}

	ComputeShader* computeShader = new ComputeShader();
	computeShader->LoadShader(filePath);
	if (!computeShader->m_isLoaded) {
		delete computeShader;
		computeShader = nullptr;
	}
	return computeShader;
}

void ComputeShader::LoadShader(std::string filePath)
{
	ID3DBlob* pCSBlob = nullptr;
	if (CompileShader(filePath, "com", "cs_5_0", &pCSBlob) != S_OK) {
		if (pCSBlob) pCSBlob->Release();
		return;
	}

	auto d = RenderCore::GetInstance()->GetDevice();
	auto hr = d->CreateComputeShader(pCSBlob->GetBufferPointer(), pCSBlob->GetBufferSize(), NULL, &m_pComputeShader);

	RenderCore::GetInstance()->AddComputeShader(GetFileNameFromFilePath(filePath), this);

	pCSBlob->Release();
	m_isLoaded = true;
	return;
}

void ComputeShader::Dispatch(int x, int y, int z)
{
	RenderCore::GetInstance()->GetDeviceContext()->CSSetShader(m_pComputeShader, NULL, 0);
	RenderCore::GetInstance()->GetDeviceContext()->Dispatch(x, y, z);

	void* NullUAV[] = { nullptr };
	RenderCore::GetInstance()->GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, (ID3D11UnorderedAccessView* const*)NullUAV, nullptr);
}
