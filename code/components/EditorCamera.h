// ========================================================
//
// 繧ｨ繝�繧｣繧ｿ繝ｼ繧ｫ繝｡繝ｩ繧ｳ繝ｳ繝昴�ｼ繝阪Φ繝医け繝ｩ繧ｹ[EditorCamera.h]
// 
//									Date:20250602
//									Author:Yuuki Otokawa
// ========================================================

#pragma once

#include "Component_Camera.h"

class EditorCamera :
    public Camera
{
private:
    XMINT2 m_OriginalCursorPosition = { 0, 0 };
    POINT m_OriginalScreenPosition = { 0, 0 };
public:
    DECLARE_COMPONENT(EditorCamera)
    EditorCamera() : Camera() {
        m_ClassID = CID_Component_EditorCamera;
    }
    void Update() override;

    void ExportComponent(YAML::Emitter& out) override {
        out << YAML::Key << "isActiveCamera" << YAML::Value << m_IsActiveCamera;

        out << YAML::Key << "target" << YAML::Value << YAML::Flow << YAML::BeginSeq
            << m_Target.x << m_Target.y << m_Target.z << m_Target.w << YAML::EndSeq;

        out << YAML::Key << "fov" << YAML::Value << m_Fov;
        out << YAML::Key << "near" << YAML::Value << m_Near;
        out << YAML::Key << "far" << YAML::Value << m_Far;

        out << YAML::Key << "up" << YAML::Value << YAML::Flow << YAML::BeginSeq
            << m_Up.x << m_Up.y << m_Up.z << m_Up.w << YAML::EndSeq;
    }

    void ImportFile(YAML::Node& node) override;

};

