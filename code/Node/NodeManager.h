#pragma once

#include <vector>
#include <map>
#include <string>
#include "imgui_node_editor.h"

using namespace ax::NodeEditor;

class Node;
class NodePinLink;

class NodeManager {
private:
    EditorContext* m_EditorContext = nullptr;
    std::vector<Node*> m_Nodes;
    std::vector<NodePinLink*> m_Links;

    static int m_NextLinkId; // リンクIDの初期値
    static constexpr auto NODEMANAGER_ID_BASE = 10000; // NodeManager用IDの基点

    int m_NodeToSetPosition = -1;
    ImVec2 m_NewNodePosition;

    bool m_NodeEditorVisible = true;

    ImVec2 m_ContextMenuPos;

    void BackgroundContextMenu();

    template<typename T>
    void CreateNode(const ImVec2& position);

public:
    NodeManager();
    ~NodeManager();

    void Initialize();
    void Update(); // ノードのゲームロジック更新
    void Draw();   // ノードエディタのGUI描画
    void Finalize(); // NodeManagerのFinalizeメソッドを追加

    // ノードの追加と削除
    void AddNode(Node* node);
    void RemoveNode(Node* node);

    // リンクの追加と削除
    void AddLink(int startPinId, int endPinId);
    void RemoveLink(int linkId);

    // 特定のピンから値を取得するヘルパー（NodeManagerがリンク解決の責務を持つ場合）
    // 後で実装
    float GetPinValue(int pinId);

    static int CountWidgetID() {
        return m_NextLinkId++;
    }
};

template<typename T>
inline void NodeManager::CreateNode(const ImVec2& position)
{
    if (!std::is_base_of<Node, T>::value) {
        return; // TがNodeの派生クラスでない場合は何もしない
    }
    T* newNode = new T();
    AddNode(newNode);
    m_NodeToSetPosition = newNode->GetID();
    m_NewNodePosition = position;
}
