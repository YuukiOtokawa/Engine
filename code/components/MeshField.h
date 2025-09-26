#pragma once
#include "Renderer.h"
#include "VertexIndex.h"
class MeshField :
    public Renderer
{
    int m_MeshSize[2] = {10,10};
    Vector2O m_CellSize = { 1.0f,1.0f };

    VertexIndex m_VertexIndex = {};

    int m_CurrentVertex[2] = { 0,0 };
    VERTEX m_CurrentFocusVertexInfo = {};

    ID3D11Buffer* m_pVertexBuffer = nullptr;
    ID3D11Buffer* m_pIndexBuffer = nullptr;

    int m_NumIndices = 0;

    void CreateMesh();
    void CreateBuffer();

    friend class MeshFieldCollider;
public:
    DECLARE_COMPONENT(MeshField)

    // コンストラクタ。
    MeshField();
    virtual ~MeshField();
    void Update() override;
    void Render() override; // ここにメインの描画ロジックを
    void DrawGUI() override;
    void InitializeTag() override;

    float GetHeight(int x, int z);
};

