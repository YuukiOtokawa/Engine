#pragma once
#include "Component.h"

#include <d3d11.h>
class MeshFilter :
    public Component
{
protected:
	int m_iVertexCount = 0;
	int m_iIndexCount = 0;
	ID3D11Buffer* m_pVertexBuffer = nullptr;
	ID3D11Buffer* m_pIndexBuffer = nullptr;

public:
	MeshFilter() = default;
	MeshFilter(int vertexCount, int indexCount);
	void Update() override;
	void Draw() override;

	void SetOwner(Object* owner) override;

	void InitializeTag() override;

	void SetOwnerIndexCount() { owner->SetIndexCount(m_iIndexCount); }

	void SetVertexBuffer(ID3D11Buffer* vertexBuffer, int vertexCount) {
		m_pVertexBuffer = vertexBuffer;
		m_iVertexCount = vertexCount;
	}

	void SetIndexBuffer(ID3D11Buffer* indexBuffer, int indexCount) {
		m_pIndexBuffer = indexBuffer;
		m_iIndexCount = indexCount;
	}
};

