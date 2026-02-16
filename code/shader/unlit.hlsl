#include "common.hlsli"

void vtx(in VS_IN In, out PS_IN Out)
{
    matrix world;
    world = mul(mul(Scale, Angle), Translation);
    
    matrix wvp;
    wvp = mul(mul(world, View), Projection);
    
    Out.Position = mul(float4(In.Position, 1.0f), wvp);
    
    float4 normal = float4(In.Normal.xyz, 0.0f);
    float4 worldNormal = mul(normal, world);
    worldNormal = normalize(worldNormal);
    
    Out.Normal = worldNormal;

    float light = -dot(worldNormal.xyz, Light.DirectionalLight.direction.xyz);
    light = saturate(light);
    
    Out.TexCoord = In.TexCoord;
    Out.Diffuse.rgb = In.Diffuse * diffuse;
    Out.Diffuse.a = In.Diffuse.a;
    
    Out.WorldPosition = mul(float4(In.Position, 1.0f), world).xyz;

}

Texture2D g_Texture : register(t0);
SamplerState g_SamplerState : register(s0);

void pix(in PS_IN In, out float4 outDiffuse : SV_Target)
{
    outDiffuse = g_Texture.Sample(g_SamplerState, In.TexCoord);

    outDiffuse *= In.Diffuse;
}