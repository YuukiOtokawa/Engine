#include "common.hlsl"

Texture2D g_Texture : register(t0);
SamplerState g_Sampler : register(s0);

void main(in PS_IN In, out float4 outDiffuse : SV_Target0,
          out float4 outNormal : SV_Target1,
          out float4 outWorldPos : SV_Target2)
{
    float4 diffuse = g_Texture.Sample(g_Sampler, In.TexCoord);
    outDiffuse = diffuse * In.Diffuse; // Diffuse component

    // 法線を0~1の範囲にエンコード（-1~1 → 0~1）
    float3 encodedNormal = In.Normal * 0.5 + 0.5;
    outNormal = float4(encodedNormal, 1.0f); // Normal component

    // ワールド座標をそのまま出力（RGB）
    outWorldPos = float4(In.WorldPosition, 1.0f); // WorldPosition component
}