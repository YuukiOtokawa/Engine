#include "Script.h"
#include "yaml.h"
#include "PrefabAsset.h"

void Script::Export(YAML::Emitter& out) {
    // 全てのプロパティをエクスポート
    for (const auto& prop : m_Properties) {
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
                out << YAML::Value << YAML::Flow << YAML::BeginSeq
                    << vec->x << vec->y << YAML::EndSeq;
                break;
            }

            case PropertyType::VECTOR3: {
                Vector3O* vec = static_cast<Vector3O*>(prop.Data);
                out << YAML::Value << YAML::Flow << YAML::BeginSeq
                    << vec->x << vec->y << vec->z << YAML::EndSeq;
                break;
            }

            case PropertyType::VECTOR4: {
                Vector4O* vec = static_cast<Vector4O*>(prop.Data);
                out << YAML::Value << YAML::Flow << YAML::BeginSeq
                    << vec->x << vec->y << vec->z << vec->w << YAML::EndSeq;
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

void Script::Import(YAML::Node& node) {
    // 全てのプロパティをインポート
    for (auto& prop : m_Properties) {
        if (!node[prop.Name]) continue; // プロパティが存在しない場合はスキップ

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
            }

            case PropertyType::CSV: {
                CSVFileInfo* text = static_cast<CSVFileInfo*>(prop.Data);
                auto info = node[prop.Name];
                text->filename = info[0].as<std::string>();
                text->filepath = info[1].as<std::string>();
            }

            case PropertyType::YAML: {
                YAMLFileInfo* text = static_cast<YAMLFileInfo*>(prop.Data);
                auto info = node[prop.Name];
                text->filename = info[0].as<std::string>();
                text->filepath = info[1].as<std::string>();
            }

            case PropertyType::SOUND: {
                SoundFileInfo* text = static_cast<SoundFileInfo*>(prop.Data);
                auto info = node[prop.Name];
                text->filename = info[0].as<std::string>();
                text->filepath = info[1].as<std::string>();
            }

            case PropertyType::METAFILE:
                // METAFILE は未実装
                break;
        }
    }
}
