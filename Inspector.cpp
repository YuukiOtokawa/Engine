#include "Inspector.h"

#include "imgui.h"

#include "Object.h"

#include "../ComponentFactory.h"
#include "../ScriptFactory.h"
#include "../ScriptComponent.h"

void AddComponentPopup(Object* object)
{
	float windowWidth = ImGui::GetWindowSize().x;
	float buttonWidth = 150.0f;

	ImGui::SetCursorPosX((windowWidth - buttonWidth) * 0.5f);

	if (ImGui::Button("Add Component", ImVec2(buttonWidth, 25.0f))) {
		ImGui::OpenPopup("AddComponentPopup");
	}

	if (ImGui::BeginPopup("AddComponentPopup")) {
		
		auto names = ComponentFactory::GetInstance()->GetRegisteredComponentNames();

		for (const auto& name : names) {
			if (name == "ScriptComponent") continue;

			if (ImGui::Selectable(name.c_str())) {
				Component* newComponent = ComponentFactory::GetInstance()->CreateComponent(name);
				if (newComponent) {
					object->AddComponentClass(newComponent);
				}
				ImGui::CloseCurrentPopup();
			}

		}
		auto scriptNames = ScriptFactory::GetInstance().GetRegisteredScriptNames();
		for (const auto& scriptName : scriptNames) {
			if (ImGui::Selectable(scriptName.c_str())) {
					
				auto* scriptComp = object->AddComponent<ScriptComponent>();

				Script* newScript = ScriptFactory::GetInstance().CreateScript(scriptName);

				if (newScript && scriptComp) {
					scriptComp->SetScript(std::unique_ptr<Script>(newScript));
				}

				ImGui::CloseCurrentPopup();
			}
		}

		ImGui::EndPopup();
	}
}

#include "CSVImporter.h"
#include "FBXImporter.h"
#include "MainEngine.h"

#include "Component_MeshFilter.h"
#include <filesystem>
void GetMesh(MeshFilter* meshFilter)
{
	std::string filePath = OpenImportFileDialog();

	// ファイルパスオブジェクト
	std::filesystem::path path(filePath);

	// 拡張子を取得して小文字に変換
	std::string extension = path.extension().string();
	std::transform(extension.begin(), extension.end(), extension.begin(), [](unsigned char c) { return std::tolower(c); });

	if (extension == ".csv") {
		int fileID = CSVImporter::ImportMesh(filePath);
		if (fileID == -1) return;
		meshFilter->SetMesh(fileID);
	}

	if (extension == ".fbx") {
		FBXImporter fbxImporter;
		fbxImporter.LoadFBX(filePath.c_str());
	}
}

std::string GetOpenFilePath()
{
	return std::string();
}
