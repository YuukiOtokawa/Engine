#include "IfNode.h"
#include "NodePin.h"

IfNode::IfNode() : Node("If")
{
	AddInput("Condition", PinType::Bool);
	AddOutput("True", PinType::Bool);
	AddOutput("False", PinType::Bool);
}

void IfNode::Update()
{
}

void IfNode::DrawNodeUI()
{
}
