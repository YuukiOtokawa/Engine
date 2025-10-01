#include "Collider.h"

#include "CollisionManager.h"

#include "ComponentFactory.h"

#include "Component_Transform.h"

REGISTER_COMPONENT(Collider)

void Collider::Start()
{
	CollisionManager::GetInstance()->AddCollisionObject(owner);
}

void Collider::OnDestroy()
{
	CollisionManager::GetInstance()->RemoveCollisionObject(owner);
}

void Collider::Update()
{
}

void Collider::DrawGUI()
{
	ImGui::Indent();

	ImGui::DragFloat3("Collision Size", &m_CollisionSize.x, 0.1f, 0.0f, 100.0f);

	ImGui::Text("Collision Detected");
	ImGui::Indent();

	for (auto obj : m_CollisionObjects) {
		ImGui::Text("%s", obj->GetName().c_str());
	}

	ImGui::Unindent();

	ImGui::Unindent();
}

Vector3O Collider::GetCollisionSize() const {
	auto transform = owner->GetComponent<Transform>();

	if (transform) {
		return m_CollisionSize * transform->GetScale().XYZ();
	}
	else {
		return m_CollisionSize;
	}
}

void Collider::OnCollisionEnter(Object* target)
{
	m_CollisionObjects.push_back(target);
}

void Collider::OnCollisionStay(Object* target)
{
	if (std::find(m_CollisionObjects.begin(), m_CollisionObjects.end(), target) == m_CollisionObjects.end()) {
		m_CollisionObjects.push_back(target);
	}
}

void Collider::OnCollisionExit(Object* target)
{
	m_CollisionObjects.remove(target);
}
