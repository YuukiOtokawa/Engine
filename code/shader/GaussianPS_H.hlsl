#include "common.hlsl"

SamplerState g_SamplerState : register(s1);
Texture2D g_Texture : register(t0);

void main(in PS_IN In,out float4 outDiffuse:SV_Target)
{
    outDiffuse.a = g_Texture.Sample(g_SamplerState, In.TexCoord).a;
    outDiffuse.rgb = Weight[0] * g_Texture.Sample(g_SamplerState, In.TexCoord).rgb;
    
    float screenWidth = 1920;
    
    //右方向へ7ピクセル分処理
    
    outDiffuse.rgb += Weight[1] * g_Texture.Sample(g_SamplerState,
                                        In.TexCoord + float2(1.0f / screenWidth, 0.0f)).rgb;
    
    outDiffuse.rgb += Weight[2] * g_Texture.Sample(g_SamplerState,
                                        In.TexCoord + float2(2.0f / screenWidth, 0.0f)).rgb;

    outDiffuse.rgb += Weight[3] * g_Texture.Sample(g_SamplerState,
                                        In.TexCoord + float2(3.0f / screenWidth, 0.0f)).rgb;

    outDiffuse.rgb += Weight[4] * g_Texture.Sample(g_SamplerState,
                                       In.TexCoord + float2(4.0f / screenWidth, 0.0f)).rgb;

    outDiffuse.rgb += Weight[5] * g_Texture.Sample(g_SamplerState,
                                       In.TexCoord + float2(5.0f / screenWidth, 0.0f)).rgb;

    outDiffuse.rgb += Weight[6] * g_Texture.Sample(g_SamplerState,
                                       In.TexCoord + float2(6.0f / screenWidth, 0.0f)).rgb;

    outDiffuse.rgb += Weight[7] * g_Texture.Sample(g_SamplerState,
                                        In.TexCoord + float2(7.0f / screenWidth, 0.0f)).rgb;


    //左方向へ7ピクセル分処理
    
    outDiffuse.rgb += Weight[1] * g_Texture.Sample(g_SamplerState,
                                        In.TexCoord - float2(1.0f / screenWidth, 0.0f)).rgb;

    outDiffuse.rgb += Weight[2] * g_Texture.Sample(g_SamplerState,
                                        In.TexCoord - float2(2.0f / screenWidth, 0.0f)).rgb;

    outDiffuse.rgb += Weight[3] * g_Texture.Sample(g_SamplerState,
                                        In.TexCoord - float2(3.0f / screenWidth, 0.0f)).rgb;

    outDiffuse.rgb += Weight[4] * g_Texture.Sample(g_SamplerState,
                                       In.TexCoord - float2(4.0f / screenWidth, 0.0f)).rgb;

    outDiffuse.rgb += Weight[5] * g_Texture.Sample(g_SamplerState,
                                       In.TexCoord - float2(5.0f / screenWidth, 0.0f)).rgb;

    outDiffuse.rgb += Weight[6] * g_Texture.Sample(g_SamplerState,
                                       In.TexCoord - float2(6.0f / screenWidth, 0.0f)).rgb;

    outDiffuse.rgb += Weight[7] * g_Texture.Sample(g_SamplerState,
                                        In.TexCoord - float2(7.0f / screenWidth, 0.0f)).rgb;
}