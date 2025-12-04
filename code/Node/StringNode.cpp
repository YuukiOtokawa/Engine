#include "StringNode.h"

#include "NodePin.h"

StringNode::StringNode() : Node("String")
{
	AddOutput("Value", PinType::String);
}

void StringNode::DrawNodeUI()
{
	ImGui::SetNextItemWidth((std::max)(50.0f, ImGui::CalcTextSize(m_Value.c_str()).x + 20.0f));

	ImGui::PushID((void*)&m_Value);
	InputTextString("##value", &m_Value);
	ImGui::PopID();
}
