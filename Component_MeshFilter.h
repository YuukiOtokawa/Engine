#pragma once
#include "Component.h"

#include <d3d11.h>
class MeshFilter :
    public Component
{
protected:
	ID3D11Buffer* m_pVertexBuffer = nullptr;
	ID3D11Buffer* m_pIndexBuffer = nullptr;

public:
	MeshFilter();
	void Update() override;
	//void Draw() override;
};

