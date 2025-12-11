// ========================================================
//
// Canvasコンポーネント[Canvas.h]
// UIの描画を管理する
//
//									Date:20251127
//									Author:Yuuki Otokawa
// ========================================================

#pragma once

//==========================================================================
// ヘッダーインクルード
//==========================================================================

#include "Component.h"
#include "VectorO.h"
#include "IRenderable.h"

//==========================================================================
// 列挙型定義
//==========================================================================

enum class RenderMode {
	ScreenSpaceOverlay,		// スクリーン空間（常に最前面）
	ScreenSpaceCamera,		// スクリーン空間（カメラベース）
	WorldSpace				// ワールド空間
};

//==========================================================================
// クラス定義
//==========================================================================

class IRenderable;
class Canvas : public Component, public IRenderable
{
private:
	// レンダーモード
	RenderMode m_RenderMode = RenderMode::ScreenSpaceOverlay;

	// ソート順序（数値が大きいほど後ろに描画される）
	int m_SortOrder = 0;

	// スクリーンサイズ（参照用）
	Vector2O m_ScreenSize = Vector2O(1920.0f, 1080.0f);

public:
	DECLARE_COMPONENT(Canvas)

    Canvas();

	/// @brief オブジェクトの状態を更新します。
	void Update() override;

	/// @brief 描画処理を実行します。
	void Draw() override;

	/// @brief Inspector用のGUI描画
	void DrawGUI() override;

	/// @brief コンポーネントに対応したタグを所有者のオブジェクトに設定します。
	void InitializeTag() override;

	void ExportComponent(YAML::Emitter& out) override;
	void ImportFile(YAML::Node& node) override;

	// セッター
	void SetRenderMode(RenderMode mode) { m_RenderMode = mode; }
	void SetSortOrder(int order) { m_SortOrder = order; }
	void SetScreenSize(Vector2O size) { m_ScreenSize = size; }

	// ゲッター
	RenderMode GetRenderMode() const { return m_RenderMode; }
	int GetSortOrder() const { return m_SortOrder; }
	Vector2O GetScreenSize() const { return m_ScreenSize; }

	/// @brief Canvasの2D射影行列を設定する
	void SetupCanvasProjection();
};
