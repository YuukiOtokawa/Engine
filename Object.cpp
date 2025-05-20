#include "Object.h"

#include "Component.h"

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
}
void Object::Finalize() {}

void Object::AddComponent(Component* component) {
	m_Components.push_back(component);
	component->SetOwner(this);
}
