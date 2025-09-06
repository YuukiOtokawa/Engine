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

REGISTER_COMPONENT(MeshRenderer)

//==========================================================================
// メンバ関数定義
//==========================================================================

MeshRenderer::MeshRenderer()
{
	m_pMaterial = new Material();
	m_ClassID = CID_Component_MeshRenderer;
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
	ImGui::Indent();
	m_pMaterial->DrawGUI();
	ImGui::Unindent();
}

void MeshRenderer::ImportFile(std::vector<std::string>& tokens)
{
	m_pMaterial = Editor::GetInstance()->GetMaterialByFileID(std::stoi(tokens[4]));
}

void MeshRenderer::ExportComponent()
{
	CSVExporter::ExportInt(m_pMaterial->GetFileID());
}

void MeshRenderer::AddExportList()
{
	CSVExporter::AddExportList(this);
	CSVExporter::AddExportList(m_pMaterial);
}

void MeshRenderer::InitializeTag() {
	//owner->SetTag(GameObjectTagLayer::ObjectTag);
}