#include "common.hlsl"

Texture2D g_Texture : register(t0); //0番テクスチャ model.cpp PSSetShaderResourcesの第一引数に対応
SamplerState g_SamplerState : register(s0); //テクスチャの取得の仕方 rennderer.cpp PSSetSamplersの第一引数に対応

void main(in PS_IN In, out float4 outDiffuse : SV_Target) {
	//法線の正規化
    float3 normal = normalize(In.Normal);
	
	//光源計算
    float light = -dot(normal, Light.DirectionalLight.direction.xyz);

	//テクスチャのピクセル色を取得
    outDiffuse = g_Texture.Sample(g_SamplerState, In.TexCoord);
    outDiffuse.rgb *= In.Diffuse.rgb * light;
    outDiffuse.a *= In.Diffuse.a;
    
    //カメラからピクセルに向かうベクトル
    float3 eyev = In.WorldPosition.xyz - CameraPosition.xyz;
    eyev = normalize(eyev);
    
    //反射ベクトルを計算
    float3 refv = reflect(Light.DirectionalLight.direction.xyz, normal.xyz);
    refv = normalize(refv);
    
    float specular = -dot(eyev, refv);
    specular = saturate(specular);
    specular = pow(specular, SpecularPower);
    
    outDiffuse.rgb += specular;
}