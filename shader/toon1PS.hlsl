#include "common.hlsl"
Texture2D g_Texture : register(t0); //テクスチャ０番
SamplerState g_SamplerState : register(s0); //サンプラー０番
void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
	//光源からピクセルへのベクトルを計算
    float4 lv = In.WorldPosition - Light.PointLight.position;
    //光源からピクセルへの距離を計算
    float4 ld = length(lv);
    lv = normalize(lv); //光源からピクセルへのベクトルを正規化
    
    //減衰率を計算
    float ofs = 1.0f - (1.0f / Light.PointLight.range.x) * ld.x;
    ofs = max(ofs, 0.0f); //減衰率は０未満にならないようにする
    
    //ピクセル法線を正規化
    float4 normal = normalize(In.Normal);
    //光源計算
    float light = -dot(normal.xyz, lv.xyz);
    light = saturate(light);
    //light *= ofs; //減衰率を掛ける

    if (light < 0.05f) {
        light = 0.2f;
    }
    else if (light < 0.5f) {
        light = 0.5f;
    }
    else if (light < 1.0f) {
        light = 1.0f;
    }
    
    //テクスチャのサンプリング
    outDiffuse = g_Texture.Sample(g_SamplerState, In.TexCoord);
    outDiffuse.rgb *= In.Diffuse.rgb * light;
    outDiffuse.a = In.Diffuse.a;

    
    //カメラからピクセルに向かうベクトル
    float3 eyev = In.WorldPosition.xyz - CameraPosition.xyz;
    eyev = normalize(eyev);
    
    //エッジ
    if (-dot(normal.xyz, eyev.xyz) < 0.25f) {
        outDiffuse.rgb = 0.0f; //エッジを付ける
    }

}
