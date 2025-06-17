#include "common.hlsl"

Texture2D g_Texture : register(t0);
SamplerState g_SamplerState : register(s0);

void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
    outDiffuse = g_Texture.Sample(g_SamplerState, In.TexCoord);
    outDiffuse *= In.Diffuse;
    
    //グレースケール変換
    float3 grayDiffuse;
    
    grayDiffuse.r = 0.299 * outDiffuse.r + 0.587 * outDiffuse.g + 0.114 * outDiffuse.b;
    grayDiffuse.g = -0.169 * outDiffuse.r - 0.3316 * outDiffuse.g + 0.500 * outDiffuse.b;
    grayDiffuse.b = 0.500 * outDiffuse.r - 0.4186 * outDiffuse.g - 0.0813 * outDiffuse.b;
    outDiffuse.rgb = grayDiffuse.rrr;
    
    //セピア変換
    float4 sepia = float4(0.96f, 0.784f, 0.58f, 1.0f);
    
    outDiffuse *= sepia;
    
}