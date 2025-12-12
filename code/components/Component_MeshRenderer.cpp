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

REGISTER_COMPONENT(MeshRenderer)

MeshRenderer::MeshRenderer() : Renderer(RenderQueue::Geometry) {
	m_ClassID = CID_Component_MeshRenderer;
	// 継承関係のため、ここでマテリアルを直接newする。
	// もしくは、基底クラスのコンストラクタで初期化することも可能。
	m_pMaterial = new Material();
}

void MeshRenderer::Update() {

}

#include "Component_MeshFilter.h"
void MeshRenderer::Render() {
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
		if (owner->IsChild() && owner->GetParent()->GetComponent<Transform>()) {
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

	transform->SetTransformMatrix(translation);

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
	auto ownerScale = transform->GetScale();
	m_pMaterial->GetMaterial()->Aspect = Vector2O(ownerScale.x,ownerScale.y);
	m_pMaterial->SetShader();
	m_pMaterial->DrawMaterial();

    // 6. 最終的な描画呼び出しを実行する
	if (meshFilter->m_iVertexCount == 0) return;
	if (meshFilter->m_iIndexCount == 0)
		MainEngine::GetInstance()->GetRenderCore()->GetDeviceContext()->Draw(meshFilter->m_iVertexCount, 0);
	else
	    MainEngine::GetInstance()->GetRenderCore()->GetDeviceContext()->DrawIndexed(meshFilter->m_iIndexCount, 0, 0);
}

void MeshRenderer::RenderShadow()
{
}

void MeshRenderer::DrawGUI() {
	ImGui::Indent();
	m_pMaterial->DrawGUI();
	ImGui::Unindent();
}

void MeshRenderer::ImportFile(YAML::Node& node)
{
	if (node["tag"]) {
		tag = static_cast<Tag>(node["tag"].as<int>());
	}
	if (node["materialFileID"]) {
		m_pMaterial = Editor::GetInstance()->GetMaterialByFileID(node["materialFileID"].as<int>());
	}
}

void MeshRenderer::ExportComponent(YAML::Emitter& out)
{
	out << YAML::Key << "materialFileID" << YAML::Value << m_pMaterial->GetFileID();
}

void MeshRenderer::AddExportList()
{
	SceneExporter::AddExportList(this);
	SceneExporter::AddExportList(m_pMaterial);
}

void MeshRenderer::InitializeTag() {
	//owner->SetTag(GameObjectTagLayer::ObjectTag);
}