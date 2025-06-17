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

    Vector4O _StartPosition;
    Vector4O _EndPosition;
    Vector4O _StartVector;
    Vector4O _EndVector;
public:
    void Start() override {

    }

    void Update() override {
  //      timer += 1.0f / 60.0f;
  //      if (timer > lifeTime) {
  //          owner->Destroy();
  //      } else {
  //          Transform* transform = owner->GetComponent<Transform>();
		//	transform->SetPosition(transform->GetPosition() + direction * speed);
		//}

        timer += 0.01f;
        float i = timer;

        //エルミート曲線
        Vector4O newPosition = {
            _StartPosition * (2.0f * i * i * i - 3.0f * i * i + 1.0f) +
            _StartVector * (i * i * i - 2.0f * i * i + i) +
            _EndPosition * (-2.0f * i * i * i + 3.0f * i * i) +
            _EndVector * (i * i * i - i * i)
        };

        Transform* transform = owner->GetComponent<Transform>();
        transform->SetPosition(newPosition);
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

    void SetDirection(const Vector4O& start, const Vector4O& end) {
        _StartPosition = start;
        _EndPosition = end;
        _StartVector = Vector4O(2.0f,2.0f,2.0f);
        _EndVector = Vector4O(2.0f, -2.0f, 2.0f);
    }

    void SetSpeed(float newSpeed) {
		speed = newSpeed;
    }

    void SetLifeTime(float newLifeTime) {
        lifeTime = newLifeTime;
	}
};

