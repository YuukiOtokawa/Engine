#pragma once

#include "GUIWindow.h"
#include "imgui.h"
#include "ImGuizmo.h"

class Camera;
class Object;

class EditorViewWindow : public ViewWindow {
    ImGuizmo::OPERATION m_GizmoOperation = ImGuizmo::OPERATION::TRANSLATE;

    Vector4O m_CameraDirection = Vector4O::Zero();
    Vector4O m_CameraRotation = Vector4O::Zero();

public:
    void Render() override;
    void CalcMatrix();
    void DrawGizmo();
};
