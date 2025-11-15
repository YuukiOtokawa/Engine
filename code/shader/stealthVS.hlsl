#include "common.hlsl"
void main(in VS_IN In, out PS_IN Out)
{
    matrix World;
    World = mul(Scale, mul(Angle, Translation));

    matrix wvp;

    wvp = mul(World, View);
    wvp = mul(wvp, Projection); //wvp = wvp *プロジェクション行列
    Out.Position = mul(In.Position, wvp); //変換結果を出力する

//頂点法線をワールド行列で回転させる(頂点と同じ回転をさせる)
    float4 worldNormal, normal; //ローカル変数を作成
    normal = float4(In.Normal.xyz, 0.0); //入力法線ベクトルのwを0としてコピー（平行移動しないため)
    worldNormal = mul(normal, World); //コピーされた法線をワールド行列で回転する
    worldNormal = normalize(worldNormal); //回転後の法線を正規化する
    Out.Normal = worldNormal; //回転後の法線出力 In.Normalでなく回転後の法線を出力
    Out.Diffuse = In.Diffuse; //頂点の物をそのまま出力

}