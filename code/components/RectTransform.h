// ========================================================
//
// RectTransformコンポーネント[RectTransform.h]
// UIオブジェクト用のトランスフォーム
//
//									Date:20251127
//									Author:Yuuki Otokawa
// ========================================================

#pragma once

//==========================================================================
// ヘッダーインクルード
//==========================================================================

#include "Component_Transform.h"
#include "VectorO.h"

//==========================================================================
// クラス定義
//==========================================================================

class RectTransform : public Transform
{
private:
	// アンカー（0.0-1.0の範囲で画面上の位置を指定）
	Vector2O m_AnchorMin = Vector2O(0.5f, 0.5f);	// 左下のアンカー
	Vector2O m_AnchorMax = Vector2O(0.5f, 0.5f);	// 右上のアンカー

	// ピボット（0.0-1.0の範囲で自分自身の中心点を指定）
	Vector2O m_Pivot = Vector2O(0.5f, 0.5f);

	// サイズ（ピクセル単位）
	Vector2O m_Size = Vector2O(100.0f, 100.0f);

	// アンカーからのオフセット（ピクセル単位）
	Vector2O m_AnchoredPosition = Vector2O(0.0f, 0.0f);

public:
	DECLARE_COMPONENT(RectTransform)

	RectTransform() {
		m_ClassID = CID_RectTransform;
	}

	/// @brief オブジェクトの状態を更新します。
	void Update() override;

	/// @brief Inspector用のGUI描画
	void DrawGUI() override;

	/// @brief コンポーネントに対応したタグを所有者のオブジェクトに設定します。
	void InitializeTag() override;

	void ExportComponent(YAML::Emitter& out) override;
	void ImportFile(YAML::Node& node) override;

	// アンカー設定
	void SetAnchorMin(Vector2O anchor) { m_AnchorMin = anchor; }
	void SetAnchorMax(Vector2O anchor) { m_AnchorMax = anchor; }
	void SetAnchor(Vector2O min, Vector2O max) {
		m_AnchorMin = min;
		m_AnchorMax = max;
	}

	// ピボット設定
	void SetPivot(Vector2O pivot) { m_Pivot = pivot; }

	// サイズ設定
	void SetSize(Vector2O size) { m_Size = size; }

	// アンカー位置設定
	void SetAnchoredPosition(Vector2O pos) { m_AnchoredPosition = pos; }

	// ゲッター
	Vector2O GetAnchorMin() const { return m_AnchorMin; }
	Vector2O GetAnchorMax() const { return m_AnchorMax; }
	Vector2O GetPivot() const { return m_Pivot; }
	Vector2O GetSize() const { return m_Size; }
	Vector2O GetAnchoredPosition() const { return m_AnchoredPosition; }

	/// @brief スクリーン座標を計算します
	/// @param screenWidth 画面幅
	/// @param screenHeight 画面高さ
	/// @return スクリーン上での実際の位置（ピクセル単位）
	Vector2O CalculateScreenPosition(float screenWidth, float screenHeight);
};
