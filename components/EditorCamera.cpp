// ========================================================
//
// エディターカメラコンポーネントクラス[EditorCamera.cpp]
// 
//									Date:20250602
//									Author:Yuuki Otokawa
// ========================================================

//==========================================================================
// ヘッダーインクルード
//==========================================================================

#include "EditorCamera.h"

#include "Component_InputSystem.h"
#include "Component_Transform.h"

//==========================================================================
// メンバ関数
//==========================================================================

void EditorCamera::UpdateComponent()
{
	auto keyboard = InputSystem::GetKeyboard();
	auto mouse = InputSystem::GetMouse();
	auto transform = owner->GetComponent<Transform>();
	float speed = 0.5f;

	// 右クリックドラッグでカメラ回転
	static bool isDragging = false;
	static XMINT2 lastMousePos = { 0, 0 };
	float rotationSpeed = 0.001f;

	if (mouse->GetDown(RightButton)) {
		// ドラッグ開始
		isDragging = true;
		lastMousePos = mouse->GetPosition();
	}
	else if (mouse->GetUp(RightButton)) {
		// ドラッグ終了
		isDragging = false;
	}

	if (!isDragging)return;

	// 移動
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

	// カメラの上方向を計算
	Vector4O up = Vector4O::Up();

	if (keyboard->GetKeyRepeat(KK_W)) {
		Vector4O movement = forward * speed;
		transform->SetPosition(transform->GetPosition() + movement);
		SetTarget(GetTarget() + movement);
	}
	if (keyboard->GetKeyRepeat(KK_S)) {
		Vector4O movement = forward * -speed;
		transform->SetPosition(transform->GetPosition() + movement);
		SetTarget(GetTarget() + movement);
	}
	if (keyboard->GetKeyRepeat(KK_A)) {
		Vector4O movement = right * -speed;
		transform->SetPosition(transform->GetPosition() + movement);
		SetTarget(GetTarget() + movement);
	}
	if (keyboard->GetKeyRepeat(KK_D)) {
		Vector4O movement = right * speed;
		transform->SetPosition(transform->GetPosition() + movement);
		SetTarget(GetTarget() + movement);
	}
	if (keyboard->GetKeyRepeat(KK_E)) {
		Vector4O movement = up * speed;
		transform->SetPosition(transform->GetPosition() + movement);
		SetTarget(GetTarget() + movement);
	}
	if (keyboard->GetKeyRepeat(KK_Q)) {
		Vector4O movement = up * -speed;
		transform->SetPosition(transform->GetPosition() + movement);
		SetTarget(GetTarget() + movement);
	}

	if (isDragging && mouse->GetRepeat(RightButton)) {
		// マウスの現在位置を取得
		XMINT2 currentMousePos = mouse->GetPosition();

		// 前回の位置からの差分を計算
		int deltaX = currentMousePos.x - lastMousePos.x;
		int deltaY = currentMousePos.y - lastMousePos.y;

		 // 視線移動方向を逆にするために符号を反転
		deltaX = deltaX;
		deltaY = -deltaY;

		// X軸の回転（上下の視線移動）
		Vector4O rotation = transform->GetRotation();
		rotation.x += deltaY * rotationSpeed;

		// 上下の視線移動を制限（-89°〜89°）
		if (rotation.x > 1.55f) rotation.x = 1.55f; // 約89度
		if (rotation.x < -1.55f) rotation.x = -1.55f; // 約-89度

		// Y軸の回転（左右の視線移動）
		rotation.y += deltaX * rotationSpeed;

		transform->SetRotation(rotation);

		// カメラのターゲット位置を更新
		Vector4O forward;
		forward.x = sinf(rotation.y) * cosf(rotation.x);
		forward.y = sinf(rotation.x);
		forward.z = cosf(rotation.y) * cosf(rotation.x);

		SetTarget(transform->GetPosition() + forward);

		// 現在位置を保存
		lastMousePos = currentMousePos;
	}
}

