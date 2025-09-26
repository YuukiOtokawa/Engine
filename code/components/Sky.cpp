#include "Sky.h"

#include "Editor.h"

#include "Component_Transform.h"

#include "ComponentFactory.h"


void Sky::Update()
{
	m_pCamera = Editor::GetInstance()->GetActiveCamera();

	if (!m_pCamera) return;

	auto transform = owner->GetComponent<Transform>();
	if (!transform) return;

	auto cameraTransform = m_pCamera->GetComponent<Transform>();
	if (!cameraTransform) return;
	transform->SetPosition(cameraTransform->GetPosition());
	transform->SetScale(Vector4O::One() * m_Size);
}
	