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


#pragma comment(lib, "code/assimp/assimp-vc143-mt.lib")
#include <d3d11.h>
#include "RenderCore.h"

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
    std::string boneName[5];
    float boneWeight[5];
};

struct BONE {
    aiMatrix4x4 matrix;
    aiMatrix4x4 animationMatrix;
    aiMatrix4x4 offsetMatrix;
    XMMATRIX finalMatrix;
};

class FBXImporter
{
public:
    FBXImporter() = default;
    ~FBXImporter() = default;

    void LoadFBX(const char* filename);

    std::pair<const aiScene*, std::string> LoadAnimation(std::string filename);

    VertexIndex** GetVertexIndex() const { return m_VertexIndex; }
    std::unordered_map<std::string, BONE>* GetBone() { return m_Bone; }
    std::vector<DEFORM_VERTEX>* GetDeformVertex() {
        return m_DeformVertex;
    }
    std::vector<Material*> GetMaterials() const { return m_Materials; }

    const aiScene* GetAiScene() { return m_Model->AiScene; }

private:
    void LoadVertexIndex(std::string filename);
    void LoadTexture(std::string filename);
    void CreateBone(aiNode* node);
    void CreateMaterial();

    VertexIndex** m_VertexIndex;

    std::unordered_map<std::string, const aiScene*> m_Animation;


    std::unordered_map<std::string, int> m_TextureFileID;
    std::vector<Material*> m_Materials;

    FBXMODEL* m_Model;
    std::vector<DEFORM_VERTEX>* m_DeformVertex;
    std::unordered_map<std::string, BONE>* m_Bone;//ボーンデータ（名前で参照）
};

