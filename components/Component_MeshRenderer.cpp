// ========================================================
//
// メッシュレンダラーコンポーネントクラス[Component_MeshRenderer.cpp]
// 
//									Date:20250520
//									Author:Yuuki Otokawa
// ========================================================

//==========================================================================
// ヘッダーインクルード
//==========================================================================

#include "Component_MeshRenderer.h"

#include "Component_Camera.h"
#include "Component_Transform.h"

#include "MainEngine.h"

//==========================================================================
// メンバ関数定義
//==========================================================================

MeshRenderer::MeshRenderer()
{
	m_pMaterial = new Material();

	m_pMaterial->DrawMaterial();

	m_ClassID = CID_MeshRenderer;
}

void MeshRenderer::UpdateComponent() {

}

void MeshRenderer::Draw() {
	// シェーダーを設定
	m_pMaterial->SetShader();
	// マテリアルを設定
	for (int i = 0; i < m_MaterialNum; i++)
		m_pMaterial->DrawMaterial();
}

void MeshRenderer::DrawGUI() {
	ImGui::Separator();
	ImGui::Text("Mesh Renderer");
	ImGui::Indent();
	m_pMaterial->DrawGUI();
	ImGui::Unindent();
}

void MeshRenderer::InitializeTag() {
	//owner->SetTag(GameObjectTagLayer::ObjectTag);
}