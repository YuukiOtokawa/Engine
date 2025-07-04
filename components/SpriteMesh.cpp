//==========================================================================
// GM31課題用コメント
// スプライトの頂点情報のUVをAnimationクラスから指令を受けて更新します
//==========================================================================


#include "SpriteMesh.h"

#include "MainEngine.h"

#include "Component_Transform.h"

SpriteMesh::SpriteMesh() : MeshFilter(SPRITE_VERTICES,0)
{
	m_ClassID = CID_SpriteMesh;
	VERTEX vertex[SPRITE_VERTICES];

	//上

	vertex[0] = {
		Vector4O(-1.0f, -1.0f, 0.0f),
		Vector4O(0.0f,0.0f,-1.0f),
		Vector4O(1.0f, 1.0f, 1.0f, 1.0f),
		Vector4O(0.0f, 0.0f),
	};

	vertex[1] = {
		Vector4O(1.0f, -1.0f, 0.0f),
		Vector4O(0.0f,0.0f,-1.0f),
		Vector4O(1.0f, 1.0f, 1.0f, 1.0f),
		Vector4O(1.0f, 0.0f),
	};

	vertex[2] = {
		Vector4O(-1.0f, 1.0f, 0.0f),
		Vector4O(0.0f,0.0f,-1.0f),
		Vector4O(1.0f, 1.0f, 1.0f, 1.0f),
		Vector4O(0.0f, 1.0f),
	};

	vertex[3] = {
		Vector4O(1.0f, 1.0f, 0.0f),
		Vector4O(0.0f,0.0f,-1.0f),
		Vector4O(1.0f, 1.0f, 1.0f, 1.0f),
		Vector4O(1.0f, 1.0f),
	};

	HRESULT hr;
	{
		// 頂点バッファ生成
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(VERTEX) * SPRITE_VERTICES; // 頂点バッファの量
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.pSysMem = vertex;

		hr = MainEngine::GetInstance()->GetRenderer()->GetDevice()->CreateBuffer(&bd, &sd, &m_pVertexBuffer);
	}
	m_PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

}

void SpriteMesh::UpdateComponent() {
	
}

bool SpriteMesh::SetSpriteByIndex(int index)
{
	int x = index % (int)m_uvRect.x;
	int y = index / (int)m_uvRect.x;
	Vector4O uvOffset = Vector4O(1.0f / m_uvRect.x, 1.0f / m_uvRect.y);

	D3D11_MAPPED_SUBRESOURCE msr;
	MainEngine::GetInstance()->GetRenderer()->GetDeviceContext()->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX* vertex = (VERTEX*)msr.pData;

	vertex[0] = {
		Vector4O(-1.0f, 1.0f, 0.0f),
		Vector4O(0.0f,0.0f,-1.0f),
		Vector4O(1.0f, 1.0f, 1.0f, 1.0f),
		Vector4O(uvOffset.x * x, uvOffset.y * y),
	};

	vertex[1] = {
		Vector4O(1.0f, 1.0f, 0.0f),
		Vector4O(0.0f,0.0f,-1.0f),
		Vector4O(1.0f, 1.0f, 1.0f, 1.0f),
		Vector4O(uvOffset.x * (x + 1), uvOffset.y * y),
	};

	vertex[2] = {
		Vector4O(-1.0f, -1.0f, 0.0f),
		Vector4O(0.0f,0.0f,-1.0f),
		Vector4O(1.0f, 1.0f, 1.0f, 1.0f),
		Vector4O(uvOffset.x * x, uvOffset.y * (y + 1)),
	};

	vertex[3] = {
		Vector4O(1.0f, -1.0f, 0.0f),
		Vector4O(0.0f,0.0f,-1.0f),
		Vector4O(1.0f, 1.0f, 1.0f, 1.0f),
		Vector4O(uvOffset.x * (x + 1), uvOffset.y * (y + 1)),
	};

	MainEngine::GetInstance()->GetRenderer()->GetDeviceContext()->Unmap(m_pVertexBuffer, 0);

	if (index >= m_uvRect.x * m_uvRect.y) {
		return false; // 範囲外のインデックス
	}
	else {
		return true; // 正常に設定
	}
}

void SpriteMesh::Draw()
{
	auto transform = owner->GetComponent<Transform>();

	// 行列の宣言
	XMMATRIX scale, angle, translation;

	// オブジェクトのスケール、回転、位置を取得して行列を計算
	{
		auto objectScale = transform->GetScale();
		auto objectRotation = transform->GetRotation().ToRadian();
		auto objectPosition = transform->GetPosition();

		// 1920.1080で画面いっぱいに表示
		objectScale.x = objectScale.x * 0.5f;
		objectScale.y = objectScale.y * 0.5f;

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
