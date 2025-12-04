#include "Node.h"
#include "NodePin.h"
#include "NodeManager.h"

#define NOMINMAX

Node::Node(const char* name) : m_ID(NodeManager::CountWidgetID()), m_Name(name), m_Size(0, 0) {}

NodePin* Node::GetPin(int id) {
    for (auto& pin : m_Inputs) if (pin->GetID() == id) return pin;
    for (auto& pin : m_Outputs) if (pin->GetID() == id) return pin;
    return nullptr;
}

void Node::DrawNodeUI() {
    // デフォルトの描画実装（必要に応じてオーバーライド）
    // ここでは単純にタイトルとピンを描画する想定
    // 実際の描画はNodeManager側でax::NodeEditor::BeginNodeなどを呼んで行うのが一般的だが
    // コンテンツ部分だけここで行う形にする
    ImGui::Text("%s", m_Name.c_str());
}

void Node::AddInput(const char* name, PinType type) {
    auto newPin = new NodePin(NodeManager::CountWidgetID(), name, type, PinKind::Input);
    m_Inputs.emplace_back(newPin);
    m_Inputs.back()->SetNode(this);
    NodeManager::AddPin(m_ID, newPin);
}

void Node::AddOutput(const char* name, PinType type) {
    auto newPin = new NodePin(NodeManager::CountWidgetID(), name, type, PinKind::Output);
    m_Outputs.emplace_back(newPin);
    m_Outputs.back()->SetNode(this);
    NodeManager::AddPin(m_ID, newPin);
}

//==========================================================================
// ObjectNodeクラスの実装(仮組)
//==========================================================================

#include "Object.h"
#include "Component_Transform.h"
#include "Editor.h"

ObjectNode::ObjectNode() : Node("Object") {
    // ゲームオブジェクトを生成して管理
	// TODO [otokawa]:これはObject生成の仮実装。

    m_pObject = new Object();

    m_pObject->SetName("Object");
    m_Name = m_pObject->GetName();
    m_pObject->AddComponent<Transform>();

    m_IsSpatialNode = true;

    // 入力ピン: 位置情報を受け取る
    AddInput("Pos X", PinType::Float);
    AddInput("Pos Y", PinType::Float);
    AddInput("Pos Z", PinType::Float);
}

ObjectNode::~ObjectNode() {
    // ノードが消えるときはオブジェクトも消す
    if (m_pObject) {
        Editor::GetInstance()->DeleteObject(m_pObject);
    }
}

void ObjectNode::Update() {
    if (!m_pObject) return;

    auto transform = m_pObject->GetComponent<Transform>();
    if (!transform) return;

    // 本来は NodeManager 経由で接続されているリンクの値を取得する必要がある
    // ここでは簡易的に「もし値が来ていれば」というロジックになる
    // 入力値の取得ロジックは NodeManager 側で解決して渡すか、
    // Node が InputPin -> Link -> OutputPin -> Node と辿って値を取得する実装が必要

    // 仮の実装:
    float x = 0.0f; // Linkから取得した値
    float y = 0.0f;
    float z = 0.0f;

    // セット
    // transform->SetPosition(Vector4O(x, y, z, 1.0f));
}

void ObjectNode::DrawNodeUI()
{
    ImGui::Text("%s", m_Name.c_str());
    ImGui::SetNextItemWidth(std::max(50.0f, ImGui::CalcTextSize(m_Name.c_str()).x + 20.0f));
    if (InputTextString("##name", &m_Name)) {
        m_pObject->SetName(m_Name);
	}
}

FloatNode::FloatNode() : Node("Number") {
    AddOutput("Value", PinType::Float);
}

void FloatNode::DrawNodeUI() {
    ImGui::PushItemWidth(80);
    ImGui::PushID(NodeManager::CountWidgetID());
    ImGui::DragFloat("##value", &m_Value, 0.1f);
    ImGui::PopID();
    ImGui::PopItemWidth();
    ImGui::Text("%s", m_Name.c_str());
}

//==========================================================================
// ヘルパー関数
//==========================================================================


bool InputTextString(const char* label, std::string* str, ImGuiInputTextFlags flags) {
    struct Funcs {
        static int ResizeCallback(ImGuiInputTextCallbackData* data) {
            if (data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
                std::string* str = (std::string*)data->UserData;
                IM_ASSERT(str->c_str() == data->Buf);
                str->resize(data->BufTextLen);
                data->Buf = (char*)str->c_str();
            }
            return 0;
        }
    };

    flags |= ImGuiInputTextFlags_CallbackResize;

    ImGui::PushID((void*)str);
    auto edited = ImGui::InputText(label, (char*)str->c_str(), str->capacity() + 1, flags, Funcs::ResizeCallback, (void*)str);
    ImGui::PopID();
    return edited;
}
