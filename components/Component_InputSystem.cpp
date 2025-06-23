// ========================================================
//
// 入力システムコンポーネント[Component_InputSystem.cpp]
// 
//									Date:20250520
//									Author:Yuuki Otokawa
// ========================================================

//==========================================================================
// ヘッダーインクルード
//==========================================================================

#include "Component_InputSystem.h"
#include "MainEngine.h"

//==========================================================================
// static変数の宣言
//==========================================================================

GamePad* InputSystem::m_pGamePad;
Keyboard* InputSystem::m_pKeyboard;
Mouse* InputSystem::m_pMouse;

//==========================================================================
// メンバ関数定義
//==========================================================================

InputSystem::InputSystem()
{
	// コンポーネントの初期化
	tag = ComponentTag::InputSystemTag;

	m_pGamePad = new GamePad();
	m_pKeyboard = Keyboard::GetInstance();
	m_pMouse = Mouse::GetInstance(MainEngine::GetInstance()->GetWindow());
}

// Update
void InputSystem::UpdateComponent()
{
	// 入力デバイスの更新処理を呼び出す
	m_pGamePad->Update();
	m_pKeyboard->Update();
	m_pMouse->Update();
}

void InputSystem::DrawGUI() {
	ImGui::Text("Input System");
}
