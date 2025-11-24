// ========================================================
//
// プレハブクラス[Prefab.cpp]
//
//									Date:20250524
//									Author:Yuuki Otokawa
// ========================================================

#include "Prefab.h"
#include "Object.h"
#include "Component.h"
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
#include "Renderer.h"
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
#include "SceneImporter.h"
#include "FBXImporter.h"
#include "OBJLoader.h"

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

        // VertexIndexとTextureの情報を収集
        std::map<int, std::string> vertexIndexPaths;  // FileID -> FilePath
        std::map<int, std::string> texturePaths;      // FileID -> FilePath

        for (const auto& metaFile : exportList) {
            if (!metaFile) continue;

            // MeshFilterからVertexIndex情報を収集
            if (metaFile->GetClassID() == CID_Component_MeshFilter) {
                MeshFilter* meshFilter = static_cast<MeshFilter*>(metaFile);
                VertexIndex* vi = meshFilter->GetVertexIndex();
                if (vi && vertexIndexPaths.find(vi->GetFileID()) == vertexIndexPaths.end()) {
                    vertexIndexPaths[vi->GetFileID()] = vi->GetFilePath();
                }
            }

            // MaterialからTexture情報を収集
            if (metaFile->GetClassID() == CID_Material) {
                Material* material = static_cast<Material*>(metaFile);
                int textureID = material->GetTexture();
                if (textureID > 0 && texturePaths.find(textureID) == texturePaths.end()) {
                    // RenderCoreからテクスチャパスを取得
                    auto textureList = MainEngine::GetInstance()->GetRenderCore()->GetTextureInfo();
                    for (const auto& tex : textureList) {
                        if (tex && tex->GetFileID() == textureID) {
                            texturePaths[textureID] = tex->GetFileName();
                            break;
                        }
                    }
                }
            }
        }

        // YAMLエミッターを作成
        YAML::Emitter out;
        out << YAML::BeginMap;

        // Prefab情報
        out << YAML::Key << "prefabName" << YAML::Value << object->GetName();
        out << YAML::Key << "version" << YAML::Value << 1;

        // VertexIndex情報
        if (!vertexIndexPaths.empty()) {
            out << YAML::Key << "vertexIndices" << YAML::Value << YAML::BeginSeq;
            for (const auto& [fileID, path] : vertexIndexPaths) {
                out << YAML::BeginMap;
                out << YAML::Key << "fileID" << YAML::Value << fileID;
                out << YAML::Key << "filePath" << YAML::Value << path;
                out << YAML::EndMap;
            }
            out << YAML::EndSeq;
        }

        // Texture情報
        if (!texturePaths.empty()) {
            out << YAML::Key << "textures" << YAML::Value << YAML::BeginSeq;
            for (const auto& [fileID, path] : texturePaths) {
                out << YAML::BeginMap;
                out << YAML::Key << "fileID" << YAML::Value << fileID;
                out << YAML::Key << "filePath" << YAML::Value << path;
                out << YAML::EndMap;
            }
            out << YAML::EndSeq;
        }

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
        //==========================================================================
        // Prefabファイルの読み込み
        //==========================================================================
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

        //==========================================================================
        // Prefab内の頂点情報を読み込み
        //==========================================================================
        // VertexIndexのFileIDマッピング（元のFileID -> 実際のFileID）
        std::map<int, int> vertexIndexFileIDMap;

        if (prefabData["vertexIndices"]) {
            for (const auto& vertexInfo : prefabData["vertexIndices"]) {
                int originalFileID = vertexInfo["fileID"].as<int>();
                std::string path = vertexInfo["filePath"].as<std::string>();

                // 既に読み込み済みならそのまま使用
                VertexIndex* existingVI = Editor::GetInstance()->GetVertexIndexByFileID(originalFileID);
                if (existingVI != nullptr) {
                    vertexIndexFileIDMap[originalFileID] = originalFileID;
                    continue;
                }

                std::filesystem::path pathObj(path);
                std::string extension = pathObj.extension().string();
                std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

                if (extension == ".csv") {
                    SceneImporter::ImportMesh(path, originalFileID);
                    // CSVの場合は指定したfileIDがそのまま使用される
                    vertexIndexFileIDMap[originalFileID] = originalFileID;
                }
                else if (extension == ".fbx") {
                    FBXImporter fbxImporter;
                    fbxImporter.LoadFBX(path.c_str());
                    fbxImporter.LoadVertexIndex(path);
                    // 読み込んだVertexIndexを登録
                    VertexIndex** viArray = fbxImporter.GetVertexIndex();
                    if (viArray && viArray[0]) {
                        viArray[0]->SetFileID(originalFileID);
                        Editor::GetInstance()->AddVertexIndex(viArray[0]);
                        vertexIndexFileIDMap[originalFileID] = originalFileID;
                    }
                }
                else if (extension == ".obj") {
                    OBJLoader objLoader;
                    int actualFileID = objLoader.Load(path.c_str());
                    // OBJLoaderは新しいFileIDを割り当てる可能性があるのでマッピング
                    VertexIndex* loadedVI = Editor::GetInstance()->GetVertexIndexByFileID(actualFileID);
                    if (loadedVI) {
                        loadedVI->SetFileID(originalFileID);
                        vertexIndexFileIDMap[originalFileID] = originalFileID;
                    }
                }
            }
        }

        //==========================================================================
        // Prefab内のテクスチャを読み込み
        //==========================================================================
        if (prefabData["textures"]) {
            for (const auto& textureInfo : prefabData["textures"]) {
                int fileID = textureInfo["fileID"].as<int>();
                std::string texturePath = textureInfo["filePath"].as<std::string>();

                int len = MultiByteToWideChar(CP_UTF8, 0, texturePath.c_str(), -1, nullptr, 0);
                std::wstring wideStringFilePath;
                if (len > 0) {
                    wideStringFilePath.resize(len - 1);
                    MultiByteToWideChar(CP_UTF8, 0, texturePath.c_str(), -1, &wideStringFilePath[0], len);
                }

                MainEngine::GetInstance()->GetRenderCore()->TextureLoad(wideStringFilePath, fileID);
            }
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

        // マテリアルのインポート（コンポーネントより先に行う）
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

        // コンポーネントのインポート
        index = 0;
        for (const auto& objectNode : prefabData["objects"]) {
            auto it = metaFiles.begin();
            std::advance(it, index);
            EngineMetaFile* metaFile = *it;

            if (metaFile->GetClassID() != CID_Object && metaFile->GetClassID() != CID_Material) {
                Component* component = static_cast<Component*>(metaFile);
                YAML::Node mutableNode = objectNode;

                // MaterialのFileIDを新しいIDに更新
                if (mutableNode["materialFileID"]) {
                    int oldMaterialID = mutableNode["materialFileID"].as<int>();
                    auto matIt = fileIDMap.find(oldMaterialID);
                    if (matIt != fileIDMap.end()) {
                        // 新しいMaterialのFileIDで上書き
                        mutableNode["materialFileID"] = matIt->second->GetFileID();
                    }
                }

                // VertexIndexのFileIDを確認・更新
                if (mutableNode["vertexIndexFileID"]) {
                    int oldVertexIndexID = mutableNode["vertexIndexFileID"].as<int>();
                    auto viIt = vertexIndexFileIDMap.find(oldVertexIndexID);
                    if (viIt != vertexIndexFileIDMap.end()) {
                        // マッピングされたFileIDで上書き
                        mutableNode["vertexIndexFileID"] = viIt->second;
                    }
                }

                component->ImportFile(mutableNode);
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
    exportList.push_back(object);

    // オブジェクトのコンポーネントをリストに追加
    for (Component* component : object->GetComponents()) {
        if (component) {
            exportList.push_back(component);

            // RendererコンポーネントからMaterialを収集
            Renderer* renderer = dynamic_cast<Renderer*>(component);
            if (renderer && renderer->GetMaterial()) {
                Material* material = renderer->GetMaterial();
                // 既にリストに含まれていないか確認
                bool alreadyAdded = false;
                for (const auto& item : exportList) {
                    if (item == material) {
                        alreadyAdded = true;
                        break;
                    }
                }
                if (!alreadyAdded) {
                    exportList.push_back(material);
                }
            }
        }
    }

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
