#include "CSVImporter.h"

#include "Component_Transform.h"

#include "fstream"

#include "Component_MeshFilter.h"
#include "Material.h"

#include "Editor.h"

// 必要な派生クラスのヘッダーをインクルード
#include "Component_MeshRenderer.h"
#include "Component_Camera.h"
#include "EditorCamera.h"
#include "PlayerCamera.h"
#include "AssimpMeshRenderer.h"
#include "PlaneMesh.h"
#include "Animation.h"
#include "SpriteMesh.h"
#include "Billboard.h"
#include "../Particle.h"
#include "Component_CubeMesh.h"
#include "MonoBehavior.h"
#include "Component_InputSystem.h"
#include "../Component_Light.h"

#include <vector>
#include <sstream>

std::list<Object*> CSVImporter::Import(const char* filePath)
{
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


	// 2行目以降を処理
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
			obj->ImportFile(data[i]); // 最初の行はObjectのデータ
			output.push_back(obj);
		}
		i++;
	}
	i = 0;
	for (auto& metaFile : metaFiles) {
		if (metaFile->GetClassID() != CID_Object && metaFile->GetClassID() != CID_Material) {
			
			Component* component = static_cast<Component*>(metaFile);
			component->ImportFile(data[i]); // 他のクラスは最初の行を使用
			Editor::GetInstance()->CreateComponent(component);
		}
		i++;
	}
	i = 0;
	for (auto& metaFile : metaFiles) {
		if (metaFile->GetClassID() == CID_Material) {
			Material* material = static_cast<Material*>(metaFile);
			material->ImportFile(data[i]); // 他のクラスは最初の行を使用
		}
		i++;
	}
	return output;
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
		case CID_Component_InputSystem:
			return new InputSystem();
		case CID_Component_Light:
			return new Light();
		case CID_Component_Camera:
			return new Camera();
		case CID_Component_EditorCamera:
			return new EditorCamera();
		case CID_PlayerCamera:
			return new PlayerCamera();
		case CID_AssimpMeshRenderer:
			return new AssimpMeshRenderer();
		case CID_PlaneMesh:
			return new PlaneMesh();
		case CID_Animation:
			return new Animation();
		case CID_SpriteMesh:
			return new SpriteMesh();
		case CID_Billboard:
			return new Billboard();
		case CID_Particle:
			return new Particle();
		case CID_CubeMesh:
			return new CubeMesh();
		case CID_MonoBehaviour:
			return nullptr;
		case CID_None:
		default:
			return nullptr;
	}
}

Vector4O CSVImporter::ImportVector4O(const std::string& token1, const std::string& token2, const std::string& token3, const std::string& token4)
{
	return Vector4O(std::stof(token1), std::stof(token2), std::stof(token3), std::stof(token4));
}
