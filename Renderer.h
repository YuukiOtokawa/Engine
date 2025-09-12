#pragma once
#include "Component.h"

#include "../EngineSystem/Material.h"

class MeshFilter;

class Renderer :
    public Component
{
protected:
    Material* m_pMaterial = nullptr;
    bool m_bIsVisible = true;
    bool m_bCastShadows = true;

public:
    // 適切なクリーンアップのための仮想デストラクタ
    virtual ~Renderer() = default;

    // 共通の描画プロパティ
    void SetMaterial(Material* material) { m_pMaterial = material; }
    Material* GetMaterial() const { return m_pMaterial; }
    void SetVisible(bool visible) { m_bIsVisible = visible; }
    bool IsVisible() const { return m_bIsVisible; }
    void SetCastShadows(bool shadows) { m_bCastShadows = shadows; }
    bool CastShadows() const { return m_bCastShadows; }

    // 子クラスが実装しなければならない純粋仮想関数
    virtual void Draw() override = 0;
};

