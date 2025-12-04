#pragma once

#include "Node.h"

class IfNode : public Node {
public:
    IfNode();
    void Update() override;
    void DrawNodeUI() override;
};
