//==========================================================================
// GM31課題用コメント
// スプライトの頂点情報のUVをAnimationクラスから指令を受けて更新します
//==========================================================================


#include "SpriteMesh.h"

#include "MainEngine.h"

#include "Component_Transform.h"
REGISTER_COMPONENT(SpriteMesh)

SpriteMesh::SpriteMesh() : MeshFilter(SPRITE_VERTICES,0)
{
	std::vector<VERTEX> vertex(SPRITE_VERTICES);

	//上

	vertex[2] = {
		Vector3O(-1.0f, -1.0f, 0.0f),
		Vector3O(0.0f,0.0f,-1.0f),
		Vector4O(1.0f, 1.0f, 1.0f, 1.0f),
		Vector2O(0.0f, 0.0f),
	};

	vertex[3] = {
		Vector3O(1.0f, -1.0f, 0.0f),
		Vector3O(0.0f,0.0f,-1.0f),
		Vector4O(1.0f, 1.0f, 1.0f, 1.0f),
		Vector2O(1.0f, 0.0f),
	};

	vertex[0] = {
		Vector3O(-1.0f, 1.0f, 0.0f),
		Vector3O(0.0f,0.0f,-1.0f),
		Vector4O(1.0f, 1.0f, 1.0f, 1.0f),
		Vector2O(0.0f, 1.0f),
	};

	vertex[1] = {
		Vector3O(1.0f, 1.0f, 0.0f),
		Vector3O(0.0f,0.0f,-1.0f),
		Vector4O(1.0f, 1.0f, 1.0f, 1.0f),
		Vector2O(1.0f, 1.0f),
	};

	m_pVertexIndex = new VertexIndex("Sprite", vertex);
	m_pVertexIndex->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	Editor::GetInstance()->AddVertexIndex(m_pVertexIndex);

	SetVertexInfo(m_pVertexIndex->GetVertexInfo(), m_pVertexIndex->GetIndexInfo());

}

void SpriteMesh::Update() {
	if (m_newIndex != -1) {

		int x = m_newIndex % (int)m_uvRect.x;
		int y = m_newIndex / (int)m_uvRect.x;
		Vector4O uvOffset = Vector4O(1.0f / m_uvRect.x, 1.0f / m_uvRect.y);

		D3D11_MAPPED_SUBRESOURCE msr;
		MainEngine::GetInstance()->GetRenderCore()->GetDeviceContext()->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		VERTEX* vertex = (VERTEX*)msr.pData;

		vertex[0] = {
			Vector3O(-1.0f, 1.0f, 0.0f),
			Vector3O(0.0f,0.0f,-1.0f),
			Vector4O(1.0f, 1.0f, 1.0f, 1.0f),
			Vector2O(uvOffset.x * x, uvOffset.y * y),
		};

		vertex[1] = {
			Vector3O(1.0f, 1.0f, 0.0f),
			Vector3O(0.0f,0.0f,-1.0f),
			Vector4O(1.0f, 1.0f, 1.0f, 1.0f),
			Vector2O(uvOffset.x * (x + 1), uvOffset.y * y),
		};

		vertex[2] = {
			Vector3O(-1.0f, -1.0f, 0.0f),
			Vector3O(0.0f,0.0f,-1.0f),
			Vector4O(1.0f, 1.0f, 1.0f, 1.0f),
			Vector2O(uvOffset.x * x, uvOffset.y * (y + 1)),
		};

		vertex[3] = {
			Vector3O(1.0f, -1.0f, 0.0f),
			Vector3O(0.0f,0.0f,-1.0f),
			Vector4O(1.0f, 1.0f, 1.0f, 1.0f),
			Vector2O(uvOffset.x * (x + 1), uvOffset.y * (y + 1)),
		};

		MainEngine::GetInstance()->GetRenderCore()->GetDeviceContext()->Unmap(m_pVertexBuffer, 0);

		m_newIndex = -1; // リセット
	}
}

void SpriteMesh::DrawGUI()
{
	ImGui::Text("Sprite Mesh");
	ImGui::Separator();
	ImGui::Indent();
	ImGui::InputInt("Sprite Index", &m_spriteIndex);
	ImGui::BeginDisabled();
	ImGui::DragFloat2("UV Rect", &m_uvRect.x, 0.1f, 1.0f, 100.0f);
	ImGui::EndDisabled();
	ImGui::Unindent();
}

void SpriteMesh::ExportComponent(YAML::Emitter& out)
{
	out << YAML::Key << "uvRect" << YAML::Value << YAML::Flow << YAML::BeginSeq
		<< m_uvRect.x << m_uvRect.y << m_uvRect.z << m_uvRect.w << YAML::EndSeq;
}

bool SpriteMesh::SetSpriteByIndex(int index)
{

	if (index >= m_uvRect.x * m_uvRect.y) {
		return false; // 範囲外のインデックス
	}
	else {
		m_newIndex = index;
		return true; // 正常に設定
	}
}

//void SpriteMesh::Draw()
//{
//	auto transform = owner->GetComponent<Transform>();
//
//	// 行列の宣言
//	XMMATRIX scale, angle, translation;
//
//	// オブジェクトのスケール、回転、位置を取得して行列を計算
//	{
//		auto objectScale = transform->GetScale();
//		auto objectRotation = transform->GetRotation().ToRadian();
//		auto objectPosition = transform->GetPosition();
//
//		// 1920.1080で画面いっぱいに表示
//		objectScale.x = objectScale.x * 0.5f;
//		objectScale.y = objectScale.y * 0.5f;
//
//		scale = XMMatrixScaling(objectScale.x, objectScale.y, objectScale.z);
//		angle = XMMatrixRotationRollPitchYaw(objectRotation.x, objectRotation.y, objectRotation.z);
//		translation = XMMatrixTranslation(objectPosition.x, objectPosition.y, objectPosition.z);
//	}
//
//	// ワールド行列を転置してシェーダーに送るための準備
//	MainEngine::GetInstance()->GetRenderCore()->SetTranslationMatrix(translation);
//	MainEngine::GetInstance()->GetRenderCore()->SetScaleMatrix(scale);
//	MainEngine::GetInstance()->GetRenderCore()->SetAngleMatrix(angle);
//
//	// シェーダーに頂点バッファを設定
//	UINT stride = sizeof(VERTEX);
//	UINT offset = 0;
//	MainEngine::GetInstance()->GetRenderCore()->GetDeviceContext()->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
//
//	// シェーダーにインデックスバッファを設定
//	MainEngine::GetInstance()->GetRenderCore()->GetDeviceContext()->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
//
//	// プリミティブトポロジを設定
//	MainEngine::GetInstance()->GetRenderCore()->GetDeviceContext()->IASetPrimitiveTopology(m_PrimitiveTopology);
//
//}
