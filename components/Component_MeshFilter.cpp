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
	m_ClassID = CID_Component_MeshFilter;
}

void MeshFilter::UpdateComponent() {

}

void MeshFilter::Draw() {
	auto transform = owner->GetComponent<Transform>();

	// 行列の宣言
	XMMATRIX scale, angle, translation;

	// オブジェクトのスケール、回転、位置を取得して行列を計算
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

#include "../Inspector.h"
void MeshFilter::DrawGUI() {
	ImGui::Separator();
	ImGui::Text("Mesh Filter");

	ImGui::Text("VertexIndex:");
	ImGui::Indent();

	if (m_pVertexIndex) {
		ImGui::Text("%s", m_pVertexIndex->GetName().c_str());
	}
	else {
		ImGui::Text("No VertexIndex assigned.");
	}

	if (ImGui::Button("Set Mesh", ImVec2(150.0f, 30.0f)))
		GetMesh(this);

	ImGui::Unindent();
}

void MeshFilter::SetOwner(Object* owner) {
	this->owner = owner;
	owner->SetDrawable(true);
	owner->SetVertexCount(m_iVertexCount);
	owner->SetIndexCount(m_iIndexCount);
	//owner->AddComponent<MeshRenderer>();
}

void MeshFilter::InitializeTag() {
	owner->SetTag(GameObjectTagLayer::ObjectTag);
	owner->SetVertexCount(m_iVertexCount);
	owner->SetIndexCount(m_iIndexCount);
}

void MeshFilter::ImportFile(std::vector<std::string>& tokens)
{
	m_pVertexIndex = Editor::GetInstance()->GetVertexIndexByFileID(std::stoi(tokens[4]));

	SetVertexInfo(m_pVertexIndex->GetVertexInfo(), m_pVertexIndex->GetIndexInfo());
}

void MeshFilter::SetMesh(int fileID) {
	m_pVertexIndex = Editor::GetInstance()->GetVertexIndexByFileID(fileID);
	SetVertexInfo(m_pVertexIndex->GetVertexInfo(), m_pVertexIndex->GetIndexInfo());
}

void MeshFilter::ExportComponent()
{
	CSVExporter::ExportInt(m_pVertexIndex->GetFileID());
}

void MeshFilter::AddExportList()
{
	CSVExporter::AddExportList(this);
	CSVExporter::AddVertexIndexExportList(m_pVertexIndex);
}

void MeshFilter::SetVertexInfo(std::vector<VERTEX> vertices, std::vector<unsigned int> indices)
{
	HRESULT hr;
	{
		// 頂点バッファ生成
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(VERTEX) * vertices.size(); // 頂点バッファの量
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.pSysMem = vertices.data();

		hr = MainEngine::GetInstance()->GetRenderer()->GetDevice()->CreateBuffer(&bd, &sd, &m_pVertexBuffer);
	}

	{
		// インデックスバッファ
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(int) * indices.size();
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.pSysMem = indices.data();

		hr = MainEngine::GetInstance()->GetRenderer()->GetDevice()->CreateBuffer(&bd, &sd, &m_pIndexBuffer);
	}

	m_PrimitiveTopology = m_pVertexIndex->GetPrimitiveTopology();
	m_iVertexCount = vertices.size();
	m_iIndexCount = indices.size();

	// 起動時とシーン読み込み時でownerが入るタイミングが違うため、if文が必要
	// できれば解消したい
	if (owner) {
		owner->SetVertexCount(m_iVertexCount);
		owner->SetIndexCount(m_iIndexCount);
	}
}
