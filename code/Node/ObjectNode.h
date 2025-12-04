#pragma once

#include "Node.h"

#include "VectorO.h"

class Object;
class ObjectNode : public Node {
    Object* m_pObject = nullptr;

public:
    ObjectNode();

    ~ObjectNode() override;

    void Update() override;

    void DrawNodeUI() override;
    void DrawNodeName() override;

    void SetPosition(Vector4O position);
};

