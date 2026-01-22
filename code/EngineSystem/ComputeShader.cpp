#include "ComputeShader.h"

#include "RenderCore.h"
#include "StringConverter.h"

#include "EngineConsole.h"

ComputeShader* ComputeShader::Load(std::string filePath, std::string entryPoint)
{
	if (RenderCore::GetInstance()->CheckComputeShaderDuplicate(GetFileNameFromFilePath(filePath) + "::" + entryPoint)) {
		return RenderCore::GetInstance()->GetComputeShader(GetFileNameFromFilePath(filePath) + "::" + entryPoint);
	}

	ComputeShader* computeShader = new ComputeShader();
	if (!computeShader->LoadShader(filePath, entryPoint)) {
		EngineConsole::LogError("Failed to load compute shader: %s", filePath.c_str());
		delete computeShader;
		return nullptr;
	}

	return computeShader;
}

bool ComputeShader::LoadShader(std::string filePath, std::string entryPoint)
{
	ID3DBlob* pCSBlob = nullptr;
	if (CompileShader(filePath, entryPoint, "cs_5_0", &pCSBlob) != S_OK) {
		return false;
	}

	auto d = RenderCore::GetInstance()->GetDevice();
	auto hr = d->CreateComputeShader(pCSBlob->GetBufferPointer(), pCSBlob->GetBufferSize(), NULL, &m_pComputeShader);

	RenderCore::GetInstance()->AddComputeShader(GetFileNameFromFilePath(filePath) + "::" + entryPoint, this);

	pCSBlob->Release();

	return true;
}

void ComputeShader::Dispatch(int x, int y, int z)
{
	RenderCore::GetInstance()->GetDeviceContext()->CSSetShader(m_pComputeShader, NULL, 0);
	RenderCore::GetInstance()->GetDeviceContext()->Dispatch(x, y, z);

	void* NullUAV[] = { nullptr };
	RenderCore::GetInstance()->GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, (ID3D11UnorderedAccessView* const*)NullUAV, nullptr);
}
