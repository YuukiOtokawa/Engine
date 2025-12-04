#include "BooleanNode.h"

#include "NodePin.h"
#include "NodeManager.h"

BooleanNode::BooleanNode() : Node("Boolean")
{
	AddOutput("Value", PinType::Bool);
}

void BooleanNode::DrawNodeUI()
{
    ImGui::PushItemWidth(80);
    ImGui::PushID((void*)&m_Value);
    ImGui::Checkbox("##value", &m_Value);
    ImGui::PopID();
    ImGui::PopItemWidth();

}
