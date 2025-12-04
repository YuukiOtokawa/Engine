#pragma once

#include "Node.h"

#include "VectorO.h"

class NumberNode : public Node {
    float m_Value = 0.0f;
public:
    NumberNode();

    void Update() override {
        // 何もしない（出力値は固定またはUIで変更）
    }

    void DrawNodeUI() override;

    float GetValue() const { return m_Value; }
};

class Vector4Node : public Node {
    Vector4O m_Value;

public:
    Vector4Node();

    void Update() override;
    void DrawNodeUI() override;

    Vector4O GetValue() const { return m_Value; }
};
