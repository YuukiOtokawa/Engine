#include "NumberNode.h"

#include "NodePin.h"
#include "NodeManager.h"

NumberNode::NumberNode() : Node("Number") {
    AddOutput("Value", PinType::Float);
}

void NumberNode::DrawNodeUI() {
    ImGui::PushItemWidth(80);
    ImGui::PushID((void*)&m_Value);
    ImGui::DragFloat("##value", &m_Value, 0.1f);
    ImGui::PopID();
    ImGui::PopItemWidth();
}

Vector4Node::Vector4Node() : Node("Vector4")
{
    AddOutput("X", PinType::Float);
	AddOutput("Y", PinType::Float);
	AddOutput("Z", PinType::Float);
	AddOutput("W", PinType::Float);
}

void Vector4Node::Update()
{
}

void Vector4Node::DrawNodeUI()
{
    ImGui::PushItemWidth(80);
    ImGui::PushID((void*)&m_Value.x);
    ImGui::DragFloat("X##value", &m_Value.x, 0.1f);
    ImGui::PopID();
    ImGui::PushID((void*)&m_Value.y);
    ImGui::DragFloat("Y##value", &m_Value.y, 0.1f);
    ImGui::PopID();
    ImGui::PushID((void*)&m_Value.z);
    ImGui::DragFloat("Z##value", &m_Value.z, 0.1f);
    ImGui::PopID();
    ImGui::PushID((void*)&m_Value.w);
    ImGui::DragFloat("W##value", &m_Value.w, 0.1f);
    ImGui::PopID();
    ImGui::PopItemWidth();
}
