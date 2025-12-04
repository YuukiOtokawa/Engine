#include "Node.h"
#include "NodePin.h"
#include "NodeManager.h"

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

void Node::DrawNodeName()
{
    ImGui::Text(m_Name.c_str());
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
