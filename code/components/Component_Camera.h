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
#include "../VectorO.h"
#include "FrameWork.h"
#include "../ComponentFactory.h"

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
    void UpdateComponent() override;
	/// @brief 描画処理を実行します。
	void Draw() override;
  
	void DrawGUI() override;
	/// @brief タグを初期化します。
	void InitializeTag() override;

    void ExportComponent() override {
        CSVExporter::ExportFloat(m_IsActiveCamera);
        CSVExporter::ExportVector4O(m_Target);
        CSVExporter::ExportFloat(m_Fov);
        CSVExporter::ExportFloat(m_Near);
        CSVExporter::ExportFloat(m_Far);
        CSVExporter::ExportVector4O(m_Up);
    }

    void ImportFile(std::vector<std::string>& tokens) override;

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

};

