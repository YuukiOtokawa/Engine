
#include "common.hlsl"
Texture2D g_Texture : register(t0); //テクスチャ０番
Texture2D g_NormalTexture : register(t1); //法線マップ１番
SamplerState g_SamplerState : register(s0); //サンプラー０番
void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
	//光源からピクセルへのベクトルを計算
    float4 lv;
    lv = In.WorldPosition - Light.PointLight.position;
    //光源からピクセルへの距離を計算
    float ld = length(lv.xyz);
    lv = normalize(lv); //光源からピクセルへのベクトルを正規化

    //減衰率を計算
    float ofs;

    ofs = 1.0f - (1.0f / Light.PointLight.range) * ld;
    ofs = max(ofs, 0.0f); //減衰率は０未満にならないようにする


    float4 normalMap = g_NormalTexture.Sample(g_SamplerState, In.TexCoord); //法線マップのサンプリング
    normalMap = (normalMap * 2.0f) - 1.0f; //法線マップの値を[-1, 1]の範囲に変換
    
    float4 normal;
    normal.x = normalMap.r; //法線マップのX成分
    normal.y = normalMap.g; //法線マップのY成分
    normal.z = normalMap.b; //法線マップのZ成分
    normal.w = 0.0f; //W成分は０に設定
    
    //ピクセル法線を正規化
    normal = normalize(normal);
    //光源計算
    float light = 0.0f;

    light += -dot(normal.xyz, lv.xyz) * ofs;

    //テクスチャのサンプリング
    outDiffuse = g_Texture.Sample(g_SamplerState, In.TexCoord);
    outDiffuse.rgb *= In.Diffuse.rgb * light + Light.AmbientColor.rgb; //拡散反射と環境光を掛け合わせる
    outDiffuse.a *= In.Diffuse.a;
    
    //カメラからピクセルに向かうベクトル
    float3 eyev = CameraPosition.xyz - In.WorldPosition.xyz;
    eyev = normalize(eyev);
    
    //ハーフベクトル
    float3 halfv;
    halfv = eyev + lv.xyz;
    halfv = normalize(halfv);

    //ブリン・フォンシェーディングの計算
    float specular = 0.0f;
    specular += -dot(halfv, normal.xyz);
    specular = saturate(specular);
    specular = mul(specular, SpecularPower); //スペキュラのパワーを掛ける

    outDiffuse.rgb += (specular * ofs); //スペキュラを出力色に加算
}
