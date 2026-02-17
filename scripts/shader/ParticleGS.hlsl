#include "common.hlsli"

[maxvertexcount(6)]

#define SPRITE_SIZE (0.1f)

void geo(in point PS_IN input[1], in uint Id : SV_PrimitiveID, inout TriangleStream<PS_IN> Out) {
    matrix vp = mul(View, Projection);

    PS_IN output[4];

    // 4つの頂点を生成してビルボードを作成
    float3 worldPos = input[0].WorldPosition;

    // 左下
    output[0].WorldPosition = worldPos + float3(-SPRITE_SIZE, -SPRITE_SIZE, 0.0f);
    output[0].Position = mul(float4(output[0].WorldPosition, 1.0f), vp);
    output[0].TexCoord = float2(0.0f, 0.0f);
    output[0].Diffuse = input[0].Diffuse;
    output[0].Normal = float3(0.0f, 0.0f, 1.0f);
    output[0].LightPosition = float3(0.0f, 0.0f, 0.0f);
    Out.Append(output[0]);

    // 右下
    output[1].WorldPosition = worldPos + float3(SPRITE_SIZE, -SPRITE_SIZE, 0.0f);
    output[1].Position = mul(float4(output[1].WorldPosition, 1.0f), vp);
    output[1].TexCoord = float2(1.0f, 0.0f);
    output[1].Diffuse = input[0].Diffuse;
    output[1].Normal = float3(0.0f, 0.0f, 1.0f);
    output[1].LightPosition = float3(0.0f, 0.0f, 0.0f);
    Out.Append(output[1]);

    // 左上
    output[2].WorldPosition = worldPos + float3(-SPRITE_SIZE, SPRITE_SIZE, 0.0f);
    output[2].Position = mul(float4(output[2].WorldPosition, 1.0f), vp);
    output[2].TexCoord = float2(0.0f, 1.0f);
    output[2].Diffuse = input[0].Diffuse;
    output[2].Normal = float3(0.0f, 0.0f, 1.0f);
    output[2].LightPosition = float3(0.0f, 0.0f, 0.0f);
    Out.Append(output[2]);

    // 右上
    output[3].WorldPosition = worldPos + float3(SPRITE_SIZE, SPRITE_SIZE, 0.0f);
    output[3].Position = mul(float4(output[3].WorldPosition, 1.0f), vp);
    output[3].TexCoord = float2(1.0f, 1.0f);
    output[3].Diffuse = input[0].Diffuse;
    output[3].Normal = float3(0.0f, 0.0f, 1.0f);
    output[3].LightPosition = float3(0.0f, 0.0f, 0.0f);
    Out.Append(output[3]);

    Out.RestartStrip();
}