#pragma once
#include "Collider.h"
class MeshFieldCollider :
    public Collider
{
public:
    DECLARE_COMPONENT(MeshFieldCollider)
    MeshFieldCollider() {
        m_ColliderType = ColliderParameter::ColliderType::MeshField;
    }
    float GetHeight(Vector3O position);
};

