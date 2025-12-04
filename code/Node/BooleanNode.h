#pragma once
#include "Node.h"
class BooleanNode :
    public Node
{
    bool m_Value = false;
public:
    BooleanNode();

    void Update() override {
        // 何もしない（出力値は固定またはUIで変更）
    }

    void DrawNodeUI() override;

    bool GetValue() const { return m_Value; }
};

