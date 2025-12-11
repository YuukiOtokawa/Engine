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

#include "IRenderable.h"


//==========================================================================
// クラス定義
//==========================================================================

class MeshRenderer :
    public Renderer
{
public:
    DECLARE_COMPONENT(MeshRenderer)

    // コンストラクタ。
    MeshRenderer();

    void Update() override;
    void Render() override; // ここにメインの描画ロジックを記述します
    void RenderShadow();
    void DrawGUI() override;
    void ImportFile(YAML::Node& node) override;
    void ExportComponent(YAML::Emitter& out) override;
    void AddExportList() override;
    void InitializeTag() override;

};

