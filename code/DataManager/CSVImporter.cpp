#include "CSVImporter.h"

#include "Component_Transform.h"

#include "fstream"

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

#include <vector>
#include <sstream>

std::list<Object*> CSVImporter::Import(std::string filePath)
{
	//==========================================================================
	// 頂点情報の読み込み
	//==========================================================================
	{
		//==========================================================================
		// 頂点情報のリストを取得 すべての頂点情報のファイル名、ID、パスを取得
		//==========================================================================

		std::ifstream vertexList("AssetList\\VertexIndexList.csv");

		if (!vertexList.is_open()) {
			throw std::runtime_error("Could not open file: " + std::string("VertexIndexList.csv"));
		}

		// 一行分のデータを格納する文字列
		std::string line;
		// 最終データを格納するvector
		std::vector<std::vector<std::string>> data;


		while (std::getline(vertexList, line)) {
			// コンマで分けられた1行分のデータ
			std::vector<std::string> tokens;
			// 生の一行分のデータ
			std::stringstream ss(line);
			// コンマで分けられた一つ分のデータ
			std::string token;
			// コンマごとにデータを読み込んでvectorに格納
			while (std::getline(ss, token, ',')) {
				tokens.push_back(token);
			}
			// 最終データのvectorに一行分のデータを格納
			data.push_back(tokens);
		}

		vertexList.close();

		//==========================================================================
		// リストから取得したすべての頂点情報の実データを取得、登録
		//==========================================================================

		for (auto& vertexindexinfo : data) {
			if (Editor::GetInstance()->GetVertexIndexByFileID(std::stoi(vertexindexinfo[0])) != nullptr)
				continue;

			std::ifstream file(vertexindexinfo[2]); // ファイル名で開く

			if (!file.is_open()) {
				throw std::runtime_error("Could not open file: " + filePath);
			}

			// 一行分のデータを格納する文字列
			std::string line;
			// 最終データを格納するvector
			std::vector<std::vector<std::string>> vertexData;
			std::vector<std::string> indexData;
			std::string topologyString;

			//==========================================================================
			// 頂点のデータとインデックスのデータを分けて取得
			//==========================================================================

			bool isVertex = true;
			bool isIndex = true;
			while (std::getline(file, line)) {
				// コンマで分けられた1行分のデータ
				std::vector<std::string> tokens;
				// 生の一行分のデータ
				std::stringstream ss(line);
				// コンマで分けられた一つ分のデータ
				std::string token;
				// コンマごとにデータを読み込んでvectorに格納
				while (std::getline(ss, token, ',')) {
					tokens.push_back(token);
				}
				if (isVertex && tokens[0] == "&") {
					isVertex = false;
					continue;
				}
				// 最終データのvectorに一行分のデータを格納
				if (isVertex)
					vertexData.push_back(tokens);
				else {
					if (isIndex) {
						indexData = tokens;
						isIndex = false;
					}
					else
						topologyString = tokens[0];
				}
			}

			//==========================================================================
			// VertexIndexの作成と登録
			//==========================================================================

			std::vector<VERTEX> vertices;
			std::vector<unsigned int> indices;

			for (auto& tokens : vertexData) {
				VERTEX vertex;
				vertex.position = Vector3O(std::stof(tokens[0]), std::stof(tokens[1]), std::stof(tokens[2]));
				vertex.normal = Vector3O(std::stof(tokens[3]), std::stof(tokens[4]), std::stof(tokens[5]));
				vertex.color = Vector4O(std::stof(tokens[6]), std::stof(tokens[7]), std::stof(tokens[8]), std::stof(tokens[9]));
				vertex.texcoord = Vector2O(std::stof(tokens[10]), std::stof(tokens[11]));
				vertices.push_back(vertex);
			}
			for (auto& tokens : indexData) {
				indices.push_back(std::stoi(tokens));
			}


			VertexIndex* vi = new VertexIndex(vertexindexinfo[1], vertices, indices);
			vi->SetPrimitiveTopology(static_cast<D3D11_PRIMITIVE_TOPOLOGY>(std::stoi(topologyString)));
			vi->SetFileID(std::stoi(vertexindexinfo[0]));
			Editor::GetInstance()->AddVertexIndex(vi);

			file.close();
		}

	}

	//==========================================================================
	// テクスチャデータの読み込み
	//==========================================================================

	{
		std::ifstream textureList("AssetList\\TextureList.csv");

		if (!textureList.is_open()) {
			throw std::runtime_error("Could not open file: " + std::string("TextureList.csv"));
		}

		// 一行分のデータを格納する文字列
		std::string line;
		// 最終データを格納するvector
		std::vector<std::vector<std::string>> data;


		while (std::getline(textureList, line)) {
			// コンマで分けられた1行分のデータ
			std::vector<std::string> tokens;
			// 生の一行分のデータ
			std::stringstream ss(line);
			// コンマで分けられた一つ分のデータ
			std::string token;
			// コンマごとにデータを読み込んでvectorに格納
			while (std::getline(ss, token, ',')) {
				tokens.push_back(token);
			}
			// 最終データのvectorに一行分のデータを格納
			data.push_back(tokens);
		}

		textureList.close();

		for (auto& textureinfo : data) {
			int len = MultiByteToWideChar(CP_UTF8, 0, textureinfo[1].c_str(), -1, nullptr, 0);
			std::wstring wideStringFilePath;
			if (len > 0) {
				wideStringFilePath.resize(len - 1); // null終端を除く
				MultiByteToWideChar(CP_UTF8, 0, textureinfo[1].c_str(), -1, &wideStringFilePath[0], len);
			}

			MainEngine::GetInstance()->GetRenderCore()->TextureLoad(wideStringFilePath, std::stoi(textureinfo[0]));
		}


	}

	//==========================================================================
	// 他ファイルの読み込み
	// Object Component Material
	//==========================================================================


	// Open the CSV file
	// 開くファイル
	std::ifstream file(filePath);
	// 出力するオブジェクトのリスト
	std::list<Object*> output;

	std::list<EngineMetaFile*> metaFiles;

	if (!file.is_open()) {
		throw std::runtime_error("Could not open file: " + std::string(filePath));
	}

	//一行分のデータを格納する文字列
	std::string line;
	std::vector<std::vector<std::string>> data;


	while (std::getline(file, line)) {
		// Split the line by commas
		// コンマで分かれた1個分のデータのvector
		std::vector<std::string> tokens;
		std::stringstream ss(line);
		std::string token;
		// データを読み込んでvectorに格納
		while (std::getline(ss, token, ',')) {
			tokens.push_back(token);
		}
		data.push_back(tokens);

		// 空のMetaFileを作る
		auto metaFile = CreateObject(static_cast<ClassID>(std::stoi(tokens[0])));
		// ClassIDとFileIDを設定
		metaFile->CreateEngineMetaFile(static_cast<ClassID>(std::stoi(tokens[0])), std::stoi(tokens[1]));
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

	int i = 0;
	for (auto& metaFile : metaFiles) {

		if (metaFile->GetClassID() == CID_Object) {
			Object* obj = static_cast<Object*>(metaFile);
			//obj->ImportFile(data[i]); // 最初の行はObjectのデータ
			output.push_back(obj);
		}
		i++;
	}
	i = 0;
	for (auto& metaFile : metaFiles) {
		if (metaFile->GetClassID() != CID_Object && metaFile->GetClassID() != CID_Material) {
			
			Component* component = static_cast<Component*>(metaFile);
			//component->ImportFile(data[i]); // 他のクラスは最初の行を使用
			Editor::GetInstance()->CreateComponent(component);
		}
		i++;
	}
	i = 0;
	for (auto& metaFile : metaFiles) {
		if (metaFile->GetClassID() == CID_Material) {
			Material* material = static_cast<Material*>(metaFile);
			//material->ImportFile(data[i]); // 他のクラスは最初の行を使用
		}
		i++;
	}
	return output;
}

#include <filesystem>

int CSVImporter::ImportMesh(std::string filePath)
{
	//==========================================================================
	// 頂点情報の読み込み
	//==========================================================================

	std::ifstream file(filePath); // ファイル名で開く

	if (!file.is_open()) {
		return -1;
	}

	// 一行分のデータを格納する文字列
	std::string line;
	// 最終データを格納するvector
	std::vector<std::vector<std::string>> vertexData;
	std::vector<std::string> indexData;
	std::string topologyString;

	//==========================================================================
	// 頂点のデータとインデックスのデータを分けて取得
	//==========================================================================

	bool isVertex = true;
	bool isIndex = true;
	while (std::getline(file, line)) {
		// コンマで分けられた1行分のデータ
		std::vector<std::string> tokens;
		// 生の一行分のデータ
		std::stringstream ss(line);
		// コンマで分けられた一つ分のデータ
		std::string token;
		// コンマごとにデータを読み込んでvectorに格納
		while (std::getline(ss, token, ',')) {
			tokens.push_back(token);
		}
		if (!token.empty() && tokens[0] == "&") {
			isVertex = false;
			continue;
		}
		// 最終データのvectorに一行分のデータを格納
		if (isVertex)
			vertexData.push_back(tokens);
		else if (isIndex) {
			indexData = tokens;
			isIndex = false;
		}
		else
			topologyString = tokens[0];
		
	}

	//==========================================================================
	// VertexIndexの作成と登録
	//==========================================================================

	std::vector<VERTEX> vertices;
	std::vector<unsigned int> indices;

	for (auto& tokens : vertexData) {
		VERTEX vertex;
		vertex.position = Vector3O(std::stof(tokens[0]), std::stof(tokens[1]), std::stof(tokens[2]));
		vertex.normal = Vector3O(std::stof(tokens[3]), std::stof(tokens[4]), std::stof(tokens[5]));
		vertex.color = Vector4O(std::stof(tokens[6]), std::stof(tokens[7]), std::stof(tokens[8]), std::stof(tokens[9]));
		vertex.texcoord = Vector2O(std::stof(tokens[10]), std::stof(tokens[11]));
		vertices.push_back(vertex);
	}
	for (auto& tokens : indexData) {
		indices.push_back(std::stoi(tokens));
	}

	std::filesystem::path p = filePath;

	std::string filename = p.stem().string();

	VertexIndex* vi = new VertexIndex(filename, vertices, indices);
	vi->SetPrimitiveTopology(static_cast<D3D11_PRIMITIVE_TOPOLOGY>(std::stoi(topologyString)));
	
	file.close();

	return Editor::GetInstance()->AddVertexIndex(vi);
}

EngineMetaFile* CSVImporter::CreateObject(ClassID classID)
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
		case CID_None:
		default:
			return nullptr;
	}
}

Vector4O CSVImporter::ImportVector4O(const std::string& token1, const std::string& token2, const std::string& token3, const std::string& token4)
{
	return Vector4O(std::stof(token1), std::stof(token2), std::stof(token3), std::stof(token4));
}
