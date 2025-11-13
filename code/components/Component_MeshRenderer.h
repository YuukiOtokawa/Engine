// ========================================================
//
// メッシュレンダラーコンポーネントクラス[Component_MeshRenderer.h]
// 
//									Date:20250520
//									Author:Yuuki Otokawa
// ========================================================

#pragma once

//==========================================================================
// ヘッダーインクルード
//==========================================================================

#include "Renderer.h"


//==========================================================================
// クラス定義
//==========================================================================

class MeshRenderer :
    public Renderer
{
public:
    DECLARE_COMPONENT(MeshRenderer)

    // コンストラクタ。
    MeshRenderer() {
        m_ClassID = CID_Component_MeshRenderer;
        // 継承関係のため、ここでマテリアルを直接newする。
        // もしくは、基底クラスのコンストラクタで初期化することも可能。
        m_pMaterial = new Material();
    }

    void Update() override;
    void Render() override; // ここにメインの描画ロジックを記述します
    void RenderShadow();
    void DrawGUI() override;
    void ImportFile(YAML::Node& node) override;
    void ExportComponent(YAML::Emitter& out) override;
    void AddExportList() override;
    void InitializeTag() override;

};

