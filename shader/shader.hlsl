//*****************************************************************************
// 定数バッファ
//*****************************************************************************
cbuffer ConstantBuffer : register( b0 )
{
	matrix WorldViewProjection;
}


//=============================================================================
// 頂点シェーダ
//=============================================================================
void VS(in  float4 inPosition	: POSITION0,
		in  float4 inDiffuse	: COLOR0,
		in  float2 inTexCoord	: TEXCOORD0,
		out float4 outPosition	: SV_POSITION,
		out float4 outDiffuse	: COLOR0,
		out float2 outTexCoord  : TEXCOORD0)
{
	outPosition = mul( inPosition, WorldViewProjection );
	outDiffuse = inDiffuse;
	outTexCoord = inTexCoord;
	
	//inNormal.w = 0.0;
	//outNormal = mul(inNormal, World);
	//outNormal = inNormal;
}
//座標変換の計算を行う
//CPUでやるととんでもなく重い
//GPUだからこそできる

Texture2D g_Texture : register(t0);
SamplerState g_SamplerState : register(s0);

//=============================================================================
// ピクセルシェーダ
//=============================================================================
void PS( in  float4 inPosition	: POSITION0,
		 in  float4 inDiffuse	: COLOR0,
		 in  float2 inTexCoord  : TEXCOORD0,
		 out float4 outDiffuse	: SV_Target )
{
	outDiffuse = g_Texture.Sample(g_SamplerState, inTexCoord) * inDiffuse;
	

	//if (LightEnable)
	//{
	//	//ランバート拡散照明モデル
	//	inNormal.xyz = normalize(inNormal.xyz); //正規化
	//	float3 lightDirection = normalize(float3(0.5, -1.0, 0.25));
	//	float3 lightColor = float3(1.0, 1.0, 0.8); //照明色
	//	float3 ambientColor = float3(0.3, 0.3, 0.5); //環境光
	//	//法線ベクトルと光源とのベクトルの内積を計算、負の値は0に合わせる
	//	float3 light = saturate(-dot(inNormal.xyz, lightDirection)) * lightColor + ambientColor;
	//	outDiffuse.rgb *= light;
	//}

	//clip(outDiffuse.a - 0.1);
}
