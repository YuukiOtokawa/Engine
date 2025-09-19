#include "common.hlsl"

SamplerState g_SamplerState : register(s1);
Texture2D g_Texture : register(t0);

void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
    // アルファ値はそのまま出力
    outDiffuse.a = g_Texture.Sample(g_SamplerState, In.TexCoord).a;
    
    // 中心のピクセル
    outDiffuse.rgb = Weight[0] * g_Texture.Sample(g_SamplerState, In.TexCoord).rgb;

    float screenHeight = 1920;
    
    
    //下方向へ7ピクセル分処理
    outDiffuse.rgb += Weight[1] * g_Texture.Sample(g_SamplerState,
                                        In.TexCoord + float2(0.0f, 1.0f / screenHeight)).rgb;
    outDiffuse.rgb += Weight[2] * g_Texture.Sample(g_SamplerState,
                                        In.TexCoord + float2(0.0f, 2.0f / screenHeight)).rgb;
    outDiffuse.rgb += Weight[3] * g_Texture.Sample(g_SamplerState,
                                        In.TexCoord + float2(0.0f, 3.0f / screenHeight)).rgb;
    outDiffuse.rgb += Weight[4] * g_Texture.Sample(g_SamplerState,
                                       In.TexCoord + float2(0.0f, 4.0f / screenHeight)).rgb;
    outDiffuse.rgb += Weight[5] * g_Texture.Sample(g_SamplerState,
                                       In.TexCoord + float2(0.0f, 5.0f / screenHeight)).rgb;
    outDiffuse.rgb += Weight[6] * g_Texture.Sample(g_SamplerState,
                                       In.TexCoord + float2(0.0f, 6.0f / screenHeight)).rgb;
    outDiffuse.rgb += Weight[7] * g_Texture.Sample(g_SamplerState,
                                        In.TexCoord + float2(0.0f, 7.0f / screenHeight)).rgb;

    //上方向へ7ピクセル分処理
    outDiffuse.rgb += Weight[1] * g_Texture.Sample(g_SamplerState,
                                        In.TexCoord - float2(0.0f, 1.0f / screenHeight)).rgb;
    outDiffuse.rgb += Weight[2] * g_Texture.Sample(g_SamplerState,
                                        In.TexCoord - float2(0.0f, 2.0f / screenHeight)).rgb;
    outDiffuse.rgb += Weight[3] * g_Texture.Sample(g_SamplerState,
                                        In.TexCoord - float2(0.0f, 3.0f / screenHeight)).rgb;
    outDiffuse.rgb += Weight[4] * g_Texture.Sample(g_SamplerState,
                                       In.TexCoord - float2(0.0f, 4.0f / screenHeight)).rgb;
    outDiffuse.rgb += Weight[5] * g_Texture.Sample(g_SamplerState,
                                       In.TexCoord - float2(0.0f, 5.0f / screenHeight)).rgb;
    outDiffuse.rgb += Weight[6] * g_Texture.Sample(g_SamplerState,
                                       In.TexCoord - float2(0.0f, 6.0f / screenHeight)).rgb;
    outDiffuse.rgb += Weight[7] * g_Texture.Sample(g_SamplerState,
                                        In.TexCoord - float2(0.0f, 7.0f / screenHeight)).rgb;
}