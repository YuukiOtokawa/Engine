#pragma once
#include "Component.h"

#include <vector>

#include "VectorO.h"

enum LightType
{
    LIGHTTYPE_DIRECTIONAL,
    LIGHTTYPE_POINT,
    LIGHTTYPE_SPOT
};

struct DIRECTIONAL_LIGHT
{
    BOOL enable = TRUE; // ライトの有効/無効
    BOOL Dummy[3] = {}; // パディング用のダミー変数
    Vector4O diffuse;   // 拡散光の色
    Vector4O direction; // 光の方向
};

struct POINT_LIGHT
{
    BOOL enable = TRUE; // ライトの有効/無効
    BOOL Dummy[2] = {}; // パディング用のダミー変数
    FLOAT range;        // 光の範囲
    Vector4O diffuse;   // 拡散光の色
    Vector4O position;  // 光源の位置
};

struct SPOT_LIGHT
{
    BOOL enable = true; // ライトの有効/無効
    FLOAT range;        // 光の範囲
    FLOAT innerAngle;   // 内部角度
    FLOAT outerAngle;   // 外部角度
    Vector4O diffuse;   // 拡散光の色
    Vector4O position;  // 光源の位置
    Vector4O direction; // 光の方向

};

struct LIGHT_BUFFER {
    DIRECTIONAL_LIGHT directionalLight;
    POINT_LIGHT pointLight;
    SPOT_LIGHT spotLight;
    Vector4O ambient = Vector4O(0.2f, 0.2f, 0.2f, 1.0f); // 環境光の色
};

class Light :
    public Component
{
private:
    LightType m_Type = LIGHTTYPE_DIRECTIONAL; // 光の種類
    bool m_isEnable = true; // ライトの有効/無効
    Vector4O m_Direction = Vector4O(0.0f, -1.0f, 0.0f, 0.0f); // ライトの方向
    static Vector4O m_Ambient; // 環境光の色
    Vector4O m_Diffuse = Vector4O(1.0f, 1.0f, 1.0f, 1.0f); // 拡散光の色
    Vector4O m_Specular = Vector4O(1.0f, 1.0f, 1.0f, 1.0f); // 鏡面光の色
    float m_Range = 20.0f; // 光の範囲
    float m_InnerAngle = 0.7f; // スポットライトの内部角度
    float m_OuterAngle = 1.0f; // スポットライトの外部角度
    
    static DIRECTIONAL_LIGHT m_DirectionalLight; // 複数の方向性ライトを保持するためのベクター
    static POINT_LIGHT m_PointLight; // 複数の点光源を保持するためのベクター
    static SPOT_LIGHT m_SpotLight; // 複数のスポットライトを保持するためのベクター
    

public:
    DECLARE_COMPONENT(Light)
    Light() = default;

    void InitializeTag() override;
    void Draw() override;
    static void DrawGeneralLight();
    void DrawGUI() override;

    void Update() override {
        // ライトの更新処理が必要な場合はここに記述
    }

    void ExportComponent() override;

    void SetType(LightType newType) {
        m_Type = newType;
    }

    void SetEnable(bool enable) {
        m_isEnable = enable;
    }
    void SetDirection(Vector4O direction) {
        m_Direction = direction;
    }
    void SetDiffuse(Vector4O diffuse) {
        m_Diffuse = diffuse;
    }
    void SetSpecular(Vector4O specular) {
        m_Specular = specular;
    }
    void SetRange(float range) {
        m_Range = range;
    }
    void SetInnerAngle(float innerAngle) {
        m_InnerAngle = innerAngle;
    }
    void SetOuterAngle(float outerAngle) {
        m_OuterAngle = outerAngle;
    }
    static void SetAmbient(Vector4O ambient) {
        m_Ambient = ambient;
    }
    
};

