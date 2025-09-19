#include "common.hlsl"

Texture2D g_Texture : register(t0); //テクスチャ０番
SamplerState g_SamplerState : register(s0); //サンプラー０番

void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
    // ポスタリゼーションの段階数
    const int numLevels = 4; // 4段階に分ける
    float4 color = g_Texture.Sample(g_SamplerState, In.TexCoord);
    // 各色成分を段階的に変化させる
    color.r = floor(color.r * numLevels) / (numLevels - 1);
    color.g = floor(color.g * numLevels) / (numLevels - 1);
    color.b = floor(color.b * numLevels) / (numLevels - 1);
    color.a = 1.0f; // アルファは不変
    outDiffuse = color;
    outDiffuse *= In.Diffuse;
}