#include "SceneExporter.h"

#include "Object.h"
#include "Component_Transform.h"
#include "MainEngine.h"

#include <memory>

#include <filesystem>

std::list<EngineMetaFile*> SceneExporter::m_ExportList;
std::list<VertexIndex*> SceneExporter::m_VertexIndicesExportList;

void SceneExporter::Export(std::list<Object*> objects)
{
    // Open a file for writing
    auto filePath = OpenExportFileDialog();
    if (filePath.empty()) {
        return; // cleanerが自動的にクリーンアップ
    }

    for (const auto& obj : objects) {
        obj->AddExportList();
    }

    // YAMLルートノードを作成 - スコープ管理とスマートポインタでより安全に
    YAML::Emitter out;

    out << YAML::BeginMap;
    out << YAML::Key << "objects" << YAML::Value << YAML::BeginSeq;

    // Write each object's data
    for (const auto& metaFile : m_ExportList) {
        if (!metaFile) continue; // null チェック追加
        out << YAML::BeginMap;

		out << YAML::Key << "classID" << YAML::Value << metaFile->GetClassID();
        out << YAML::Key << "fileID" << YAML::Value << metaFile->GetFileID();

        // Export metaFile details
        metaFile->ExportFile(out);

        out << YAML::EndMap;
    }
    out << YAML::EndSeq;
    out << YAML::EndMap;


    // ファイルに書き込み
    std::ofstream fout(filePath);
    if (!fout.is_open()) {
        throw std::runtime_error("Failed to open file for writing: " + filePath);
    }

    fout << out.c_str();
    fout.close();

    ExportVertexIndexList();
    ExportTextureInfoList();

    // cleanerが自動的にクリーンアップを実行
    
}

#include <unordered_set>

void SceneExporter::ExportVertexIndexList()
{
    std::unordered_set<std::string> existing_lines;

	YAML::Node vertexIndexList;
        
    // スマートポインタでYAML Nodeのライフサイクルを管理

    std::ifstream fileVertexInfoData("AssetList\\VertexIndexList.yaml");
    if (!fileVertexInfoData.is_open()) {
        // ファイルが存在しない場合はスキップ
		fileVertexInfoData.close();
		return;
	}

    YAML::Node existingData = YAML::Load(fileVertexInfoData);
    if (existingData && existingData["vertexIndices"]) {
        for (const auto& item : existingData["vertexIndices"]) {
            if (item && item["fileID"]) {
                existing_lines.insert(std::to_string(item["fileID"].as<int>()));
                // 既存のデータをvertexIndexListに追加
                vertexIndexList.push_back(item);
            }
        }
    }

    fileVertexInfoData.close();

    for (const auto& vertexIndex : m_VertexIndicesExportList)
    {
        std::filesystem::path fsPath(vertexIndex->GetFilePath());

        if (fsPath.extension().string() == ".csv") {
	        YAML::Emitter vertexIndexData;
            const auto& indexInfo = vertexIndex->GetIndexInfo();

            const auto& strPath = vertexIndex->GetFilePath();
            const auto& vertexInfo = vertexIndex->GetVertexInfo();
            std::ofstream vertexFile(strPath);


            vertexIndexData << YAML::BeginMap;
            if (!vertexIndex) continue; // null チェック追加


            // 頂点データ
            vertexIndexData << YAML::Key << "vertices" << YAML::Value << YAML::BeginSeq;

            for (const auto& vertex : vertexInfo) {
                vertexIndexData << YAML::BeginMap;
                vertexIndexData << YAML::Key << "position" << YAML::Value << YAML::BeginSeq;
                vertexIndexData << vertex.position.x << vertex.position.y << vertex.position.z << YAML::EndSeq;

                vertexIndexData << YAML::Key << "normal" << YAML::Value << YAML::BeginSeq;
                vertexIndexData << vertex.normal.x << vertex.normal.y << vertex.normal.z << YAML::EndSeq;


                vertexIndexData << YAML::Key << "color" << YAML::Value << YAML::BeginSeq;
                vertexIndexData << vertex.color.x << vertex.color.y << vertex.color.z << vertex.color.w << YAML::EndSeq;

                vertexIndexData << YAML::Key << "texcoord" << YAML::Value << YAML::BeginSeq;
                vertexIndexData << vertex.texcoord.x << vertex.texcoord.y << YAML::EndSeq;

                vertexIndexData << YAML::EndMap;
            }
            vertexIndexData << YAML::EndSeq;

            // インデックスデータ
            vertexIndexData << YAML::Key << "indices" << YAML::Value << YAML::Flow << YAML::BeginSeq;
            for (const auto& index : indexInfo) {
                vertexIndexData << index;
            }
            vertexIndexData << YAML::EndSeq;
            vertexIndexData << YAML::Key << "primitiveTopology" << YAML::Value << static_cast<int>(vertexIndex->GetPrimitiveTopology());
            vertexIndexData << YAML::EndMap;

            // ファイルに書き込み
            if (!vertexFile.is_open()) {
                throw std::runtime_error("Could not open vertex file for writing: " + strPath);
            }
            vertexFile << vertexIndexData.c_str();
            vertexFile.close();

        }
        const auto fileIDStr = std::to_string(vertexIndex->GetFileID());
        if (existing_lines.find(fileIDStr) == existing_lines.end()) {
            YAML::Node listItem;
            listItem["fileID"] = vertexIndex->GetFileID();
            listItem["name"] = vertexIndex->GetName();
            listItem["filePath"] = vertexIndex->GetFilePath();
            vertexIndexList.push_back(std::move(listItem));


        }
    } 

    // リストに追加（既存にない場合のみ）
    


    // 上書きモードで書き込み（追記モードではない）
    std::ofstream fileVertexInfoList("AssetList\\VertexIndexList.yaml", std::ios::out);
    if (fileVertexInfoList.is_open()) {
        fileVertexInfoList << vertexIndexList;
        if (!fileVertexInfoList.good()) {
            throw std::runtime_error("Failed to write vertex index list");
        }
        fileVertexInfoList.close();
    } else {
        throw std::runtime_error("Failed to open AssetList\\VertexIndexList.yaml for writing");
    }

    // 明示的なリセット
    vertexIndexList.reset();
}
void SceneExporter::ExportTextureInfoList()
{
    try {
        std::unordered_set<std::string> existing_lines;
        
        // スマートポインタでYAML Nodeのライフサイクルを管理
        {
            auto textureListRoot = std::make_unique<YAML::Node>();
            auto textureListNode = std::make_unique<YAML::Node>();

            // 既存のデータを読み込んで、textureListNodeに追加
            {
                std::ifstream fileTextureData("AssetList\\TextureList.yaml");
                if (fileTextureData.is_open()) {
                    try {
                        YAML::Node existingData = YAML::Load(fileTextureData);
                        if (existingData && existingData["textures"]) {
                            for (const auto& item : existingData["textures"]) {
                                if (item && item["fileID"]) {
                                    existing_lines.insert(item["filePath"].as<std::string>());
                                    // 既存のデータをtextureListNodeに追加
                                    textureListNode->push_back(item);
                                }
                            }
                        }
                    }
                    catch (const YAML::Exception& e) {
                        // 既存ファイルの読み込み失敗は警告として処理
                        OutputDebugStringA(("Warning: Failed to load existing texture list: " + std::string(e.what()) + "\n").c_str());
                    }
                    fileTextureData.close();
                }
            } // 既存データのYAML Nodeはここで適切に破棄される

            // MainEngineのnullチェック追加
            auto mainEngine = MainEngine::GetInstance();
            if (!mainEngine) {
                throw std::runtime_error("MainEngine instance is null");
            }

            auto renderCore = mainEngine->GetRenderCore();
            if (!renderCore) {
                throw std::runtime_error("RenderCore is null");
            }

            const auto& textureList = renderCore->GetTextureInfo();

            for (const auto& texture : textureList)
            {
                if (!texture || texture->GetFileID() == 0) continue;

                const WCHAR* filename = texture->filename.c_str();
                if (!filename) continue;

                int len = WideCharToMultiByte(CP_UTF8, 0, filename, -1, nullptr, 0, nullptr, nullptr);
                std::string narrowStringFilePath;
                if (len > 0) {
                    narrowStringFilePath.resize(len - 1); // null終端を除く
                    WideCharToMultiByte(CP_UTF8, 0, filename, -1, &narrowStringFilePath[0], len - 1, nullptr, nullptr);
                }

                const auto fileNameStr = texture->GetFileName();

                // 既存にない場合のみ追加
                if (existing_lines.find(fileNameStr) != existing_lines.end()) {
                    continue; // Skip if this FileID already exists
                }

                if (texture->toExport) {
                    YAML::Node textureNode;
                    textureNode["fileID"] = texture->GetFileID();
                    textureNode["filePath"] = narrowStringFilePath;
                    textureListNode->push_back(std::move(textureNode));
                }
            }

            (*textureListRoot)["textures"] = *textureListNode;

            std::ofstream fileTextureInfoList("AssetList\\TextureList.yaml");
            if (fileTextureInfoList.is_open()) {
                fileTextureInfoList << *textureListRoot;
                if (!fileTextureInfoList.good()) {
                    throw std::runtime_error("Failed to write texture info list");
                }
                fileTextureInfoList.close();
            } else {
                throw std::runtime_error("Failed to open AssetList\\TextureList.yaml for writing");
            }

            // 明示的なリセット
            textureListNode.reset();
            textureListRoot.reset();
        } // 全てのYAML Nodeがここで適切に破棄される
    }
    catch (const YAML::Exception& e) {
        // YAML固有のエラーハンドリング
        OutputDebugStringA(("YAML Error in ExportTextureInfoList: " + std::string(e.what()) + "\n").c_str());
        throw std::runtime_error("YAML processing failed in texture export: " + std::string(e.what()));
    }
    catch (const std::exception& e) {
        // エラーハンドリング
        OutputDebugStringA(("ExportTextureInfoList Error: " + std::string(e.what()) + "\n").c_str());
        throw; // 再スロー
    }
}

int SceneExporter::CheckTextureFileNameExist(std::string fileName)
{
    std::unordered_set<std::string> existing_lines;
    auto textureListNode = std::make_unique<YAML::Node>();

    // 既存のデータを読み込んで、textureListNodeに追加
    {
        std::ifstream fileTextureData("AssetList\\TextureList.yaml");
        if (fileTextureData.is_open()) {
            YAML::Node existingData = YAML::Load(fileTextureData);
            if (existingData && existingData["textures"]) {
                for (const auto& item : existingData["textures"]) {
                    if (item && item["filePath"]) {
                        auto data = item["filePath"].as<std::string>();
                        
                        // ファイルパスからファイル名だけを取得
                        std::filesystem::path fsPath(data);
                        std::string existingFileName = fsPath.filename().string();
                        
                        // 比較対象のファイル名も抽出（念のため）
                        std::filesystem::path inputPath(fileName);
                        std::string inputFileName = inputPath.filename().string();
                        
                        if (inputFileName == existingFileName) {
                            fileTextureData.close();
                            return item["fileID"].as<int>();
                        }
                    }
                }
            }
            
            fileTextureData.close();
        }
    } // 既存データのYAML Nodeはここで適切に破棄される

    return -1;
}
