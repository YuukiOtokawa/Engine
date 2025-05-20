#include "InputSystem.h"

InputSystem::InputSystem(HWND window) {
	m_pGamePad = new GamePad;
	m_pKeyboard = Keyboard::GetInstance();
	m_pMouse = Mouse::GetInstance(window);
}

void InputSystem::Update() {
	//ゲームパッドの更新
	m_pGamePad->Update();
	//m_pGamePad->ControllerDebug();

	//キーボードの更新
	m_pKeyboard->Update();

	//マウスの更新
	m_pMouse->Update();
	//m_pMouse->MouseDebug();
}
