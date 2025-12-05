// ========================================================
//
// カメラポストプロセスコンポーネント[Component_CameraPostProcess.h]
//
//									Date:20251205
//									Author:Yuuki Otokawa
// ========================================================

#pragma once

//==========================================================================
// ヘッダーインクルード
//==========================================================================

#include "Component.h"
#include "VectorO.h"
#include "Material.h"

//==========================================================================
// クラス定義
//==========================================================================

class CameraPostProcess : public Component
{
private:
    bool m_IsEnabled = false;                           // ポストプロセス有効/無効
    Material* m_pPostProcessMaterial[3] = {};          // ポストプロセス用マテリアル
    Vector4O m_Parameter = { 1920.0f,1080.0f,0.0f,0.0f }; // ポストエフェクトのパラメーター
    float m_Weight[8];                                  // ガウス分布の重み

    void CalcGaussianWeight(float dispersion);

public:
    DECLARE_COMPONENT(CameraPostProcess)

    CameraPostProcess();
    ~CameraPostProcess();

    void Start() override;
    void Update() override;
    void DrawGUI() override;

    /// @brief ポストプロセスが有効かどうかを取得します。
    /// @return 有効な場合true
    bool IsEnabled() const { return m_IsEnabled; }

    /// @brief ポストプロセスの有効/無効を設定します。
    /// @param enabled 有効にする場合true
    void SetEnabled(bool enabled) { m_IsEnabled = enabled; }

    /// @brief 指定されたインデックスのポストプロセスマテリアルを取得します。
    /// @param n マテリアルのインデックス（0-2）
    /// @return ポストプロセスマテリアルへのポインタ
    Material* GetPostProcessMaterial(int n) {
        if (n >= 0 && n < 3)
            return m_pPostProcessMaterial[n];
        return nullptr;
    }

    /// @brief ガウシアンブラーの重み配列を取得します。
    /// @return 重み配列の先頭ポインタ
    float* GetWeight() { return m_Weight; }

    /// @brief ポストプロセスパラメータを取得します。
    /// @return パラメータ
    Vector4O GetParameter() const { return m_Parameter; }

    /// @brief ポストプロセスパラメータを設定します。
    /// @param param パラメータ
    void SetParameter(Vector4O param) { m_Parameter = param; }

    void ExportComponent(YAML::Emitter& out) override {
        out << YAML::Key << "isEnabled" << YAML::Value << m_IsEnabled;
        out << YAML::Key << "parameter" << YAML::Value << YAML::Flow << YAML::BeginSeq
            << m_Parameter.x << m_Parameter.y << m_Parameter.z << m_Parameter.w << YAML::EndSeq;
    }

    void ImportFile(YAML::Node& node) override;
};
