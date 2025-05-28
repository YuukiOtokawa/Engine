#include "common.hlsl"

Texture2D g_Texture : register(t0); //0番テクスチャ model.cpp PSSetShaderResourcesの第一引数に対応
SamplerState g_SamplerState : register(s0); //テクスチャの取得の仕方 rennderer.cpp PSSetSamplersの第一引数に対応

void main(in PS_IN In, out float4 outDiffuse : SV_Target) {
	//法線の正規化
    float4 normal = normalize(In.Normal);
	
	//光源計算
    float1 light = -dot(normal.xyz, Light.Direction.xyz);
	
	//テクスチャのピクセル色を取得
    outDiffuse = g_Texture.Sample(g_SamplerState, In.TexCoord);
    //outDiffuse = 1.0f;
    outDiffuse.rgb *= In.Diffuse.rgb * light;
    outDiffuse.a *= In.Diffuse.a;
    
    //カメラからピクセルに向かうベクトル
    float3 eyev = In.WorldPosition.xyz - CameraPosition.xyz;
    eyev = normalize(eyev);
    
    //ハーフベクトル
    float3 halfv = eyev + normalize(Light.Direction.xyz);
    halfv = normalize(halfv);
    float specular = -dot(halfv, normal.xyz);
    specular = saturate(specular);
    specular = pow(specular, 30);
    
    //半球ライティング
    float norm = dot(normal, normalize(Light.GroundNormal));
    norm = (norm + 1.0f) * 0.5f; // -1.0 ~ 1.0 -> 0.0 ~ 1.0)
    float3 hemiColor = lerp(Light.GroundColor, Light.SkyColor, norm);
    
    
    outDiffuse.rgb += hemiColor + specular;
}