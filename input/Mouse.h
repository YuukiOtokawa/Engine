// ========================================================
//
// マウスクラス[Mouse.h]
// 
//									Date:20250115
//									Author:Yuuki Otokawa
// ========================================================
#pragma once
#include "FrameWork.h"

//==========================================================================
// マウス
//==========================================================================

// マウスモード
typedef enum Mouse_PositionMode_tag {
	MOUSE_POSITION_MODE_ABSOLUTE, // 絶対座標モード ウィンドウ上の実際の座標を取得
	MOUSE_POSITION_MODE_RELATIVE, // 相対座標モード 前回の座標からの移動量を取得 カーソルが映らなくなるので注意
} Mouse_PositionMode;

//マウスボタン列挙体 関数用
enum MOUSE_BUTTON {
	LeftButton,
	MiddleButton,
	RightButton,
	XButton1,//サイドボタン手前
	XButton2,//サイドボタン奥
	Button_Max
};

// マウス状態構造体
typedef struct MouseState_tag {
	bool ButtonUp[Button_Max];
	bool ButtonDown[Button_Max];
	bool ButtonRepeat[Button_Max];
	bool ButtonLast[Button_Max];
	XMINT2 Position;
	int scrollWheelValue;
	Mouse_PositionMode positionMode;
} Mouse_State;

/// @brief マウスクラス定義
class Mouse {
private:
	Mouse_State			m_State;					//マウスの状態
	HWND				m_Window;					//ウィンドウハンドル
	Mouse_PositionMode	m_Mode;						//マウスの座標取得モード Mouse_PositionMode列挙体も確認すること
	HANDLE				m_ScrollWheelValue = NULL;	//マウスホイールの累計回転量
	HANDLE				m_RelativeRead = NULL;		//
	HANDLE				m_AbsoluteMode = NULL;
	HANDLE				m_RelativeMode = NULL;
	XMINT2				m_LastPosition;				//前フレームの座標
	XMINT2				m_RelativePosition;			//前フレームからの座標移動量
	bool				m_InFocus;

	void ClipToWindow();
	/// @brief コンストラクタ メンバ変数の初期化 マウスに必要な設定
	/// @param hWnd ウィンドウハンドル
	Mouse(HWND hWnd);
	
	POINT m_SavedCursorPos; // 相対モードに切り替える前のカーソル位置を保存

	static Mouse* m_Instance;	//インスタンス
public:
	~Mouse();

	/// @brief 各種ステートの更新
	void Update();

	// カーソル位置を保存するメソッド
	void SaveCursorPosition();

	// 保存したカーソル位置を復元するメソッド
	void RestoreCursorPosition();

	static Mouse* GetInstance(HWND hWnd = NULL) {
		if (m_Instance == NULL) {
			m_Instance = new Mouse(hWnd);
		}
		return m_Instance;
	}

	/// @brief ホイールの移動量をリセット フレームごとに回すとフレームごとの移動量を取得できる
	void ResetScrollWheelValue();

	/// @brief マウスの座標取得モードを設定
	/// @param mode 切り替え先モード Mouse_PositionMode列挙体も確認すること
	void SetMode(Mouse_PositionMode mode);

	/// @brief 接続状態を確認
	/// @return 
	bool GetIsConnected();

	/// @brief カーソルが見える状態か確認
	/// @return 
	bool GetIsVisible();

	/// @brief マウスボタンを離した瞬間を検知
	/// @param button ボタンコード MOUSE_BUTTON列挙体を使用
	/// @return 
	bool GetUp(MOUSE_BUTTON button);

	/// @brief マウスボタンを押した瞬間を検知
	/// @param button ボタンコード MOUSE_BUTTON列挙体を使用
	/// @return 
	bool GetDown(MOUSE_BUTTON button);

	/// @brief マウスボタンを押している間検知
	/// @param button ボタンコード MOUSE_BUTTON列挙体を使用
	/// @return 
	bool GetRepeat(MOUSE_BUTTON button);

	/// @brief カーソルのウィンドウ上座標を取得 m_Modeによって返り値の性質が異なる Mouse_PositionMode参照
	/// @return 
	XMINT2 GetPosition();

	/// @brief ホイールの累計回転量を取得
	/// @return 
	int GetWheelValue();

	Mouse_PositionMode GetMode();

	/// @brief カーソルを見えるように/見えなくなるようにする
	/// @param visible true:見える false:見えなくなる
	void SetIsVisible(bool visible);

	void ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam);

	void MouseDebug();
};
