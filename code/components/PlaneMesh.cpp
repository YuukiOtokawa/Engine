// ========================================================
//
// プレーン平面用メッシュフィルタークラス[PlaneMesh.cpp]
// 
//									Date:20250528
//									Author:Yuuki Otokawa
// ========================================================

#include "PlaneMesh.h"

#include "MainEngine.h"

PlaneMesh::PlaneMesh() : MeshFilter(NUM_PLANE_VERTEX, NUM_PLANE_INDEX)
{
	std::vector<VERTEX> vertex(NUM_PLANE_VERTEX);

	vertex[0] = {
		Vector3O(-1.0f, 0.0f, 1.0f), // Position
		Vector3O(0.0f, 1.0f, 0.0f), // Normal
		Vector4O(1.0f, 1.0f, 1.0f, 1.0f), // Color
		Vector2O(0.0f, 0.0f) // UV
	};

	vertex[1] = {
		Vector3O(1.0f, 0.0f, 1.0f), // Position
		Vector3O(0.0f, 1.0f, 0.0f), // Normal
		Vector4O(1.0f, 1.0f, 1.0f, 1.0f), // Color
		Vector2O(1.0f, 0.0f) // UV
	};

	vertex[2] = {
		Vector3O(-1.0f, 0.0f, -1.0f), // Position
		Vector3O(0.0f, 1.0f, 0.0f), // Normal
		Vector4O(1.0f, 1.0f, 1.0f, 1.0f), // Color
		Vector2O(0.0f, 1.0f) // UV
	};

	vertex[3] = {
		Vector3O(1.0f, 0.0f, -1.0f), // Position
		Vector3O(0.0f, 1.0f, 0.0f), // Normal
		Vector4O(1.0f, 1.0f, 1.0f, 1.0f), // Color
		Vector2O(1.0f, 1.0f) // UV
	};

	std::vector<UINT> index = {
		0, 1, 2, 3
	};

	m_pVertexIndex = new VertexIndex("Plane", vertex, index);
	m_pVertexIndex->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	Editor::GetInstance()->AddVertexIndex(m_pVertexIndex);

	SetVertexInfo(m_pVertexIndex->GetVertexInfo(), m_pVertexIndex->GetIndexInfo());
}

