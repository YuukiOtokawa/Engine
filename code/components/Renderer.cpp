#include "Renderer.h"

#include "MainEngine.h"

std::string  Renderer::m_PixelShaderKey = "DepthShadow";
ID3D11InputLayout* Renderer::m_pInputLayout = nullptr;

void Renderer::Draw(bool castShadow)
{
	if (castShadow && m_bCastShadows)
	{
		auto renderer = MainEngine::GetInstance()->GetRenderCore();

		// シャドウマップ生成時はピクセルシェーダーを使用しない（深度値のみを書き込むため）
		renderer->GetDeviceContext()->PSSetShader(nullptr, nullptr, 0);

		auto depth = renderer->GetDepthStencil();
		renderer->GetDeviceContext()->PSSetShaderResources(1, 1, &depth);

		RenderShadow();
	}

	if (!castShadow)
		Render();
}