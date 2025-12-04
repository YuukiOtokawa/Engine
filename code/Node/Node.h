#pragma once

#include <memory>
#include <vector>
#include <string>
#include <list>
#include "imgui.h"
#include "imgui_node_editor.h"

using namespace ax::NodeEditor;

class NodePin;
enum class PinType;

// ノードの基底クラス
class Node {
protected:
    std::string m_Name;
    ImVec2 m_Size;
    std::vector<NodePin*> m_Inputs;
    std::vector<NodePin*> m_Outputs;

    // 3D空間ノード用プロパティ
    // m_Positionをワールド座標系xz、m_SpatialDepthをy軸として扱う想定
    ImVec2 m_Position;              // X,Z平面上の位置 X座標とZ座標
    float m_SpatialDepth = 0.0f;    // Y軸深度 3D空間上の高さ Y座標

public:
    bool m_IsSpatialNode = false;   // 3D空間に位置する = m_SpatialDepthを使用
    int m_ID;

    Node(const char* name);
    virtual ~Node() {}

    int GetID() const { return m_ID; }
    std::string GetName() const { return m_Name; }
    std::vector<NodePin*> GetInputs() { return m_Inputs; }
    std::vector<NodePin*> GetOutputs() { return m_Outputs; }

    // ピンIDからピンを取得
    NodePin* GetPin(int id);

    // ゲームロジック更新（毎フレーム呼ばれる）
    virtual void Update() = 0;

    // ノード自体の描画（imgui-node-editor内での表示）
    virtual void DrawNodeUI();

    void SetPositionXY(ImVec2 pos) {
        m_Position.x = pos.x;
        m_SpatialDepth = pos.y;
    }
    void SetPositionXZ(ImVec2 pos) {
        m_Position = pos;
    }

    ImVec2 GetPositionXY() const {
        return ImVec2(m_Position.x, m_SpatialDepth);
    }
    ImVec2 GetPositionXZ() const {
        return m_Position;
    }

protected:
    void AddInput(const char* name, PinType type);

    void AddOutput(const char* name, PinType type);
};

// =================================================
// 実装例: 数値ノード (定数)
// =================================================
class FloatNode : public Node {
    float m_Value = 0.0f;
public:
    FloatNode();

    void Update() override {
        // 何もしない（出力値は固定またはUIで変更）
    }

    void DrawNodeUI() override;

    float GetValue() const { return m_Value; }
};

// =================================================
// 実装例: オブジェクトノード (Game Builder Garage風)
// =================================================

class Object;
class ObjectNode : public Node {
    Object* m_pObject = nullptr;

public:
    ObjectNode();

    ~ObjectNode() override;

    void Update() override;

    void DrawNodeUI() override;
};


//==========================================================================
// ヘルパー関数
//==========================================================================


// std::string を ImGui::InputText で使えるようにする関数
bool InputTextString(const char* label, std::string* str, ImGuiInputTextFlags flags = 0);
