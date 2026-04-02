#include "ScriptComponent.h"
#include "PrefabAsset.h"
#include <yaml-cpp/yaml.h>
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
				auto path = GetDropPath("PREFAB");
				if (path != "") {
					prefabAsset->SetPath(path);
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
		case PropertyType::TEXT:
		{
			TextFileInfo* text = static_cast<TextFileInfo*>(prop.Data);
			if (text) {
				std::string displayName = text->filename;
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
					ofn.lpstrFilter = "Text Files\0*.txt\0All Files\0*.*\0";
					ofn.nFilterIndex = 1;
					ofn.lpstrFileTitle = NULL;
					ofn.nMaxFileTitle = 0;
					ofn.lpstrInitialDir = "scripts";
					ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

					if (GetOpenFileNameA(&ofn) == TRUE) {
						text->filepath = szFile;
						text->filename = fs::path(szFile).filename().string();
					}
				}

				// ドラッグ&ドロップのターゲット
				auto path = GetDropPath("TEXT");
				if (path != "") {
					text->filepath = path;
					text->filename = fs::path(path).filename().string();
				}

				// クリアボタン
				if (!text->filename.empty()) {
					ImGui::SameLine();
					std::string clearLabel = "X##clear_" + prop.Name;
					if (ImGui::Button(clearLabel.c_str(), ImVec2(20, 0))) {
						text->filename.clear();
						text->filepath.clear();
					}
				}
			}
			break;
		}
		case PropertyType::CSV:
		{
			CSVFileInfo* text = static_cast<CSVFileInfo*>(prop.Data);
			if (text) {
				std::string displayName = text->filename;
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
					ofn.lpstrFilter = "CSV Files\0*.csv\0All Files\0*.*\0";
					ofn.nFilterIndex = 1;
					ofn.lpstrFileTitle = NULL;
					ofn.nMaxFileTitle = 0;
					ofn.lpstrInitialDir = "scripts";
					ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

					if (GetOpenFileNameA(&ofn) == TRUE) {
						text->filepath = szFile;
						text->filename = fs::path(szFile).filename().string();
					}
				}

				// ドラッグ&ドロップのターゲット
				auto path = GetDropPath("CSV");
				if (path != "") {
					text->filepath = path;
					text->filename = fs::path(path).filename().string();
				}

				// クリアボタン
				if (!text->filename.empty()) {
					ImGui::SameLine();
					std::string clearLabel = "X##clear_" + prop.Name;
					if (ImGui::Button(clearLabel.c_str(), ImVec2(20, 0))) {
						text->filename.clear();
						text->filepath.clear();
					}
				}
			}
			break;
		}
		case PropertyType::YAML:
		{
			YAMLFileInfo* text = static_cast<YAMLFileInfo*>(prop.Data);
			if (text) {
				std::string displayName = text->filename;
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
					ofn.lpstrFilter = "YAML Files\0*.yaml;*.yml\0All Files\0*.*\0";
					ofn.nFilterIndex = 1;
					ofn.lpstrFileTitle = NULL;
					ofn.nMaxFileTitle = 0;
					ofn.lpstrInitialDir = "scripts";
					ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

					if (GetOpenFileNameA(&ofn) == TRUE) {
						text->filepath = szFile;
						text->filename = fs::path(szFile).filename().string();
					}
				}

				// ドラッグ&ドロップのターゲット
				auto path = GetDropPath("YAML");
				if (path != "") {
					text->filepath = path;
					text->filename = fs::path(path).filename().string();
				}

				// クリアボタン
				if (!text->filename.empty()) {
					ImGui::SameLine();
					std::string clearLabel = "X##clear_" + prop.Name;
					if (ImGui::Button(clearLabel.c_str(), ImVec2(20, 0))) {
						text->filename.clear();
						text->filepath.clear();
					}
				}
			}
			break;
		}
		case PropertyType::SOUND:
		{
			SoundFileInfo* text = static_cast<SoundFileInfo*>(prop.Data);
			if (text) {
				std::string displayName = text->filename;
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
					ofn.lpstrFilter = "YAML Files\0*.yaml;*.yml\0All Files\0*.*\0";
					ofn.nFilterIndex = 1;
					ofn.lpstrFileTitle = NULL;
					ofn.nMaxFileTitle = 0;
					ofn.lpstrInitialDir = "scripts";
					ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

					if (GetOpenFileNameA(&ofn) == TRUE) {
						text->filepath = szFile;
						text->filename = fs::path(szFile).filename().string();
					}
				}

				// ドラッグ&ドロップのターゲット
				auto path = GetDropPath("SOUND");
				if (path != "") {
					text->filepath = path;
					text->filename = fs::path(path).filename().string();
				}

				// クリアボタン
				if (!text->filename.empty()) {
					ImGui::SameLine();
					std::string clearLabel = "X##clear_" + prop.Name;
					if (ImGui::Button(clearLabel.c_str(), ImVec2(20, 0))) {
						text->filename.clear();
						text->filepath.clear();
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

	// スクリプト固有のカスタムGUI描画
	m_ScriptInstance->DrawCustomGUI();
}

void ScriptComponent::ExportComponent(YAML::Emitter& out) {
    if (!m_ScriptInstance) return;

    out << YAML::Key << "scriptName" << YAML::Value << m_ScriptInstance->GetScriptName();

    for (const auto& prop : m_ScriptInstance->GetProperties()) {
        out << YAML::Key << prop.Name;

        switch (prop.Type) {
            case PropertyType::INT:
                out << YAML::Value << *static_cast<int*>(prop.Data);
                break;
            case PropertyType::FLOAT:
                out << YAML::Value << *static_cast<float*>(prop.Data);
                break;
            case PropertyType::DOUBLE:
                out << YAML::Value << *static_cast<double*>(prop.Data);
                break;
            case PropertyType::STRING:
                out << YAML::Value << *static_cast<std::string*>(prop.Data);
                break;
            case PropertyType::BOOL:
                out << YAML::Value << *static_cast<bool*>(prop.Data);
                break;
            case PropertyType::VECTOR2: {
                Vector2O* vec = static_cast<Vector2O*>(prop.Data);
                out << YAML::Value << YAML::Flow << YAML::BeginSeq << vec->x << vec->y << YAML::EndSeq;
                break;
            }
            case PropertyType::VECTOR3: {
                Vector3O* vec = static_cast<Vector3O*>(prop.Data);
                out << YAML::Value << YAML::Flow << YAML::BeginSeq << vec->x << vec->y << vec->z << YAML::EndSeq;
                break;
            }
            case PropertyType::VECTOR4: {
                Vector4O* vec = static_cast<Vector4O*>(prop.Data);
                out << YAML::Value << YAML::Flow << YAML::BeginSeq << vec->x << vec->y << vec->z << vec->w << YAML::EndSeq;
                break;
            }
            case PropertyType::PREFAB: {
                PrefabAsset* prefabAsset = static_cast<PrefabAsset*>(prop.Data);
                out << YAML::Value << prefabAsset->GetPath();
                break;
            }
            case PropertyType::TEXT: {
                TextFileInfo* text = static_cast<TextFileInfo*>(prop.Data);
                out << YAML::Value << YAML::BeginSeq << text->filename << text->filepath << YAML::EndSeq;
                break;
            }
            case PropertyType::CSV: {
                CSVFileInfo* csv = static_cast<CSVFileInfo*>(prop.Data);
                out << YAML::Value << YAML::BeginSeq << csv->filename << csv->filepath << YAML::EndSeq;
                break;
            }
            case PropertyType::YAML: {
                YAMLFileInfo* yaml = static_cast<YAMLFileInfo*>(prop.Data);
                out << YAML::Value << YAML::BeginSeq << yaml->filename << yaml->filepath << YAML::EndSeq;
                break;
            }
            case PropertyType::SOUND: {
                SoundFileInfo* sound = static_cast<SoundFileInfo*>(prop.Data);
                out << YAML::Value << YAML::BeginSeq << sound->filename << sound->filepath << YAML::EndSeq;
                break;
            }
            case PropertyType::METAFILE:
                // METAFILE は未実装
                out << YAML::Value << "";
                break;
        }
    }
}

void ScriptComponent::ImportFile(YAML::Node& node) {
    if (node["tag"]) {
        tag = static_cast<Tag>(node["tag"].as<int>());
    }
    if (node["scriptName"]) {
        m_ScriptName = node["scriptName"].as<std::string>();
        m_ScriptInstance = ScriptFactory::GetInstance().CreateScript(m_ScriptName);
        if (m_ScriptInstance) {
            m_ScriptInstance->gameobject = this->owner;
            m_ScriptInstance->BindProperties();

            for (auto& prop : m_ScriptInstance->GetProperties()) {
                if (!node[prop.Name]) continue;
        
                switch (prop.Type) {
                    case PropertyType::INT:
                        *static_cast<int*>(prop.Data) = node[prop.Name].as<int>();
                        break;
                    case PropertyType::FLOAT:
                        *static_cast<float*>(prop.Data) = node[prop.Name].as<float>();
                        break;
                    case PropertyType::DOUBLE:
                        *static_cast<double*>(prop.Data) = node[prop.Name].as<double>();
                        break;
                    case PropertyType::STRING:
                        *static_cast<std::string*>(prop.Data) = node[prop.Name].as<std::string>();
                        break;
                    case PropertyType::BOOL:
                        *static_cast<bool*>(prop.Data) = node[prop.Name].as<bool>();
                        break;
                    case PropertyType::VECTOR2: {
                        Vector2O* vec = static_cast<Vector2O*>(prop.Data);
                        auto arr = node[prop.Name];
                        if (arr.IsSequence() && arr.size() >= 2) {
                            vec->x = arr[0].as<float>();
                            vec->y = arr[1].as<float>();
                        }
                        break;
                    }
                    case PropertyType::VECTOR3: {
                        Vector3O* vec = static_cast<Vector3O*>(prop.Data);
                        auto arr = node[prop.Name];
                        if (arr.IsSequence() && arr.size() >= 3) {
                            vec->x = arr[0].as<float>();
                            vec->y = arr[1].as<float>();
                            vec->z = arr[2].as<float>();
                        }
                        break;
                    }
                    case PropertyType::VECTOR4: {
                        Vector4O* vec = static_cast<Vector4O*>(prop.Data);
                        auto arr = node[prop.Name];
                        if (arr.IsSequence() && arr.size() >= 4) {
                            vec->x = arr[0].as<float>();
                            vec->y = arr[1].as<float>();
                            vec->z = arr[2].as<float>();
                            vec->w = arr[3].as<float>();
                        }
                        break;
                    }
                    case PropertyType::PREFAB: {
                        PrefabAsset* prefabAsset = static_cast<PrefabAsset*>(prop.Data);
                        std::string path = node[prop.Name].as<std::string>();
                        prefabAsset->SetPath(path);
                        break;
                    }
                    case PropertyType::TEXT: {
                         TextFileInfo* text = static_cast<TextFileInfo*>(prop.Data);
                         auto info = node[prop.Name];
                         text->filename = info[0].as<std::string>();
                         text->filepath = info[1].as<std::string>();
                         break;
                    }
                    case PropertyType::CSV: {
                         CSVFileInfo* csv = static_cast<CSVFileInfo*>(prop.Data);
                         auto info = node[prop.Name];
                         csv->filename = info[0].as<std::string>();
                         csv->filepath = info[1].as<std::string>();
                         break;
                    }
                    case PropertyType::YAML: {
                         YAMLFileInfo* yaml = static_cast<YAMLFileInfo*>(prop.Data);
                         auto info = node[prop.Name];
                         yaml->filename = info[0].as<std::string>();
                         yaml->filepath = info[1].as<std::string>();
                         break;
                    }
                    case PropertyType::SOUND: {
                         SoundFileInfo* sound = static_cast<SoundFileInfo*>(prop.Data);
                         auto info = node[prop.Name];
                         sound->filename = info[0].as<std::string>();
                         sound->filepath = info[1].as<std::string>();
                         break;
                    }
                    case PropertyType::METAFILE:
                        // METAFILE は未実装
                        break;
                }
            }
        }
    }
}

