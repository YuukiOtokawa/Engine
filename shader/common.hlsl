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
    float4 Position : POSITION0;
    float4 Normal : NORMAL0;
    float4 Diffuse : COLOR0;
    float2 TexCoord : TEXCOORD0;
    
};

struct PS_IN
{
    float4 Position : SV_POSITION;
    float4 WorldPosition : POSITION0;
    float4 Normal : NORMAL0;
    float4 Diffuse : COLOR0;
    float2 TexCoord : TEXCOORD0;
    
};

struct LIGHT
{
    bool Enable;
    bool2 Dummy;
    float SpecularPower;
    float4 Direction;
    float4 Diffuse;
    float4 Ambient;
    
    float4 SkyColor;
    float4 GroundColor;
    float4 GroundNormal;
    
    float4 Position;
    float4 PointLightRange;
   
    float4 SpotLightAngle;
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
    float Dummy;
    float4 ambient;
    float4 diffuse;
    float4 specular;
    float4 emissive;
    float4 SkyColor;
    float4 GroundColor;
    float4 GroundNormal;
};