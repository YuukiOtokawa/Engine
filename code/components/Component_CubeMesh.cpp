#include "Component_CubeMesh.h"



#include "MainEngine.h"

CubeMesh::CubeMesh() : MeshFilter(NUM_CUBE_VERTEX, NUM_CUBE_INDEX)
{
	// 頂点情報の作成
	std::vector<VERTEX> vertex(NUM_CUBE_VERTEX);

	//上

	vertex[0] = {
		Vector4O(-1.0f, 1.0f, 1.0f),
		Vector4O(0.0f,1.0f,0.0f),
		Vector4O(1.0f, 1.0f, 1.0f, 1.0f),
		Vector2O(0.0f, 0.0f),
	};

	vertex[1] = {
		Vector4O(1.0f, 1.0f, 1.0f),
		Vector4O(0.0f,1.0f,0.0f),
		Vector4O(1.0f, 1.0f, 1.0f, 1.0f),
		Vector2O(1.0f, 0.0f),
	};

	vertex[2] = {
		Vector4O(-1.0f, 1.0f, -1.0f),
		Vector4O(0.0f,1.0f,0.0f),
		Vector4O(1.0f, 1.0f, 1.0f, 1.0f),
		Vector2O(0.0f, 1.0f),
	};

	vertex[3] = {
		Vector4O(1.0f, 1.0f, -1.0f),
		Vector4O(0.0f,1.0f,0.0f),
		Vector4O(1.0f, 1.0f, 1.0f, 1.0f),
		Vector2O(1.0f, 1.0f),
	};

	//手前

	vertex[4] = {
		Vector4O(-1.0f, 1.0f, -1.0f),
		Vector4O(0.0f,0.0f,-1.0f),
		Vector4O(1.0f, 1.0f, 1.0f, 1.0f),
		Vector2O(0.0f, 0.0f),
	};

	vertex[5] = {
		Vector4O(1.0f, 1.0f, -1.0f),
		Vector4O(0.0f,0.0f,-1.0f),
		Vector4O(1.0f, 1.0f, 1.0f, 1.0f),
		Vector2O(1.0f, 0.0f),
	};


	vertex[6] = {
		Vector4O(-1.0f, -1.0f, -1.0f),
		Vector4O(0.0f,0.0f,-1.0f),
		Vector4O(1.0f, 1.0f, 1.0f, 1.0f),
		Vector2O(0.0f, 1.0f),
	};


	vertex[7] = { 
		Vector4O(1.0f, -1.0f, -1.0f),
		Vector4O(0.0f,0.0f,-1.0f),
		Vector4O(1.0f, 1.0f, 1.0f, 1.0f),
		Vector2O(1.0f, 1.0f),
	};

	//右

	vertex[8] = {
		Vector4O(1.0f, 1.0f, -1.0f),
		Vector4O(1.0f,0.0f,0.0f),
		Vector4O(1.0f, 1.0f, 1.0f, 1.0f),
		Vector2O(0.0f, 0.0f),
	};

	vertex[9] = {
		Vector4O(1.0f, 1.0f, 1.0f),
		Vector4O(1.0f,0.0f,0.0f),
		Vector4O(1.0f, 1.0f, 1.0f, 1.0f),
		Vector2O(1.0f, 0.0f),
	};

	vertex[10] = {
		Vector4O(1.0f, -1.0f, -1.0f),
		Vector4O(1.0f,0.0f,0.0f),
		Vector4O(1.0f, 1.0f, 1.0f, 1.0f),
		Vector2O(0.0f, 1.0f),
	};

	vertex[11] = {
		Vector4O(1.0f, -1.0f, 1.0f),
		Vector4O(1.0f,0.0f,0.0f),
		Vector4O(1.0f, 1.0f, 1.0f, 1.0f),
		Vector2O(1.0f, 1.0f),
	};

	//奥

	vertex[12] = {
		Vector4O(1.0f, 1.0f, 1.0f),
		Vector4O(0.0f,0.0f,1.0f),
		Vector4O(1.0f, 1.0f, 1.0f, 1.0f),
		Vector2O(0.0f, 0.0f),
	};

	vertex[13] = {
		Vector4O(-1.0f, 1.0f, 1.0f),
		Vector4O(0.0f,0.0f,1.0f),
		Vector4O(1.0f, 1.0f, 1.0f, 1.0f),
		Vector2O(1.0f, 0.0f),
	};

	vertex[14] = {
		Vector4O(1.0f, -1.0f, 1.0f),
		Vector4O(0.0f,0.0f,1.0f),
		Vector4O(1.0f, 1.0f, 1.0f, 1.0f),
		Vector2O(0.0f, 1.0f),
	};

	vertex[15] = {
		Vector4O(-1.0f, -1.0f, 1.0f),
		Vector4O(0.0f,0.0f,1.0f),
		Vector4O(1.0f, 1.0f, 1.0f, 1.0f),
		Vector2O(1.0f, 1.0f),
	};

	//左

	vertex[16] = {
		Vector4O(-1.0f, 1.0f, 1.0f),
		Vector4O(-1.0f,0.0f,0.0f),
		Vector4O(1.0f, 1.0f, 1.0f, 1.0f),
		Vector2O(0.0f, 0.0f),
	};

	vertex[17] = {
		Vector4O(-1.0f, 1.0f, -1.0f),
		Vector4O(-1.0f,0.0f,0.0f),
		Vector4O(1.0f, 1.0f, 1.0f, 1.0f),
		Vector2O(1.0f, 0.0f),
	};

	vertex[18] = {
		Vector4O(-1.0f, -1.0f, 1.0f),
		Vector4O(-1.0f,0.0f,0.0f),
		Vector4O(1.0f, 1.0f, 1.0f, 1.0f),
		Vector2O(0.0f, 1.0f),
	};

	vertex[19] = {
		Vector4O(-1.0f, -1.0f, -1.0f),
		Vector4O(-1.0f,0.0f,0.0f),
		Vector4O(1.0f, 1.0f, 1.0f, 1.0f),
		Vector2O(1.0f, 1.0f),
	};

	//下

	vertex[20] = {
		Vector4O(-1.0f, -1.0f, -1.0f),
		Vector4O(0.0f,-1.0f,0.0f),
		Vector4O(1.0f, 1.0f, 1.0f, 1.0f),
		Vector2O(0.0f, 0.0f),
	};

	vertex[21] = {
		Vector4O(1.0f, -1.0f, -1.0f),
		Vector4O(0.0f,-1.0f,0.0f),
		Vector4O(1.0f, 1.0f, 1.0f, 1.0f),
		Vector2O(1.0f, 0.0f),
	};

	vertex[22] = {
		Vector4O(-1.0f, -1.0f, 1.0f),
		Vector4O(0.0f,-1.0f,0.0f),
		Vector4O(1.0f, 1.0f, 1.0f, 1.0f),
		Vector2O(0.0f, 1.0f),
	};

	vertex[23] = {
		Vector4O(1.0f, -1.0f, 1.0f),
		Vector4O(0.0f,-1.0f,0.0f),
		Vector4O(1.0f, 1.0f, 1.0f, 1.0f),
		Vector2O(1.0f, 1.0f),
	};

	//HRESULT hr;
	//{
	//	// 頂点バッファ生成
	//	D3D11_BUFFER_DESC bd;
	//	ZeroMemory(&bd, sizeof(bd));
	//	bd.Usage = D3D11_USAGE_DEFAULT;
	//	bd.ByteWidth = sizeof(VERTEX) * NUM_CUBE_VERTEX; // 頂点バッファの量
	//	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	//	bd.CPUAccessFlags = 0;

	//	D3D11_SUBRESOURCE_DATA sd;
	//	ZeroMemory(&sd, sizeof(sd));
	//	sd.pSysMem = vertex;

	//	hr = MainEngine::GetInstance()->GetRenderer()->GetDevice()->CreateBuffer(&bd, &sd, &m_pVertexBuffer);
	//}

	// インデックス設定
	std::vector<UINT> index = {
		0, 1, 2,
		1, 3, 2,
		4, 5, 6,
		5, 7, 6,
		8, 9, 10,
		9, 11, 10,
		12, 13, 14,
		13, 15, 14,
		16, 17, 18,
		17, 19, 18,
		20, 21, 22,
		21, 23, 22 };

	//{
	//	// インデックスバッファ
	//	D3D11_BUFFER_DESC bd;
	//	ZeroMemory(&bd, sizeof(bd));
	//	bd.Usage = D3D11_USAGE_DEFAULT;
	//	bd.ByteWidth = sizeof(unsigned int) * NUM_CUBE_INDEX;
	//	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	//	bd.CPUAccessFlags = 0;

	//	D3D11_SUBRESOURCE_DATA sd;
	//	ZeroMemory(&sd, sizeof(sd));
	//	sd.pSysMem = index;

	//	MainEngine::GetInstance()->GetRenderer()->GetDevice()->CreateBuffer(&bd, &sd, &m_pIndexBuffer);
	//}

	m_pVertexIndex = new VertexIndex("Cube", vertex, index);
	m_pVertexIndex->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	Editor::GetInstance()->AddVertexIndex(m_pVertexIndex);

	SetVertexInfo(m_pVertexIndex->GetVertexInfo(), m_pVertexIndex->GetIndexInfo());
}
