#pragma once
#include "MonoBehavior.h"

#include "imgui.h"
#include "Component_InputSystem.h"

#include "Component_Transform.h"

class Player :
    public MonoBehavior
{
private:
	bool isControllable = true;
	float speed = 0.5f; // 移動速度
public:
    void Start() override {

    }

    void Update() override {
        if (isControllable) {
            // Handle player input and movement here
            auto keyboard = InputSystem::GetKeyboard();
			auto transform = owner->GetComponent<Transform>();

			// カメラの方向ベクトルを計算
			Object* camera = Editor::GetInstance()->GetActiveCamera();

			Vector4O direction = Vector4O::Zero();
			float rotation;
			Vector4O cameraAngle = camera->GetComponent<Transform>()->GetRotation();
			Vector4O cameraForward = transform->GetPosition() - camera->GetComponent<Transform>()->GetPosition();
			cameraForward.Normalize();

			Vector4O forward = Vector4O::Zero();

			Vector4O move;

			// カメラの方向から右方向ベクトルを計算
			// 右方向はカメラの上方向と前方向の外積で求める

			if (keyboard->GetKeyRepeat(KK_W)) {
				direction.z += 1.0f;
				move.x -= sinf(cameraAngle.y + 0.5f * 3.14f) * speed;
				move.z -= -cosf(cameraAngle.y + 0.5f * 3.14f) * speed;
			}
			if (keyboard->GetKeyRepeat(KK_S)) {
				move.x += sinf(cameraAngle.y + 0.5f * 3.14f) * speed;
				move.z += -cosf(cameraAngle.y + 0.5f * 3.14f) * speed;
				direction.z -= 1.0f;
			}
			if (keyboard->GetKeyRepeat(KK_A)) {
				direction.x -= 1.0f;
				move.x -= -sinf(cameraAngle.y) * speed;
				move.z -= cosf(cameraAngle.y) * speed;
			}
			if (keyboard->GetKeyRepeat(KK_D)) {
				direction.x += 1.0f;
				move.x += -sinf(cameraAngle.y) * speed;
				move.z += cosf(cameraAngle.y) * speed;
			}

			move.Normalize();
			move *= speed;

			direction.Normalize();
			float angleDiff = 0.0f;
			Vector4O newAngle = transform->GetRotation();
			if (direction != Vector4O::Zero()) {
				cameraForward = Vector4O(cameraForward.x, cameraForward.z, 0.0f, 0.0f);

				angleDiff = atan2(direction.x, direction.z);
				newAngle.y = angleDiff - cameraAngle.y + 1.57f;
				newAngle = newAngle.ToEuler();
			}

			transform->SetPosition(transform->GetPosition() + move * speed);
			transform->SetRotation(newAngle);
		}
	}

    void DrawGUI() override {
		ImGui::Checkbox("Controllable", &isControllable);
		ImGui::InputFloat("Speed", &speed, 0.01f, 0.1f);
    }
};

