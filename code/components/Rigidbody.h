#pragma once
#include "Component.h"
class Rigidbody :
    public Component
{
    static constexpr float GRAVITY = -9.8f;
    static constexpr float DEFAULT_DRAG = 0.1f;
    bool m_MovableX = true;
    bool m_MovableY = true;
    bool m_MovableZ = true;
    bool m_RotableX = true;
    bool m_RotableY = true;
    bool m_RotableZ = true;

    float m_Mass = 1.0f;
    float m_Drag = 0.1f;
    float m_Gravity = 1.0f;

    float m_Bounce = 1.0f;

    // trueなら物理演算の影響を受けない
    bool m_IsKinematic = false; 

    Vector4O m_Velocity = Vector4O::Zero();

public:
    DECLARE_COMPONENT(Rigidbody)
    void Update() override;

    void DrawGUI() override;

    void ExportComponent(YAML::Emitter& out) override {
        out << YAML::Key << "isKinematic" << YAML::Value << m_IsKinematic;
        out << YAML::Key << "movableX" << YAML::Value << m_MovableX;
        out << YAML::Key << "movableY" << YAML::Value << m_MovableY;
        out << YAML::Key << "movableZ" << YAML::Value << m_MovableZ;
        out << YAML::Key << "rotableX" << YAML::Value << m_RotableX;
        out << YAML::Key << "rotableY" << YAML::Value << m_RotableY;
        out << YAML::Key << "rotableZ" << YAML::Value << m_RotableZ;
        out << YAML::Key << "mass" << YAML::Value << m_Mass;
        out << YAML::Key << "drag" << YAML::Value << m_Drag;
        out << YAML::Key << "gravity" << YAML::Value << m_Gravity;
        out << YAML::Key << "bounce" << YAML::Value << m_Bounce;

        out << YAML::Key << "velocity" << YAML::Value << YAML::Flow << YAML::BeginSeq
            << m_Velocity.x << m_Velocity.y << m_Velocity.z << m_Velocity.w << YAML::EndSeq;
    }

    void OnCollisionEnter(Object* target) override;
    void OnCollisionStay(Object* target) override;
    void OnCollisionExit(Object* target) override;

    void SetVelocity(float velocity) { m_Velocity.y = velocity; }

    void InitializeTag() override {}
};

