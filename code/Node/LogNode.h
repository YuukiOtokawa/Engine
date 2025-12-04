#pragma once

#include "Node.h"

class LogNode : public Node {
    std::string m_Message;

public :
    LogNode();

    void Update() override;

    void DrawNodeUI() override;
};
