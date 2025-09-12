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


void MeshRenderer::UpdateComponent() {

}

#include "Component_MeshFilter.h"
void MeshRenderer::Draw() {
	if (!m_bIsVisible) return;

	// 1. 所有者オブジェクトからMeshFilterコンポーネントを取得
	auto meshFilter = owner->GetComponent<MeshFilter>();
	if (!meshFilter || !meshFilter->m_pVertexBuffer) {
		return;
	}

	// 2. Transformコンポーネントを取得
	auto transform = owner->GetComponent<Transform>();
	if (!transform) return;
	XMMATRIX scale, angle, translation;
	{
		if (owner->IsChild()) {
			/// 子オブジェクトのtransformは親オブジェクトからの相対位置であるため、親オブジェクトのTransformを考慮
			auto parentTransform = owner->GetParent()->GetComponent<Transform>();

			auto parentScale = parentTransform->GetScale();
			auto parentRotation = parentTransform->GetRotation().ToRadian();
			auto parentPosition = parentTransform->GetPosition();

			auto objectScale = transform->GetScale();
			auto objectRotation = transform->GetRotation().ToRadian();
			auto objectPosition = transform->GetPosition();

			scale = XMMatrixScaling(objectScale.x * parentScale.x, objectScale.y * parentScale.y, objectScale.z * parentScale.z);
			angle = XMMatrixRotationRollPitchYaw(objectRotation.x + parentRotation.x, objectRotation.y + parentRotation.y, objectRotation.z + parentRotation.z);
			translation = XMMatrixTranslation(
				objectPosition.x + parentPosition.x,
				objectPosition.y + parentPosition.y,
				objectPosition.z + parentPosition.z
			);
		}
		else {
			auto objectScale = transform->GetScale();
			auto objectRotation = transform->GetRotation().ToRadian();
			auto objectPosition = transform->GetPosition();

			scale = XMMatrixScaling(objectScale.x, objectScale.y, objectScale.z);
			angle = XMMatrixRotationRollPitchYaw(objectRotation.x, objectRotation.y, objectRotation.z);
			translation = XMMatrixTranslation(objectPosition.x, objectPosition.y, objectPosition.z);
		}

	}

    // 行列をレンダラーに設定する
    MainEngine::GetInstance()->GetRenderCore()->SetTranslationMatrix(translation);
    MainEngine::GetInstance()->GetRenderCore()->SetScaleMatrix(scale);
    MainEngine::GetInstance()->GetRenderCore()->SetAngleMatrix(angle);

    // 3. 頂点・インデックスバッファを設定する (MeshFilter::Draw()から移動)
    UINT stride = sizeof(VERTEX);
    UINT offset = 0;
    MainEngine::GetInstance()->GetRenderCore()->GetDeviceContext()->IASetVertexBuffers(0, 1, &meshFilter->m_pVertexBuffer, &stride, &offset);
	if (meshFilter->m_pIndexBuffer)
		MainEngine::GetInstance()->GetRenderCore()->GetDeviceContext()->IASetIndexBuffer(meshFilter->m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // 4. プリミティブトポロジーを設定する (MeshFilter::Draw()から移動)
    MainEngine::GetInstance()->GetRenderCore()->GetDeviceContext()->IASetPrimitiveTopology(meshFilter->m_PrimitiveTopology);

    // 5. シェーダーとマテリアルのプロパティを設定する (すでにあなたのコードに記述されています)
    m_pMaterial->SetShader();
    m_pMaterial->DrawMaterial();

    // 6. 最終的な描画呼び出しを実行する
	if (meshFilter->m_iVertexCount == 0) return;
	if (meshFilter->m_iIndexCount == 0)
		MainEngine::GetInstance()->GetRenderCore()->GetDeviceContext()->Draw(meshFilter->m_iVertexCount, 0);
	else
	    MainEngine::GetInstance()->GetRenderCore()->GetDeviceContext()->DrawIndexed(meshFilter->m_iIndexCount, 0, 0);
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