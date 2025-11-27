// ========================================================
//
// UIImageMeshコンポーネント[UIImageMesh.cpp]
// UI用の矩形メッシュ（共有リソース）
//
//									Date:20251127
//									Author:Yuuki Otokawa
// ========================================================

#include "UIImageMesh.h"
#include "MainEngine.h"
#include "Editor.h"
#include "GUI.h"

REGISTER_COMPONENT(UIImageMesh)

UIImageMesh::UIImageMesh() : MeshFilter(VERTEX_COUNT, 0)
{
	m_ClassID = CID_UIImageMesh;

	// 矩形の頂点データ
	std::vector<VERTEX> vertices(VERTEX_COUNT);

	// 左上
	vertices[0] = {
		Vector3O(-0.5f, 0.5f, 0.0f),
		Vector3O(0.0f, 0.0f, -1.0f),
		Vector4O(1.0f, 1.0f, 1.0f, 1.0f),
		Vector2O(0.0f, 1.0f),
	};

	// 右上
	vertices[1] = {
		Vector3O(0.5f, 0.5f, 0.0f),
		Vector3O(0.0f, 0.0f, -1.0f),
		Vector4O(1.0f, 1.0f, 1.0f, 1.0f),
		Vector2O(1.0f, 1.0f),
	};

	// 左下
	vertices[2] = {
		Vector3O(-0.5f, -0.5f, 0.0f),
		Vector3O(0.0f, 0.0f, -1.0f),
		Vector4O(1.0f, 1.0f, 1.0f, 1.0f),
		Vector2O(0.0f, 0.0f),
	};

	// 右下
	vertices[3] = {
		Vector3O(0.5f, -0.5f, 0.0f),
		Vector3O(0.0f, 0.0f, -1.0f),
		Vector4O(1.0f, 1.0f, 1.0f, 1.0f),
		Vector2O(1.0f, 0.0f),
	};

	// インデックスデータ
	std::vector<unsigned int> indices = {
		0, 1, 2,  // 最初の三角形
		2, 1, 3   // 二番目の三角形
	};

	// VertexIndexを作成して共有リソースとして登録
	m_pVertexIndex = new VertexIndex("UIImageMesh", vertices, indices);
	m_pVertexIndex->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	Editor::GetInstance()->AddVertexIndex(m_pVertexIndex);

	SetVertexInfo(m_pVertexIndex->GetVertexInfo(), m_pVertexIndex->GetIndexInfo());
}

void UIImageMesh::Update() {
	// 特に更新処理なし
}

void UIImageMesh::DrawGUI() {
	ImGui::Text("UI Image Mesh");
	ImGui::Separator();
}

void UIImageMesh::InitializeTag() {
	owner->SetTag(GameObjectTagLayer::UITag);
	owner->SetLayer(GameObjectTagLayer::UILayer);
}

void UIImageMesh::ExportComponent(YAML::Emitter& out) {
	// 特にエクスポートする情報なし
}
