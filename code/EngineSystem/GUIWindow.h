#pragma once

#include "FrameWork.h"

#include <string>

#include "imgui.h"

class GUIWindow {
protected:
    bool m_IsOpen = true;
    
public:
    virtual void Render() = 0;

    void OpenWindow() { m_IsOpen = true; }

    void StartWindow(std::string windowName, ImGuiWindowFlags flag = ImGuiWindowFlags_None);
};

class ViewWindow : public GUIWindow {

protected:
    XMMATRIX m_ProjectionMatrix;
    XMMATRIX m_ViewMatrix;

    bool m_IsHovered;
public:

    void Render() override = 0;

};
