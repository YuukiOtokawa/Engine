#pragma once
#include "Node.h"
class StringNode :
    public Node
{
    std::string m_Value;

public:
    StringNode();
    void Update() override {
        // 何もしない（出力値は固定またはUIで変更）
    }
    void DrawNodeUI() override;
};

