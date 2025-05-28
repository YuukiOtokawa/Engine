#pragma once

#define NOMINMAX

#include <unordered_map>

#include "assimp/cimport.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/matrix4x4.h"

#pragma comment(lib, "assimp-vc143-mt.lib")

#include "object.h"
#include <d3d11.h>
class ModelLoader
{
private:
	struct MODEL
	{
		const aiScene* AiScene = nullptr;

		ID3D11Buffer** VertexBuffer;
		ID3D11Buffer** IndexBuffer;

		std::unordered_map<std::string, ID3D11ShaderResourceView*> Texture;
	};

public:
	void LoadModel(Object* object, const char* filename);
};

