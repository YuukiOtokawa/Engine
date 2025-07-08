// ========================================================
//
// マテリアルクラス[Material.h]
// 
//									Date:	20250520
//									Author:	Yuuki Otokawa
// ========================================================

//==========================================================================
// ヘッダーインクルード
//==========================================================================

#include "Material.h"

#include "MainEngine.h"

//==========================================================================
// メンバ関数
//==========================================================================

void Material::DrawMaterial()
{
	auto renderer = MainEngine::GetInstance()->GetRenderer();


	renderer->GetDeviceContext()->PSSetShaderResources(0, 1, &m_Texture);
	if (m_BumpTexture)
		renderer->GetDeviceContext()->PSSetShaderResources(1, 1, &m_BumpTexture);

	renderer->SetMaterialBuffer(m_Material);

}

void Material::DrawGUI() {
	ImGui::Text("Material Properties");
	ImGui::DragFloat("Shininess", &m_Material.shininess, 0.01f, 0.0f, 100.0f);
	ImGui::DragFloat("Specular Power", &m_Material.SpecularPower, 0.01f, 0.0f, 100.0f);
	ImGui::DragFloat4("ambient", &m_Material.ambient.x, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat4("diffuse", &m_Material.diffuse.x, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat4("specular", &m_Material.specular.x, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat4("emissive", &m_Material.emissive.x, 0.01f, 0.0f, 1.0f);

	ImGui::DragFloat4("Sky Color", &m_Material.SkyColor.x, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat4("Ground Color", &m_Material.GroundColor.x, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat4("Ground Normal", &m_Material.GroundNormal.x, 0.01f, -1.0f, 1.0f);

	ImGui::Text("Texture");
	ImGui::Image((ImTextureID)m_Texture, ImVec2(300, 300));
	ImGui::Text("Bump Map");
	ImGui::Image((ImTextureID)m_BumpTexture, ImVec2(300, 300));

}

void Material::SetVertexShaderKey(std::string key)
{
	m_VertexShader = key;
}

void Material::SetPixelShaderKey(std::string key)
{
	m_PixelShader = key;
}

void Material::SetShader()
{
	auto renderer = MainEngine::GetInstance()->GetRenderer();
	renderer->SetVertexShader(m_VertexShader);
	renderer->SetPixelShader(m_PixelShader);
}
