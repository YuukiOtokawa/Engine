// ========================================================
//
// 蜈･蜉帙す繧ｹ繝�繝�繧ｳ繝ｳ繝昴�ｼ繝阪Φ繝�[Component_InputSystem.h]
// 
//									Date:20250520
//									Author:Yuuki Otokawa
// ========================================================

#pragma once

//==========================================================================
// 繝倥ャ繝繝ｼ繧､繝ｳ繧ｯ繝ｫ繝ｼ繝�
//==========================================================================

#include "Component.h"

#include "Keyboard.h"
#include "Mouse.h"
#include "Gamepad.h"

#include "../ComponentFactory.h"

class InputSystem :
    public Component
{
private:
	static GamePad* m_pGamePad;
	static Keyboard* m_pKeyboard;
	static Mouse* m_pMouse;

public:
    DECLARE_COMPONENT(InputSystem)

	InputSystem();

	//蜷�繝�繝舌う繧ｹ縺ｮ蜈･蜉帛､縺ｮ蜿門ｾ励�ｻ譖ｴ譁ｰ
	void UpdateComponent() override;
	void DrawGUI() override;

	//蜷�遞ｮ繧ｲ繝�繧ｿ繝ｼ
	static GamePad* GetGamePad() { return m_pGamePad; }
	static Keyboard* GetKeyboard() { return m_pKeyboard; }
	static Mouse* GetMouse() { return m_pMouse; }

	void InitializeTag() override
	{
		owner->SetTag(GameObjectTagLayer::InputSystemTag);
        owner->SetLayer(GameObjectLayer::InputSystemLayer);
		owner->SetDrawable(false);
	}
};

