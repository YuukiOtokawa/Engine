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

    ID3D11RenderTargetView* m_pRenderTargetView = nullptr;
    ID3D11ShaderResourceView* m_pShaderResourceView = nullptr;
    int m_ShadowTextureID = -1;

    static std::string m_PixelShaderKey;
    static ID3D11InputLayout* m_pInputLayout;
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
    virtual void Render() = 0;
    virtual void RenderShadow() = 0;

    void Draw(bool castShadow) override;

    void ExportComponent(YAML::Emitter& out) override {
        out << YAML::Key << "isVisible" << YAML::Value << m_bIsVisible;
        out << YAML::Key << "castShadows" << YAML::Value << m_bCastShadows;
        if (m_pMaterial) {
            out << YAML::Key << "materialFileID" << YAML::Value << m_pMaterial->GetFileID();
        }
    }
};

