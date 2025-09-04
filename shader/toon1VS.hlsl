#include "common.hlsl"

void main(in VS_IN In, out PS_IN Out)
{
    matrix World; //ワールド行列を作成
    World = mul(Scale, mul(Angle, Translation));
	//頂点変換行列を作る World * View * Projection
    matrix wvp;
    wvp = mul(World, View);
    wvp = mul(wvp, Projection);

	//ポリゴンの頂点を変換行列で変換して出力
    Out.Position = mul(float4(In.Position, 1.0f), wvp); 
	
//==========================================================================
// 法線の回転処理
//==========================================================================

    float4 normal = float4(In.Normal.xyz, 0.0f); //w成分を0にする
    float4 worldNormal = mul(normal, World); //法線をワールド座標に変換
    worldNormal = normalize(worldNormal); //法線ベクトルは正規化する
    
    Out.Normal = worldNormal; //法線データを出力
    
//==========================================================================
// 光源計算処理
//==========================================================================

    float light = -dot(worldNormal.xyz, Light.DirectionalLight.direction.xyz); //光源の方向ベクトルと法線ベクトルの内積を計算
    light = saturate(light); //0.0f~1.0fの範囲に収める
    
    Out.TexCoord = In.TexCoord; //テクスチャ座標
    Out.Diffuse.rgb = light; //輝度をDiffuseのRGB成分に格納
    Out.Diffuse.a = In.Diffuse.a; //頂点色のアルファ成分をそのまま使用
    
    Out.WorldPosition = mul(In.Position, World);
}