// ========================================================
//
// imgui描画クラス[GUI.h]
// 
//									Date:20250520
//									Author:Yuuki Otokawa
// ========================================================

#pragma once

//==========================================================================
// ヘッダーインクルード
//==========================================================================

#include "Vector4O.h"
#include "d3d11.h"

//==========================================================================
// クラス定義
//==========================================================================

class GUI {
private:
	bool checkbox = false; 
	float sliderfloat = 0.0f;
	float Float[4] = {};
	int count = 0;

	/// @brief レンダーターゲットを作成します。
	void CreateRenderTarget();
	/// @brief レンダーターゲットをクリーンアップします。
	void CleanupRenderTarget();

	// imgui用レンダーターゲットビュー
	ID3D11RenderTargetView* m_mainRenderTargetView = nullptr;
public:
	/// @brief 描画開始
	void Start();
	/// @brief 描画終了
	void End();
	/// @brief 描画処理を実行します。
	void Draw();
};

