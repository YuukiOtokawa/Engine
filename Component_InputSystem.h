#pragma once
#include "Component.h"

#include "Keyboard.h"
#include "Mouse.h"
#include "Gamepad.h"

class InputSystem :
    public Component
{
private:
	static GamePad* m_pGamePad;
	static Keyboard* m_pKeyboard;
	static Mouse* m_pMouse;

public:
	InputSystem();

	//各デバイスの入力値の取得・更新
	void Update() override;

	//各種ゲッター
	static GamePad* GetGamePad() { return m_pGamePad; }
	static Keyboard* GetKeyboard() { return m_pKeyboard; }
	static Mouse* GetMouse() { return m_pMouse; }

	void InitializeTag() override
	{
		owner->SetTag(GameObjectTagLayer::InputSystemTag);
		owner->SetDrawable(false);
	}
};

