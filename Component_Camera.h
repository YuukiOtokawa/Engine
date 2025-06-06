// ========================================================
//
// �J�����R���|�[�l���g[Component_Camera.h]
// 
//									Date:20250520
//									Author:Yuuki Otokawa
// ========================================================

#pragma once

//==========================================================================
// �w�b�_�[�C���N���[�h
//==========================================================================

#include "Component.h"
#include "Vector4O.h"
#include "FrameWork.h"

//==========================================================================
// �N���X��`
//==========================================================================

class Camera : public Component
{
private:
	bool m_IsActiveCamera = false;

	// 前フレームの位置
    Vector4O m_LastPosition = Vector4O::Zero();

	// カメラの視線ターゲット位置
    Vector4O m_Target;
	// カメラの角度
    Vector4O m_Angle;

	// カメラの視野角（FOV）
    float m_Fov = 90.0f;

	// プロジェクション行列
    XMMATRIX m_Projection;
	// ビュー行列
	XMMATRIX m_View;

	// 近距離クリッピング距離
	float m_Near = 0.1f;
	// 遠距離クリッピング距離
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

	/// @brief カメラの角度を取得します。
	/// @return 現在のカメラの角度を表す Vector4O 型の値。
	Vector4O GetAngle() const { return m_Angle; }

    /// @brief コンポーネントの状態を更新します。
    void UpdateComponent() override;
	/// @brief 描画処理を実行します。
	void Draw() override;
  
	void DrawGUI() override;
	/// @brief タグを初期化します。
	void InitializeTag() override;

	/// @brief 視線ターゲットとなるVector4O値を設定します。
	/// @param target 設定するVector4O型のターゲット値。
	void SetTarget(Vector4O target) { m_Target = target; }
	/// @brief カメラの角度を設定します。
	/// @param angle 設定する角度を表す Vector4O 型の値。
	void SetAngle(Vector4O angle) { m_Angle = angle; }
	/// @brief 視野角（FOV）を設定します。
	/// @param fov 設定する視野角（FOV）の値。
	void SetFov(float fov) { m_Fov = fov; }

	Vector4O GetTarget() const { return m_Target; }

	void SetActiveCamera(bool active) { 
		m_IsActiveCamera = active;
	}
	bool IsActiveCamera() const { 
		return m_IsActiveCamera;
	}
};

