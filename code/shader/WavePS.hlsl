
#include "common.hlsl"

Texture2D g_Texture : register(t0); //0番テクスチャ model.cpp PSSetShaderResourcesの第一引数に対応
Texture2D g_EnvTexture : register(t3);

SamplerState g_SamplerState : register(s0); //テクスチャの取得の仕方 rennderer.cpp PSSetSamplersの第一引数に対応

void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
    // 視線ベクトル
    float3 eyev = In.WorldPosition.xyz - CameraPosition.xyz;
    eyev = normalize(eyev);
    
    // 法線ベクトル
    float3 normal = normalize(In.Normal);
    
    // 反射ベクトル
    float3 reflectv = reflect(eyev, normal);
    reflectv = normalize(reflectv);
    
    // 環境マッピングテクスチャ座標(パノラマ)
    float2 envTexCoord;
    envTexCoord.x = atan2(reflectv.z, reflectv.x) / (2.0f * 3.14159f);
    envTexCoord.y = acos(reflectv.y) / 3.14159f;
    
    if (textureEnable)
    {
        outDiffuse = g_EnvTexture.Sample(g_SamplerState, envTexCoord);
        outDiffuse *= In.Diffuse;
    }
    else
    {
        outDiffuse = In.Diffuse;
    }

}

