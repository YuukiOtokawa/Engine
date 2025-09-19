// ========================================================
//
// コントローラークラス[Gamepad.h]
// 
//									Date:20250114
//									Author:Yuuki Otokawa
// ========================================================

//==========================================================================
// 
// 20250116現在XInputにのみ対応
// DirectInputにも対応したい
// 
//==========================================================================

#pragma once

#include <DirectXMath.h>
using namespace DirectX;

#include "Xinput.h"

//==========================================================================
// コントローラー
//==========================================================================

constexpr auto XINPUT_GAMEPAD_LEFTTRIGGER = 0x0400;
constexpr auto XINPUT_GAMEPAD_RIGHTTRIGGER = 0x0800;

/// @brief ゲームパッドの各種ボタンのキーコードにトリガーを追加したもの
enum GAMEPADBUTTON {
	GAMEPAD_A = XINPUT_GAMEPAD_A,
	GAMEPAD_B = XINPUT_GAMEPAD_B,
	GAMEPAD_X = XINPUT_GAMEPAD_X,
	GAMEPAD_Y = XINPUT_GAMEPAD_Y,
	GAMEPAD_RIGHT_STICK_BUTTON = XINPUT_GAMEPAD_RIGHT_THUMB,
	GAMEPAD_LEFT_STICK_BUTTON = XINPUT_GAMEPAD_LEFT_THUMB,
	GAMEPAD_RIGHT_BUTTON = XINPUT_GAMEPAD_RIGHT_SHOULDER,
	GAMEPAD_LEFT_BUTTON = XINPUT_GAMEPAD_LEFT_SHOULDER,
	GAMEPAD_RIGHT_TRIGGER = XINPUT_GAMEPAD_RIGHTTRIGGER,
	GAMEPAD_LEFT_TRIGGER = XINPUT_GAMEPAD_LEFTTRIGGER,
	GAMEPAD_START = XINPUT_GAMEPAD_START,
	GAMEPAD_BACK = XINPUT_GAMEPAD_BACK,
	GAMEPAD_DPAD_UP = XINPUT_GAMEPAD_DPAD_UP,
	GAMEPAD_DPAD_DOWN = XINPUT_GAMEPAD_DPAD_DOWN,
	GAMEPAD_DPAD_RIGHT = XINPUT_GAMEPAD_DPAD_RIGHT,
	GAMEPAD_DPAD_LEFT = XINPUT_GAMEPAD_DPAD_LEFT

};

/// @brief ゲームパッドのスティック軸コード 関数の引数用
enum GAMEPADAXIS {
	GAMEPAD_STICK_RIGHT,
	GAMEPAD_STICK_LEFT,
	GAMEPAD_STICK_MAX
};

/// @brief ゲームパッドの各種ステートとスティックの値の構造体
struct GAMEPADSTATE {
	WORD NowState;
	WORD TriggerState;
	WORD UpState;
	WORD LastState;
	Vector4O StickValue[GAMEPAD_STICK_MAX];
};

/// @brief ゲームパッドクラス宣言
class GamePad {
private:
	/// @brief コントローラーから送られてくるやつ
	XINPUT_STATE m_XInputState;

	/// @brief m_XInputStateをコントローラーごとに関数で読み出せるように加工したやつ
	GAMEPADSTATE m_GamePadState[XUSER_MAX_COUNT];

	DWORD m_dwResult;
public:
	/// @brief コンストラクタ 各種ステートのメンバ変数を初期化
	GamePad();

	/// @brief ゲームパッドの状態を取得、各種ステートの更新
	void Update();

	/// @brief ゲームパッドのボタンを離した瞬間を検知
	/// @param keycord ボタンコード GAMEPADBUTTON列挙体を使用
	/// @param deviceID コントローラーID 0~3
	/// @return 
	bool GetKeyUp(GAMEPADBUTTON keycord, int deviceID);

	/// @brief ゲームパッドのボタンを押した瞬間を検知
	/// @param keycord ボタンコード GAMEPADBUTTON列挙体を使用
	/// @param deviceID コントローラーID 0~3
	/// @return 
	bool GetKeyDown(GAMEPADBUTTON keycord, int deviceID);

	/// @brief ゲームパッドのボタンを押している間検知
	/// @param keycord ボタンコード GAMEPADBUTTON列挙体を使用
	/// @param deviceID コントローラーID 0~3
	/// @return 
	bool GetKeyRepeat(GAMEPADBUTTON keycord, int deviceID);

	/// @brief ゲームパッドのスティック入力値を取得
	/// @param axis 左右スティックを選択 GAMEPADAXIS列挙体を使用
	/// @param deviceID コントローラーID 0~3
	/// @return 
	Vector4O GetAxis(GAMEPADAXIS axis, int deviceID);

	/// @brief デバッグ用 コンソールにコントローラー入力状況を出力
	void ControllerDebug();
};
