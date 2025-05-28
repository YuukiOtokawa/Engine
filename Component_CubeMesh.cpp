#include "Component_CubeMesh.h"

#include "MainEngine.h"

CubeMesh::CubeMesh() : MeshFilter(NUM_CUBE_VERTEX, NUM_CUBE_INDEX)
{

	// 頂点情報の作成
	VERTEX vertex[NUM_CUBE_VERTEX];

	//上

	vertex[0] = {
		XMFLOAT3(-1.0f, 1.0f, 1.0f),
		XMFLOAT3(0.0f,1.0f,0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		XMFLOAT2(0.0f, 0.0f),
	};

	vertex[1] = {
		XMFLOAT3(1.0f, 1.0f, 1.0f),
		XMFLOAT3(0.0f,1.0f,0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		XMFLOAT2(1.0f, 0.0f),
	};

	vertex[2] = {
		XMFLOAT3(-1.0f, 1.0f, -1.0f),
		XMFLOAT3(0.0f,1.0f,0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		XMFLOAT2(0.0f, 1.0f),
	};

	vertex[3] = {
		XMFLOAT3(1.0f, 1.0f, -1.0f),
		XMFLOAT3(0.0f,1.0f,0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		XMFLOAT2(1.0f, 1.0f),
	};

	//手前

	vertex[4] = {
		XMFLOAT3(-1.0f, 1.0f, -1.0f),
		XMFLOAT3(0.0f,0.0f,-1.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		XMFLOAT2(0.0f, 0.0f),
	};

	vertex[5] = {
		XMFLOAT3(1.0f, 1.0f, -1.0f),
		XMFLOAT3(0.0f,0.0f,-1.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		XMFLOAT2(1.0f, 0.0f),
	};


	vertex[6] = {
		XMFLOAT3(-1.0f, -1.0f, -1.0f),
		XMFLOAT3(0.0f,0.0f,-1.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		XMFLOAT2(0.0f, 1.0f),
	};


	vertex[7] = { 
		XMFLOAT3(1.0f, -1.0f, -1.0f),
		XMFLOAT3(0.0f,0.0f,-1.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		XMFLOAT2(1.0f, 1.0f),
	};

	//右

	vertex[8] = {
		XMFLOAT3(1.0f, 1.0f, -1.0f),
		XMFLOAT3(1.0f,0.0f,0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		XMFLOAT2(0.0f, 0.0f),
	};

	vertex[9] = {
		XMFLOAT3(1.0f, 1.0f, 1.0f),
		XMFLOAT3(1.0f,0.0f,0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		XMFLOAT2(1.0f, 0.0f),
	};

	vertex[10] = {
		XMFLOAT3(1.0f, -1.0f, -1.0f),
		XMFLOAT3(1.0f,0.0f,0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		XMFLOAT2(0.0f, 1.0f),
	};

	vertex[11] = {
		XMFLOAT3(1.0f, -1.0f, 1.0f),
		XMFLOAT3(1.0f,0.0f,0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		XMFLOAT2(1.0f, 1.0f),
	};

	//奥

	vertex[12] = {
		XMFLOAT3(1.0f, 1.0f, 1.0f),
		XMFLOAT3(0.0f,0.0f,1.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		XMFLOAT2(0.0f, 0.0f),
	};

	vertex[13] = {
		XMFLOAT3(-1.0f, 1.0f, 1.0f),
		XMFLOAT3(0.0f,0.0f,1.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		XMFLOAT2(1.0f, 0.0f),
	};

	vertex[14] = {
		XMFLOAT3(1.0f, -1.0f, 1.0f),
		XMFLOAT3(0.0f,0.0f,1.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		XMFLOAT2(0.0f, 1.0f),
	};

	vertex[15] = {
		XMFLOAT3(-1.0f, -1.0f, 1.0f),
		XMFLOAT3(0.0f,0.0f,1.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		XMFLOAT2(1.0f, 1.0f),
	};

	//左

	vertex[16] = {
		XMFLOAT3(-1.0f, 1.0f, 1.0f),
		XMFLOAT3(-1.0f,0.0f,0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		XMFLOAT2(0.0f, 0.0f),
	};

	vertex[17] = {
		XMFLOAT3(-1.0f, 1.0f, -1.0f),
		XMFLOAT3(-1.0f,0.0f,0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		XMFLOAT2(1.0f, 0.0f),
	};

	vertex[18] = {
		XMFLOAT3(-1.0f, -1.0f, 1.0f),
		XMFLOAT3(-1.0f,0.0f,0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		XMFLOAT2(0.0f, 1.0f),
	};

	vertex[19] = {
		XMFLOAT3(-1.0f, -1.0f, -1.0f),
		XMFLOAT3(-1.0f,0.0f,0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		XMFLOAT2(1.0f, 1.0f),
	};

	//下

	vertex[20] = {
		XMFLOAT3(-1.0f, -1.0f, -1.0f),
		XMFLOAT3(0.0f,-1.0f,0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		XMFLOAT2(0.0f, 0.0f),
	};

	vertex[21] = {
		XMFLOAT3(1.0f, -1.0f, -1.0f),
		XMFLOAT3(0.0f,-1.0f,0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		XMFLOAT2(1.0f, 0.0f),
	};

	vertex[22] = {
		XMFLOAT3(-1.0f, -1.0f, 1.0f),
		XMFLOAT3(0.0f,-1.0f,0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		XMFLOAT2(0.0f, 1.0f),
	};

	vertex[23] = {
		XMFLOAT3(1.0f, -1.0f, 1.0f),
		XMFLOAT3(0.0f,-1.0f,0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		XMFLOAT2(1.0f, 1.0f),
	};

	HRESULT hr;
	{
		// 頂点バッファ生成
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(VERTEX) * NUM_CUBE_VERTEX; // 頂点バッファの量
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.pSysMem = vertex;

		hr = MainEngine::GetInstance()->GetRenderer()->GetDevice()->CreateBuffer(&bd, &sd, &m_pVertexBuffer);
	}

	// インデックス設定
	UINT index[NUM_CUBE_INDEX] = {
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

	{
		// インデックスバッファ
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(unsigned int) * NUM_CUBE_INDEX;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.pSysMem = index;

		MainEngine::GetInstance()->GetRenderer()->GetDevice()->CreateBuffer(&bd, &sd, &m_pIndexBuffer);

	}



}

