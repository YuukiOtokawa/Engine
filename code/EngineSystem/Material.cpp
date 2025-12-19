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

	auto keys = MainEngine::GetInstance()->GetRenderCore()->GetPixelShaderKeys();
	current_shader_index = (int)std::distance(
		keys.begin(),
		std::find(keys.begin(), keys.end(), m_PixelShader)
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
		m_PixelShader = keys[current_shader_index];
	}

	ID3D11ShaderResourceView** srv;
	ImGui::Text("Texture");
	srv = renderer->GetTexture(m_TextureFileID);
	if (srv == nullptr) {
		m_TextureFileID = 0;
	}
	ImGui::Image((ImTextureID)*srv, ImVec2(300, 300));
	if (ImGui::IsItemClicked()) {
		// クリックされた場合、編集モードに入る
		auto filename = OpenImportFileDialog();

		if (filename != "") {
			m_TextureFileID = renderer->TextureLoad(std::wstring(filename.begin(),filename.end()));
		}
	}

	auto path = GetDropPath("TEXTURE");
	if (path != "") {
		m_TextureFileID = renderer->TextureLoad(std::wstring(path.begin(), path.end()));
	}




	ImGui::Text("Bump Map");
	srv = renderer->GetTexture(m_BumpTextureFileID);
	if (srv == nullptr) {
		m_BumpTextureFileID = 0;
	}
	ImGui::Image((ImTextureID)*srv, ImVec2(300, 300));
	if (ImGui::IsItemClicked()) {
		// クリックされた場合、編集モードに入る
		auto filename = OpenImportFileDialog();

		if (filename != "") {
			m_BumpTextureFileID = renderer->TextureLoad(std::wstring(filename.begin(), filename.end()));
		}
	}
	path = GetDropPath("TEXTURE");
	if (path != "") {
		m_BumpTextureFileID = renderer->TextureLoad(std::wstring(path.begin(), path.end()));
	}

	ImGui::Text("Toon Texture");
	srv = renderer->GetTexture(m_ToonTextureFileID);
	if (srv == nullptr) {
		m_ToonTextureFileID = 0;
	}
	ImGui::Image((ImTextureID)*srv, ImVec2(300, 300));
	if (ImGui::IsItemClicked()) {
		// クリックされた場合、編集モードに入る
		auto filename = OpenImportFileDialog();

		if (filename != "") {
			m_ToonTextureFileID = renderer->TextureLoad(std::wstring(filename.begin(), filename.end()));
		}
	}
	path = GetDropPath("TEXTURE");
	if (path != "") {
		m_ToonTextureFileID = renderer->TextureLoad(std::wstring(path.begin(), path.end()));
	}

	ImGui::Text("Enviroment Texture");
	srv = renderer->GetTexture(m_EnvTextureFileID);
	if (srv == nullptr) {
		m_EnvTextureFileID = 0;
	}
	ImGui::Image((ImTextureID)*srv, ImVec2(300, 300));
	if (ImGui::IsItemClicked()) {
		// クリックされた場合、編集モードに入る
		auto filename = OpenImportFileDialog();

		if (filename != "") {
			m_EnvTextureFileID = renderer->TextureLoad(std::wstring(filename.begin(), filename.end()));
		}
	}
	path = GetDropPath("TEXTURE");
	if (path != "") {
		m_EnvTextureFileID = renderer->TextureLoad(std::wstring(path.begin(), path.end()));
	}
}

void Material::ImportFile(YAML::Node& node)
{
	if (node["vertexShader"]) {
		m_VertexShader = node["vertexShader"].as<std::string>();
	}
	if (node["pixelShader"]) {
		m_PixelShader = node["pixelShader"].as<std::string>();
	}
	if (node["textureFileID"]) {
		m_TextureFileID = node["textureFileID"].as<int>();
	}
	if (node["bumpTextureFileID"]) {
		m_BumpTextureFileID = node["bumpTextureFileID"].as<int>();
	}

	// m_Materialの内容をインポート
	if (node["material"]) {
		auto material = node["material"];
		if (material["textureEnable"]) {
			m_Material.textureEnable = material["textureEnable"].as<bool>();
		}
		if (material["shininess"]) {
			m_Material.shininess = material["shininess"].as<float>();
		}
		if (material["specularPower"]) {
			m_Material.SpecularPower = material["specularPower"].as<float>();
		}
		if (material["rgbShift"]) {
			m_Material.RGBShift = material["rgbShift"].as<float>();
		}
		if (material["ambient"]) {
			auto ambient = material["ambient"];
			m_Material.ambient.x = ambient[0].as<float>();
			m_Material.ambient.y = ambient[1].as<float>();
			m_Material.ambient.z = ambient[2].as<float>();
			m_Material.ambient.w = ambient[3].as<float>();
		}
		if (material["diffuse"]) {
			auto diffuse = material["diffuse"];
			m_Material.diffuse.x = diffuse[0].as<float>();
			m_Material.diffuse.y = diffuse[1].as<float>();
			m_Material.diffuse.z = diffuse[2].as<float>();
			m_Material.diffuse.w = diffuse[3].as<float>();
		}
		if (material["specular"]) {
			auto specular = material["specular"];
			m_Material.specular.x = specular[0].as<float>();
			m_Material.specular.y = specular[1].as<float>();
			m_Material.specular.z = specular[2].as<float>();
			m_Material.specular.w = specular[3].as<float>();
		}
		if (material["emissive"]) {
			auto emissive = material["emissive"];
			m_Material.emissive.x = emissive[0].as<float>();
			m_Material.emissive.y = emissive[1].as<float>();
			m_Material.emissive.z = emissive[2].as<float>();
			m_Material.emissive.w = emissive[3].as<float>();
		}
		if (material["mosaicSize"]) {
			m_Material.MosaicSize = material["mosaicSize"].as<float>();
		}
		if (material["blockSize"]) {
			auto blockSize = material["blockSize"];
			m_Material.BlockSize.x = blockSize[0].as<float>();
			m_Material.BlockSize.y = blockSize[1].as<float>();
		}
	}
}

void Material::ExportFile(YAML::Emitter& out)
{
	out << YAML::Key << "vertexShader" << YAML::Value << m_VertexShader;
	out << YAML::Key << "pixelShader" << YAML::Value << m_PixelShader;

	int exist = SceneExporter::CheckTextureFileNameExist(MainEngine::GetInstance()->GetRenderCore()->GetTextureFileName(m_TextureFileID));
	out << YAML::Key << "textureFileID" << YAML::Value ;
	if (exist == -1)
		out << m_TextureFileID;
	else
		out << exist;
	
	out << YAML::Key << "bumpTextureFileID" << YAML::Value;
	exist = SceneExporter::CheckTextureFileNameExist(MainEngine::GetInstance()->GetRenderCore()->GetTextureFileName(m_BumpTextureFileID));
	if (exist == -1)
		out << m_BumpTextureFileID;
	else
		out << exist;

	// m_Materialの内容を出力
	out << YAML::Key << "material" << YAML::Value << YAML::BeginMap;
	out << YAML::Key << "textureEnable" << YAML::Value << (bool)m_Material.textureEnable;
	out << YAML::Key << "shininess" << YAML::Value << m_Material.shininess;
	out << YAML::Key << "specularPower" << YAML::Value << m_Material.SpecularPower;
	out << YAML::Key << "rgbShift" << YAML::Value << m_Material.RGBShift;
	out << YAML::Key << "ambient" << YAML::Value << YAML::Flow << YAML::BeginSeq 
		<< m_Material.ambient.x << m_Material.ambient.y << m_Material.ambient.z << m_Material.ambient.w << YAML::EndSeq;
	out << YAML::Key << "diffuse" << YAML::Value << YAML::Flow << YAML::BeginSeq 
		<< m_Material.diffuse.x << m_Material.diffuse.y << m_Material.diffuse.z << m_Material.diffuse.w << YAML::EndSeq;
	out << YAML::Key << "specular" << YAML::Value << YAML::Flow << YAML::BeginSeq 
		<< m_Material.specular.x << m_Material.specular.y << m_Material.specular.z << m_Material.specular.w << YAML::EndSeq;
	out << YAML::Key << "emissive" << YAML::Value << YAML::Flow << YAML::BeginSeq 
		<< m_Material.emissive.x << m_Material.emissive.y << m_Material.emissive.z << m_Material.emissive.w << YAML::EndSeq;
	out << YAML::Key << "mosaicSize" << YAML::Value << m_Material.MosaicSize;
	out << YAML::Key << "blockSize" << YAML::Value << YAML::Flow << YAML::BeginSeq 
		<< m_Material.BlockSize.x << m_Material.BlockSize.y << YAML::EndSeq;
	out << YAML::EndMap;
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
	renderer->SetVertexShader("vertex");
	renderer->SetPixelShader(m_PixelShader);
}
