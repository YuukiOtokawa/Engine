
#include "common.hlsl"

Texture2D g_Texture : register(t0); //0番テクスチャ model.cpp PSSetShaderResourcesの第一引数に対応
Texture2D g_EnvTexture : register(t1);

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
    envTexCoord.x = atan2(reflectv.z, reflectv.x) / (2.0f * 3.14159f) + 0.5f;
    envTexCoord.y = acos(reflectv.y) / 3.14159f;
    
    //環境マッピング
    float3 envMap = g_EnvTexture.Sample(g_SamplerState, envTexCoord);
    
    // 屈折ベクトル
    float3 refractv = refract(eyev.xyz, normal, 0.5);
    refractv = normalize(refractv);
    
    // 屈折マッピングテクスチャ座標
    float2 refTexCoord;
    refTexCoord.x = atan2(refractv.z, refractv.x) / (2.0f * 3.14159f) + 0.5f;
    refTexCoord.y = acos(refractv.y) / 3.14159f;
    
    float3 refMap = g_EnvTexture.Sample(g_SamplerState, refTexCoord);
    
    // テクスチャサンプリング
    if (textureEnable)
    {
        outDiffuse = g_Texture.Sample(g_SamplerState, In.TexCoord);
        outDiffuse *= In.Diffuse;
    }
    else
    {
        outDiffuse = In.Diffuse;
    }
    

    // フレネル反射
    float f0 = 0.08;
    float d = dot(-eyev, normal);
    float fresnel = f0 + (1.0 - f0) * pow((1.0 - d), 4.0);
    
    // ライティング結果と環境マッピングを合成
    outDiffuse.rgb = lerp(refMap, envMap, fresnel);
    
    
    outDiffuse.a = 1.0f;
    
    
}

