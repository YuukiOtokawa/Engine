#include "Object.h"

#include "Component.h"

#include "MainEngine.h"

#include "Editor.h"

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
		component->UpdateComponent();
	}
	if (HasChild())
	{
		for (auto& child : m_Children) {
			child->Update();
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

#include "../Inspector.h"

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

	Component* toDelete = nullptr;

	for (auto& component : m_Components) {
		ImGui::PushID(component->GetFileID());
		ImGui::Separator();
		ImGui::Text(component->GetComponentName());
		ImGui::SameLine();
		if (ImGui::Button("x")) {
			toDelete = component;
		}
		component->DrawGUI();
		ImGui::PopID();
	}

	AddComponentPopup(this);

	if (toDelete) {
		DeleteComponent(toDelete);
	}
}
void Object::Finalize() {}

void Object::Destroy()
{
	Editor::GetInstance()->DeleteObject(this);
}


void Object::AddComponentClass(Component* component) {
	component->SetOwner(this);
	m_Components.push_back(component);
}

void Object::DeleteComponent(Component* component)
{
	m_Components.remove(component);
	Editor::GetInstance()->DeleteComponent(component);
	component = nullptr;
}

void Object::ExportFile() {
	CSVExporter::ExportString(m_Name);
	CSVExporter::ExportInt(static_cast<int>(m_Tag));
	CSVExporter::ExportInt(static_cast<int>(m_Layer));
	for (Component* component : m_Components) {
		CSVExporter::ExportInt(component->GetFileID());
	}
	CSVExporter::ExportString("&");
	CSVExporter::ExportInt(m_pParent != nullptr ? m_pParent->GetFileID() : -1); // Export parent ID, -1 if no parent
	if (m_Children.empty()) {
		CSVExporter::ExportInt(-1); // No children
	} else {
		for (Object* child : m_Children) {
			CSVExporter::ExportInt(child->GetFileID());
		}
	}
	CSVExporter::ExportString("&");
}

void Object::AddExportList()
{
	CSVExporter::AddExportList(this);
	for (Component* component : m_Components) {
		component->AddExportList();
	}
}

void Object::ImportFile(std::vector<std::string>& tokens)
{
	m_Name = tokens[2];
	m_Tag = static_cast<GameObjectTag>(std::stoi(tokens[3]));
	m_Layer = static_cast<GameObjectLayer>(std::stoi(tokens[4]));
	int i = 5;
	while (tokens[i] != "&") {
		int fileID = std::stoi(tokens[i]);
		Component* component = Editor::GetInstance()->GetComponentByFileID(fileID);
		if (component) {
			AddComponentClass(component);
		}
		i++;
	}
}
