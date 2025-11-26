#include "Object.h"

#include "Component.h"

#include "MainEngine.h"

#include "Editor.h"

#include "CollisionManager.h"

#include "Prefab.h"

#include <yaml-cpp/yaml.h>


#define COMPONENT_NAME(ComponentType) \
	return #ComponentType;




Object::Object(bool editable) : EngineMetaFile(CID_Object) {
	m_Name = "Object";
	if (editable)
		Editor::GetInstance()->AddObject(this);
}

Object::~Object()
{
	for (Component* component : m_Components) {
		delete component; // Clean up dynamically allocated components
		component = nullptr;
	}
}
void Object::Initialize() {}


void Object::Update() {
	for (auto& component : m_Components) {
		component->OnUpdate();
	}
	if (HasChild())
	{
		for (auto& child : m_Children) {
			child->Update();
		}
	}
	for (auto& component : m_DeleteComponents) {
		DeleteComponent(component);
	}
	m_DeleteComponents.clear();

}
void Object::OnCollision()
{
	auto collisionObjects = CollisionManager::GetInstance()->ObjectOnCollision(this);

	if (!collisionObjects.empty()) {
		for (const auto& [obj, state] : collisionObjects) {
			for (auto& component : m_Components) {
				switch (state) {
				case CollisionState::Enter:
					component->OnCollisionEnter(obj);
					break;
				case CollisionState::Stay:
					component->OnCollisionStay(obj);
					break;
				case CollisionState::Exit:
					component->OnCollisionExit(obj);
					break;
				default:
					break;
				}
			}
		}
	}
}
void Object::Draw() {
	for (auto& component : m_Components) {
		component->Draw();
	}
	if (m_IsDrawable == false) return;
	if (HasChild())
	{
		for (auto& child : m_Children) {
			child->Draw();
		}
	}
}

#include "Inspector.h"

void Object::DrawGUI(){
	GUI::SetFontObjectName();
	if (m_IsInEditName) {
		// 編集モードに入る
		ImGui::SetKeyboardFocusHere();
		ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll;

		if (ImGui::InputText("##ObjectName", m_EditNameBuffer, IM_ARRAYSIZE(m_EditNameBuffer), flags)) {
			// Enterキーが押された場合、編集モードを終了
			m_IsInEditName = false;
			m_Name = m_EditNameBuffer;
		}

		if (ImGui::IsItemDeactivated()) {
			// フォーカスが外れた場合、編集モードを終了
			m_IsInEditName = false;
		}
	} else {
		// 通常表示モード
		ImGui::Text("%s", m_Name.c_str());
		if (ImGui::IsItemClicked()) {
			// クリックされた場合、編集モードに入る
			m_IsInEditName = true;
			strcpy_s(m_EditNameBuffer, m_Name.c_str());
		}
	}
	//ImGui::Text(GetName().c_str());
	GUI::SetFontDefault();
	
	ImGui::SetNextItemWidth(150.0f);
	int currentTag = static_cast<int>(m_Tag);
	if (ImGui::Combo("Tag", &currentTag, GameObjectTagLayer::GameObjectTagString, IM_ARRAYSIZE(GameObjectTagLayer::GameObjectTagString))) {
		m_Tag = static_cast<GameObjectTag>(currentTag);
	}
	ImGui::SameLine();
	ImGui::SetNextItemWidth(150.0f);
	int currentLayer = static_cast<int>(m_Layer);
	if (ImGui::Combo("Layer", &currentLayer, GameObjectTagLayer::GameObjectLayerString, IM_ARRAYSIZE(GameObjectTagLayer::GameObjectLayerString))) {
		m_Layer = static_cast<GameObjectLayer>(currentLayer);
	}

	for (auto& component : m_Components) {
		ImGui::PushID(component->GetFileID());
		ImGui::Separator();

		auto isOpen = ImGui::TreeNodeEx(component->GetComponentName(), ImGuiTreeNodeFlags_DefaultOpen);
		RightClickMenu();
		ImGui::SameLine();
		if (ImGui::Button("x")) {
			m_DeleteComponents.push_back(component);
		}

		if (isOpen) {
			ImGui::BeginChild(component->GetComponentName(), ImVec2(0,0), ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_Border);

			if (ImGui::BeginPopupContextWindow(m_Name.c_str())) {
				if (ImGui::Selectable("Delete Object")) {
					Destroy();
				}
				ImGui::EndPopup();
			}

			component->DrawGUI();
			ImGui::EndChild();
			ImGui::TreePop();
		}

		ImGui::PopID();
	}

	// エディタモードでもコンポーネント削除を即座に処理
	for (auto& component : m_DeleteComponents) {
		DeleteComponent(component);
	}
	m_DeleteComponents.clear();

	AddComponentPopup(this);

}

void Object::RightClickMenu()
{
	if (ImGui::BeginPopupContextItem(m_Name.c_str())) {
		if (ImGui::Selectable("Create Prefab")) {
			Prefab::ExportWithDialog(this);
		}
		ImGui::Separator();
		if (ImGui::Selectable("Delete Object")) {
			Destroy();
		}
		ImGui::EndPopup();
	}
}

void Object::Finalize() {}

void Object::Destroy()
{
	Editor::GetInstance()->DeleteObject(this);
}


void Object::AddComponentClass(Component* component, bool isInGame) {
	component->SetOwner(this);
	m_Components.push_back(component);
	if (isInGame)
		Editor::GetInstance()->CreateComponent(component);
}

void Object::DeleteComponent(Component* component)
{
	component->OnDestroy();
	m_Components.remove(component);
	Editor::GetInstance()->DeleteComponent(component);
	component = nullptr;
}

void Object::ExportFile(YAML::Emitter& out) {
	out << YAML::Key << "name" << YAML::Value << m_Name;
	out << YAML::Key << "tag" << YAML::Value << static_cast<int>(m_Tag);
	out << YAML::Key << "layer" << YAML::Value << static_cast<int>(m_Layer);

	// コンポーネントのFileIDリスト（空でない場合のみ出力）
	if (!m_Components.empty()) {
		out << YAML::Key << "components" << YAML::Value << YAML::BeginSeq;
		for (Component* component : m_Components) {
			out << component->GetFileID();
		}
		out << YAML::EndSeq;
	}

	// 親オブジェクトのFileID（親が存在する場合のみ出力）
	if (m_pParent != nullptr) {
		out << YAML::Key << "parentID" << YAML::Value << m_pParent->GetFileID();
	}

	// 子オブジェクトのFileIDリスト（空でない場合のみ出力）
	if (!m_Children.empty()) {
		out << YAML::Key << "children" << YAML::Value << YAML::BeginSeq;
		for (Object* child : m_Children) {
			out << child->GetFileID();
		}
		out << YAML::EndSeq;
	}
}

void Object::AddExportList()
{
	SceneExporter::AddExportList(this);
	for (Component* component : m_Components) {
		component->AddExportList();
	}
}

void Object::ImportFile(YAML::Node& node)
{
	if (node["name"]) {
		m_Name = node["name"].as<std::string>();
	}
	if (node["tag"]) {
		m_Tag = static_cast<GameObjectTag>(node["tag"].as<int>());
	}
	if (node["layer"]) {
		m_Layer = static_cast<GameObjectLayer>(node["layer"].as<int>());
	}
	if (node["components"]) {
		for (const auto& componentFileID : node["components"]) {
			int fileID = componentFileID.as<int>();
			Component* component = Editor::GetInstance()->GetComponentByFileID(fileID);
			if (component) {
				AddComponentClass(component);
			}
		}
	}
	if (node["parentID"]) {
		int parentID = node["parentID"].as<int>();
		if (parentID != -1) {
			Object* parent = Editor::GetInstance()->GetObjectByFileID(parentID);
			if (parent) {
				parent->AddChild(this);
			}
		}
	}
}
