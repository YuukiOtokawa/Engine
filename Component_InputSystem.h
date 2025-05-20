#pragma once
#include "Component.h"

#include "Keyboard.h"
#include "Mouse.h"
#include "Gamepad.h"

class Component_InputSystem :
    public Component
{
private:
	GamePad* m_pGamePad;
	Keyboard* m_pKeyboard;
	Mouse* m_pMouse;

public:
	Component_InputSystem();

	//各デバイスの入力値の取得・更新
	void Update() override;

	//各種ゲッター
	GamePad* GetGamePad() { return m_pGamePad; }
	Keyboard* GetKeyboard() { return m_pKeyboard; }
	Mouse* GetMouse() { return m_pMouse; }

};

