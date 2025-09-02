#pragma once
#include "MonoBehavior.h"

#include "Editor.h" 
#include "Component_Transform.h"

#include "Animation.h"
#include "Billboard.h"

class Bullet :
    public MonoBehaviour
{
private:
	Vector4O direction = Vector4O(0, 0, 1, 0);
    float speed = 1000.0f;
    float lifeTime = 5.0f;
	float timer = 0.0f;
    int effectSpan = 5; // エフェクトの間隔
    int effectCount = 0; // エフェクトのカウント

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
            effectCount++;
            if (effectCount % effectSpan == 0) {
                // エフェクトを生成
                Object* sprite = new Object();
                sprite->SetName("sprite");
                sprite->AddComponent<Transform>();
                sprite->AddComponent<Billboard>();
                sprite->AddComponent<MeshRenderer>();
                sprite->AddComponent<Animation>();

                MATERIAL material;
                material.diffuse = Vector4O(1.0f, 1.0f, 1.0f, 1.0f);
                material.ambient = Vector4O(1.0f, 1.0f, 1.0f, 1.0f);

                LIGHT light;

                light.Diffuse = Vector4O(0.8f, 0.8f, 0.8f, 1.0f);
                light.Ambient = Vector4O(0.1f, 0.1f, 0.1f, 1.0f);
                light.Direction = Vector4O(0.2f, -1.0f, -1.0f, 0.0f);

                sprite->GetComponent<Transform>()->SetScale(Vector4O(1.0f, 1.0f, 1.0f));
                sprite->GetComponent<Transform>()->SetPosition(owner->GetComponent<Transform>()->GetPosition());
                sprite->GetComponent<MeshRenderer>()->SetMaterial(material);
                auto texture2 = MainEngine::GetInstance()->GetRenderer()->TextureLoad(L"asset\\texture\\explosion.png");
                sprite->GetComponent<MeshRenderer>()->SetTexture(texture2);

                sprite->GetComponent<MeshRenderer>()->SetVertexShader("unlit");
                sprite->GetComponent<MeshRenderer>()->SetPixelShader("unlit");

                sprite->GetComponent<Animation>()->SetFramePerSecond(30);
                sprite->GetComponent<Billboard>()->SetUVRect(Vector4O(4, 4));
                sprite->GetComponent<Animation>()->SetLoop(false);

                Editor::GetInstance()->AddObject(sprite);
            }
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

