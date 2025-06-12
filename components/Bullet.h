#pragma once
#include "MonoBehavior.h"

#include "../EngineSystem/Editor.h" 
#include "Component_Transform.h"

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

