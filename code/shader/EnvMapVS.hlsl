
#include "common.hlsl"
void main(in VS_IN In, out PS_IN Out)
{
    // 1. ワールド行列を合成する (S * R * T)
    // この時点のWorld行列は、C++と同じ行優先行列
    matrix world = mul(mul(Scale, Angle), Translation);
    // 2. WVP行列を合成する
    // 頂点を行ベクトルとして扱う(mul(vector, matrix))ため、
    // 行列の乗算順は W -> V -> P の順にする
    matrix wvp = mul(mul(world, View), Projection);
    
    // ★★★ 不要なtransposeは行わない ★★★

    // 3. 座標変換を実行する
    // In.Position (行ベクトル) に wvp (行優先行列) を乗算する
    Out.Position = mul(float4(In.Position, 1.0f), wvp);

    // ★★★ w成分の強制的な上書きは行わない ★★★

    // 他のデータを渡す処理
    Out.TexCoord = In.TexCoord;
    Out.Diffuse = In.Diffuse * diffuse; // マテリアルの拡散反射色を掛け合わせる
	
    float4 worldNormal, normal;
    normal = float4(In.Normal, 0.0f);
    worldNormal = normalize(mul(normal, world));
    
    Out.Normal = worldNormal.xyz;
    Out.WorldPosition = mul(In.Position, world);
    
    //float3 lightDirection = normalize(Light.DirectionalLight.direction.xyz);
    //float light = -dot(worldNormal.xyz, lightDirection); //光源の方向ベクトルと法線ベクトルの内積を計算
    //light = saturate(light); //0.0f~1.0fの範囲に収める
    
    //Out.Diffuse.rgb = light; //輝度をDiffuseのRGB成分に格納

}

