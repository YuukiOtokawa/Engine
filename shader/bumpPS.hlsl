
#include "common.hlsl"
Texture2D g_Texture : register(t0); //テクスチャ０番
Texture2D g_NormalTexture : register(t1); //法線マップ１番
SamplerState g_SamplerState : register(s0); //サンプラー０番
void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
	//光源からピクセルへのベクトルを計算
    float4 lv = In.WorldPosition - Light.Position;
    //光源からピクセルへの距離を計算
    float ld = length(lv);
    lv = normalize(lv); //光源からピクセルへのベクトルを正規化
    
    //減衰率を計算
    float ofs = 1.0f - (1.0f / Light.PointLightRange.x) * ld;
    ofs = max(ofs, 0.0f); //減衰率は０未満にならないようにする
    
    float4 normalMap = g_NormalTexture.Sample(g_SamplerState, In.TexCoord); //法線マップのサンプリング
    normalMap = (normalMap * 2.0f) - 1.0f; //法線マップの値を[-1, 1]の範囲に変換
    
    float4 normal;
    normal.x = normalMap.r * -1.0f; //法線マップのX成分
    normal.y = normalMap.b; //法線マップのY成分
    normal.z = normalMap.g; //法線マップのZ成分
    normal.w = 0.0f; //W成分は０に設定
    
    //ピクセル法線を正規化
    normal = normalize(normal);
    //光源計算
    float light = -dot(normal.xyz, lv.xyz);
    //light = saturate(light);
    light *= ofs; //減衰率を掛ける
    
    //テクスチャのサンプリング
    outDiffuse = g_Texture.Sample(g_SamplerState, In.TexCoord);
    outDiffuse.rgb *= In.Diffuse.rgb * light + Light.Ambient.rgb; //拡散反射と環境光を掛け合わせる
    outDiffuse.a *= In.Diffuse.a;
    
    //カメラからピクセルに向かうベクトル
    float3 eyev = CameraPosition.xyz - In.WorldPosition.xyz;
    eyev = normalize(eyev);
    
    //ハーフベクトル
    float3 halfv = eyev + lv.xyz;
    halfv = normalize(halfv);
    
    //ブリン・フォンシェーディングの計算
    float specular = -dot(halfv, normal.xyz);
    specular = saturate(specular);
    specular = pow(specular, Light.SpecularPower); //スペキュラのパワーを掛ける

    outDiffuse.rgb += (specular * ofs);
}
