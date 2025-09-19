#include "Collider.h"

#include "CollisionManager.h"

#include "ComponentFactory.h"

REGISTER_COMPONENT(Collider)


void Collider::Start()
{
	CollisionManager::GetInstance()->AddCollisionObject(owner);
}

void Collider::OnDestroy()
{
	CollisionManager::GetInstance()->RemoveCollisionObject(owner);
}

void Collider::UpdateComponent()
{
}

void Collider::DrawGUI()
{
	ImGui::Indent();

	ImGui::Text("Collision Detected");
	ImGui::Indent();

	for (auto obj : m_CollisionObjects) {
		ImGui::Text("%s", obj->GetName().c_str());
	}

	ImGui::Unindent();

	ImGui::Unindent();
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
