// ========================================================
//
// 入力システムコンポーネント[Component_InputSystem.h]
// 
//									Date:20250520
//									Author:Yuuki Otokawa
// ========================================================

#pragma once

//==========================================================================
// ヘッダーインクルード
//==========================================================================

#include "Component.h"

#include "Keyboard.h"
#include "Mouse.h"
#include "Gamepad.h"

//==========================================================================
// クラス定義
//==========================================================================

class Component_InputSystem :
    public Component
{
private:

	// 各種入力デバイスのポインタ
	GamePad* m_pGamePad;
	Keyboard* m_pKeyboard;
	Mouse* m_pMouse;

public:
	// コンストラクタ
	Component_InputSystem();

	//各デバイスの入力値の取得・更新
	void Update() override;

	//各種ゲッター
	GamePad* GetGamePad() { return m_pGamePad; }
	Keyboard* GetKeyboard() { return m_pKeyboard; }
	Mouse* GetMouse() { return m_pMouse; }

};

