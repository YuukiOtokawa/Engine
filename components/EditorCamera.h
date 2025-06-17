// ========================================================
//
// エディターカメラコンポーネントクラス[EditorCamera.h]
// 
//									Date:20250602
//									Author:Yuuki Otokawa
// ========================================================

#pragma once

#include "Component_Camera.h"

class EditorCamera :
    public Camera
{
private:
    XMINT2 m_OriginalCursorPosition = { 0, 0 };
    POINT m_OriginalScreenPosition = { 0, 0 };
public:
    EditorCamera() = default;
	~EditorCamera() = default;
    void UpdateComponent() override;

};

