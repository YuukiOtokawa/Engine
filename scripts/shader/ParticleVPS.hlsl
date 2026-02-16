#include "common.hlsli"

void vtx(in VS_IN In, out PS_IN Out)
{
    matrix world;
    world = mul(mul(Scale, Angle), Translation);
    
    Out.Position = mul(float4(In.Position, 1.0f), world);

    
    Out.TexCoord = In.TexCoord;
    Out.Diffuse.rgb = In.Diffuse * diffuse;
    Out.Diffuse.a = In.Diffuse.a;
    
}

Texture2D g_Texture : register(t0);
SamplerState g_SamplerState : register(s0);

void pix(in PS_IN In, out float4 outDiffuse : SV_Target)
{
    outDiffuse = g_Texture.Sample(g_SamplerState, In.TexCoord);

    outDiffuse *= In.Diffuse;
}