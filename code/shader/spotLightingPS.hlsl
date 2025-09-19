
#include "common.hlsl"
Texture2D g_Texture : register(t0); //テクスチャ０番
SamplerState g_SamplerState : register(s0); //サンプラー０番
void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
	//光源からピクセルへのベクトルを計算
    float3 lv = In.WorldPosition - Light.SpotLight.position;
    //光源からピクセルへの距離を計算
    float3 normal = normalize(In.Normal);
    float light = -dot(normal, lv);
    
    //減衰率を計算
    float ld = length(In.WorldPosition.xyz - Light.SpotLight.position.xyz);
    float ofs = 1.0f - (1.0f / Light.SpotLight.range.x) * ld;
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
    float angle = dot(normalize(Light.SpotLight.direction), normalize(lv));
    
    //明るさを計算
    float spot = 0.0f;
    if (angle < Light.SpotLight.innerAngle)
    {
        spot = 0.0f; //スポットライトの内角より小さい場合は最大値に設定
    }
    else if (angle < Light.SpotLight.outerAngle)
    {
        //スポットライトの内角と外角の間の場合は、線形補間でスポットライトの強さを計算
        //スポットライトの内角と外角の間で、スポットライトの強さを線形に変化させる
        //スポットライトの強さは、内角から外角までの範囲で0.9から0.0まで変化する
        float t = (angle - Light.SpotLight.innerAngle) / (Light.SpotLight.outerAngle - Light.SpotLight.innerAngle);
        spot = 0.9f * t; //スポットライトの強さを計算
    }
    else
    {
        spot = 0.9f; //スポットライトの外角より大きい場合も光が当たらない
    }
    spot = saturate(spot); //スポットライトの強さを０～１の範囲に収める

    outDiffuse.rgb *= Light.SpotLight.diffuse.rgb * In.Diffuse.rgb * light * spot + Light.AmbientColor.rgb;
    outDiffuse.rgb += (specular * spot);

    outDiffuse.a = In.Diffuse.a; //アルファ値はそのまま使用
}
