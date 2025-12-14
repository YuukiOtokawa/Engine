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

    // ライトの方向（ライトが向いている方向の逆）
    float3 lightDir = -normalize(Light.DirectionalLight.direction.xyz);

    // ディフューズ（ランバートの法則）
    float diffuseIntensity = max(dot(normal, lightDir), 0.0);
    diffuseIntensity = saturate(diffuseIntensity);

    // スペキュラ（Blinn-Phong）
    float3 viewDir = normalize(CameraPosition.xyz - worldPos.xyz);
    float3 halfDir = normalize(lightDir + viewDir);
    float specularIntensity = pow(max(dot(normal, halfDir), 0.0), Shininess);
    specularIntensity = saturate(specularIntensity) * SpecularPower;

    // アンビエントライト
    float3 ambient = Light.AmbientColor.rgb * diffuse.rgb;

    // 最終カラー = アンビエント + ディフューズ + スペキュラ
    float3 diffuseColor = Light.DirectionalLight.diffuse.rgb * diffuse.rgb * diffuseIntensity;
    float3 specularColor = Light.DirectionalLight.diffuse.rgb * specularIntensity;

    outDiffuse.rgb = ambient + diffuseColor + specularColor;
    outDiffuse.a = diffuse.a;
}