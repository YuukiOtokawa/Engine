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

auto key_getter = [](void* data, int idx, const char** out_text) -> bool {
	auto& keys = *static_cast<std::vector<std::string>*>(data);
	if (idx < 0 || idx >= static_cast<int>(keys.size())) {
		return false;
	}
	*out_text = keys[idx].c_str();
	return true;
	};

//==========================================================================
// メンバ関数
//==========================================================================

void Material::DrawMaterial()
{
	auto renderer = MainEngine::GetInstance()->GetRenderer();

	if (m_Texture != -1)
		renderer->GetDeviceContext()->PSSetShaderResources(0, 1, renderer->GetTexture(m_Texture));
	if (m_BumpTexture != -1)
		renderer->GetDeviceContext()->PSSetShaderResources(1, 1, renderer->GetTexture(m_BumpTexture));
	if (m_ToonTexture != -1)
		renderer->GetDeviceContext()->PSSetShaderResources(2, 1, renderer->GetTexture(m_ToonTexture));

	renderer->SetMaterialBuffer(m_Material);

}

void Material::DrawGUI() {
	Renderer* renderer = MainEngine::GetInstance()->GetRenderer();
	static int current_shader_index = 0;

	auto keys = MainEngine::GetInstance()->GetRenderer()->GetVertexShaderKeys();
	current_shader_index = (int)std::distance(
		keys.begin(),
		std::find(keys.begin(), keys.end(), m_VertexShader)
		);

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

	if (ImGui::Combo("Shader", &current_shader_index, key_getter,
		static_cast<void*>(&keys), keys.size())) {
		m_VertexShader = keys[current_shader_index];
		m_PixelShader = keys[current_shader_index];
	}

	ImGui::Text("Texture");
	ImGui::Image((ImTextureID)(*renderer->GetTexture(m_Texture)), ImVec2(300, 300));
	ImGui::Text("Bump Map");
	ImGui::Image((ImTextureID)(*renderer->GetTexture(m_BumpTexture)), ImVec2(300, 300));

}

void Material::ImportFile(std::vector<std::string>& tokens)
{
	m_Material.SpecularPower = std::stoi(tokens[2]);
	m_VertexShader = tokens[3];
	m_PixelShader = tokens[4];
	m_Texture = std::stoi(tokens[5]);
	m_BumpTexture = std::stoi(tokens[6]);
}

void Material::ExportFile()
{
	CSVExporter::ExportInt(m_Material.SpecularPower);
	CSVExporter::ExportString(m_VertexShader);
	CSVExporter::ExportString(m_PixelShader);
	CSVExporter::ExportInt(m_Texture);
	CSVExporter::ExportInt(m_BumpTexture);
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
