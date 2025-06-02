#include "Component_Transform.h"

void Transform::Update() {

}

void Transform::Draw() {

}

void Transform::DrawGUI() {
	ImGui::Text("Transform");
}

void Transform::InitializeTag() {
	owner->SetTag(GameObjectTagLayer::SystemTag);
}