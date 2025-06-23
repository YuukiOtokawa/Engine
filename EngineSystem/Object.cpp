#include "Object.h"

#include "Component.h"

#include "MainEngine.h"

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
}
void Object::Draw() {
	for (auto& component : m_Components) {
		component->Draw();
	}
	if (m_IsDrawable == false) return;
	if (m_iIndexCount == 0)
		MainEngine::GetInstance()->GetRenderer()->GetDeviceContext()->Draw(m_iVertexCount, 0);
	else
		MainEngine::GetInstance()->GetRenderer()->GetDeviceContext()->DrawIndexed(m_iIndexCount, 0, 0);
}
void Object::DrawGUI(){
	GUI::SetFontObjectName();
	ImGui::Text(GetName().c_str());
	GUI::SetFontDefault();
	
	ImGui::Text("Tag: %s", GameObjectTagLayer::GameObjectTagString[GetTag()]);

	for (auto& component : m_Components) {
		component->DrawGUI();
	}
}
void Object::Finalize() {}

void Object::Destroy()
{
	Editor::GetInstance()->DeleteObject(this);
}


void Object::AddComponentClass(Component* component) {
	component->SetOwner(this);
	component->InitializeTag();
	m_Components.push_back(component);
}

void Object::ExportFile() {
	for (Component* component : m_Components) {
		component->ExportFile();
	}
}
