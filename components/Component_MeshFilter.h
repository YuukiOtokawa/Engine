// ========================================================
//
// メッシュフィルターコンポーネント基底クラス[Component_MeshFilter.h]
// 
//									Date:20250520
//									Author:Yuuki Otokawa
// ========================================================

#pragma once

//==========================================================================
// ヘッダーインクルード
//==========================================================================

#include "Component.h"

#include <d3d11.h>

//==========================================================================
// クラス定義
//==========================================================================

class MeshFilter :
    public Component
{
protected:

	// 頂点数
	int m_iVertexCount = 0;
	// インデックス数
	int m_iIndexCount = 0;

	// 頂点バッファ
	ID3D11Buffer* m_pVertexBuffer = nullptr;
	// インデックスバッファ
	ID3D11Buffer* m_pIndexBuffer = nullptr;

	// このメッシュで使用するプリミティブトポロジー
	D3D11_PRIMITIVE_TOPOLOGY m_PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

public:
	/// @brief MeshFilter のデフォルトコンストラクタです。
	MeshFilter() {
        m_ClassID = CID_MeshFilter;
    }
	/// @brief 頂点数とインデックス数を指定して、MeshFilter オブジェクトを作成します。
	/// @param vertexCount メッシュ内の頂点の数。
	/// @param indexCount メッシュ内のインデックスの数。
	MeshFilter(int vertexCount, int indexCount);

	/// @brief オブジェクトの状態を更新します（オーバーライドされたメソッド）。
	void UpdateComponent() override;
	/// @brief 描画処理を実行します。
	void Draw() override;
	void DrawGUI() override;

	/// @brief コンポーネントの所有者オブジェクトを設定します。
	/// @param owner 新しい所有者となるObject型のポインタ。
	void SetOwner(Object* owner) override;

	/// @brief コンポーネントに対応したタグを所有者のオブジェクトに設定します。
	void InitializeTag() override;

	/// @brief 所有者オブジェクトのインデックス数を設定します。
	void SetOwnerIndexCount() { owner->SetIndexCount(m_iIndexCount); }

	/// @brief 頂点バッファと頂点数を設定します。
	/// @param vertexBuffer 設定するID3D11Buffer型の頂点バッファへのポインタ。
	/// @param vertexCount 頂点バッファ内の頂点数。
	void SetVertexBuffer(ID3D11Buffer* vertexBuffer, int vertexCount) {
		m_pVertexBuffer = vertexBuffer;
		m_iVertexCount = vertexCount;
	}

	/// @brief インデックスバッファとインデックス数を設定します。
	/// @param indexBuffer 設定するID3D11Buffer型のインデックスバッファへのポインタ。
	/// @param indexCount インデックスの数。
	void SetIndexBuffer(ID3D11Buffer* indexBuffer, int indexCount) {
		m_pIndexBuffer = indexBuffer;
		m_iIndexCount = indexCount;
		owner->SetIndexCount(indexCount);
	}


};

