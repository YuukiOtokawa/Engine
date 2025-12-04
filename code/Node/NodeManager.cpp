#include "NodeManager.h"
#include <algorithm>
#include "imgui.h"

#include "Node.h"
#include "NodePin.h"

#include "ObjectNode.h"
#include "NumberNode.h"
#include "BooleanNode.h"
#include "StringNode.h"
#include "LogNode.h"
#include "ComparisonNode.h"
#include "IfNode.h"

namespace ed = ax::NodeEditor;

int NodeManager::m_NextLinkId = NODEMANAGER_ID_BASE;

EditorContext* NodeManager::m_EditorContext = nullptr;
bool NodeManager::m_IsCameraAbove = true;
std::map<int, std::vector<NodePin*>> NodeManager::m_PinMap;

void NodeManager::BackgroundContextMenu()
{

    if (ShowBackgroundContextMenu()) {
        m_ContextMenuPos = ScreenToCanvas(ImGui::GetMousePos());
        ImGui::OpenPopup("Background ContextMenu");
    }

    if (ImGui::BeginPopup("Background ContextMenu")) {
        if (ImGui::MenuItem("Add Float Node")) {
            CreateNode<NumberNode>(m_ContextMenuPos);
        }
        if (ImGui::MenuItem("Add Vector4 Node")) {
            CreateNode<Vector4Node>(m_ContextMenuPos);
        }
        if (ImGui::MenuItem("Add Object Node")) {
            CreateNode<ObjectNode>(m_ContextMenuPos);
        }
        if (ImGui::MenuItem("Add Boolean Node")) {
			CreateNode<BooleanNode>(m_ContextMenuPos);
        }
        if (ImGui::MenuItem("Add String Node")) {
			CreateNode<StringNode>(m_ContextMenuPos);
        }
        if (ImGui::MenuItem("Add Log Node")) {
			CreateNode<LogNode>(m_ContextMenuPos);
        }
        if (ImGui::MenuItem("Add Less Than Node")) {
            CreateNode<LessNode>(m_ContextMenuPos);
        }
		if (ImGui::MenuItem("Add Greater Than Node")) {
            CreateNode<GreaterNode>(m_ContextMenuPos);
        }
		if (ImGui::MenuItem("Add Equal To Node")) {
            CreateNode<EqualNode>(m_ContextMenuPos);
        }
        if (ImGui::MenuItem("Add If Node")) {
            CreateNode<IfNode>(m_ContextMenuPos);
		}
        // 他のノードタイプの追加メニューもここに追加可能
        ImGui::EndPopup();
    }
}

void NodeManager::NodeContextMenu()
{
    if (m_ContextNodeID.Get() == 0 && ShowNodeContextMenu(&m_ContextNodeID))
    {
        ImGui::OpenPopup("Node Context Menu");
    }

    if (ImGui::BeginPopup("Node Context Menu"))
    {
        if (ImGui::MenuItem("Delete Node")){
            RemoveNode(m_ContextNodeID);

        }
        ImGui::EndPopup();
    }
    else if (m_ContextNodeID.Get() != 0) {
        m_ContextNodeID = 0;
    }
}

void NodeManager::NodeEditorOptions()
{
    ViewCameraDirectionGizmo();
    EditorViewResetButton();
}

void NodeManager::ChangeNodeEditorViewMode()
{
    m_IsCameraAbove ^= 1;
    for (auto node : m_Nodes) {
        if (!node->m_IsSpatialNode) continue;

        SetNodePosition(
            NodeId(node->GetID()),
            m_IsCameraAbove ? node->GetPositionXZ() : node->GetPositionXY()
        );
    }
}

void NodeManager::ViewCameraDirectionGizmo()
{
    ImDrawList* drawList = ImGui::GetForegroundDrawList();

//==========================================================================
// ギズモのパラメータ設定
//==========================================================================

    float size = 80.0f;     // ギズモのサイズ
    ImVec2 padding = ImVec2(10.0f,30.0f);

    auto windowPos = ImGui::GetWindowPos();
    auto windowSize = ImGui::GetWindowSize();

	// ギズモを配置する位置 = エディター領域の右上
    ImVec2 basePos = ImVec2(
        windowPos.x + windowSize.x - size - padding.x,
        windowPos.y + padding.y
    );

	auto center = ImVec2(basePos.x + size * 0.5f, basePos.y + size * 0.5f);
	float axisLen = size * 0.35f; // 描画する軸の長さ

    ImGui::SetCursorScreenPos(basePos);

    bool clicked = ImGui::InvisibleButton("GizmoButton", ImVec2(size, size));
    bool isHovered = ImGui::IsItemHovered();

	// 軸の色設定
	ImU32 colorX = IM_COL32(255, 0, 0, 255);   // X軸 - 赤
	ImU32 colorY = IM_COL32(0, 255, 0, 255);   // Y軸 - 緑
	ImU32 colorZ = IM_COL32(0, 0, 255, 255);   // Z軸 - 青

    // テキスト表示
	ImU32 textColor = IM_COL32(255, 255, 255, 255);
    // 背景
	ImU32 bgColor = isHovered ? IM_COL32(60,60,70,225) : IM_COL32(30, 30, 30, 200);  // 半透明の黒

//==========================================================================
// ギズモ描画
//==========================================================================

    // ギズモの背景四角形を描画
    drawList->AddRectFilled(basePos, ImVec2(basePos.x + size, basePos.y + size), bgColor, 5.0f);
    
	// X軸 常に右を向いているし常に映る
    drawList->AddLine(center, ImVec2(center.x + axisLen, center.y), colorX, 3.0f);
    drawList->AddText(ImVec2(center.x + axisLen + 2, center.y - 7), textColor, "X");

    if (m_IsCameraAbove) {
        // 上から見た座標軸 => Z軸が上方向に見える
        drawList->AddLine(center, ImVec2(center.x, center.y - axisLen), colorZ, 3.0f);
        drawList->AddText(ImVec2(center.x - 4, center.y - axisLen - 2), textColor, "Z");
    
    }
    else {
        // 手前から見た座標軸 => Y軸が上方向に見える
        drawList->AddLine(center, ImVec2(center.x, center.y - axisLen), colorY, 3.0f);
        drawList->AddText(ImVec2(center.x - 4, center.y - axisLen - 2), textColor, "Y");
    }
	

    if (clicked)
        ChangeNodeEditorViewMode();
}

void NodeManager::EditorViewResetButton()
{


    ImDrawList* drawList = ImGui::GetForegroundDrawList();

//==========================================================================
// ボタンのパラメータ設定
//==========================================================================

    float width = 80.0f;
    float height = 30.0f;
    ImVec2 padding = ImVec2(10.0f, 30.0f);
    float gizmoSize = 80.0f;
    float spacing = 10.0f;

    auto windowPos = ImGui::GetWindowPos();
    auto windowSize = ImGui::GetWindowSize();

    // ギズモの下に配置
    ImVec2 basePos = ImVec2(
        windowPos.x + windowSize.x - width - padding.x,
        windowPos.y + padding.y + gizmoSize + spacing
    );

    ImGui::SetCursorScreenPos(basePos);

    bool clicked = ImGui::InvisibleButton("ResetViewButton", ImVec2(width, height));
    bool isHovered = ImGui::IsItemHovered();

    // 色設定
    ImU32 bgColor = isHovered ? IM_COL32(70, 70, 80, 225) : IM_COL32(40, 40, 50, 200);
    ImU32 textColor = IM_COL32(255, 255, 255, 255);

//==========================================================================
// ボタン描画
//==========================================================================

    // 背景
    drawList->AddRectFilled(basePos, ImVec2(basePos.x + width, basePos.y + height), bgColor, 5.0f);
    
    // テキスト
    const char* text = "Reset";
    ImVec2 textSize = ImGui::CalcTextSize(text);
    ImVec2 textPos = ImVec2(
        basePos.x + (width - textSize.x) * 0.5f,
        basePos.y + (height - textSize.y) * 0.5f
    );
    drawList->AddText(textPos, textColor, text);

    // クリック時の処理
    if (clicked) {
        // ウィンドウの左上の位置をスクリーン座標からキャンバス座標に変換
        ImVec2 targetPos = ScreenToCanvas(windowPos);
        // 原点(0, 0)をウィンドウの左上に配置するため、反転した座標に移動
        
        NavigateToSelection();
    }
}

NodeManager::NodeManager() {
    m_EditorContext = nullptr;
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
        BeginNode(NodeId(node->GetID()));
            node->DrawNodeName();

            ImGui::BeginGroup();
            // 入力ピン列
            if (!node->GetInputs().empty()) {
                ImGui::BeginGroup();
                for (auto& pin : node->GetInputs()) {
                    BeginPin(pin->GetID(), PinKind::Input);
                    ImGui::Text("<-");
                    EndPin();
                    ImGui::SameLine();
                    ImGui::Text("%s", pin->GetName().c_str());
                }
                ImGui::EndGroup();
                ImGui::SameLine();
            }
            
            // ノードコンテンツ
            ImGui::BeginGroup();
            node->DrawNodeUI();
            ImGui::EndGroup();
            
            // 出力ピン列
            if (!node->GetOutputs().empty()) {
                ImGui::SameLine();
                ImGui::BeginGroup();
                for (auto& pin : node->GetOutputs()) {
                    // ピン名の幅を計算
                    float textWidth = ImGui::CalcTextSize(pin->GetName().c_str()).x;
                    float arrowWidth = ImGui::CalcTextSize("->").x;
                    float groupWidth = textWidth + arrowWidth + ImGui::GetStyle().ItemSpacing.x;
                    
                    // グループの最大幅を取得するために、すべてのピンの幅を計算
                    float maxWidth = 0.0f;
                    for (auto& p : node->GetOutputs()) {
                        float w = ImGui::CalcTextSize(p->GetName().c_str()).x + arrowWidth + ImGui::GetStyle().ItemSpacing.x;
                        if (w > maxWidth) maxWidth = w;
                    }
                    
                    // 右揃えのためにカーソル位置を調整
                    float offset = maxWidth - groupWidth;
                    ImGui::Dummy(ImVec2(offset, 0.0f));
                    ImGui::SameLine(0.0f, 0.0f);
                    
                    ImGui::Text("%s", pin->GetName().c_str());
                    ImGui::SameLine();
                    BeginPin(pin->GetID(), PinKind::Output);
                    ImGui::Text("->");
                    EndPin();
                }
                ImGui::EndGroup();
            }
            ImGui::EndGroup();
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
                if (CheckPinKind(inputPinId.Get(), outputPinId.Get())) {
                    if (AcceptNewItem()) {
                        AddLink(inputPinId.Get(), outputPinId.Get());
                    }
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

    NodeContextMenu();
    NodeEditorOptions();

    Resume();


    End();

    for (auto node : m_Nodes) {
        if (m_IsCameraAbove) {
            node->SetPositionXZ(GetNodePosition(NodeId(node->GetID())));
        }
        else {
            node->SetPositionXY(GetNodePosition(NodeId(node->GetID())));
		}
    }

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

void NodeManager::RemoveNode(NodeId id) {
    for (auto node = m_Nodes.begin(); node != m_Nodes.end(); ++node) {
        if ((*node)->m_ID == id.Get()) {
            
            // TODO [otokawa]:ピンとかリンクの情報もここで消去

            // ノードに属するピンを消去
            m_PinMap.erase((*node)->m_ID);


            delete* node;
            m_Nodes.erase(node);
            break;
        }
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

bool NodeManager::CheckPinKind(int inputPinId, int outputPinId)
{
    PinKind in, out;
    for (auto pair : m_PinMap) {
        for (auto pin : pair.second) {
            if (pin->GetID() == inputPinId) {
                in = pin->GetKind();
                goto INPUT_FINDED;
            }
        }
    }
    return false; //そもそもピンがない

INPUT_FINDED:
    for (auto pair : m_PinMap) {
        for (auto pin : pair.second) {
            if (pin->GetID() == outputPinId) {
                if (in != pin->GetKind())
                    return true;    // PinKindが不一致 => 接続してよい
                else
                    return false;   // PinKindが一致 => 接続できない
            }
        }
    }
    return false; //そもそもピンがない
}

// 実際の値の伝播ロジックはより複雑になるため、ここでは省略
float NodeManager::GetPinValue(int pinId) {
    // リンクを辿って出力ピンの値を取得するロジック
    return 0.0f; 
}

void NodeManager::StartEditorContext()
{
    SetCurrentEditor(m_EditorContext);
}

void NodeManager::EndEditorContext()
{
    SetCurrentEditor(nullptr);
}

void NodeManager::Finalize() {
    // 現在のところ特別なFinalize処理は不要
}

