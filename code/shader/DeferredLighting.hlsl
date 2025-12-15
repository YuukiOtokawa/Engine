#include "common.hlsl"

Texture2D g_TextureDiffuse : register(t0);
Texture2D g_TextureNormal : register(t1);
Texture2D g_TextureWorldPos : register(t2);
SamplerState g_Sampler : register(s0);

void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
    float4 diffuse = g_TextureDiffuse.Sample(g_Sampler, In.TexCoord);
    float4 encodedNormal = g_TextureNormal.Sample(g_Sampler, In.TexCoord);
    float4 worldPos = g_TextureWorldPos.Sample(g_Sampler, In.TexCoord);

    // 法線を0~1から-1~1にデコード
    float3 normal = encodedNormal.rgb * 2.0 - 1.0;
    normal = normalize(normal);

    // ベクトル定義（全て「点から外」に向かうベクトルに統一）
    float3 lightDir = normalize(Light.PointLight.position.xyz - worldPos.xyz); // 点 -> ライト
    float3 viewDir = normalize(CameraPosition.xyz - worldPos.xyz);             // 点 -> カメラ

    float3 diffuseColor = float3(0, 0, 0);
    float3 specularColor = float3(0, 0, 0);

    if (Light.PointLight.enable)
    {
        // 拡散反射 (Lambert)
        float NdotL = saturate(dot(normal, lightDir));
        diffuseColor = Light.PointLight.diffuse.rgb * diffuse.rgb * NdotL;

        // スペキュラ (Blinn-Phong)
        float3 halfDir = normalize(lightDir + viewDir);
        float NdotH = saturate(dot(normal, halfDir));
        float specularFactor = pow(NdotH, Shininess);
        specularColor = Light.PointLight.diffuse.rgb * specularFactor * SpecularPower;
    }

    // アンビエントライト
    float3 ambient = Light.AmbientColor.rgb * diffuse.rgb;

    // 最終カラー = アンビエント + ディフューズ + スペキュラ
    outDiffuse.rgb = ambient + diffuseColor + specularColor;
    outDiffuse.a = diffuse.a;
}