
#include "common.hlsl"
Texture2D g_Texture : register(t0); 
SamplerState g_SamplerState : register(s0); 
void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
	
    float4 normal = normalize(In.Normal);
    float light = -dot(normal.xyz, Light.Direction.xyz); 
    light = saturate(light); 


    outDiffuse = g_Texture.Sample(g_SamplerState, In.TexCoord);
    outDiffuse.rgb *= In.Diffuse.rgb * light; 
    outDiffuse.a *= In.Diffuse.a;

    
    float3 eyev = In.WorldPosition.xyz - CameraPosition.xyz;
    eyev = normalize(eyev);
    
    
    float3 halfv = eyev + normalize(Light.Direction.xyz);
    halfv = normalize(halfv);
    float specular = -dot(halfv, normal.xyz);
    specular = saturate(specular);
    specular = pow(specular, 30);
    
	
    outDiffuse.rgb += specular; 



}
