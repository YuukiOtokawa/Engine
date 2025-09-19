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
    EditorCamera() : Camera() {
        m_ClassID = CID_Component_EditorCamera;
    }
    void UpdateComponent() override;

    void ExportComponent() override {
        CSVExporter::ExportFloat(m_IsActiveCamera);
        CSVExporter::ExportVector4O(m_Target);
        CSVExporter::ExportFloat(m_Fov);
        CSVExporter::ExportFloat(m_Near);
        CSVExporter::ExportFloat(m_Far);
        CSVExporter::ExportVector4O(m_Up);
    }

    void ImportFile(std::vector<std::string>& tokens) override;

};

