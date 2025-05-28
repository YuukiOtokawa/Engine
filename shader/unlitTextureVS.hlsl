
#include "common.hlsl"
void main(in VS_IN In, out PS_IN Out)
{
	//頂点変換行列を作る World * View * Projection
	matrix wvp;
	wvp = mul(World, View);
	wvp = mul(wvp, Projection);

	//ポリゴンの頂点を変換行列で変換して出力
	Out.Position = mul(In.Position, wvp); //Position * wvp
	
	Out.Normal = In.Normal;     //法線データ
	Out.TexCoord = In.TexCoord; //テクスチャ座標
	Out.Diffuse = In.Diffuse;	//頂点色
	
}

