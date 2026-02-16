#include "common.hlsli"

void vtx(in VS_IN In, out PS_IN Out)
{
    matrix world;
    world = mul(mul(Scale, Angle), Translation);
    
    matrix wvp;
    wvp = mul(mul(world, View), Projection);
    
    //Out.Position = mul(float4(In.Position, 1.0f), wvp);
    
    //float4 normal = float4(In.Normal.xyz, 0.0f);
    //float4 worldNormal = mul(normal, world);
    //worldNormal = normalize(worldNormal);
    
    //Out.Normal = worldNormal;

    //float light = -dot(worldNormal.xyz, Light.DirectionalLight.direction.xyz);
    //light = saturate(light);
    
    //Out.TexCoord = In.TexCoord;
    //Out.Diffuse.rgb = In.Diffuse * diffuse;
    //Out.Diffuse.a = In.Diffuse.a;
    
    //Out.WorldPosition = mul(float4(In.Position, 1.0f), world).xyz;
    
    In.Position.xyz += (In.Normal.xyz * Parameter.y * Parameter.z);
    
    Out.Position = mul(float4(In.Position, 1.0f), wvp);
    Out.Diffuse = In.Diffuse * diffuse;
    Out.TexCoord = In.TexCoord;

}

Texture2D g_Texture : register(t0);
Texture2D g_FarTexture : register(t1);

SamplerState g_SamplerState : register(s0);

void pix(in PS_IN In, out float4 outDiffuse : SV_Target)
{
    outDiffuse = g_Texture.Sample(g_SamplerState, In.TexCoord);
    
    float shell = Parameter.z;
    float total = Parameter.x;
    float ratio = shell / total;
    
    if (shell > 0.0f)
    {
        // UV‚ğ‘å‚«‚­‚µ‚ÄƒmƒCƒY‚ğ×‚©‚­‚·‚é
        float2 furUV = In.TexCoord * 20.0f;
        float noise = g_FarTexture.Sample(g_SamplerState, furUV).r;
        if (noise < ratio)
            discard;
    }
    
    outDiffuse.a *= 1.0f - ratio;
}