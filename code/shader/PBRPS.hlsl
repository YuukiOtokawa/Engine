#include "common.hlsl"

Texture2D g_Texture : register(t0); //テクスチャ０番
SamplerState g_SamplerState : register(s0); //サンプラー０番

// PBR用の関数宣言
float3 CalculatePBRLighting(float3 N, float3 V, float3 L, float3 lightColor, 
                           float3 albedo, float metallic, float roughness, float3 f0);
float DistributionGGX(float3 N, float3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(float3 N, float3 V, float3 L, float roughness);
float3 FresnelSchlick(float cosTheta, float3 F0);

static const float PI = 3.14159265359f;

void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
    // 法線計算
    float3 normal = normalize(In.Normal.xyz);
    
    // マテリアルプロパティ
    float4 albedoColor = g_Texture.Sample(g_SamplerState, In.TexCoord);
    float metallic = 0.2f;      // メタリック値（0.0 = 誘電体, 1.0 = 金属）
    float roughness = 0.3f;     // ラフネス値（0.0 = 完全に滑らか, 1.0 = 完全に粗い）
    float3 f0 = lerp(float3(0.04f, 0.04f, 0.04f), albedoColor.rgb, metallic); // 基底反射率
    
    // 視線ベクトル
    float3 viewDir = normalize(CameraPosition.xyz - In.WorldPosition.xyz);
    
    float3 finalColor = float3(0.0f, 0.0f, 0.0f);
    
    // ディレクショナルライト
    if (Light.DirectionalLight.enable)
    {
        float3 lightDir = normalize(-Light.DirectionalLight.direction.xyz);
        float3 lightColor = Light.DirectionalLight.diffuse.rgb;
        
        finalColor += CalculatePBRLighting(normal, viewDir, lightDir, lightColor, 
                                         albedoColor.rgb, metallic, roughness, f0);
    }
    
    // ポイントライト
    if (Light.PointLight.enable)
    {
        float3 lightPos = Light.PointLight.position.xyz;
        float3 lightVector = lightPos - In.WorldPosition.xyz;
        float distance = length(lightVector);
        float3 lightDir = lightVector / distance;
        
        // 距離による減衰
        float attenuation = 1.0f / (1.0f + 0.09f * distance + 0.032f * distance * distance);
        float3 lightColor = Light.PointLight.diffuse.rgb * attenuation;
        
        finalColor += CalculatePBRLighting(normal, viewDir, lightDir, lightColor, 
                                         albedoColor.rgb, metallic, roughness, f0);
    }
    
    // 環境光
    finalColor += Light.AmbientColor.rgb * albedoColor.rgb;
    
    // 最終出力
    outDiffuse = float4(finalColor, albedoColor.a * In.Diffuse.a);
}

// PBRライティング計算のメイン関数
float3 CalculatePBRLighting(float3 N, float3 V, float3 L, float3 lightColor, 
                           float3 albedo, float metallic, float roughness, float3 f0)
{
    float3 H = normalize(V + L);
    
    // 各種内積の計算
    float NdotV = max(dot(N, V), 0.0f);
    float NdotL = max(dot(N, L), 0.0f);
    float HdotV = max(dot(H, V), 0.0f);
    
    // Cook-Torrance BRDF
    float D = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    float3 F = FresnelSchlick(HdotV, f0);
    
    // スペキュラ項の計算
    float3 numerator = D * G * F;
    float denominator = 4.0f * NdotV * NdotL + 0.001f; // 0除算を防ぐ
    float3 specular = numerator / denominator;
    
    // 拡散反射の寄与を計算
    float3 kS = F;
    float3 kD = float3(1.0f, 1.0f, 1.0f) - kS;
    kD *= 1.0f - metallic; // 金属は拡散反射を持たない
    
    return (kD * albedo / PI + specular) * lightColor * NdotL;
}

// GGX分布（Trowbridge-Reitz分布）
float DistributionGGX(float3 N, float3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0f);
    float NdotH2 = NdotH * NdotH;
    
    float numerator = a2;
    float denominator = (NdotH2 * (a2 - 1.0f) + 1.0f);
    denominator = PI * denominator * denominator;
    
    return numerator / denominator;
}

// Schlick-GGXジオメトリ遮蔽関数
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0f);
    float k = (r * r) / 8.0f;
    
    float numerator = NdotV;
    float denominator = NdotV * (1.0f - k) + k;
    
    return numerator / denominator;
}

// Smith's method（遮蔽と陰影の両方を考慮）
float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0f);
    float NdotL = max(dot(N, L), 0.0f);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
    
    return ggx1 * ggx2;
}

// Schlickフレネル近似
float3 FresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.0f - F0) * pow(1.0f - cosTheta, 5.0f);
}