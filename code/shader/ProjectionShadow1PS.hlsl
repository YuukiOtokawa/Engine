
#include "common.hlsl"

Texture2D g_Texture : register(t0); //0番テクスチャ model.cpp PSSetShaderResourcesの第一引数に対応
Texture2D g_Texture2 : register(t1); //1番テクスチャ model.cpp PSSetShaderResourcesの第一引数に対応)

SamplerState g_SamplerState : register(s0); //テクスチャの取得の仕方 rennderer.cpp PSSetSamplersの第一引数に対応
SamplerState g_SamplerState1 : register(s1); //テクスチャの取得の仕方 rennderer.cpp PSSetSamplersの第一引数に対応)

void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{

    float3 lv = In.WorldPosition - Light.PointLight.position;
    
    float ld = length(lv);
    
    lv = normalize(lv);
    
    float ofs = 1.0f - (1.0f / Light.PointLight.range.x) * ld;
    ofs = max(0, ofs);
    
    float3 normal = normalize(In.Normal);
    float light = -dot(normal, Light.DirectionalLight.direction.xyz);
    light *= ofs;

    outDiffuse = g_Texture.Sample(g_SamplerState, In.TexCoord);
    outDiffuse.rgb *= In.Diffuse.rgb * light;
    outDiffuse.a *= In.Diffuse.a;
    
    float3 eyev = In.WorldPosition.xyz - CameraPosition.xyz;
    eyev = normalize(eyev);

    float3 halfv = normalize(eyev + (-lv.xyz));
    
    float specular = -dot(halfv, normal.xyz);
    specular = saturate(specular);
    specular = pow(specular, SpecularPower);

    outDiffuse.rgb += specular * ofs;
    
    float2 uv = In.LightPosition.xy / In.LightPosition.z;
    uv *= float2(0.5f, -0.5f);
    uv += 0.5f;
    
    outDiffuse.rgb *= g_Texture2.Sample(g_SamplerState, uv).rgb;
}

