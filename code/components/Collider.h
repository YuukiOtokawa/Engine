#pragma once
#include "Component.h"

namespace ColliderParameter {
    enum class ColliderType {
        Box,
        Sphere,
        MeshField,
        COLLIDER_TYPE_MAX
    };
}

class Collider :
    public Component
{
    std::list<Object*> m_CollisionObjects;
protected:

    Vector3O m_CollisionSize = Vector3O::One();

    ColliderParameter::ColliderType m_ColliderType = ColliderParameter::ColliderType::Box;
    friend class CollisionManager;
public:
    DECLARE_COMPONENT(Collider)
    Collider() {
        m_ClassID = CID_Component_Collider;
    }

    void Start() override;
    void OnDestroy() override;

    void Update() override;
    void DrawGUI() override;
    void InitializeTag() override {}

    void ExportComponent(YAML::Emitter& out) override {
        out << YAML::Key << "collisionSize" << YAML::Value << YAML::Flow << YAML::BeginSeq
            << m_CollisionSize.x << m_CollisionSize.y << m_CollisionSize.z << YAML::EndSeq;
        out << YAML::Key << "colliderType" << YAML::Value << static_cast<int>(m_ColliderType);
    }

    Vector3O GetCollisionSize() const;

    void OnCollisionEnter(Object* target) override;
    void OnCollisionStay(Object* target) override;
    void OnCollisionExit(Object* target) override;

    Vector3O GetForward();
    Vector3O GetRight();

};

