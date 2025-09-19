#pragma once
#include "Renderer.h"

#include "ComponentFactory.h"
#include "FBXImporter.h"
#include <unordered_map>

class Transform;

class SkinnedMeshRenderer :
    public Renderer
{
private:
    std::vector<DEFORM_VERTEX>* m_pDeformVertex = nullptr;//変形後頂点データ
    std::unordered_map<std::string, BONE>* m_pBoneMap;
    const aiScene* m_pAiScene = nullptr;
    std::unordered_map<std::string, const aiScene*>* m_pAnimation;
    std::unordered_map<std::string, std::string> m_AnimationFilePathMap;
    std::unordered_map<std::string, int>* m_pTextureFID;

    std::string m_MeshFilePath;

    std::string m_CurrentAnimation;
    std::string m_NextAnimation;

    int m_NumMeshes = 0;

    ID3D11Buffer** m_pVertexBuffer = nullptr;
    ID3D11Buffer** m_pIndexBuffer = nullptr;

    std::vector<Transform*> m_pBones;

    int m_CurrentAnimationFrame = 0;
    int m_NextAnimationFrame = 0;

    float m_BlendRate = 0.0f;
    float m_AnimationBlendTime = 0.2f;

    void UpdateBoneMatrix(aiNode* node, aiMatrix4x4 parentMatrix);
    void GetTexture();

public:
    DECLARE_COMPONENT(SkinnedMeshRenderer)
    // コンストラクタ。
    SkinnedMeshRenderer();
    virtual ~SkinnedMeshRenderer();

    void SetMeshData(std::vector<DEFORM_VERTEX>* deformVertex,
                     std::unordered_map<std::string, BONE>* boneMap,
                     const aiScene* aiScene);

    void SetAnimationData(std::unordered_map<std::string, const aiScene*>* aiAnimation);

    void SetBones(const std::vector<Transform*>& bones) { m_pBones = bones; }

    void InitializeBuffers();

    void UpdateComponent() override;
    void Render() override; // ここにメインの描画ロジックを
    void DrawGUI() override;

    void SetNextAnimation(std::string animationKey);

    void AddExportList() override;
    void ExportComponent() override;
    void ImportFile(std::vector<std::string>& tokens) override;

    void InitializeTag() override;
};

