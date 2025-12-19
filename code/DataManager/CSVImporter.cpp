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

std::list<std::list<std::string>> CSVImporter::Import(std::string filePath)
{
	std::ifstream file(filePath); // ファイル名で開く
	std::list<std::list<std::string>> dataList;
	if (!file.is_open()) {
		return dataList;
	}
	// 一行分のデータを格納する文字列
	std::string line;
	while (std::getline(file, line)) {
		// コンマで分けられた1行分のデータ
		std::list<std::string> tokens;
		// 生の一行分のデータ
		std::stringstream ss(line);
		// コンマで分けられた一つ分のデータ
		std::string token;
		// コンマごとにデータを読み込んでvectorに格納
		while (std::getline(ss, token, ',')) {
			tokens.push_back(token);
		}
		// 最終データのvectorに一行分のデータを格納
		dataList.push_back(tokens);
	}
	file.close();
	return dataList;
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
