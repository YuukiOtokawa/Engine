#include "Rigidbody.h"

#include "Component_Transform.h"

#include "TimeSystem.h"

REGISTER_COMPONENT(Rigidbody)

void Rigidbody::Update()
{
	if (m_IsKinematic) return;
	// 重力の影響を受ける
	if (m_MovableY) {
		m_Velocity.y += GRAVITY * m_Gravity * 0.016f;
	}

	


	// 速度に基づいて位置を更新
	// ドラッグの影響を受ける
	m_Velocity.x -= m_Velocity.x * m_Drag * Time::DeltaTime();
	m_Velocity.y -= m_Velocity.y * m_Drag * Time::DeltaTime();
	m_Velocity.z -= m_Velocity.z * m_Drag * Time::DeltaTime();

	Vector4O displacement = m_Velocity * 0.016f;
	if (!m_MovableX) displacement.x = 0.0f;
	if (!m_MovableY) displacement.y = 0.0f;
	if (!m_MovableZ) displacement.z = 0.0f;

	auto transform = owner->GetComponent<Transform>();

	if (transform) {
		transform->SetPosition(
			transform->GetPosition() + displacement
		);
	}
}

void Rigidbody::DrawGUI()
{
	ImGui::Indent();

	ImGui::Checkbox("Is Kinematic", &m_IsKinematic);

	ImGui::PushID(0);
	ImGui::Text("Movable");
	ImGui::SameLine();
	ImGui::Checkbox("X", &m_MovableX);
	ImGui::SameLine();
	ImGui::Checkbox("Y", &m_MovableY);
	ImGui::SameLine();
	ImGui::Checkbox("Z", &m_MovableZ);
	ImGui::PopID();


	ImGui::PushID(1);
	ImGui::Text("Rotable");
	ImGui::SameLine();
	ImGui::Checkbox("X", &m_RotableX);
	ImGui::SameLine();
	ImGui::Checkbox("Y", &m_RotableY);
	ImGui::SameLine();
	ImGui::Checkbox("Z", &m_RotableZ);
	ImGui::PopID();

	ImGui::DragFloat("Mass", &m_Mass, 0.1f, 0.1f, 100.0f);
	ImGui::DragFloat("Drag", &m_Drag, 0.01f, 0.0f, 10.0f);
	ImGui::DragFloat("Gravity Scale", &m_Gravity, 0.1f, 0.0f, 10.0f);

	ImGui::DragFloat3("Velocity", &m_Velocity.x, 0.1f);

	ImGui::DragFloat("Bounce", &m_Bounce, 0.01f, 0.0f, 10.0f);

	ImGui::Unindent();
}

void Rigidbody::OnCollisionEnter(Object* target)
{
	auto targetTf = target->GetComponent<Transform>();
	auto ownerTf = owner->GetComponent<Transform>();

	Vector3O direction = (ownerTf->GetPosition() - targetTf->GetPosition()).XYZ();

	Vector3O axisX = targetTf->GetRight();
	float dotX = abs(direction.Dot(axisX, direction));

	Vector3O axisY = targetTf->GetUp();
	float dotY = abs(direction.Dot(axisY, direction));

	Vector3O axisZ = targetTf->GetForward();
	float dotZ = abs(direction.Dot(axisZ, direction));

	float dx = targetTf->GetScale().x - fabs(dotX);
	float dy = targetTf->GetScale().y - fabs(dotY);
	float dz = targetTf->GetScale().z - fabs(dotZ);

	// 一番浅い軸に沿って跳ね返る
	if (dx <= dy && dx <= dz) {
		// X軸に沿って跳ね返る
		m_Velocity.x = -m_Velocity.x * m_Bounce;
	}
	else if (dy <= dx && dy <= dz) {
		// Y軸に沿って跳ね返る
		m_Velocity.y = -m_Velocity.y * m_Bounce;
	}
	else {
		// Z軸に沿って跳ね返る
		m_Velocity.z = -m_Velocity.z * m_Bounce;
	}
}

void Rigidbody::OnCollisionStay(Object* target)
{


}

void Rigidbody::OnCollisionExit(Object* target)
{
}
