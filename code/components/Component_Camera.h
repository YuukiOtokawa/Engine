// ========================================================
//
// カメラコンポーネントクラス[Component_Camera.h]
// 
//									Date:20250520
//									Author:Yuuki Otokawa
// ========================================================

#pragma once

//==========================================================================
// ヘッダーインクルード
//==========================================================================

#include "Component.h"
#include "VectorO.h"
#include "FrameWork.h"
//==========================================================================
// クラス定義
//==========================================================================

class Camera : public Component
{
protected:
	bool m_IsActiveCamera = false;

    // 前フレームの位置
    Vector4O m_LastPosition = Vector4O::Zero();

	// 注視点
    Vector4O m_Target;

    // 視野角（FOV）
    float m_Fov = 90.0f;

	// プロジェクション行列
    XMMATRIX m_Projection;
	// ビュー行列
    XMMATRIX m_View;

	// クリッピング距離
	float m_Near = 0.1f;
    float m_Far = 1000.0f;

	// 上方ベクトル
	Vector4O m_Up = Vector4O::Up();

public:
    DECLARE_COMPONENT(Camera)
    // コンストラクタ
    Camera();

	/// @brief 投影行列を取得します。
	/// @return 現在の投影行列（XMMATRIX型）。
	XMMATRIX GetProjection() const { return m_Projection; }

	/// @brief ビュー行列を取得します。
	/// @return 現在のビュー行列（XMMATRIX型）を返します。
	XMMATRIX GetView() const { 
		return m_View;
	}

    void Start() override {
        owner->SetTag(GameObjectTag::CameraTag);
        owner->SetLayer(GameObjectLayer::CameraLayer);
    }

    /// @brief コンポーネントの状態を更新します。
    void Update() override;
	/// @brief 描画処理を実行します。
	void Draw(bool castShadow = false) override;
  
	void DrawGUI() override;
	/// @brief タグを初期化します。
	void InitializeTag() override;

    void ExportComponent(YAML::Emitter& out) override {
        out << YAML::Key << "isActiveCamera" << YAML::Value << m_IsActiveCamera;

        out << YAML::Key << "target" << YAML::Value << YAML::Flow << YAML::BeginSeq
            << m_Target.x << m_Target.y << m_Target.z << m_Target.w << YAML::EndSeq;

        out << YAML::Key << "fov" << YAML::Value << m_Fov;
        out << YAML::Key << "near" << YAML::Value << m_Near;
        out << YAML::Key << "far" << YAML::Value << m_Far;

        out << YAML::Key << "up" << YAML::Value << YAML::Flow << YAML::BeginSeq
            << m_Up.x << m_Up.y << m_Up.z << m_Up.w << YAML::EndSeq;
    }

    void ImportFile(YAML::Node& node) override;

	/// @brief ターゲットとなるVector4Oオブジェクトを設定します。
	/// @param target 設定するVector4Oオブジェクト。
	void SetTarget(Vector4O target) { m_Target = target; }
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

    // 視錘台カリング用
    bool IsInView(Vector3O position);

};

