
#include "common.hlsl"
Texture2D g_Texture : register(t0); //テクスチャ０番
Texture2D g_NormalTexture : register(t1); //法線マップ１番
SamplerState g_SamplerState : register(s0); //サンプラー０番

float Beckmann(float m, float cosTheta);
float Fresnel(float n, float c);
float GeometricDamping(float nh, float nv, float nl, float vh);

float4 main(PS_IN In) : SV_Target
{
    float4 outDiffuse;
	// 光源ベクトル
    float3 lv;
    
    lv = Light.PointLight.position.xyz - In.Position.xyz;
    
    // 光源ベクトルを正規化
    lv = normalize(lv); //光源からピクセルへのベクトルを正規化
    //物体と光源の距離
    float ld;
    ld = length(lv);
    // 減衰率
    float ofs;
    ofs = 1.0f - (1.0f / SpecularPower) * ld;
    ofs = max(ofs, 0.0f); //減衰率は０未満にならないようにする

    // 法線ベクトル
    float3 normal = normalize(In.Normal.xyz);
    // 明るさ
    float light = 0.0f;
    light += saturate(-dot(normal, lv)) * ofs; // 光源からの拡散反射を計算
    // テクスチャカラー
    outDiffuse = g_Texture.Sample(g_SamplerState, In.TexCoord);
    outDiffuse.rgb *= In.Diffuse.rgb * light + Light.AmbientColor.rgb; //拡散反射と環境光を掛け合わせる
    outDiffuse.a = In.Diffuse.a;
    
    //視線ベクトル
    float3 eyev = In.WorldPosition.xyz - CameraPosition.xyz;
    eyev = normalize(eyev);
    //ハーフベクトル
    float3 halfv = 0.0f;
    halfv += eyev + lv;
    halfv = normalize(halfv);
    //ハーフベクトルを計算
    //float3 halfv = eyev + lv.xyz; //視線ベクトル
    
    //スペキュラー計算
    float specular;
    float nv = saturate(dot(normal, lv));
    float nh = saturate(dot(normal, halfv));
    float vh = saturate(dot(eyev, halfv));
    float nl = saturate(dot(normal, lv));
    
    float D = Beckmann(0.3f, nh);
    float G = GeometricDamping(nh, nv, nl, vh);
    float F = Fresnel(0.8f, dot(lv.xyz, halfv));
    
    specular = D * G * F / (4.0f * nv * nl);
    
    outDiffuse.rgb += specular * ofs; //スペキュラーを加算

    return outDiffuse;
}


float Beckmann(float m, float cosTheta)
{
    //ベックマン分布関数の計算
    return exp((cosTheta * cosTheta - 1.0f) / (m * m * cosTheta * cosTheta)) / (4 * m * m * cosTheta * cosTheta * cosTheta * cosTheta);
}

float Fresnel(float n, float c)
{
    //フレネル方程式の計算
    return n + (1 - n) * pow(1 - c, 5);
}

float GeometricDamping(float nh, float nv, float nl, float vh)
{
    //幾何学的減衰の計算
    return min(1.0f, min((2 * nh * nv) / vh, (2 * nh * nl) / vh));
}