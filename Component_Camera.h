// ========================================================
//
// カメラコンポーネント[Component_Camera.h]
// 
//									Date:20250520
//									Author:Yuuki Otokawa
// ========================================================

#pragma once

//==========================================================================
// ヘッダーインクルード
//==========================================================================

#include "Component.h"
#include "Vector4O.h"
#include "FrameWork.h"

//==========================================================================
// クラス定義
//==========================================================================

class Camera : public Component
{
private:
	// イージング用 前フレームの位置を保存
    Vector4O m_LastPosition = Vector4O::Zero();

	// カメラの注視点
    Vector4O m_Target;
	// カメラの角度（ピッチ、ヨー、ロール）
    Vector4O m_Angle;

	// カメラの視野角（FOV）
    float m_Fov = 90.0f;

	// 投影行列
    XMMATRIX m_Projection;
	// ビュー行列
	XMMATRIX m_View;

	// 近接クリッピング距離
	float m_Near = 0.1f;
	// 遠方クリッピング距離
    float m_Far = 1000.0f;

	// カメラの上方向ベクトル
	Vector4O m_Up = Vector4O::Up();

public:
	// コンストラクタ
    Camera();

	/// @brief 投影行列を取得します。
	/// @return 現在の投影行列（XMMATRIX型）。
	XMMATRIX GetProjection() const { return m_Projection; }

	/// @brief ビュー行列を取得します。
	/// @return 現在のビュー行列（XMMATRIX型）。
	XMMATRIX GetView() const { return m_View; }

	/// @brief 角度を表すベクトルを取得します。
	/// @return 角度を表す Vector4O 型の値。
	Vector4O GetAngle() const { return m_Angle; }

    /// @brief オブジェクトの状態を更新します（オーバーライドされたメソッド）。
    void Update() override;
	/// @brief 描画処理を実行します。
	void Draw() override;
	/// @brief コンポーネントに対応したタグを所有者のオブジェクトに設定します（オーバーライドされたメソッド）。
    void InitializeTag() override;

	/// @brief 注視点を設定します。
	/// @param target 設定するVector4O型の注視点。
	void SetTarget(Vector4O target) { m_Target = target; }
	/// @brief 角度を設定します。
	/// @param angle 設定する角度を表す Vector4O 型の値。
	void SetAngle(Vector4O angle) { m_Angle = angle; }
	/// @brief 視野角（FOV）を設定します。
	/// @param fov 設定する視野角（FOV）の値。
	void SetFov(float fov) { m_Fov = fov; }
};

