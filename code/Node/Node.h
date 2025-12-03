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
    int m_ID;
    std::string m_Name;
    ImVec2 m_Size;
    std::vector<NodePin*> m_Inputs;
    std::vector<NodePin*> m_Outputs;

public:
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
};
