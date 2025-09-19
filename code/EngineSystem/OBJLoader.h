#pragma once

#include <unordered_map>
#include <string>

#include "FrameWork.h"

#include <d3d11.h>

#include "../VectorO.h"


#include "Material.h"

#include "Object.h"

// マテリアル構造体
struct MODEL_MATERIAL
{
	char						Name[256];
	MATERIAL					Material;
	char						TextureName[256];
	ID3D11ShaderResourceView* Texture;

};


// 描画サブセット構造体
struct SUBSET
{
	unsigned int	StartIndex;
	unsigned int	IndexNum;
	MODEL_MATERIAL	Material;
};


// モデル構造体
struct MODEL_OBJ
{
	VERTEX* VertexArray;
	unsigned int	VertexNum;

	unsigned int* IndexArray;
	unsigned int	IndexNum;

	SUBSET* SubsetArray;
	unsigned int	SubsetNum;
};

#include "../VertexIndex.h"
struct MODEL
{
    VertexIndex* pVertexIndex;

	Material* MaterialArray;
	unsigned int	SubsetNum;
};

class OBJLoader
{
private:

	static std::unordered_map<std::string, MODEL*> m_ModelPool;

	static MODEL_OBJ LoadModel(const char* FileName, MODEL* Model);
	static void LoadObj(const char* FileName, MODEL_OBJ* ModelObj);
	static void LoadMaterial(const char* FileName, MODEL_MATERIAL** MaterialArray, unsigned int* MaterialNum);

	MODEL* m_Model{};

    VertexIndex* m_VertexIndex = new VertexIndex;

public:

	static void Preload(const char* FileName);
	static void UnloadAll();


	void Load(const char* FileName, Object* object);
};

