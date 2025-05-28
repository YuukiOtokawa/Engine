#include "Component_Transform.h"

void Transform::Update() {

}

void Transform::Draw() {

}

void Transform::InitializeTag() {
	owner->SetTag(GameObjectTagLayer::SystemTag);
}