#pragma once

enum class NodeType
{
    Input,
    Output,
    InOut,
};

// NodeEditorに表示されるノードクラス
class EditorNode
{
    int nodeID;
    NodeType type;
};

class RuntimeNode
{
    int nodeID;
};


class Node {
    int nodeID;
    NodeType type;
};



