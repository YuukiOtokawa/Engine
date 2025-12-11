#pragma once
#include "Component.h"
#include "IRenderable.h"

#include "../EngineSystem/Material.h"

#include "RenderQueueManager.h"

class MeshFilter;

class Renderer :
    public Component, public IRenderable
{
protected:
    Material* m_pMaterial = nullptr;
    bool m_bIsVisible = true;
    bool m_bCastShadows = true;

public:
    Renderer(RenderQueue queue) : IRenderable(queue) {}

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
    virtual void Render() = 0;

    void Draw() override;

    void ExportComponent(YAML::Emitter& out) override {
        out << YAML::Key << "isVisible" << YAML::Value << m_bIsVisible;
        out << YAML::Key << "castShadows" << YAML::Value << m_bCastShadows;
        if (m_pMaterial) {
            out << YAML::Key << "materialFileID" << YAML::Value << m_pMaterial->GetFileID();
        }
    }
};

