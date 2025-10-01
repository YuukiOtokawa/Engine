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
	auto renderer = MainEngine::GetInstance()->GetRenderCore();

	ID3D11ShaderResourceView** srv;
	srv = renderer->GetTexture(m_TextureFileID);
	if (srv != nullptr)
		renderer->GetDeviceContext()->PSSetShaderResources(0, 1, srv);
	srv = renderer->GetTexture(m_BumpTextureFileID);
	if (srv != nullptr)
		renderer->GetDeviceContext()->PSSetShaderResources(1, 1, srv);
	srv = renderer->GetTexture(m_ToonTextureFileID);
	if (srv != nullptr)
		renderer->GetDeviceContext()->PSSetShaderResources(2, 1, srv);
	srv = renderer->GetTexture(m_EnvTextureFileID);
	if (srv != nullptr)
		renderer->GetDeviceContext()->PSSetShaderResources(3, 1, srv);



	renderer->SetMaterialBuffer(m_Material);

}

void Material::DrawGUI() {
	RenderCore* renderer = MainEngine::GetInstance()->GetRenderCore();
	static int current_shader_index = 0;

	auto keys = MainEngine::GetInstance()->GetRenderCore()->GetVertexShaderKeys();
	current_shader_index = (int)std::distance(
		keys.begin(),
		std::find(keys.begin(), keys.end(), m_VertexShader)
	);

	ImGui::Text("Material Properties");
	ImGui::DragFloat("Shininess", &m_Material.shininess, 0.01f, 0.0f, 100.0f);
	ImGui::DragFloat("Specular Power", &m_Material.SpecularPower, 0.01f, 0.0f, 100.0f);
	ImGui::DragFloat("RGB Shift", &m_Material.RGBShift, 0.01f, -1.0f, 1.0f);
	ImGui::DragFloat4("ambient", &m_Material.ambient.x, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat4("diffuse", &m_Material.diffuse.x, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat4("specular", &m_Material.specular.x, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat4("emissive", &m_Material.emissive.x, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat("MosaicSize", &m_Material.MosaicSize, 0.01f, 0.0f, 4.0f);
	ImGui::DragFloat2("BlockSize", &m_Material.BlockSize.x, 0.1f, 1.0f, 512.0f);

	//ImGui::DragFloat4("Sky Color", &m_Material.SkyColor.x, 0.01f, 0.0f, 1.0f);
	//ImGui::DragFloat4("Ground Color", &m_Material.GroundColor.x, 0.01f, 0.0f, 1.0f);
	//ImGui::DragFloat4("Ground Normal", &m_Material.GroundNormal.x, 0.01f, -1.0f, 1.0f);

	if (ImGui::Combo("Shader", &current_shader_index, key_getter,
		static_cast<void*>(&keys), keys.size())) {
		m_VertexShader = keys[current_shader_index];
		m_PixelShader = keys[current_shader_index];
	}

	ID3D11ShaderResourceView** srv;
	ImGui::Text("Texture");
	srv = renderer->GetTexture(m_TextureFileID);
	if (srv != nullptr) {
		ImGui::Image((ImTextureID)*srv, ImVec2(300, 300));
		if (ImGui::IsItemClicked()) {
			// クリックされた場合、編集モードに入る
			auto filename = OpenImportFileDialog();

			if (filename != "") {
				m_TextureFileID = renderer->TextureLoad(std::wstring(filename.begin(),filename.end()));
			}
		}
	}

	ImGui::Text("Bump Map");
	srv = renderer->GetTexture(m_BumpTextureFileID);
	if (srv != nullptr) {
		ImGui::Image((ImTextureID)*srv, ImVec2(300, 300));
		if (ImGui::IsItemClicked()) {
			// クリックされた場合、編集モードに入る
			auto filename = OpenImportFileDialog();

			if (filename != "") {
				m_BumpTextureFileID = renderer->TextureLoad(std::wstring(filename.begin(), filename.end()));
			}
		}
	}

	ImGui::Text("Toon Texture");
	srv = renderer->GetTexture(m_ToonTextureFileID);
	if (srv != nullptr) {
		ImGui::Image((ImTextureID)*srv, ImVec2(300, 300));
		if (ImGui::IsItemClicked()) {
			// クリックされた場合、編集モードに入る
			auto filename = OpenImportFileDialog();

			if (filename != "") {
				m_ToonTextureFileID = renderer->TextureLoad(std::wstring(filename.begin(), filename.end()));
			}
		}
	}

	ImGui::Text("Enviroment Texture");
	srv = renderer->GetTexture(m_EnvTextureFileID);
	if (srv != nullptr) {
		ImGui::Image((ImTextureID)*srv, ImVec2(300, 300));
		if (ImGui::IsItemClicked()) {
			// クリックされた場合、編集モードに入る
			auto filename = OpenImportFileDialog();

			if (filename != "") {
				m_EnvTextureFileID = renderer->TextureLoad(std::wstring(filename.begin(), filename.end()));
			}
		}
	}
}

void Material::ImportFile(std::vector<std::string>& tokens)
{
	m_Material.SpecularPower = (FLOAT)std::stoi(tokens[2]);
	m_VertexShader = tokens[3];
	m_PixelShader = tokens[4];
	m_TextureFileID = std::stoi(tokens[5]);
	m_BumpTextureFileID = std::stoi(tokens[6]);
}

void Material::ExportFile()
{
	CSVExporter::ExportInt(m_Material.SpecularPower);
	CSVExporter::ExportString(m_VertexShader);
	CSVExporter::ExportString(m_PixelShader);
	CSVExporter::ExportInt(m_TextureFileID);
	CSVExporter::ExportInt(m_BumpTextureFileID);
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
	auto renderer = MainEngine::GetInstance()->GetRenderCore();
	renderer->SetVertexShader(m_VertexShader);
	renderer->SetPixelShader(m_PixelShader);
}
