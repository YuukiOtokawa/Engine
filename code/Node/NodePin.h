#pragma once

#include <memory>
#include <string>
#include "imgui_node_editor.h"
using namespace ax::NodeEditor;

// TODO [otokawa]:ノードエディター用共通情報をまとめるヘッダーが必要かも
// ピンのデータ型
enum class PinType {
	Flow,
	Bool,
	Int,
	Float,
	Vector3,
	Object,
    String,
};

class Node;

// imgui表示用ノードピンクラス
class NodePin
{
	int m_PinID;          // imgui-node-editor用ピンID
	Node* m_Node;         // 接続先ノード
	std::string m_Name;   // ピン名
	PinType m_Type;       // ピンのデータ型
	PinKind m_Kind;       // ピンの種類


public:
	NodePin(int id, const char* name, PinType type, PinKind kind)
		: m_PinID(id), m_Node(nullptr), m_Name(name), m_Type(type), m_Kind(kind) {
	}
    int GetID() const { return m_PinID; }
    std::string GetName() const { return m_Name; }
    void SetNode(Node* node) { m_Node = node; }
    PinKind GetKind() const { return m_Kind; }
};

class Node;

// imgui表示用ノードピンリンククラス
class NodePinLink {
	int m_LinkID;
	int m_OutputPinID;    //左側
	int m_InputPinID;     //右側

public:
	NodePinLink(int id, int outputPinID, int inputPinID)
	  : m_LinkID(id),
		m_OutputPinID(outputPinID),
		m_InputPinID(inputPinID) {
	}

    int GetID() const { return m_LinkID; }
    int GetOutputPinID() const { return m_OutputPinID; }
    int GetInputPinID() const { return m_InputPinID; }
};

