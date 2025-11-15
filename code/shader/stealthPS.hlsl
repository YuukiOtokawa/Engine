
//stealthPS.hlsl

// 後方のレンダーテクスチャを使ってステルス効果を出すピクセルシェーダー
// スクリーン座標でテクスチャを参照し、乗算する
// カメレオン的な？
// 特に光の演算などしてない

//ピクセルシェーダー
#include "common.hlsl"	//必ずインクルード

//DirectXのテクスチャの設定を受け継ぐ
Texture2D g_Texture : register(t0); //テクスチャ0番
Texture2D g_Texture2 : register(t1); //テクスチャ0番
//DirectXのサンプラーステート設定を受け継ぐ
SamplerState g_SamplerState : register(s0); //テクスチャサンプラー0番

void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
    float2 tex = In.Position.xy;
    tex.x /= In.Position.w;
    tex.y /= In.Position.w;
    
    // 法線をビュー方向に変換
    float vNorm = mul(In.Normal, View);
    vNorm = normalize(vNorm);
    tex.x = (tex.x * 0.5f) + 0.5f + (vNorm * Parameter.x);
    tex.y = (-tex.y * 0.5f) + 0.5f + (vNorm * Parameter.y);

    tex.x = (tex.x * 0.5f) + 0.5f + Parameter.x;
    tex.y = (tex.y * 0.5f) + 0.5f + Parameter.x;

    tex = saturate(tex);
    In.TexCoord = tex;



	//このピクセルに使われるテクスチャの色を取得
    outDiffuse = g_Texture2.Sample(g_SamplerState, In.TexCoord);
	//このピクセルのデフューズ色を乗算
    outDiffuse *= In.Diffuse;

}
