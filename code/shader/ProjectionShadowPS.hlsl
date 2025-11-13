
#include "common.hlsl"

Texture2D g_Texture : register(t0); //0番テクスチャ model.cpp PSSetShaderResourcesの第一引数に対応

SamplerState g_SamplerState : register(s0); //テクスチャの取得の仕方 rennderer.cpp PSSetSamplersの第一引数に対応

void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{

    outDiffuse = float4(0.5f, 0.5f, 0.5f, 1.0f);

}

