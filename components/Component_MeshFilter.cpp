// ========================================================
//
// メッシュフィルターコンポーネントクラス[Componenet_MeshFilter.cpp]
// 
//									Date:20250520
//									Author:Yuuki Otokawa
// ========================================================

//==========================================================================
// ヘッダーインクルード
//==========================================================================

#include "Component_MeshFilter.h"

#include "Component_MeshRenderer.h"

#include "Component_Transform.h"

#include "MainEngine.h"

//==========================================================================
// メンバ関数定義
//==========================================================================

MeshFilter::MeshFilter(int vertexCount, int indexCount) : m_iVertexCount(vertexCount), m_iIndexCount(indexCount) {
}

void MeshFilter::UpdateComponent() {

}

void MeshFilter::Draw() {
	auto transform = owner->GetComponent<Transform>();

	// 行列の宣言
	XMMATRIX scale, angle, translation;

	// オブジェクトのスケール、回転、位置を取得して行列を計算
	{
		auto objectScale = transform->GetScale();
		auto objectRotation = transform->GetRotation().ToRadian();
		auto objectPosition = transform->GetPosition();

		scale = XMMatrixScaling(objectScale.x, objectScale.y, objectScale.z);
		angle = XMMatrixRotationRollPitchYaw(objectRotation.x, objectRotation.y, objectRotation.z);
		translation = XMMatrixTranslation(objectPosition.x, objectPosition.y, objectPosition.z);
	}

	// ワールド行列を転置してシェーダーに送るための準備
	MainEngine::GetInstance()->GetRenderer()->SetTranslationMatrix(translation);
	MainEngine::GetInstance()->GetRenderer()->SetScaleMatrix(scale);
	MainEngine::GetInstance()->GetRenderer()->SetAngleMatrix(angle);

	// シェーダーに頂点バッファを設定
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	MainEngine::GetInstance()->GetRenderer()->GetDeviceContext()->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	// シェーダーにインデックスバッファを設定
	MainEngine::GetInstance()->GetRenderer()->GetDeviceContext()->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// プリミティブトポロジを設定
	MainEngine::GetInstance()->GetRenderer()->GetDeviceContext()->IASetPrimitiveTopology(m_PrimitiveTopology);

}

void MeshFilter::DrawGUI() {
	ImGui::Separator();
	ImGui::Text("Mesh Filter");

}

void MeshFilter::SetOwner(Object* owner) {
	this->owner = owner;
	owner->SetDrawable(true);
	//owner->AddComponent<MeshRenderer>();
}

void MeshFilter::InitializeTag() {
	owner->SetTag(GameObjectTagLayer::ObjectTag);
	owner->SetVertexCount(m_iVertexCount);
	owner->SetIndexCount(m_iIndexCount);
}