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

#include "scene.h"

//==========================================================================
// クラス定義
//==========================================================================

class AssimpMeshRenderer :
    public MeshRenderer
{
private:

public:
	/// @brief AssimpMeshRenderer のデフォルトコンストラクタです。
	AssimpMeshRenderer() {
        m_ClassID = CID_AssimpMeshRenderer;
    }

};

