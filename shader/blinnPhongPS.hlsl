
#include "common.hlsl"
Texture2D g_Texture : register(t0); 
SamplerState g_SamplerState : register(s0); 
void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
	
    float4 normal = normalize(In.Normal);
    float light = 0.0f;
    if (Light.DirectionalLight.enable)
    light += -dot(normal.xyz, Light.DirectionalLight.direction.xyz);
    light = saturate(light); 


    outDiffuse = g_Texture.Sample(g_SamplerState, In.TexCoord);
    outDiffuse.rgb *= In.Diffuse.rgb * light; 
    outDiffuse.a *= In.Diffuse.a;

    
    float3 eyev = In.WorldPosition.xyz - CameraPosition.xyz;
    eyev = normalize(eyev);
    
    float3 halfv = 0.0f;
    if (Light.PointLight.enable)
    {
        halfv += eyev + normalize(Light.DirectionalLight.direction.xyz);
    }
    halfv = normalize(halfv);
    float specular = -dot(halfv, normal.xyz);
    specular = saturate(specular);
    specular = mul(specular, SpecularPower);

    outDiffuse.rgb += specular;


}
