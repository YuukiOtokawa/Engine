#include "NodeEditor.h"
#include <algorithm>

#include "Node.h"

void NodeEditor::CreateLink()
{
	// 新しいリンクの作成をチェック
	if (BeginCreate())
	{
		PinId inputPinId, outputPinId;
		if (QueryNewLink(&inputPinId, &outputPinId))
		{
			if (inputPinId && outputPinId)
			{
				// リンクを承認
				if (AcceptNewItem())
				{
					// 新しいリンクを追加
					LinkInfo newLink;
					newLink.id = CountWidgetID();
					newLink.inputPinId = inputPinId.Get();
					newLink.outputPinId = outputPinId.Get();
					m_Links.push_back(newLink);
				}
			}
		}
	}
	EndCreate();

}

void NodeEditor::DeleteLink()
{
	// リンクの削除をチェック
	if (BeginDelete())
	{
		LinkId deletedLinkId;
		while (QueryDeletedLink(&deletedLinkId))
		{
			if (AcceptDeletedItem())
			{
				// リンクリストから削除
				for (auto it = m_Links.begin(); it != m_Links.end(); ++it)
				{
					if (it->id == deletedLinkId.Get())
					{
						m_Links.erase(it);
						break;
					}
				}
			}
		}
	}
	EndDelete();

	// 右クリック削除
	LinkId contextLinkId = 0;
	if (ShowLinkContextMenu(&contextLinkId))
	{
		std::vector<LinkInfo>::iterator linkId;

		for (auto it = m_Links.begin(); it != m_Links.end(); ++it)
		{
			if (it->id == contextLinkId.Get())
			{
				linkId = it;
				break;
			}
		}

		m_Links.erase(linkId);
	}
}

void NodeEditor::DrawNode(NodeInfo& node)
{
	BeginNode(node.id);

	// ノードレイアウト
	ImGui::BeginGroup();

	// ノードタイトル
	ImGui::BeginGroup();

	ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.2f, 0.3f, 0.5f, 1.0f));
	ImGui::Text(node.name.c_str());
	ImGui::PopStyleColor();

	// ノードタイトル
	ImGui::EndGroup();


	// 分割線
	ImGui::Spacing();

	// ピン
	ImGui::BeginGroup();

	// 入力ピン
	ImGui::BeginGroup();

	for (auto pinId : node.pins[PinKind::Input]) {
		BeginPin(pinId, PinKind::Input);
		ImGui::Text("->");
		EndPin();

		ImGui::SameLine();
		ImGui::PushID(pinId);
		ImGui::Text("Input");
		ImGui::PopID();
	}

	ImGui::EndGroup();

	ImGui::SameLine();
	ImGui::Dummy(ImVec2(50, 0));
	ImGui::SameLine();

	ImGui::BeginGroup();

	for (auto pinId : node.pins[PinKind::Output]) {
		//ImGui::SameLine();
		ImGui::PushID(pinId);
		ImGui::Text("Output");
		ImGui::PopID();

		ImGui::SameLine();

		BeginPin(pinId, PinKind::Output);
		ImGui::Text("->");
		EndPin();
	}

	// 出力ピン
	ImGui::EndGroup();

	// ピン
	ImGui::EndGroup();

	// ノードレイアウト
	ImGui::EndGroup();

	EndNode();
}

void NodeEditor::RightClickNode()
{
	if (m_ContextNodeID.Get() == 0 && ShowNodeContextMenu(&m_ContextNodeID))
	{
		ImGui::OpenPopup("Node Context Menu");
	}

	if (ImGui::BeginPopup("Node Context Menu"))
	{
		DeleteNode(m_ContextNodeID);
		ImGui::EndPopup();
	}
	else if (m_ContextNodeID.Get() != 0) {
		m_ContextNodeID = 0;
	}
}

void NodeEditor::CreateNode(ImVec2 position)
{
	if (ImGui::MenuItem("Create Input Node"))
	{
		NodeInfo newNode;
		newNode.name = "Input Node";
		newNode.id = CountWidgetID();
		newNode.type = NodeType::Input;
		newNode.pins[PinKind::Input].push_back(CountWidgetID());
		m_Nodes.push_back(newNode);

		// 次のフレームで位置を設定
		m_NodeToSetPosition = newNode.id;
		m_NewNodePosition = position;
	}

	if (ImGui::MenuItem("Create Output Node"))
	{
		NodeInfo newNode;
		newNode.name = "Output Node";
		newNode.id = CountWidgetID();
		newNode.type = NodeType::Output;
		newNode.pins[PinKind::Output].push_back(CountWidgetID());
		m_Nodes.push_back(newNode);

		// 次のフレームで位置を設定
		m_NodeToSetPosition = newNode.id;
		m_NewNodePosition = position;
	}
}

void NodeEditor::DeleteNode(NodeId id)
{
	if (ImGui::MenuItem("Delete Node"))
	{
		// ノードを削除
		for (auto it = m_Nodes.begin(); it != m_Nodes.end(); ++it)
		{
			if (it->id == id.Get())
			{
				// このノードのすべてのピンIDを収集
				std::vector<int> pinIds;
				for (auto& pinPair : it->pins)
				{
					for (int pinId : pinPair.second)
					{
						pinIds.push_back(pinId);
					}
				}

				// このノードに接続されているリンクも削除
				m_Links.erase(
					std::remove_if(m_Links.begin(), m_Links.end(),
					[&](const LinkInfo& link) {
						return std::find(pinIds.begin(), pinIds.end(), link.inputPinId) != pinIds.end() ||
						       std::find(pinIds.begin(), pinIds.end(), link.outputPinId) != pinIds.end();
					}),
					m_Links.end()
				);

				m_Nodes.erase(it);
				break;
			}
		}
	}
}

void NodeEditor::Initialize()
{
	Config config;
	config.SettingsFile = "NodeEditor.json";
	m_pContext = CreateEditor(&config);

	SetCurrentEditor(m_pContext);

	// 初期ノードを2つ作成
	NodeInfo inputNode;
	inputNode.name = "Input Node";
	inputNode.id = CountWidgetID();
	inputNode.type = NodeType::Input;
	inputNode.pins[PinKind::Input].push_back(CountWidgetID());
	m_Nodes.push_back(inputNode);

	NodeInfo outputNode;
	outputNode.name = "Output Node";
	outputNode.id = CountWidgetID();
	outputNode.type = NodeType::Output;
	outputNode.pins[PinKind::Output].push_back(CountWidgetID());
	m_Nodes.push_back(outputNode);

	NodeInfo inoutNode;
	inoutNode.name = "InOut Node";
	inoutNode.id = CountWidgetID();
	inoutNode.type = NodeType::InOut;
	inoutNode.pins[PinKind::Input].push_back(CountWidgetID());
	inoutNode.pins[PinKind::Output].push_back(CountWidgetID());
	inoutNode.pins[PinKind::Input].push_back(CountWidgetID());
	inoutNode.pins[PinKind::Output].push_back(CountWidgetID());
	m_Nodes.push_back(inoutNode);

	// 初期ノードの位置を設定（重ならないように配置）
	SetNodePosition(inputNode.id, ImVec2(100, 100));
	SetNodePosition(outputNode.id, ImVec2(400, 100));
	SetNodePosition(inoutNode.id, ImVec2(700, 100));

	SetCurrentEditor(nullptr);
}

void NodeEditor::Finalize()
{
	DestroyEditor(m_pContext);
}

void NodeEditor::Render()
{
	if (!m_NodeEditorVisible)
		return;

	SetCurrentEditor(m_pContext);

	//m_WidgetIDCounter = NodeEditor_ID_Base;

	ImGui::Begin("Node Editor", &m_NodeEditorVisible);

	Begin("Node Editor");

	// 前のフレームで作成されたノードの位置を設定
	if (m_NodeToSetPosition != -1)
	{
		SetNodePosition(m_NodeToSetPosition, m_NewNodePosition);
		m_NodeToSetPosition = -1;
	}

	for (auto& node : m_Nodes) {
		DrawNode(node);
	}

	// 既存のリンクを描画
	for (auto link : m_Links)
	{
		Link(link.id, link.inputPinId, link.outputPinId, ImVec4(1, 1, 1, 1), 1.0f);
	}

	CreateLink();

	DeleteLink();

	// 背景を右クリックしたときのメニュー
	Suspend();

	ImVec2 contextMenuPosition;
	if (ShowBackgroundContextMenu())
	{
		contextMenuPosition = ScreenToCanvas(ImGui::GetMousePos());
		ImGui::OpenPopup("Background Context Menu");
	}

	if (ImGui::BeginPopup("Background Context Menu"))
	{
		CreateNode(contextMenuPosition);
		ImGui::EndPopup();
	}

	// ノードを右クリックしたときのメニュー
	RightClickNode();

	Resume();

	End();

	ImGui::End();

	SetCurrentEditor(nullptr);

}

// NodeInfoのデフォルト実装
void NodeInfo::CreatePins(NodeEditor* pEditor)
{
	// デフォルト実装（派生クラスでオーバーライドされる）
}
