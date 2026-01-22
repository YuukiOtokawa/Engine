#include "common.hlsli"

void vtx(in VS_IN In, out PS_IN Out)
{
    matrix World; 
    World = mul(mul(Scale, Angle), Translation);
	
    matrix wvp;
    wvp = mul(mul(World, View), Projection);

	
    Out.Position = mul(float4(In.Position, 1.0f), wvp); 
	

    float4 normal = float4(In.Normal.xyz, 0.0f); 
    float4 worldNormal = mul(normal, World); 
    worldNormal = normalize(worldNormal); 
    
    Out.Normal = worldNormal; 
    

    float light = -dot(worldNormal.xyz, Light.DirectionalLight.direction.xyz); 
    light = saturate(light); 
    
    Out.TexCoord = In.TexCoord; 
    Out.Diffuse.rgb = In.Diffuse * diffuse; 
    Out.Diffuse.a = In.Diffuse.a; 
    
    Out.WorldPosition = mul(float4(In.Position, 1.0f), World).xyz;
}

Texture2D g_Texture : register(t0); //テクスチャ０番
SamplerState g_SamplerState : register(s0); //サンプラー０番

void pix(in PS_IN In, out float4 outDiffuse : SV_Target)
{
    const float offset = 1.0f / 512.0f;

    // ラプラシアン
    //float filter[3][3] =
    //{
    //    { 0.0, 1.0, 0.0 },
    //    { 1.0, -4.0, 1.0 },
    //    { 0.0, 1.0, 0.0 }
    //};

    // 鮮鋭化
    //float filter[3][3] =
    //{
    //    { 0.0, -1.0, 0.0 },
    //    { -1.0, 5.0, -1.0 },
    //    { 0.0, -1.0, 0.0 }
    //};

    // エンボス
    //float filter[3][3] =
    //{
    //    { 1.0, 0.0, 0.0 },
    //    { 0.0, 0.0, 0.0 },
    //    { 0.0, 0.0, -1.0 }
    //};

    // ガウシアン
    //float filter[3][3] =
    //{
    //    { 1.0, 2.0, 1.0 },
    //    { 2.0, 4.0, 2.0 },
    //    { 1.0, 2.0, 1.0 }
    //};
    
    //for (int y = -1; y <= 1; y++)
    //{
    //    for (int x = -1; x <= 1; x++)
    //    {
    //        outDiffuse += g_Texture.Sample(g_SamplerState, In.TexCoord + float2(x, y) * (1.0f / 512.0f)) * filter[y + 1][x + 1];
    //    }
    //}
    
    //outDiffuse /= 16.0;

    float weight = 0.0;
    for (int y = -5; y <= 5; y++)
    {
        for (int x = -5; x <= 5; x++)
        {
            float lensq = x * x + y * y;
            float gauss = exp(-lensq * 0.008);
            outDiffuse += g_Texture.Sample(g_SamplerState, In.TexCoord + float2(x, y) * offset) * gauss;

            weight += gauss;
        }

    }
    outDiffuse /= weight;
}