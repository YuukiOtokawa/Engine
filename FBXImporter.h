#pragma once

//==========================================================================
// マクロ定義
//==========================================================================

#define NOMINMAX

//==========================================================================
// ヘッダーインクルード
//==========================================================================

#include <unordered_map>

#include "cimport.h"
#include "scene.h"
#include "postprocess.h"
#include "matrix4x4.h"


#pragma comment(lib, "assimp/assimp-vc143-mt.lib")
#include <d3d11.h>
#include "Renderer.h"

#include "Component_MeshRenderer.h"

struct FBXMODEL {
    const aiScene* AiScene = nullptr;

    ID3D11Buffer** vertexBuffer;
    ID3D11Buffer** indexBuffer;

    std::unordered_map<std::string, Texture*> texture;
};

struct DEFORM_VERTEX {
    aiVector3D position;
    aiVector3D normal;
    int boneNum;
    std::string boneName[4];//本来はボーンインデックスで管理
    float boneWeight[4];
};

struct BONE {
    aiMatrix4x4 matrix;
    aiMatrix4x4 animationMatrix;
    aiMatrix4x4 offsetMatrix;
};

class FBXImporter
{
public:
    FBXImporter() = default;
    ~FBXImporter() = default;

    void LoadFBX(const char* filename, Object* target);

private:
    void LoadVertexIndex(std::string filename);
    void LoadTexture(std::string filename);
    void CreateBone(aiNode* node);
    void LoadAnimation(std::string filename, const char* name);
    void CreateMaterial();

    VertexIndex* m_VertexIndex;

    std::unordered_map<std::string, const aiScene*> m_Animation;

    VertexIndex* m_VertexIndex;

    std::unordered_map<std::string, int> m_TextureFileID;
    std::vector<SUB_MATERIAL*> m_SubMaterial;

    FBXMODEL* m_Model;
    std::vector<DEFORM_VERTEX>* m_DeformVertex;
    std::unordered_map<std::string, BONE> m_Bone;
};

