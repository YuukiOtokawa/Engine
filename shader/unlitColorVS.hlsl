
#include "common.hlsl"
void main(in VS_IN In, out PS_IN Out)
{
    matrix World; //ÉèÅ[ÉãÉhçsóÒÇçÏê¨
    World = mul(Scale, mul(Angle, Translation));
    matrix wvp;
    wvp = mul(World, View);
    wvp = mul(wvp, Projection);
    Out.Position = mul(float4(In.Position, 1.0f), wvp); 
    Out.WorldPosition = float4(0.0f, 0.0f, 0.0f, 0.0f);
    Out.Normal = In.Normal;
    Out.TexCoord = In.TexCoord;
    Out.Diffuse = In.Diffuse;
    
}

