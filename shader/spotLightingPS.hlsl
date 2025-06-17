
#include "common.hlsl"
Texture2D g_Texture : register(t0); //テクスチャ０番
SamplerState g_SamplerState : register(s0); //サンプラー０番
void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
	//光源からピクセルへのベクトルを計算
    float4 lv = In.WorldPosition - Light.Position;
    //光源からピクセルへの距離を計算
    float4 normal = normalize(In.Normal);
    float light = -dot(normal.xyz, lv.xyz);
    
    //減衰率を計算
    float3 ld = length(In.WorldPosition.xyz - Light.Position.xyz);
    float ofs = 1.0f - (1.0f / Light.PointLightRange.x) * ld.x;
    ofs = saturate(ofs); //減衰率は０～１の範囲に収める
    light += ofs;
    
    //テクスチャのサンプリング
    outDiffuse = g_Texture.Sample(g_SamplerState, In.TexCoord);
    
    //カメラからピクセルに向かうベクトル
    float3 eyev = In.WorldPosition.xyz - CameraPosition.xyz;
    eyev = normalize(eyev);
    
    //反射ベクトルを計算
    float3 reflectv = reflect(-lv.xyz, normal.xyz);
    reflectv = normalize(reflectv);
    
    //スペキュラ反射の計算
    float specular = pow(saturate(dot(reflectv, eyev)), 30);
    
    //スポットライトの向きと光源からピクセルへのベクトルの角度を計算
    float angle = dot(normalize(Light.Direction), normalize(lv));
    
    //明るさを計算
    float spot = saturate(((angle + Light.SpotLightAngle) / (Light.SpotLightAngle + 1.0f)).x);
    
    outDiffuse.rgb *= Light.Diffuse.rgb * In.Diffuse.rgb * light * spot + Light.Ambient.rgb;
    outDiffuse.rgb += (specular * spot);

    outDiffuse.a = In.Diffuse.a; //アルファ値はそのまま使用
}
