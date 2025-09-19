#include "common.hlsl"

Texture2D g_Texture : register(t0); //テクスチャ０番
SamplerState g_SamplerState : register(s0); //サンプラー０番

void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
    float2 uv = In.TexCoord.xy;
    
    // 中心からの距離を計算
    float2 center = float2(0.5f, 0.5f);
    float2 distFromCenter = center - uv;
    distFromCenter.y /= (AspectRatio.x / AspectRatio.y); // アスペクト比を考慮
    float distance = length(distFromCenter);
    
    // 中心から一定距離内でモザイク効果を適用
    if (distance < MosaicSize) // 0.15fから0.3fに変更してより広い範囲に適用
    {
        // if文をなくすとモザイクが全画面にかかる
        
        // UVを画面解像度に変換
        float2 pixelCoord = uv * BlockSize;
        
        // ブロック座標を計算（floor で整数化）
        float2 blockCoord = floor(pixelCoord);
        
        // ブロックの中心座標を計算
        float2 blockCenter = blockCoord + 0.5f;
        
        // ブロック中心のUV座標に戻す
        uv = blockCenter / BlockSize;
        
        // UV座標をクランプして範囲外アクセスを防ぐ
        uv = clamp(uv, 0.0f, 1.0f);
    }
    
    outDiffuse = g_Texture.Sample(g_SamplerState, uv);
    outDiffuse *= In.Diffuse;
}