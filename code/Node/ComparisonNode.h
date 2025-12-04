#pragma once

#include "Node.h"

//==========================================================================
// ”äŠr‰‰ŽZŽq< 
//==========================================================================

class LessNode : public Node {
public:
    LessNode();
    void Update() override;
    void DrawNodeUI() override;
};

//==========================================================================
// ”äŠr‰‰ŽZŽq>
//==========================================================================

class GreaterNode : public Node {
public:
    GreaterNode();
    void Update() override;
    void DrawNodeUI() override;
};

//==========================================================================
// ”äŠr‰‰ŽZŽq=
//==========================================================================

class EqualNode : public Node {
public:
    EqualNode();
    void Update() override;
    void DrawNodeUI() override;
};
