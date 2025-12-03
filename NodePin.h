#pragma once

#include "imgui_node_editor.h"
using namespace ax::NodeEditor;

// imgui表示用ノードピンクラス
class NodePin
{
    int pinID;
    PinKind kind;
};

// imgui表示用ノードピンリンククラス
class NodePinLink {
    int linkID;
    int inputPinId;
    int outputPinId;

public:
    double GetValue() const;
};

class RuntimeNodeLink {
    int linkID;
    double value;
};
