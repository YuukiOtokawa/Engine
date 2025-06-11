
#include "common.hlsl"
Texture2D g_Texture : register(t0); //テクスチャ０番
SamplerState g_SamplerState : register(s0); //サンプラー０番
void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
	//光源からピクセルへのベクトルを計算
    float4 lv = In.WorldPosition - Light.Position;
    //光源からピクセルへの距離を計算
    float4 ld = length(lv);
    lv = normalize(lv); //光源からピクセルへのベクトルを正規化
    
    //減衰率を計算
    float ofs = 1.0f - (1.0f / Light.PointLightRange.x) * ld.x;
    ofs = max(ofs, 0.0f); //減衰率は０未満にならないようにする
    
    //ピクセル法線を正規化
    float4 normal = normalize(In.Normal);
    //光源計算
    float light = -dot(normal.xyz, lv.xyz);
    light = saturate(light);
    light *= ofs; //減衰率を掛ける
    
    //テクスチャのサンプリング
    outDiffuse = g_Texture.Sample(g_SamplerState, In.TexCoord);
    outDiffuse.rgb *= In.Diffuse.rgb * light + Light.Ambient.rgb; //拡散反射と環境光を掛け合わせる
    outDiffuse.a = In.Diffuse.a;
    
    //カメラからピクセルに向かうベクトル
    float3 eyev = In.WorldPosition.xyz - CameraPosition.xyz;
    eyev = normalize(eyev);
    
    //ハーフベクトル
    float3 halfv = eyev + lv.xyz;
    halfv = normalize(halfv);
    
    //ブリン・フォンシェーディングの計算
    float specular = -dot(halfv, normal.xyz);
    specular = saturate(specular);
    specular = pow(specular, 30);

    outDiffuse.rgb += (specular * ofs);
    
    //リムライティング
    //光源ベクトルとカメラベクトルの角度を計算
    float lit = 1.0f - max(0, dot(lv.xyz, eyev)); //光源とカメラの角度を計算
    
    float lim = 1.0f - max(0, dot(normal.xyz, -eyev)); //ピクセル法線とカメラの角度を計算
    
    //リムライティングの計算
    lim *= lit;
    lim = pow(lim, 3);

    outDiffuse.rgb += lim;
}
