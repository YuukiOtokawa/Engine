// ========================================================
//
// Assimp読み込みモデル用メッシュレンダラークラス[AssimpMeshRenderer.h]
// 
//									Date:20250520
//									Author:Yuuki Otokawa
// ========================================================

#pragma once

//==========================================================================
// ヘッダーインクルード
//==========================================================================

#include "Component_MeshRenderer.h"

#include <unordered_map>

//==========================================================================
// クラス定義
//==========================================================================

class AssimpMeshRenderer :
    public MeshRenderer
{
private:
	// Assimpで読み込んだテクスチャを格納するマップ
    std::unordered_map<std::string, ID3D11ShaderResourceView*> m_AssimpTexture;

public:
	/// @brief AssimpMeshRenderer のデフォルトコンストラクタです。
	AssimpMeshRenderer() = default;

    /// @brief メッシュのテクスチャ情報を設定します。
    /// @param assimpTexture テクスチャ名とID3D11ShaderResourceView* のペアを格納した連想配列。
    void SetMesh(std::unordered_map<std::string, ID3D11ShaderResourceView*> assimpTexture) {
		m_AssimpTexture = assimpTexture;
    }


};

