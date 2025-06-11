#include "PlaneMesh.h"

#include "MainEngine.h"

PlaneMesh::PlaneMesh() : MeshFilter(NUM_PLANE_VERTEX, NUM_PLANE_INDEX)
{
	VERTEX vertex[NUM_PLANE_VERTEX];

	vertex[0] = {
		Vector4O(-1.0f, 0.0f, 1.0f), // Position
		Vector4O(0.0f, 1.0f, 0.0f), // Normal
		Vector4O(1.0f, 1.0f, 1.0f, 1.0f), // Color
		Vector4O(0.0f, 0.0f) // UV
	};

	vertex[1] = {
		Vector4O(1.0f, 0.0f, 1.0f), // Position
		Vector4O(0.0f, 1.0f, 0.0f), // Normal
		Vector4O(1.0f, 1.0f, 1.0f, 1.0f), // Color
		Vector4O(1.0f, 0.0f) // UV
	};

	vertex[2] = {
		Vector4O(-1.0f, 0.0f, -1.0f), // Position
		Vector4O(0.0f, 1.0f, 0.0f), // Normal
		Vector4O(1.0f, 1.0f, 1.0f, 1.0f), // Color
		Vector4O(0.0f, 1.0f) // UV
	};

	vertex[3] = {
		Vector4O(1.0f, 0.0f, -1.0f), // Position
		Vector4O(0.0f, 1.0f, 0.0f), // Normal
		Vector4O(1.0f, 1.0f, 1.0f, 1.0f), // Color
		Vector4O(1.0f, 1.0f) // UV
	};

	{
		// 頂点バッファ生成
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(VERTEX) * NUM_PLANE_VERTEX; // 頂点バッファの量
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.pSysMem = vertex;

		MainEngine::GetInstance()->GetRenderer()->GetDevice()->CreateBuffer(&bd, &sd, &m_pVertexBuffer);
	}

	// インデックス設定
	UINT index[NUM_PLANE_INDEX] = {
		0, 1, 2, 3
	};

	{
		// インデックスバッファ
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(unsigned int) * NUM_PLANE_INDEX;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.pSysMem = index;

		MainEngine::GetInstance()->GetRenderer()->GetDevice()->CreateBuffer(&bd, &sd, &m_pIndexBuffer);

	}

	m_PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
}

