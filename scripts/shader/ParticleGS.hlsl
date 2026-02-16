#include "common.hlsli"

[maxvertexcount(6)]

#define SPRITE_SIZE (0.1f)

void geo(in point PS_IN input[1], in uint Id : SV_PrimitiveID, inout TriangleStream<PS_IN> Out) {
    matrix vp = mul(View, Projection);
    
    PS_IN copy[4];
    for (int i = 0; i < 4; i++)
    {
        copy[i] = input[0];
    }
    
    copy[0].Position.x = copy[0].Position.x - SPRITE_SIZE;
    copy[0].Position.y = copy[0].Position.y - SPRITE_SIZE;
    copy[0].TexCoord = float2(0.0f,0.0f);
    copy[0].Position = mul(copy[0].Position, vp);
    Out.Append(copy[0]);
    
    copy[1].Position.x = copy[1].Position.x + SPRITE_SIZE;
    copy[1].Position.y = copy[1].Position.y - SPRITE_SIZE;
    copy[1].TexCoord = float2(1.0f,0.0f);
    copy[1].Position = mul(copy[1].Position, vp);
    Out.Append(copy[1]);
    
    copy[2].Position.x = copy[2].Position.x - SPRITE_SIZE;
    copy[2].Position.y = copy[2].Position.y + SPRITE_SIZE;
    copy[2].TexCoord = float2(0.0f,1.0f);
    copy[2].Position = mul(copy[2].Position, vp);
    Out.Append(copy[2]);
    
    copy[3].Position.x = copy[3].Position.x + SPRITE_SIZE;
    copy[3].Position.y = copy[3].Position.y + SPRITE_SIZE;
    copy[3].TexCoord = float2(1.0f,1.0f);
    copy[3].Position = mul(copy[3].Position, vp);
    Out.Append(copy[3]);
    
    Out.RestartStrip();
}