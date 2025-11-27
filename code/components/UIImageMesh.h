// ========================================================
//
// UIImageMeshコンポーネント[UIImageMesh.h]
// UI用の矩形メッシュ（共有リソース）
//
//									Date:20251127
//									Author:Yuuki Otokawa
// ========================================================

#pragma once

//==========================================================================
// ヘッダーインクルード
//==========================================================================

#include "Component_MeshFilter.h"

//==========================================================================
// クラス定義
//==========================================================================

class UIImageMesh : public MeshFilter
{
private:
	// 頂点数（4頂点の矩形）
	static constexpr int VERTEX_COUNT = 4;

public:
	DECLARE_COMPONENT(UIImageMesh)

	UIImageMesh();

	/// @brief オブジェクトの状態を更新します。
	void Update() override;

	/// @brief Inspector用のGUI描画
	void DrawGUI() override;

	/// @brief コンポーネントに対応したタグを所有者のオブジェクトに設定します。
	void InitializeTag() override;

	void ExportComponent(YAML::Emitter& out) override;
};
