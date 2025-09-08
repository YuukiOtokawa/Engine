#pragma once
#include "MonoBehavior.h"

#include "imgui.h"
#include "Component_InputSystem.h"

#include "Component_Transform.h"

#include "../Score.h"

#include "Object.h"

//#include "../components/Bullet.h"

class Player :
    public MonoBehaviour
{
private:
	bool isControllable = true;
	float speed = 0.5f; // 遘ｻ蜍暮溷ｺｦ

	Vector4O m_Direction = Vector4O::Backward(); // 繝励Ξ繧､繝､繝ｼ縺ｮ遘ｻ蜍墓婿蜷�

	float bulletSpeed = 1.0f;
	float bulletInterval = 0.5f; // 蠑ｾ縺ｮ逋ｺ蟆�髢馴囈
	float bulletLifeTime = 5.0f; // 蠑ｾ縺ｮ繝ｩ繧､繝輔ち繧､繝�
	float bulletTimer = 0.0f; // 蠑ｾ縺ｮ逋ｺ蟆�繧ｿ繧､繝槭�ｼ
public:
    void Start() override {

    }

    void Update() override {
		if (bulletTimer > 0.0f) {
			bulletTimer -= 1.0f / 60.0f;
			if (bulletTimer < 0.0f) {
				bulletTimer = 0.0f;
			}
		}
        if (isControllable) {
            // Handle player input and movement here
			auto keyboard = InputSystem::GetKeyboard();
			auto transform = owner->GetComponent<Transform>();

			// 繧ｫ繝｡繝ｩ縺ｮ譁ｹ蜷代�吶け繝医Ν繧定ｨ育ｮ�
			Object* camera = Editor::GetInstance()->GetActiveCamera();

			Vector4O direction = Vector4O::Zero();
			Vector4O cameraAngle = camera->GetComponent<Transform>()->GetRotation();
			Vector4O cameraForward = transform->GetPosition() - camera->GetComponent<Transform>()->GetPosition();
			cameraForward.Normalize();

			Vector4O forward = Vector4O::Zero();

			Vector4O move;

			// 繧ｫ繝｡繝ｩ縺ｮ譁ｹ蜷代°繧牙承譁ｹ蜷代�吶け繝医Ν繧定ｨ育ｮ�
			// 蜿ｳ譁ｹ蜷代�ｯ繧ｫ繝｡繝ｩ縺ｮ荳頑婿蜷代→蜑肴婿蜷代�ｮ螟也ｩ阪〒豎ゅａ繧�

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
				m_Direction = move;
			}

			transform->SetPosition(transform->GetPosition() + move * speed);
			transform->SetRotation(newAngle);

			if (keyboard->GetKeyDown(KK_F)&&
				bulletTimer <= 0.0f)
   //         {
			//	// 蠑ｾ繧堤匱蟆�
			//	Object* bullet = new Object();

			//	bullet->SetName("Bullet");
			//	bullet->AddComponent<Transform>();
			//	bullet->AddComponent<MeshFilter>();
			//	bullet->AddComponent<MeshRenderer>();
			//	//bullet->AddComponent<Bullet>();

			//	OBJLoader* loader = new OBJLoader();
			//	loader->Load("asset\\m_Model\\bullet.obj", bullet);

			//	bullet->GetComponent<MeshRenderer>()->SetVertexShader("BlinnPhong");
			//	bullet->GetComponent<MeshRenderer>()->SetPixelShader("BlinnPhong");

			//	bullet->GetComponent<Bullet>()->SetSpeed(bulletSpeed);
			//	bullet->GetComponent<Bullet>()->SetLifeTime(bulletLifeTime);

			//	bullet->GetComponent<Transform>()->SetPosition(transform->GetPosition());
			//	bullet->GetComponent<Bullet>()->SetDirection(m_Direction);

			//	auto texture1 = MainEngine::GetInstance()->GetRenderer()->TextureLoad(L"asset/texture/gravel 1.jpg");
			//	bullet->GetComponent<MeshRenderer>()->SetTexture(texture1);

			//	Editor::GetInstance()->AddObject(bullet);
			//	bulletTimer = bulletInterval;
			//}

            if (keyboard->GetKeyDown(KK_P)) {
                Editor::GetInstance()->GetObject("ScoreManager")->GetComponent<Score>()->AddScore(1);
            }
		}

	}

    void DrawGUI() override {
		ImGui::Separator();
		ImGui::Text("Player Settings");
		ImGui::Indent();
		ImGui::Checkbox("Controllable", &isControllable);
		ImGui::InputFloat("Speed", &speed, 0.01f, 0.1f);
		ImGui::InputFloat("Bullet Speed", &bulletSpeed, 0.1f, 1.0f);
		ImGui::InputFloat("Bullet Interval", &bulletInterval, 0.01f, 0.1f);
		ImGui::InputFloat("Bullet Life Time", &bulletLifeTime, 0.1f, 1.0f);
		ImGui::Text("Bullet Timer: %.2f", bulletTimer);
		ImGui::Unindent();
    }
};

