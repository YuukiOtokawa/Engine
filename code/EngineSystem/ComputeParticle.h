#pragma once
#include "Renderer.h"
#include "VertexIndex.h"

#include "ComputeBuffer.h"
#include "ComputeShader.h"

class ComputeParticle : public Renderer {
    ID3D11Buffer* m_pVertexBuffer = nullptr;
    ID3D11Buffer* m_pIndexBuffer = nullptr;

    int m_NumIndices = 0;

    void CreateMesh();
    void CreateBuffer();

    VertexIndex m_VertexIndex = {};

    static const int m_NumParticles = 100;
    PARTICLE m_Particles[m_NumParticles];

    std::string m_pComputeShader;
    ComputeBuffer* m_pBuffer;
    char m_comShaderNameBuffer[100];

public:
    DECLARE_COMPONENT(ComputeParticle)

    // コンストラクタ。
    ComputeParticle();
    ~ComputeParticle();

    void Update() override;
    void Render() override; // ここにメインの描画ロジックを記述します
    void DrawGUI() override;
    void ImportFile(YAML::Node& node) override;
    void ExportComponent(YAML::Emitter& out) override;
    void AddExportList() override;
    void InitializeTag() override;

    void CreateParticle(int i);
};
