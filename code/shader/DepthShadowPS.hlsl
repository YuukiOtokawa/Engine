#include "common.hlsl"

Texture2D g_Texture : register(t0);
Texture2D g_ShadowDepth : register(t1);
SamplerState g_SamplerState : register(s0);

void main(in PS_IN In, out float4 outDiffuse : SV_TARGET)
{
    outDiffuse = g_Texture.Sample(g_SamplerState, In.TexCoord); // テクスチャの色
    outDiffuse *= In.Diffuse;

    In.LightPosition.xyz /= In.LightPosition.w; // 正規化デバイス座標
    In.LightPosition.x = In.LightPosition.x * 0.5f + 0.5f; // 0~1に変換
    In.LightPosition.y = In.LightPosition.y * -0.5f + 0.5f;

    float depth = g_ShadowDepth.Sample(g_SamplerState, In.LightPosition.xy).r; // シャドウマップから深度値を取得

    if (depth < In.LightPosition.z - 0.001f) // シャドウマップの深度値が小さい場合は影になる
    {
        outDiffuse.rgb *= 0.5f; // 影の部分を暗くする
    }
}