#include "common.hlsl"
void main(in VS_IN In, out PS_IN Out)
{
	
    matrix World; 
    World = mul(Scale, mul(Angle, Translation));
    matrix wvp;
    wvp = mul(World, View); 
    wvp = mul(wvp, Projection); 
    Out.Position = mul(float4(In.Position, 1.0f), wvp); 

    float4 worldNormal, normal;
    normal = float4(In.Normal.xyz, 0.0);
    worldNormal = mul(normal, World);
    worldNormal = normalize(worldNormal);
    Out.Normal = worldNormal;

    Out.Diffuse = In.Diffuse;
	
    Out.TexCoord = In.TexCoord;

	
    Out.WorldPosition = mul(In.Position, World);
}
