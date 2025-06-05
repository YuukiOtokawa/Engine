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
public:
    void Start() override {

    }

    void Update() override {
        if (isControllable) {
            // Handle player input and movement here
            auto keyboard = InputSystem::GetKeyboard();
			auto transform = owner->GetComponent<Transform>();

			float speed = 0.5f;

			// 移動
			
			//auto camera = owner->GetScene()->GetMainCamera();
			
			// カメラの回転から方向ベクトルを計算
			Vector4O rotation = transform->GetRotation();
			Vector4O forward;
			forward.x = sinf(rotation.y) * cosf(rotation.x);
			forward.y = sinf(rotation.x);
			forward.z = cosf(rotation.y) * cosf(rotation.x);
			forward.Normalize();

			// カメラの方向から右方向ベクトルを計算
			// 右方向はカメラの上方向と前方向の外積で求める
			Vector4O right;
			right.x = forward.z * Vector4O::Up().y - forward.y * Vector4O::Up().z;
			right.y = forward.x * Vector4O::Up().z - forward.z * Vector4O::Up().x;
			right.z = forward.y * Vector4O::Up().x - forward.x * Vector4O::Up().y;
			right.Normalize();

			if (keyboard->GetKeyRepeat(KK_W)) {
				Vector4O movement = forward * speed;
				transform->SetPosition(transform->GetPosition() + movement);
			}
			if (keyboard->GetKeyRepeat(KK_S)) {
				Vector4O movement = forward * -speed;
				transform->SetPosition(transform->GetPosition() + movement);
			}
			if (keyboard->GetKeyRepeat(KK_A)) {
				Vector4O movement = right * -speed;
				transform->SetPosition(transform->GetPosition() + movement);
			}
			if (keyboard->GetKeyRepeat(KK_D)) {
				Vector4O movement = right * speed;
				transform->SetPosition(transform->GetPosition() + movement);
			}
		}
	}

    void DrawGUI() override {
		ImGui::Checkbox("Controllable", &isControllable);
    }
};

