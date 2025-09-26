#pragma once
#include "RenderTexture.h"

#include "RenderCore.h"

class PostProcessTexture :
    public RenderTexture
{
    Texture* m_pPostProcessTexture[3] = {};
public:
    DECLARE_COMPONENT(PostProcessTexture)
    PostProcessTexture();

    void Update() override;

    void SetShaderResourceView(Texture* texture, int n);

    void DrawGUI() override;
};

