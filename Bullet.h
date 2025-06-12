#pragma once
#include "MonoBehavior.h"

#include "EngineSystem/Editor.h"
#include "components/Component_Transform.h"

#include "imgui/imgui.h"

class Bullet :
    public MonoBehavior
{
private:
	Vector4O direction = Vector4O(0, 0, 1, 0);
    float speed = 10.0f;
    float lifeTime = 5.0f;
	float timer = 0.0f;

public:
    void Start() override {

    }

    void Update() override {
        timer += 1.0f / 60.0f;
        if (timer > lifeTime) {
            owner->Destroy();
        } else {
            Transform* transform = owner->GetComponent<Transform>();
			transform->SetPosition(transform->GetPosition() + direction * speed);
		}
    }

    void DrawGUI() override {
        ImGui::Text("Bullet");
        ImGui::Indent();
        ImGui::Text("Direction: %.2f, %.2f, %.2f", direction.x, direction.y, direction.z);
        ImGui::Text("Speed: %.2f", speed);
        ImGui::Text("Life Time: %.2f", lifeTime);
        ImGui::Text("Timer: %.2f", timer);
        ImGui::Unindent();
    }

    void SetDirection(const Vector4O& dir) {
        direction = dir;
	}

    void SetSpeed(float newSpeed) {
		speed = newSpeed;
    }

    void SetLifeTime(float newLifeTime) {
        lifeTime = newLifeTime;
	}
};

