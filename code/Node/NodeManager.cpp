#include "NodeManager.h"
#include <algorithm>
#include "imgui.h"

#include "Node.h"
#include "NodePin.h"

namespace ed = ax::NodeEditor;

int NodeManager::m_NextLinkId = NODEMANAGER_ID_BASE;

void NodeManager::BackgroundContextMenu()
{

    if (ShowBackgroundContextMenu()) {
        m_ContextMenuPos = ScreenToCanvas(ImGui::GetMousePos());
        ImGui::OpenPopup("Background ContextMenu");
    }

    if (ImGui::BeginPopup("Background ContextMenu")) {
        if (ImGui::MenuItem("Add Float Node")) {
            CreateNode<FloatNode>(m_ContextMenuPos);
        }
		if (ImGui::MenuItem("Add Object Node")) {
            CreateNode<ObjectNode>(m_ContextMenuPos);
        }
        // 他のノードタイプの追加メニューもここに追加可能
        ImGui::EndPopup();
    }
}

NodeManager::NodeManager() : m_EditorContext(nullptr) {
}

NodeManager::~NodeManager() {
    if (m_EditorContext) {
        DestroyEditor(m_EditorContext);
    }
    for (Node* node : m_Nodes) {
        delete node;
    }
    m_Nodes.clear();
}

void NodeManager::Initialize() {
    Config config;
    config.SettingsFile = "NodeEditor.json"; // 設定ファイルを保存するパス
    m_EditorContext = ed::CreateEditor(&config);
}

void NodeManager::Update() {
    // 全てのノードのUpdateメソッドを呼び出し、ゲームロジックを更新
    for (Node* node : m_Nodes) {
        node->Update();
    }
    // ここでピン間の値の伝播ロジックを実装する
    // 例:
    // for (const auto& link : m_Links) {
    //     NodePin* startPin = nullptr; // NodeManager::GetPin(link.StartPinID);
    //     NodePin* endPin = nullptr;   // NodeManager::GetPin(link.EndPinID);
    //     if (startPin && endPin && startPin->Node && endPin->Node) {
    //         // ここで startPin のノードから値を取得し、endPin のノードに渡す
    //         // この処理は Node::Update 内に任せるか、NodeManagerが集中管理するかは設計による
    //     }
    // }

}

void NodeManager::Draw() {
    if (!m_EditorContext) return;

    SetCurrentEditor(m_EditorContext);

    if (m_NodeToSetPosition != -1) {
        SetNodePosition(m_NodeToSetPosition, m_NewNodePosition);
        m_NodeToSetPosition = -1;
    }

    
    ImGui::Begin("Node Editor");
    Begin("Node Editor", ImVec2(0.0, 0.0f));

    // ノードの描画
    for (Node* node : m_Nodes) {
        BeginNode(node->GetID());
            node->DrawNodeUI(); // ノードのコンテンツを描画
            // 各ピンの描画
            for (auto& pin : node->GetInputs()) {
                BeginPin(pin->GetID(), PinKind::Input);
                ImGui::Text("<- %s", pin->GetName().c_str()); // 仮の表示
                EndPin();
            }
            for (auto& pin : node->GetOutputs()) {
                BeginPin(pin->GetID(), PinKind::Output);
                ImGui::Text("%s ->", pin->GetName().c_str()); // 仮の表示
                EndPin();
            }
        EndNode();
    }

    // リンクの描画
    for (const auto& link : m_Links) {
        Link(link->GetID(), link->GetOutputPinID(), link->GetInputPinID());
    }

    // 新しいリンクの処理
    if (BeginCreate()) {
        PinId inputPinId, outputPinId;
        if (QueryNewLink(&inputPinId, &outputPinId)) {
            if (inputPinId && outputPinId) {
                // PinKindが正しいか確認 (Input -> Output または Output -> Input)
                // この辺りのロジックは imgui-node-editor のサンプルを参考に実装
                if (AcceptNewItem()) {
                    AddLink(inputPinId.Get(), outputPinId.Get());
                }
            }
        }
    }
    EndCreate();

    // リンク削除の処理
    if (BeginDelete()) {
        LinkId linkId;
        while (QueryDeletedLink(&linkId)) {
            if (AcceptDeletedItem()) {
                RemoveLink(linkId.Get());
            }
        }
    }
    EndDelete();

//==========================================================================
// コンテキストメニュー処理
//==========================================================================

    Suspend();

    // ノードエディターのコンテキストメニュー
    BackgroundContextMenu();

    Resume();


    End();
    ImGui::End();

    SetCurrentEditor(nullptr);
}

void NodeManager::AddNode(Node* node) {
    if (node) {
        m_Nodes.push_back(node);
        // ノードが作成されたら imgui-node-editor に登録
        //SetNodePosition(node->GetID(), ImVec2(0, 0)); // 初期位置設定
    }
}

void NodeManager::RemoveNode(Node* node) {
    if (node) {
        m_Nodes.erase(std::remove(m_Nodes.begin(), m_Nodes.end(), node), m_Nodes.end());
        delete node; // メモリ解放
    }
}

void NodeManager::AddLink(int startPinId, int endPinId) {
    m_Links.emplace_back(new NodePinLink(m_NextLinkId++, startPinId, endPinId));
}

void NodeManager::RemoveLink(int linkId) {
    m_Links.erase(std::remove_if(m_Links.begin(), m_Links.end(),
        [linkId](const NodePinLink* link) { return link->GetID() == linkId; }),
        m_Links.end());
}

// 実際の値の伝播ロジックはより複雑になるため、ここでは省略
float NodeManager::GetPinValue(int pinId) {
    // リンクを辿って出力ピンの値を取得するロジック
    return 0.0f; 
}

void NodeManager::Finalize() {
    // 現在のところ特別なFinalize処理は不要
}

