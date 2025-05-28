#include "Material.h"

#include "MainEngine.h"

void Material::DrawMaterial()
{
	auto renderer = MainEngine::GetInstance()->GetRenderer();

	renderer->SetVertexShader(m_VertexShader);
	renderer->SetPixelShader(m_PixelShader);

	renderer->GetDeviceContext()->PSSetShaderResources(0, 1, &m_Texture);

	//‚±‚±‚Åhikarinojouhou‚ð‘—M
	renderer->SetMaterial(m_Material);
	renderer->SetLight(m_Light);

}

void Material::SetVertexShaderKey(std::string key)
{
	m_VertexShader = key;
}

void Material::SetPixelShaderKey(std::string key)
{
	m_PixelShader = key;
}
