// ========================================================
//
// コンポーネント基底クラス[Component.h]
// 
//									Date:	20250520
//									Author:	Yuuki Otokawa
// ========================================================

#pragma once

//==========================================================================
// ヘッダーインクルード
//==========================================================================

#include "FrameWork.h"
#include "Object.h"

using namespace ComponentTag;

//==========================================================================
// クラス定義
//==========================================================================

class Component
{
protected:
	// このコンポーネントが所属するオブジェクト
	Object* owner = nullptr;

	// trueならUpdateやDrawが呼ばれる
	bool isActive = true;

	// コンポーネントのタグ
	Tag tag;

public:
	// デフォルトコンストラクタとデストラクタ
	Component() = default;
	~Component() = default;
	
	/// @brief オブジェクトの所有者を設定し、タグを初期化します。
	/// @param obj 新しい所有者となるObject型のポインタ。
	virtual void SetOwner(Object* obj) { 
		owner = obj;
		InitializeTag();
	}

	/// @brief コンポーネントに対応したタグを所有者のオブジェクトに設定します。
	virtual void InitializeTag() = 0;

	/// @brief オブジェクトの状態を更新します。
	virtual void UpdateComponent() = 0;

	/// @brief 図形やオブジェクトを描画するための仮想関数です。
	virtual void Draw() {}

	/// @brief GUI を描画するための仮想関数です。
	virtual void DrawGUI() {}
};

