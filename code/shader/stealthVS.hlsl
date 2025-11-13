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

    float2 tex = Out.Position.xy;
    tex.x /= Out.Position.w;
    tex.y /= Out.Position.w;
    
    // 法線をビュー方向に変換
    float vNorm = mul(worldNormal, View);
    vNorm = normalize(vNorm);
    tex.x = (tex.x * 0.5f) + 0.5f + (vNorm * Parameter.x);
    tex.y = (-tex.y * 0.5f) + 0.5f + (vNorm * Parameter.y);

    tex.x = (tex.x * 0.5f) + 0.5f + Parameter.x;
    tex.y = (tex.y * 0.5f) + 0.5f + Parameter.x;

    tex = saturate(tex);
    Out.TexCoord = tex;
}