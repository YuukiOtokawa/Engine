// ========================================================
//
// プレハブクラス[Prefab.cpp]
//
//									Date:20250524
//									Author:Yuuki Otokawa
// ========================================================

#include "Prefab.h"
#include "Object.h"
#include "Editor.h"
#include "EngineConsole.h"
#include "MainEngine.h"

#include "yaml.h"
#include <fstream>
#include <filesystem>

#include <Windows.h>
#include <commdlg.h>

#pragma comment(lib, "comdlg32.lib")

// コンポーネントのインクルード
#include "Component_Transform.h"
#include "Component_MeshFilter.h"
#include "Component_MeshRenderer.h"
#include "Component_Camera.h"
#include "EditorCamera.h"
#include "PlayerCamera.h"
#include "Animation.h"
#include "Billboard.h"
#include "Particle.h"
#include "Component_InputSystem.h"
#include "Component_Light.h"
#include "ScriptComponent.h"
#include "RenderTexture.h"
#include "SkinnedMeshRenderer.h"
#include "Collider.h"
#include "Material.h"

bool Prefab::Export(Object* object, const std::string& filePath)
{
    if (!object) {
        EngineConsole::LogError("Prefab::Export: オブジェクトがnullです");
        return false;
    }

    try {
        // エクスポートリストを収集
        std::list<EngineMetaFile*> exportList;
        CollectExportData(object, exportList);

        // YAMLエミッターを作成
        YAML::Emitter out;
        out << YAML::BeginMap;

        // Prefab情報
        out << YAML::Key << "prefabName" << YAML::Value << object->GetName();
        out << YAML::Key << "version" << YAML::Value << 1;

        // オブジェクトとコンポーネントのデータ
        out << YAML::Key << "objects" << YAML::Value << YAML::BeginSeq;

        for (const auto& metaFile : exportList) {
            if (!metaFile) continue;

            out << YAML::BeginMap;
            out << YAML::Key << "classID" << YAML::Value << metaFile->GetClassID();
            out << YAML::Key << "fileID" << YAML::Value << metaFile->GetFileID();
            metaFile->ExportFile(out);
            out << YAML::EndMap;
        }

        out << YAML::EndSeq;
        out << YAML::EndMap;

        // ファイルに書き込み
        std::ofstream fout(filePath);
        if (!fout.is_open()) {
            EngineConsole::LogError("Prefab::Export: ファイルを開けませんでした: %s", filePath.c_str());
            return false;
        }

        fout << out.c_str();
        fout.close();

        EngineConsole::Log("Prefab::Export: 保存しました: %s", filePath.c_str());
        return true;
    }
    catch (const std::exception& e) {
        EngineConsole::LogError("Prefab::Export: エラー: %s", e.what());
        return false;
    }
}

Object* Prefab::Import(const std::string& filePath)
{
    try {
        // ファイルを開く
        std::ifstream file(filePath);
        if (!file.is_open()) {
            EngineConsole::LogError("Prefab::Import: ファイルを開けませんでした: %s", filePath.c_str());
            return nullptr;
        }

        YAML::Node prefabData = YAML::Load(file);
        file.close();

        if (!prefabData["objects"]) {
            EngineConsole::LogError("Prefab::Import: 無効なPrefabファイルです");
            return nullptr;
        }

        // FileIDのマッピング（古いID -> 新しいMetaFile）
        std::map<int, EngineMetaFile*> fileIDMap;
        std::list<EngineMetaFile*> metaFiles;
        Object* rootObject = nullptr;

        // 1. 全てのオブジェクト/コンポーネントを生成
        for (const auto& objectNode : prefabData["objects"]) {
            int classID = objectNode["classID"].as<int>();
            int oldFileID = objectNode["fileID"].as<int>();

            EngineMetaFile* metaFile = CreateFromClassID(static_cast<ClassID>(classID));
            if (!metaFile) continue;

            // 新しいFileIDは自動的に割り当てられる
            fileIDMap[oldFileID] = metaFile;
            metaFiles.push_back(metaFile);

            // ルートオブジェクトを記録
            if (metaFile->GetClassID() == CID_Object && !rootObject) {
                rootObject = static_cast<Object*>(metaFile);
            }

            // Editorに登録
            if (metaFile->GetClassID() == CID_Object) {
                Editor::GetInstance()->AddObject(static_cast<Object*>(metaFile));
            }
            else if (metaFile->GetClassID() == CID_Material) {
                Editor::GetInstance()->AddMaterial(static_cast<Material*>(metaFile));
            }
            else {
                Editor::GetInstance()->CreateComponent(static_cast<Component*>(metaFile));
            }
        }

        // 2. ImportFileを呼び出してデータを復元
        // オブジェクトのインポート（FileIDマッピングを使用）
        int index = 0;
        for (const auto& objectNode : prefabData["objects"]) {
            auto it = metaFiles.begin();
            std::advance(it, index);
            EngineMetaFile* metaFile = *it;

            if (metaFile->GetClassID() == CID_Object) {
                Object* obj = static_cast<Object*>(metaFile);

                // 名前を設定
                if (objectNode["name"]) {
                    obj->SetName(objectNode["name"].as<std::string>());
                }

                // タグとレイヤー
                if (objectNode["tag"]) {
                    obj->SetTag(static_cast<GameObjectTag>(objectNode["tag"].as<int>()));
                }
                if (objectNode["layer"]) {
                    obj->SetLayer(static_cast<GameObjectLayer>(objectNode["layer"].as<int>()));
                }

                // コンポーネントの関連付け
                if (objectNode["components"]) {
                    for (const auto& compFileID : objectNode["components"]) {
                        int oldCompID = compFileID.as<int>();
                        auto compIt = fileIDMap.find(oldCompID);
                        if (compIt != fileIDMap.end()) {
                            Component* comp = static_cast<Component*>(compIt->second);
                            obj->AddComponentClass(comp, false);
                        }
                    }
                }
            }
            index++;
        }

        // コンポーネントのインポート
        index = 0;
        for (const auto& objectNode : prefabData["objects"]) {
            auto it = metaFiles.begin();
            std::advance(it, index);
            EngineMetaFile* metaFile = *it;

            if (metaFile->GetClassID() != CID_Object && metaFile->GetClassID() != CID_Material) {
                Component* component = static_cast<Component*>(metaFile);
                YAML::Node mutableNode = objectNode;
                component->ImportFile(mutableNode);
            }
            index++;
        }

        // マテリアルのインポート
        index = 0;
        for (const auto& objectNode : prefabData["objects"]) {
            auto it = metaFiles.begin();
            std::advance(it, index);
            EngineMetaFile* metaFile = *it;

            if (metaFile->GetClassID() == CID_Material) {
                Material* material = static_cast<Material*>(metaFile);
                YAML::Node mutableNode = objectNode;
                material->ImportFile(mutableNode);
            }
            index++;
        }

        EngineConsole::Log("Prefab::Import: 読み込みました: %s", filePath.c_str());
        return rootObject;
    }
    catch (const std::exception& e) {
        EngineConsole::LogError("Prefab::Import: エラー: %s", e.what());
        return nullptr;
    }
}

bool Prefab::ExportWithDialog(Object* object)
{
    if (!object) {
        EngineConsole::LogError("Prefab::ExportWithDialog: オブジェクトがnullです");
        return false;
    }

    std::string filePath = OpenExportDialog(object->GetName());
    if (filePath.empty()) {
        return false;
    }

    return Export(object, filePath);
}

Object* Prefab::ImportWithDialog()
{
    std::string filePath = OpenImportDialog();
    if (filePath.empty()) {
        return nullptr;
    }

    return Import(filePath);
}

void Prefab::CollectExportData(Object* object, std::list<EngineMetaFile*>& exportList)
{
    if (!object) return;

    // オブジェクト自体をリストに追加
    object->AddExportList();

    // SceneExporterのリストから取得（既存の仕組みを利用）
    // 注: AddExportListはSceneExporter::m_ExportListに追加するので
    // ここでは直接オブジェクトを追加
    exportList.push_back(object);

    // 子オブジェクトも再帰的に収集
    for (Object* child : object->GetChildren()) {
        CollectExportData(child, exportList);
    }
}

EngineMetaFile* Prefab::CreateFromClassID(ClassID classID)
{
    switch (classID) {
        case CID_Object:
            return new Object(false); // Editor自動登録を避ける
        case CID_Material:
            return new Material();
        case CID_Component_Transform:
            return new Transform();
        case CID_Component_MeshFilter:
            return new MeshFilter();
        case CID_Component_MeshRenderer:
            return new MeshRenderer();
        case CID_Component_RenderTexture:
            return new RenderTexture();
        case CID_Component_SkinnedMeshRenderer:
            return new SkinnedMeshRenderer();
        case CID_Component_Collider:
            return new Collider();
        case CID_Component_InputSystem:
            return new InputSystem();
        case CID_Component_Light:
            return new Light();
        case CID_Component_Camera:
            return new Camera();
        case CID_Component_EditorCamera:
            return new EditorCamera();
        case CID_Animation:
            return new Animation();
        case CID_Billboard:
            return new Billboard();
        case CID_Particle:
            return new Particle();
        case CID_Script:
            return new ScriptComponent();
        default:
            return nullptr;
    }
}

std::string Prefab::OpenExportDialog(const std::string& defaultName)
{
    char szFile[260] = { 0 };
    strncpy_s(szFile, defaultName.c_str(), sizeof(szFile) - 1);

    OPENFILENAMEA ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "Prefab Files (*.prefab)\0*.prefab\0All Files (*.*)\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = "prefabs";
    ofn.lpstrDefExt = "prefab";
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;

    if (GetSaveFileNameA(&ofn) == TRUE) {
        return std::string(szFile);
    }

    return "";
}

std::string Prefab::OpenImportDialog()
{
    char szFile[260] = { 0 };

    OPENFILENAMEA ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "Prefab Files (*.prefab)\0*.prefab\0All Files (*.*)\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = "prefabs";
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileNameA(&ofn) == TRUE) {
        return std::string(szFile);
    }

    return "";
}
