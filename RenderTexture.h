#pragma once
#include "Component.h"

#include "RenderCore.h"

//#include <d3d11.h>
class RenderTexture :
    public Component
{
protected:
    ID3D11Texture2D* m_pRenderTargetTexture = nullptr;
    ID3D11RenderTargetView* m_pRenderTargetView = nullptr;

    ID3D11Texture2D* m_pDepthStencilTexture = nullptr;
    ID3D11DepthStencilView* m_pDepthStencilView = nullptr;

    Vector2O m_Resolusion = Vector2O(400.0f, 400.0f);
    Vector2O m_LastResolusion = Vector2O(400.0f, 400.0f);

    // テクスチャ情報
    Texture* m_pTexture = nullptr;
    int m_TextureFID = -1;

    bool m_IsTextureAttached = false;

    Object* m_pCamera;
public:
    DECLARE_COMPONENT(RenderTexture)
    RenderTexture();
    ~RenderTexture();

    void UpdateComponent() override;
    void DrawRenderTexture();
    void DrawGUI() override;
    void InitializeTag() override;

    void ExportComponent() override {
        CSVExporter::ExportVector2O(m_Resolusion);
    }
};

