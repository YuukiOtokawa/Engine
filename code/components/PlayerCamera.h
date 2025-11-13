#pragma once

#include "MonoBehavior.h"

#include "Object.h"
#include "Component_Transform.h"
#include "Component_Camera.h"

class PlayerCamera : public MonoBehaviour {
private:
	Object* player = nullptr;
	float lengthFrag = 10.0f;
	float speed = 0.05f;
public:
	PlayerCamera() {
        m_ClassID = CID_PlayerCamera;
    }

	void Start() override {

	}
	void OnUpdate() override {
		if (player == nullptr) return;

		Transform* playerTransform = player->GetComponent<Transform>();
		Transform* ownerTransform = owner->GetComponent<Transform>();

		owner->GetComponent<Camera>()->SetTarget(playerTransform->GetPosition());

		Vector4O nv;

		float length = (playerTransform->GetPosition() - owner->GetComponent<Transform>()->GetPosition()).Length();

		if (length > lengthFrag) {
			nv = Vector4O{
				(ownerTransform->GetPosition().x - playerTransform->GetPosition().x) * (1.0f - lengthFrag / length),
				0.0f,
				(ownerTransform->GetPosition().z - playerTransform->GetPosition().z) * (1.0f - lengthFrag / length),
				0.0f
			};
			//Vector4O move = playerTransform->GetPosition() - owner->GetComponent<Transform>()->GetPosition();
			//move.y = 0.0f;
			//owner->GetComponent<Transform>()->SetPosition(owner->GetComponent<Transform>()->GetPosition() + move * speed);
		}

		Vector4O rotation = ownerTransform->GetRotation();
		rotation.y = atan2(ownerTransform->GetPosition().z - playerTransform->GetPosition().z, ownerTransform->GetPosition().x - playerTransform->GetPosition().x);
		ownerTransform->SetRotation(rotation);
		
		ownerTransform->SetPosition(ownerTransform->GetPosition() - nv);
	}

	void DrawGUI() override {
		ImGui::Separator();
		ImGui::Text("Player Camera");

		ImGui::Indent();
		ImGui::InputFloat("Length Frag", &lengthFrag, 0.1f, 0.1f);
		ImGui::InputFloat("Speed", &speed, 0.01f, 0.1f);
		ImGui::Unindent();
	}

    void ExportComponent(YAML::Emitter& out) override {
        out << YAML::Key << "lengthFrag" << YAML::Value << lengthFrag;
        out << YAML::Key << "speed" << YAML::Value << speed;
    }

	void SetPlayer(Object* playerObject) {
		player = playerObject;
	}
};
