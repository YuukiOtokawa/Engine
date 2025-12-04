#include "ComparisonNode.h"

#include "NodePin.h"

//==========================================================================
// ”äŠr‰‰ŽZŽq< 
//==========================================================================

LessNode::LessNode() : Node("Less <")
{
	AddInput("A", PinType::Float);
	AddInput("B", PinType::Float);
	AddOutput("Result", PinType::Bool);
}

void LessNode::Update()
{
}

void LessNode::DrawNodeUI()
{
}

//==========================================================================
// ”äŠr‰‰ŽZŽq>
//==========================================================================

GreaterNode::GreaterNode() : Node("Greater >")
{
	AddInput("A", PinType::Float);
	AddInput("B", PinType::Float);
	AddOutput("Result", PinType::Bool);
}

void GreaterNode::Update()
{
}

void GreaterNode::DrawNodeUI()
{
}

//==========================================================================
// ”äŠr‰‰ŽZŽq=
//==========================================================================

EqualNode::EqualNode() : Node("Equal =")
{
	AddInput("A", PinType::Float);
	AddInput("B", PinType::Float);
	AddOutput("Result", PinType::Bool);
}

void EqualNode::Update()
{
}

void EqualNode::DrawNodeUI()
{
}
