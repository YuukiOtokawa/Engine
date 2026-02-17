cbuffer TranslationBuffer : register(b0)
{
    matrix Translation;
}
cbuffer AngleBuffer : register(b1)
{
    matrix Angle;
}
cbuffer ScaleBuffer : register(b2)
{
    matrix Scale;
}
cbuffer ViewBuffer : register(b3)
{
    matrix View;
}
cbuffer ProjectionBuffer : register(b4)
{
    matrix Projection;
}

struct VS_IN
{
    float3 Position : POSITION0;
    float3 Normal : NORMAL0;
    float4 Diffuse : COLOR0;
    float2 TexCoord : TEXCOORD0;
    
};

struct PS_IN
{
    float4 Position : SV_POSITION;
    float3 WorldPosition : POSITION0;
    float3 Normal : NORMAL0;
    float4 Diffuse : COLOR0;
    float2 TexCoord : TEXCOORD0;
    float3 LightPosition : POSITION1;
};

// VS→GS用の構造体（SV_POSITIONを使わない）
struct VS_TO_GS
{
    float4 WorldPosition : POSITION0;
    float3 Normal : NORMAL0;
    float4 Diffuse : COLOR0;
    float2 TexCoord : TEXCOORD0;
};

// パーティクル専用: VS→GS構造体
struct PARTICLE_VS_TO_GS
{
    float3 WorldPosition : POSITION0;
    float4 Diffuse : COLOR0;
};

struct DIRECTIONAL_LIGHT
{
    bool enable;
    bool3 dummy;
    float4 diffuse;
    float4 direction;
};

struct POINT_LIGHT
{
    bool enable;
    bool2 dummy;
    float range;
    float4 diffuse;
    float4 position;
};

struct SPOT_LIGHT
{
    bool enable;
    float range;
    float innerAngle;
    float outerAngle;
    float4 diffuse;
    float4 position;
    float4 direction;
};

struct LIGHT_BUFFER
{
    DIRECTIONAL_LIGHT DirectionalLight;
    POINT_LIGHT PointLight;
    SPOT_LIGHT SpotLight;
    float4 AmbientColor;
};

cbuffer LightBuffer : register(b5)
{
    LIGHT_BUFFER Light;
}

cbuffer CameraBuffer : register(b6)
{
    float4 CameraPosition;
}

cbuffer ParameterBuffer : register(b7)
{
    float4 Parameter;
}

cbuffer MaterialBuffer : register(b8)
{
    bool textureEnable;
    
    float Shininess;
    float SpecularPower;
    float rgbShiftAmount;
    float4 ambient;
    float4 diffuse;
    float4 specular;
    float4 emissive;
    float4 SkyColor;
    float4 GroundColor;
    float3 GroundNormal;
    float MosaicSize;
    float2 AspectRatio; // �ǉ�
    float2 BlockSize;
    // ���U�C�N�̒��S�ʒu
    // �ۃX�^���C�Y�̒i�K��
};

cbuffer WeightBuffer : register(b9)
{
    float Weight[8];
};

struct PARTICLE {
    float4 Position;
    float4 Acceleration;
    float4 Velocity;
    float4 Parameter;
};
