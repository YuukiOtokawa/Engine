#pragma once
#include "MonoBehavior.h"

#include "Component_Transform.h"

#define BOUND_CONST 1.0f // はねかえり係数の定数

class Pack :
	public MonoBehaviour
{
private:
	static float before_length;		            // 前回のボール間の実際の距離
	float length;								// ボール間の実際の距離
	float dt = 0.1f;									// 微少時間Δｔ
	Vector4O d;									// 座標差分
	float e = BOUND_CONST;					    // はねかえり係数
	Vector4O v0_new, v1_new;					// 衝突後の速度
	int i;
	int out;								    // 画面外に出たボールの数

    Vector4O velocity = Vector4O::Forward() + Vector4O::Right();
public:
	void Start() override {
		// 初期化処理
	}
    void Update() override {
        // 更新処理
        auto transform = owner->GetComponent<Transform>();
        if (transform) {
            transform->SetPosition(transform->GetPosition() + velocity * dt);
        }
        if (transform->GetPosition().x > 20.0f || transform->GetPosition().x < -20.0f) {
            // 画面外に出た場合の処理
            velocity.x *= -1.0f; // X軸方向の速度を反転
        }
        if (transform->GetPosition().z > 20.0f || transform->GetPosition().z < -20.0f) {
            // 画面外に出た場合の処理
            velocity.z *= -1.0f; // Z軸方向の速度を反転
        }
        Editor::GetInstance()->CheckCollision(owner); // 衝突判定を行う
    }
	void DrawGUI() override {
		// GUI描画処理
	}
	void ExportComponent() override {
		// コンポーネントのエクスポート処理
	}

    Vector4O GetVelocity() {
        return velocity;
    }

    void SetVelocity(const Vector4O& vel) {
        velocity = vel;
    }
};

