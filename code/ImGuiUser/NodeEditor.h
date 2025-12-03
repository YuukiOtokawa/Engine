
#pragma once

#include "imgui_node_editor.h"
#include <vector>
#include <map>
#include <string>

using namespace ax::NodeEditor;

enum class NodeType;

enum class PinType {
    Boolean,
    Integer,
    Float,
    String
};

// 前方宣言
class NodeEditor;
struct NodeInfo;
struct IfNode;

struct LinkInfo
{
    int id;
    int inputPinId;
    int outputPinId;
};

struct PinInfo {
    int id;
    PinKind kind;

};

class NodeEditor
{
    static constexpr auto NodeEditor_ID_Base = 10000;
    EditorContext* m_pContext = nullptr;

    int m_WidgetIDCounter = NodeEditor_ID_Base;

    // ノードとリンクのリスト
    std::vector<NodeInfo> m_Nodes;
    std::vector<LinkInfo> m_Links;

    // 次のフレームで位置を設定するノード
    int m_NodeToSetPosition = -1;
    ImVec2 m_NewNodePosition;

    bool m_NodeEditorVisible = true;


    NodeId m_ContextNodeID = 0;

    std::vector<std::string> m_NodeNames;

    void DrawNode(NodeInfo& node);

    void RightClickNode();

    void CreateNode(ImVec2 position);
    void DeleteNode(NodeId nodeID);

    void CreateLink();
    void DeleteLink();

public:
    void Initialize();

    void Finalize();

    void Render();

    void SetVisible() { m_NodeEditorVisible = true; }

    int CountWidgetID() {
        return m_WidgetIDCounter++;
    }
};

// NodeEditorクラスの定義の後に配置
struct NodeInfo
{
    std::string name;
    int id;
    NodeType type;
    std::map<PinKind, std::vector<int>> pins;

public:
    virtual void CreatePins(NodeEditor* pEditor);
};

struct IfNode : public NodeInfo
{
    void CreatePins(NodeEditor* pEditor) override {
        name = "if";
        pins[PinKind::Input].push_back(pEditor->CountWidgetID());  // Condition
        pins[PinKind::Input].push_back(pEditor->CountWidgetID());  // Condition
        pins[PinKind::Output].push_back(pEditor->CountWidgetID());  // Condition
    }
};

