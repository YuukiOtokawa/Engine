#pragma once
#include "Renderer.h"
#include "VertexIndex.h"
class MeshField :
    public Renderer
{
protected:
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

    void ExportComponent() override {
        CSVExporter::ExportInt(m_VertexIndex.GetFileID());

        CSVExporter::ExportInt(m_MeshSize[0]);
        CSVExporter::ExportInt(m_MeshSize[1]);
        CSVExporter::ExportVector2O(m_CellSize);
    }
    void ImportFile(std::vector<std::string>& tokens) override;

    float GetHeight(int x, int z);
};

