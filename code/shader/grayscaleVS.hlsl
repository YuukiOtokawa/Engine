#include "common.hlsl"

void main(in VS_IN In, out PS_IN Out)
{
    Out = (PS_IN) 0; // ‰Šú‰»
    
    matrix World; //ƒ[ƒ‹ƒhs—ñ‚ğì¬
    World = mul(Scale, mul(Angle, Translation));
    matrix wvp;
    wvp = mul(World, View);
    wvp = mul(wvp, Projection);
    
    Out.Position = mul(float4(In.Position, 1.0f), wvp); 
    Out.TexCoord = In.TexCoord;
    Out.Diffuse = In.Diffuse;
}