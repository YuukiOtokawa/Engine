#include "ComputeShader.h"

#include "RenderCore.h"
#include "StringConverter.h"

using namespace EngineCoreSystem;

bool ComputeShader::Load(std::string filePath, std::string entryPoint)
{
	if (CompileShader(filePath, entryPoint, "cs_5_0", (ID3DBlob**)&m_pComputeShader) != S_OK) {
		return false;
	}

	RenderCore::GetInstance()->CreateComputeShader(filePath, GetFileNameFromFilePath(filePath));
}

void ComputeShader::Dispatch(int x, int y, int z)
{
	RenderCore::GetInstance()->GetDeviceContext()->CSSetShader(m_pComputeShader, NULL, 0);
	RenderCore::GetInstance()->GetDeviceContext()->Dispatch(x, y, z);

	void* NullUAV[] = { nullptr };
	RenderCore::GetInstance()->GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, (ID3D11UnorderedAccessView* const*)NullUAV, nullptr);
}
