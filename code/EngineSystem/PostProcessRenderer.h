#pragma once
#include "Component_MeshRenderer.h"
class PostProcessRenderer :
    public MeshRenderer
{
    Material* m_pPostProcessMaterial[3] = {};

    Vector4O m_Parameter = { 1920.0f,1080.0f,0.0f,0.0f }; // ポストエフェクトのパラメーター

    float m_Weight[8]; // ガウス分布の重み
    void CalcGaussianWeight(float);

public:
    DECLARE_COMPONENT(PostProcessRenderer)
    PostProcessRenderer();

    void Update() override;

    void DrawPostProcess(int n);

    void DrawGUI() override;

    Material* GetPostProcessMaterial(int n) {
        return m_pPostProcessMaterial[n];
    }

};

