#pragma once
#include "Component.h"
class Collider :
    public Component
{
    std::list<Object*> m_CollisionObjects;
public:
    DECLARE_COMPONENT(Collider)
    Collider() {
        m_ClassID = CID_Component_Collider;
    }



    void Start() override;
    void OnDestroy() override;

    void UpdateComponent() override;
    void Draw() override {}
    void DrawGUI() override;
    void InitializeTag() override {}

    void OnCollisionEnter(Object* target) override;
    void OnCollisionStay(Object* target) override;
    void OnCollisionExit(Object* target) override;

};

