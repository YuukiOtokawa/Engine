#include "common.hlsl"

Texture2D g_Texture : register(t0); //テクスチャ０番
SamplerState g_SamplerState : register(s0); //サンプラー０番

void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
    float2 uv = In.TexCoord.xy;

    outDiffuse.r = g_Texture.Sample(g_SamplerState, uv + float2(rgbShiftAmount, 0.0f)).r; // 赤チャンネルを右にシフト
    outDiffuse.g = g_Texture.Sample(g_SamplerState, uv).g; // 緑チャンネルはそのまま
    outDiffuse.b = g_Texture.Sample(g_SamplerState, uv - float2(rgbShiftAmount, 0.0f)).b; // 青チャンネルを左にシフト
    outDiffuse.a = 1.0f; // アルファチャンネルは不透明に設定
}