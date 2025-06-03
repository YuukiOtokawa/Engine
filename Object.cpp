#include "Object.h"

#include "Component.h"

#include "MainEngine.h"

void Object::Initialize() {}
void Object::Update() {
	for (auto& component : m_Components) {
		component->Update();
	}
}
void Object::Draw() {
	for (auto& component : m_Components) {
		component->Draw();
	}
	if (m_IsDrawable == false) return;
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


void Object::AddComponentClass(Component* component) {
	component->SetOwner(this);
	component->InitializeTag();
	m_Components.push_back(component);
}
