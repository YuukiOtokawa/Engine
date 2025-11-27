// ========================================================
//
// UIImageコンポーネント[UIImage.h]
// UI用のスプライト描画コンポーネント（Rendererベース）
//
//									Date:20251127
//									Author:Yuuki Otokawa
// ========================================================

#pragma once

//==========================================================================
// ヘッダーインクルード
//==========================================================================

#include "Renderer.h"
#include "VectorO.h"

//==========================================================================
// クラス定義
//==========================================================================

class UIImage : public Renderer
{
private:
	// 色（乗算）
	Vector4O m_Color = Vector4O(1.0f, 1.0f, 1.0f, 1.0f);

	// UV矩形（テクスチャの一部を表示する場合）
	Vector4O m_UVRect = Vector4O(0.0f, 0.0f, 1.0f, 1.0f); // (u_min, v_min, u_max, v_max)

public:
	DECLARE_COMPONENT(UIImage)

	UIImage() {
		m_ClassID = CID_UIImage;
		m_pMaterial = new Material();
	}

	~UIImage() override {
		if (m_pMaterial) delete m_pMaterial;
	}

	/// @brief オブジェクトの状態を更新します。
	void Update() override;

	/// @brief 描画処理を実行します（Rendererから呼ばれる）
	void Render() override;

	/// @brief Inspector用のGUI描画
	void DrawGUI() override;

	/// @brief コンポーネントに対応したタグを所有者のオブジェクトに設定します。
	void InitializeTag() override;

    void AddExportList() override;
	void ExportComponent(YAML::Emitter& out) override;
	void ImportFile(YAML::Node& node) override;

	// セッター
	void SetTexture(int fileID);
	void SetColor(Vector4O color) { m_Color = color; }
	void SetUVRect(Vector4O uvRect) { m_UVRect = uvRect; }

	// ゲッター
	int GetTexture() const;
	Vector4O GetColor() const { return m_Color; }
	Vector4O GetUVRect() const { return m_UVRect; }
};
