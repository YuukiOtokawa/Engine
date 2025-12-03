#pragma once

#include <memory>
#include <vector>

class NodePin;
class EditorPinLink;

enum class NodeType
{
    Input,
    Output,
    InOut,
};

// NodeEditorに表示されるノードクラス
class EditorNode
{
    std::string name;
    std::vector<NodePin*> inputPins;
    std::vector<NodePin*> outputPins;

    std::vector<EditorPinLink*> links;
};


class Node {
    int nodeID;
    NodeType type;
    std::unique_ptr<EditorNode> pEditorNode;

    // 入力リンク
    // 出力リンク

public:
    virtual void Update() = 0;
};



