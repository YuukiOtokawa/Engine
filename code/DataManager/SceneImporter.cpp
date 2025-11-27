#include "SceneImporter.h"

#include "Component_Transform.h"

#include <fstream>
#include <yaml-cpp/yaml.h>

#include "Component_MeshFilter.h"
#include "Material.h"

#include "MainEngine.h"
#include "Editor.h"

// 必要な派生クラスのヘッダーをインクルード
#include "Component_MeshRenderer.h"
#include "Component_Camera.h"
#include "EditorCamera.h"
#include "PlayerCamera.h"
#include "PlaneMesh.h"
#include "Animation.h"
#include "SpriteMesh.h"
#include "Billboard.h"
#include "Particle.h"
#include "Component_CubeMesh.h"
#include "MonoBehavior.h"
#include "Component_InputSystem.h"
#include "Component_Light.h"
#include "ScriptComponent.h"
#include "RenderTexture.h"
#include "SkinnedMeshRenderer.h"
#include "Collider.h"
#include "RectTransform.h"
#include "Canvas.h"
#include "UIImageMesh.h"
#include "UIImage.h"

#include "OBJLoader.h"

#include <filesystem>

#include <vector>
#include <sstream>

std::list<Object*> SceneImporter::Import(std::string filePath)
{
	//==========================================================================
	// 頂点情報の読み込み
	//==========================================================================
	{
		//==========================================================================
		// 頂点情報のリストを取得 すべての頂点情報のファイル名、ID、パスを取得
		//==========================================================================

		std::ifstream vertexList("AssetList\\VertexIndexList.yaml");

		if (!vertexList.is_open()) {
			throw std::runtime_error("Could not open file: VertexIndexList.yaml");
		}

		YAML::Node vertexListData = YAML::Load(vertexList);
		vertexList.close();

		//==========================================================================
		// リストから取得したすべての頂点情報の実データを取得、登録
		//==========================================================================

		for (const auto& vertexindexinfo : vertexListData) {
			int fileID = vertexindexinfo["fileID"].as<int>();
			std::string name = vertexindexinfo["name"].as<std::string>();
			std::string path = vertexindexinfo["filePath"].as<std::string>();

			if (Editor::GetInstance()->GetVertexIndexByFileID(fileID) != nullptr)
				continue;

			//std::ifstream file(path);

			//if (!file.is_open()) {
			//	throw std::runtime_error("Could not open file: " + path);
			//}

			//YAML::Node vertexIndexData = YAML::Load(file);
			//file.close();

			//==========================================================================
			// 頂点データとインデックスデータの読み込み
			//==========================================================================

			std::vector<VERTEX> vertices;
			std::vector<unsigned int> indices;

			// ファイルパスオブジェクト
			std::filesystem::path pathObj(path);

			// 拡張子を取得して小文字に変換
			std::string extension = pathObj.extension().string();
			std::transform(extension.begin(), extension.end(), extension.begin(), [](unsigned char c) { return std::tolower(c); });

			if (extension == ".csv") {
				SceneImporter::ImportMesh(path, fileID);
			}

			if (extension == ".fbx") {
				FBXImporter fbxImporter;
				fbxImporter.LoadFBX(path.c_str());
			}

			if (extension == ".obj") {
				OBJLoader objLoader;
				objLoader.Load(path.c_str());
			}


		}
	}

TEXTURE_LOAD:
	//==========================================================================
	// テクスチャデータの読み込み
	//==========================================================================

	{
		std::ifstream textureList("AssetList\\TextureList.yaml");

		if (!textureList.is_open()) {
			throw std::runtime_error("Could not open file: TextureList.yaml");
		}

		YAML::Node textureListData = YAML::Load(textureList);
		textureList.close();

		if (textureListData["textures"]) {
			for (const auto& textureinfo : textureListData["textures"]) {
				int fileID = textureinfo["fileID"].as<int>();
				std::string filePath = textureinfo["filePath"].as<std::string>();

				int len = MultiByteToWideChar(CP_UTF8, 0, filePath.c_str(), -1, nullptr, 0);
				std::wstring wideStringFilePath;
				if (len > 0) {
					wideStringFilePath.resize(len - 1); // null終端を除く
					MultiByteToWideChar(CP_UTF8, 0, filePath.c_str(), -1, &wideStringFilePath[0], len);
				}

				MainEngine::GetInstance()->GetRenderCore()->TextureLoad(wideStringFilePath, fileID);
			}
		}
	}

	//==========================================================================
	// 他ファイルの読み込み
	// Object Component Material
	//==========================================================================

	// YAMLファイルを開く
	std::ifstream file(filePath);
	// 出力するオブジェクトのリスト
	std::list<Object*> output;

	std::list<EngineMetaFile*> metaFiles;

	if (!file.is_open()) {
		throw std::runtime_error("Could not open file: " + filePath);
	}

	YAML::Node sceneData = YAML::Load(file);
	file.close();

	if (!sceneData["objects"]) {
		throw std::runtime_error("Invalid scene file: objects node not found");
	}

	// オブジェクトデータの読み込み
	for (const auto& objectNode : sceneData["objects"]) {
		int classID = objectNode["classID"].as<int>();
		int fileID = objectNode["fileID"].as<int>();

		// 空のMetaFileを作る
		auto metaFile = CreateObject(static_cast<ClassID>(classID));
		// ClassIDとFileIDを設定
		metaFile->CreateEngineMetaFile(static_cast<ClassID>(classID), fileID);
		metaFiles.push_back(metaFile);

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

	// オブジェクトのImportFile呼び出し
	{
		for (size_t i = 0; i < sceneData["objects"].size(); ++i) {
			YAML::Node objectNode = sceneData["objects"][i];
			auto metaFile = *std::next(metaFiles.begin(), i);
			if (metaFile->GetClassID() == CID_Object) {
				Object* obj = static_cast<Object*>(metaFile);
				obj->ImportFile(objectNode);
				output.push_back(obj);
			}
		}
	}

	// コンポーネントのImportFile呼び出し
	{
		for (size_t i = 0; i < sceneData["objects"].size(); ++i) {
			YAML::Node objectNode = sceneData["objects"][i];
			auto metaFile = *std::next(metaFiles.begin(), i);
			if (metaFile->GetClassID() != CID_Object && metaFile->GetClassID() != CID_Material) {
				Component* component = static_cast<Component*>(metaFile);
				component->ImportFile(objectNode);
				Editor::GetInstance()->CreateComponent(component);
			}
		}
	}

	// マテリアルのImportFile呼び出し
	{
		for (size_t i = 0; i < sceneData["objects"].size(); ++i) {
			YAML::Node objectNode = sceneData["objects"][i];
			auto metaFile = *std::next(metaFiles.begin(), i);
			if (metaFile->GetClassID() == CID_Material) {
				Material* material = static_cast<Material*>(metaFile);
				material->ImportFile(objectNode);
			}
		}
	}

	return output;
}

#include <filesystem>

int SceneImporter::ImportMesh(std::string filePath, int fileID)
{
	//==========================================================================
	// 頂点情報の読み込み
	//==========================================================================

	std::ifstream file(filePath);

	if (!file.is_open()) {
		return -1;
	}

	YAML::Node vertexIndexData = YAML::Load(file);
	file.close();

	//==========================================================================
	// 頂点データとインデックスデータの読み込み
	//==========================================================================

	std::vector<VERTEX> vertices;
	std::vector<unsigned int> indices;

	// 頂点データの読み込み
	if (vertexIndexData["vertices"]) {
		for (const auto& vertexNode : vertexIndexData["vertices"]) {
			VERTEX vertex;
			if (vertexNode["position"]) {
				auto pos = vertexNode["position"];
				vertex.position = Vector3O(pos[0].as<float>(), pos[1].as<float>(), pos[2].as<float>());
			}
			if (vertexNode["normal"]) {
				auto norm = vertexNode["normal"];
				vertex.normal = Vector3O(norm[0].as<float>(), norm[1].as<float>(), norm[2].as<float>());
			}
			if (vertexNode["color"]) {
				auto col = vertexNode["color"];
				vertex.color = Vector4O(col[0].as<float>(), col[1].as<float>(), col[2].as<float>(), col[3].as<float>());
			}
			if (vertexNode["texcoord"]) {
				auto tex = vertexNode["texcoord"];
				vertex.texcoord = Vector2O(tex[0].as<float>(), tex[1].as<float>());
			}
			vertices.push_back(vertex);
		}
	}

	// インデックスデータの読み込み
	if (vertexIndexData["indices"]) {
		for (const auto& indexNode : vertexIndexData["indices"]) {
			indices.push_back(indexNode.as<unsigned int>());
		}
	}

	//==========================================================================
	// VertexIndexの作成と登録
	//==========================================================================

	std::filesystem::path p = filePath;
	std::string filename = p.stem().string();

	VertexIndex* vi = new VertexIndex(filename, vertices, indices);

	if (vertexIndexData["primitiveTopology"]) {
		vi->SetPrimitiveTopology(static_cast<D3D11_PRIMITIVE_TOPOLOGY>(vertexIndexData["primitiveTopology"].as<int>()));
	}

	if (fileID != -1) {
		vi->SetFileID(fileID);
	}

	return Editor::GetInstance()->AddVertexIndex(vi);
}

EngineMetaFile* SceneImporter::CreateObject(ClassID classID)
{
	switch (classID) {
		case CID_Object:
			return new Object();
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
		case CID_RectTransform:
			return new RectTransform();
		case CID_Canvas:
			return new Canvas();
		case CID_UIImageMesh:
			return new UIImageMesh();
		case CID_UIImage:
			return new UIImage();
		case CID_None:
		default:
			return nullptr;
	}
}

Vector4O SceneImporter::ImportVector4O(const std::string& token1, const std::string& token2, const std::string& token3, const std::string& token4)
{
	return Vector4O(std::stof(token1), std::stof(token2), std::stof(token3), std::stof(token4));
}
