// ========================================================
//
// トランスフォームコンポーネント[Component_Transform.h]
// 
//									Date:202505220
//									Author:Yuuki Otokawa
// ========================================================

#pragma once

//==========================================================================
// ヘッダーインクルード
//==========================================================================

#include "Component.h"

#include "Vector4O.h"

//==========================================================================
// クラス定義
//==========================================================================

class Transform :
    public Component
{
private:
	// 座標
	Vector4O m_Position = Vector4O(0.0f,0.0f,0.0f);
	// 回転（ピッチ、ヨー、ロール）
	Vector4O m_Rotation = Vector4O(0.0f, 0.0f, 0.0f);
	// スケール（X, Y, Z）
	Vector4O m_Scale = Vector4O(1.0f, 1.0f, 1.0f);

public:
	/// @brief オブジェクトの状態を更新します。
	void UpdateComponent() override;
	/// @brief 描画処理を実行します。
	void Draw() override;
	void DrawGUI() override;

	/// @brief コンポーネントに対応したタグを所有者のオブジェクトに設定します。
	void InitializeTag() override;

	/// @brief 位置を設定します。
	/// @param position 設定する位置を表す Vector4O 型の値。
	void SetPosition(Vector4O position) { m_Position = position; }
	/// @brief 回転を設定します。
	/// @param rotation 設定する回転値。
	void SetRotation(Vector4O rotation) { m_Rotation = rotation; }
	/// @brief スケール値を設定します。
	/// @param scale 設定するスケール値。
	void SetScale(Vector4O scale) { m_Scale = scale; }

	/// @brief 現在の位置ベクトルを取得します。
	/// @return 現在の位置を表す Vector4O 型の値。
	Vector4O GetPosition() { return m_Position; }
	/// @brief 現在の回転を表す Vector4O オブジェクトを取得します。
	/// @return 現在の回転を表す Vector4O 型の値。
	Vector4O GetRotation() { return m_Rotation; }
	/// @brief スケール値を取得します。
	/// @return オブジェクトのスケールを表す Vector4O 型の値。
	Vector4O GetScale() { return m_Scale; }
};

