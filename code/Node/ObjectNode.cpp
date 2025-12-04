#include "ObjectNode.h"

#include "imgui_node_editor.h"

#include "Object.h"
#include "Component_Transform.h"
#include "Editor.h"
#include "NodePin.h"
#include "NodeManager.h"

#include <algorithm>
#define NOMINMAX

ObjectNode::ObjectNode() : Node("Object") {
    // ゲームオブジェクトを生成して管理
    // TODO [otokawa]:これはObject生成の仮実装。

    m_pObject = new Object();

    m_pObject->SetName("Object");
    m_Name = m_pObject->GetName();
    m_pObject->AddComponent<Transform>();
    m_pObject->SetObjectNode(this);

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
    transform->SetPosition(Vector4O(m_Position.x / 32.0f, (m_SpatialDepth - 2.0f) / -32.0f, (m_Position.y - 2.0f) / -32.0f, 1.0f));
}

void ObjectNode::DrawNodeUI()
{
}

void ObjectNode::DrawNodeName()
{
    ImGui::SetNextItemWidth((std::max)(50.0f, ImGui::CalcTextSize(m_Name.c_str()).x + 20.0f));
    if (InputTextString("##name", &m_Name)) {
        m_pObject->SetName(m_Name);
    }
}

void ObjectNode::SetPosition(Vector4O position)
{
    m_Position.x = position.x * 32.0f;
    m_SpatialDepth = -position.y * 32.0f + 2.0f;
	m_Position.y = -position.z * 32.0f + 2.0f;
    
    NodeManager::StartEditorContext();
    if (NodeManager::IsCameraAbove()) {
		SetNodePosition(m_ID, ImVec2(m_Position.x, m_Position.y));
    }
    else {
		SetNodePosition(m_ID, ImVec2(m_Position.x, m_SpatialDepth));
    }
    NodeManager::EndEditorContext();
}
