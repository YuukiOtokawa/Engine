#pragma once
#include "../Renderer.h"

#include "ComponentFactory.h"

class FieldMeshRenderer :
    public Renderer
{
    int m_NumMeshesX = 0;
    int m_NumMeshesZ = 0;

    int m_LastNumMeshesX = 0;
    int m_LastNumMeshesZ = 0;

    ID3D11Buffer** m_pVertexBuffer = nullptr;
    ID3D11Buffer** m_pIndexBuffer = nullptr;

public:
    DECLARE_COMPONENT(FieldMeshRenderer)

    FieldMeshRenderer();
    virtual ~FieldMeshRenderer();
    void UpdateComponent() override;
    void Draw() override;
    void DrawGUI() override;
    void InitializeTag() override;
};

