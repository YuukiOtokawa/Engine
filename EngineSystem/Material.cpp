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

	renderer->SetLight(m_Light);

}

void Material::DrawGUI() {
	ImGui::Text("Material Properties");
	//ImGui::Indent();
	//ImGui::ColorEdit4("Ambient", &m_Material.ambient.x);
	//ImGui::ColorEdit4("Diffuse", &m_Material.diffuse.x);
	//ImGui::ColorEdit4("Specular", &m_Material.specular.x);
	//ImGui::ColorEdit4("Emissive", &m_Material.emissive.x);
	//ImGui::InputFloat("Shininess", &m_Material.shininess, 0.0f, 128.0f);
	//ImGui::Unindent();
	ImGui::Text("Light Properties");
	ImGui::Indent();
	ImGui::InputFloat3( "Direction", &m_Light.Direction.x);
	ImGui::ColorEdit4("Ambient", &m_Light.Ambient.x);
	ImGui::ColorEdit4("Color", &m_Light.Diffuse.x);
	ImGui::ColorEdit4("Sky Color", &m_Light.SkyColor.x);
	ImGui::ColorEdit4("Ground Color", &m_Light.GroundColor.x);
	ImGui::InputFloat3("Point Light Position", &m_Light.Position.x);
	ImGui::InputFloat3("Point Light Range", &m_Light.PointLightRange.x);
	ImGui::Unindent();
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
