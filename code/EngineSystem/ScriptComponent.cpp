#include "ScriptComponent.h"
#include "PrefabAsset.h"
#include <Windows.h>

REGISTER_COMPONENT(ScriptComponent)

void ScriptComponent::DrawGUI()
{
	for (const auto& prop : m_ScriptInstance->GetProperties()) {
		ImGui::Text("%s:", prop.Name.c_str());
		switch (prop.Type) {
		case PropertyType::INT:
			ImGui::InputInt(("##" + prop.Name).c_str(), static_cast<int*>(prop.Data));
			break;
		case PropertyType::FLOAT:
			ImGui::InputFloat(("##" + prop.Name).c_str(), static_cast<float*>(prop.Data));
			break;
		case PropertyType::DOUBLE:
			ImGui::InputDouble(("##" + prop.Name).c_str(), static_cast<double*>(prop.Data));
			break;
		case PropertyType::STRING:
			ImGui::InputText(("##" + prop.Name).c_str(), static_cast<std::string*>(prop.Data)->data(), 256);
			break;
		case PropertyType::BOOL:
			ImGui::Checkbox(("##" + prop.Name).c_str(), static_cast<bool*>(prop.Data));
			break;
		case PropertyType::VECTOR2:
			ImGui::InputFloat2(("##" + prop.Name).c_str(), reinterpret_cast<float*>(prop.Data));
			break;
		case PropertyType::VECTOR3:
			ImGui::InputFloat3(("##" + prop.Name).c_str(), reinterpret_cast<float*>(prop.Data));
			break;
		case PropertyType::VECTOR4:
			ImGui::InputFloat4(("##" + prop.Name).c_str(), reinterpret_cast<float*>(prop.Data));
			break;
		case PropertyType::METAFILE:
			// MetaFile の場合の処理をここに追加
			ImGui::Text("MetaFile property editing not implemented.");
			break;
		case PropertyType::PREFAB:
		{
			PrefabAsset* prefabAsset = static_cast<PrefabAsset*>(prop.Data);
			if (prefabAsset) {
				std::string displayName = prefabAsset->GetFileName();
				ImGui::TextWrapped("%s", displayName.c_str());

				// ファイル選択ボタン
				ImGui::SameLine();
				std::string buttonLabel = "...##" + prop.Name;
				if (ImGui::Button(buttonLabel.c_str(), ImVec2(30, 0))) {
					// ファイル選択ダイアログを開く
					OPENFILENAMEA ofn = {};
					char szFile[260] = { 0 };
					ofn.lStructSize = sizeof(ofn);
					ofn.hwndOwner = NULL;
					ofn.lpstrFile = szFile;
					ofn.nMaxFile = sizeof(szFile);
					ofn.lpstrFilter = "Prefab Files\0*.prefab\0All Files\0*.*\0";
					ofn.nFilterIndex = 1;
					ofn.lpstrFileTitle = NULL;
					ofn.nMaxFileTitle = 0;
					ofn.lpstrInitialDir = "scripts";
					ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

					if (GetOpenFileNameA(&ofn) == TRUE) {
						prefabAsset->SetPath(szFile);
					}
				}

				// ドラッグ&ドロップのターゲット
				if (ImGui::BeginDragDropTarget()) {
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("PREFAB_FILE")) {
						const char* droppedPath = static_cast<const char*>(payload->Data);
						prefabAsset->SetPath(std::string(droppedPath));
					}
					ImGui::EndDragDropTarget();
				}

				// クリアボタン
				if (prefabAsset->IsValid()) {
					ImGui::SameLine();
					std::string clearLabel = "X##clear_" + prop.Name;
					if (ImGui::Button(clearLabel.c_str(), ImVec2(20, 0))) {
						prefabAsset->SetPath("");
					}
				}
			}
			break;
		}
		default:
			ImGui::Text("Unknown property type.");
			break;
		}
	}
}
