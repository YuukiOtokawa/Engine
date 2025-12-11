#include "MeshField.h"

#include "MainEngine.h"

#include "Component_Transform.h"

//==========================================================================
// メッシュフィールドコンポーネント
// 指定したサイズとセルサイズに基づいてメッシュフィールドを生成し、描画します。
// GUI上で頂点数・座標の動的な編集もできます。
// Vertexパラメータで編集する頂点を指定し、Positionパラメータで頂点の座標を編集します。
// MeshSizeパラメータで縦横それぞれの方向の頂点数を指定します。
//==========================================================================

REGISTER_COMPONENT(MeshField)

void MeshField::CreateMesh()
{
	// 頂点データとインデックスデータの生成
	std::vector<VERTEX> vertex((int)(m_MeshSize[0]) * (int)(m_MeshSize[1]));

	{
		int index = 0;

		for (int x = 0; x < (int)m_MeshSize[0]; x++) {
			for (int y = 0; y < (int)m_MeshSize[1]; y++) {
				vertex[index].position = Vector3O(
					(x - m_MeshSize[0] / 2) * m_CellSize.x,
					0.0f,
					(m_MeshSize[1] / 2 - y) * m_CellSize.y
				);
				vertex[index].normal = Vector3O::Up();
				vertex[index].texcoord = Vector2O(
					x,y
				);
				vertex[index].color = Vector4O::One();

				index++;
			}
		}
	}

	std::vector<UINT> index((int)(m_MeshSize[0] - 1) * (int)(m_MeshSize[1] - 1) * 6);
	m_NumIndices = 0;
	for (int x = 0; x < (int)m_MeshSize[0]; x++) {
		int left = 0;
		int right = 0;

		for (int y = 0; y < (int)m_MeshSize[1]; y++) {
			left = y + x * (int)m_MeshSize[1];
			right = y + (x + 1) * (int)m_MeshSize[1];
			if (y == 0&&x !=0) {
				index[m_NumIndices++] = left;
			}
			index[m_NumIndices++] = left;
			index[m_NumIndices++] = right;
		}
		index[m_NumIndices++] = right;
	}

	m_CurrentFocusVertexInfo = vertex[0];

	m_VertexIndex.SetVertexInfo(vertex);
	m_VertexIndex.SetIndexInfo(index);
	m_VertexIndex.SetName("MeshField");
	m_VertexIndex.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

}

void MeshField::CreateBuffer()
{
	// 既存のバッファがあれば解放
	if (m_pVertexBuffer) {
		m_pVertexBuffer->Release();
	}
	if (m_pIndexBuffer) {
		m_pIndexBuffer->Release();
	}

	auto vertices = m_VertexIndex.GetVertexInfo();
	// 頂点バッファ生成
	if (vertices.empty()) return;

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX) * vertices.size();
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.pSysMem = vertices.data();

	HRESULT hr = MainEngine::GetInstance()->GetRenderCore()->GetDevice()->CreateBuffer(&bd, &sd, &m_pVertexBuffer);

	auto indices = m_VertexIndex.GetIndexInfo();
	// インデックスバッファ生成
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(unsigned int) * indices.size();
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	ZeroMemory(&sd, sizeof(sd));
	sd.pSysMem = indices.data();

	hr = MainEngine::GetInstance()->GetRenderCore()->GetDevice()->CreateBuffer(&bd, &sd, &m_pIndexBuffer);
}

MeshField::MeshField() : Renderer(RenderQueue::Geometry)
{
	m_ClassID = CID_Component_MeshField;
	m_pMaterial = new Material();
	m_pMaterial->SetVertexShaderKey("unlit");
	m_pMaterial->SetPixelShaderKey("unlit");
	MATERIAL material;
	m_pMaterial->SetMaterial(material);

	CreateMesh();
	CreateBuffer();
}

MeshField::~MeshField()
{
	if (m_pVertexBuffer) {
		m_pVertexBuffer->Release();
	}
	if (m_pIndexBuffer) {
		m_pIndexBuffer->Release();
	}
	if (m_pMaterial) {
		delete m_pMaterial;
	}
}

void MeshField::Update()
{

}

void MeshField::Render()
{
	if (!m_pMaterial || !m_bIsVisible) return;
	if (!m_pVertexBuffer || !m_pIndexBuffer) return;

	// プリミティブトポロジ設定
	MainEngine::GetInstance()->GetRenderCore()->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// 行列計算
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

	MainEngine::GetInstance()->GetRenderCore()->SetTranslationMatrix(translation);
	MainEngine::GetInstance()->GetRenderCore()->SetScaleMatrix(scale);
	MainEngine::GetInstance()->GetRenderCore()->SetAngleMatrix(angle);

	m_pMaterial->SetShader();
	m_pMaterial->DrawMaterial();

	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	MainEngine::GetInstance()->GetRenderCore()->GetDeviceContext()->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	// インデックスバッファ設定
	MainEngine::GetInstance()->GetRenderCore()->GetDeviceContext()->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// ポリゴン描画
	MainEngine::GetInstance()->GetRenderCore()->GetDeviceContext()->DrawIndexed(m_NumIndices, 0, 0);

}

void MeshField::DrawGUI()
{
	ImGui::Indent();

	ImGui::InputInt2("Vertex", &m_CurrentVertex[0]);
	if (ImGui::IsItemDeactivatedAfterEdit()) {
		if (m_CurrentVertex[0] < 0) m_CurrentVertex[0] = 0;
		if (m_CurrentVertex[0] >= m_MeshSize[0]) m_CurrentVertex[0] = (int)m_MeshSize[0] - 1;
		if (m_CurrentVertex[1] < 0) m_CurrentVertex[1] = 0;
		if (m_CurrentVertex[1] >= m_MeshSize[1]) m_CurrentVertex[1] = (int)m_MeshSize[1] - 1;
		m_CurrentFocusVertexInfo = m_VertexIndex.GetVertexInfo()[m_CurrentVertex[0] * (int)m_MeshSize[1] + m_CurrentVertex[1]];
	}

	ImGui::InputFloat3("Position", (float*)&m_CurrentFocusVertexInfo.position, "%.1f");
	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		m_VertexIndex.SetVertexInfo(&m_CurrentFocusVertexInfo, (int)(m_CurrentVertex[0] * (int)m_MeshSize[1] + m_CurrentVertex[1]));
		CreateBuffer();
	}

	ImGui::InputInt2("MeshSize", &m_MeshSize[0]);
	if (ImGui::IsItemDeactivatedAfterEdit()) {
		CreateMesh();
		CreateBuffer();
	}

	m_pMaterial->DrawGUI();
	ImGui::Unindent();
}

void MeshField::InitializeTag()
{

}

void MeshField::ImportFile(YAML::Node& node)
{
	if (node["tag"]) {
		tag = static_cast<Tag>(node["tag"].as<int>());
	}
	if (node["vertexIndexFileID"]) {
		int fileID = node["vertexIndexFileID"].as<int>();
		// VertexIndexの読み込みが必要な場合、ここで実装
	}
	if (node["meshSizeX"]) {
		m_MeshSize[0] = node["meshSizeX"].as<int>();
	}
	if (node["meshSizeY"]) {
		m_MeshSize[1] = node["meshSizeY"].as<int>();
	}
	if (node["cellSize"]) {
		auto cellSize = node["cellSize"];
		m_CellSize = Vector2O(cellSize[0].as<float>(), cellSize[1].as<float>());
	}
}

float MeshField::GetHeight(int x, int z)
{
	// 指定した位置の頂点の高さを返す
	if (x < 0 || x >= m_MeshSize[0] || z < 0 || z >= m_MeshSize[1]) {
		return 0.0f; // 範囲外の場合は0を返す
	}
	return m_VertexIndex.GetVertexInfo()[x * (int)m_MeshSize[1] + z].position.y;
}

