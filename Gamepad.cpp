#include "framework.h"
#include "Gamepad.h"

#include <memory>

GamePad::GamePad() {

	//メンバ変数の初期化
	for (int i = 0; i < XUSER_MAX_COUNT; i++) {
		ZeroMemory(&m_XInputState, sizeof(XINPUT_STATE));
		ZeroMemory(&m_GamePadState[i].NowState, sizeof(GAMEPADSTATE));
		ZeroMemory(&m_GamePadState[i].TriggerState, sizeof(GAMEPADSTATE));
		ZeroMemory(&m_GamePadState[i].UpState, sizeof(GAMEPADSTATE));
		ZeroMemory(&m_GamePadState[i].LastState, sizeof(GAMEPADSTATE));
	}
}

void GamePad::Update() {
	for (DWORD i = 0; i < XUSER_MAX_COUNT; i++) {

		//コントローラー情報の取得
		m_dwResult = XInputGetState(i, &m_XInputState);

		if (m_dwResult == ERROR_SUCCESS) {//コントローラーが接続されている

			//NowStateにトリガー情報を追加
			m_GamePadState[i].NowState = m_XInputState.Gamepad.wButtons;
			if ((int)m_XInputState.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD) {//左トリガーが入力されている(誤作動防止の閾値を超えた)
				m_GamePadState[i].NowState |= XINPUT_GAMEPAD_LEFTTRIGGER;
			}
			if ((int)m_XInputState.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD) {//右トリガーが入力されている(誤作動防止の閾値を超えた)
				m_GamePadState[i].NowState |= XINPUT_GAMEPAD_RIGHTTRIGGER;
			}

			//NowStateを元にTriggerState,UpStateを更新
			const WORD* nowState = &m_GamePadState[i].NowState;

			LPBYTE pt = (LPBYTE)&m_GamePadState[i].TriggerState;
			LPBYTE pl = (LPBYTE)&m_GamePadState[i].LastState;
			LPBYTE pn = (LPBYTE)nowState;
			//ビットごとにチェック
			for (int n = 0; n < sizeof(WORD); n++) {
				//pn = 1, pl = 0 のときだけpt = 1 ->押されている
				pt[n] = pn[n] ^ pl[n] & pn[n];
			}

			LPBYTE pu = (LPBYTE)&m_GamePadState[i].UpState;
			//ビットごとにチェック
			for (int n = 0; n < sizeof(WORD); n++) {
				//pn = 0, pl = 1 のときだけpt = 1 ->押されていない
				pu[n] = (pn[n] ^ pl[n]) & pl[n];
			}

			//最後にLastStateを更新
			m_GamePadState[i].LastState = *nowState;


			//スティックの入力値がデッドゾーン以下だった場合丸める => 触っただけで反応した！ を防ぐ
			if ((m_XInputState.Gamepad.sThumbLX <  XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
				 m_XInputState.Gamepad.sThumbLX > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) &&
				(m_XInputState.Gamepad.sThumbLY <  XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
				 m_XInputState.Gamepad.sThumbLY > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)) {
				m_XInputState.Gamepad.sThumbLX = 0;
				m_XInputState.Gamepad.sThumbLY = 0;
			}
			if ((m_XInputState.Gamepad.sThumbRX <  XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE &&
				 m_XInputState.Gamepad.sThumbRX > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) &&
				(m_XInputState.Gamepad.sThumbRY <  XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE &&
				 m_XInputState.Gamepad.sThumbRY > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)) {
				m_XInputState.Gamepad.sThumbRX = 0;
				m_XInputState.Gamepad.sThumbRY = 0;
			}

			//関数で読み出す用のメンバ変数に記録
			m_GamePadState[i].StickValue[GAMEPAD_STICK_RIGHT] = XMFLOAT2(m_XInputState.Gamepad.sThumbRX, m_XInputState.Gamepad.sThumbRY);
			m_GamePadState[i].StickValue[GAMEPAD_STICK_LEFT] = XMFLOAT2(m_XInputState.Gamepad.sThumbLX, m_XInputState.Gamepad.sThumbLY);

		}
		else {
			//コントローラーは接続されていない
			//ので何もできない
		}
	}
}

bool GamePad::GetKeyUp(GAMEPADBUTTON keycord, int deviceID) {
	if (keycord > 0x8000) return false;

	//キーコードに対応するビットが1だったらtrueが返る
	return m_GamePadState[deviceID].UpState & keycord;
}

bool GamePad::GetKeyDown(GAMEPADBUTTON keycord, int deviceID) {
	if (keycord > 0x8000) return false;

	//キーコードに対応するビットが1だったらtrueが返る
	return m_GamePadState[deviceID].TriggerState & keycord;
}

bool GamePad::GetKeyRepeat(GAMEPADBUTTON keycord, int deviceID) {
	if (keycord > 0x8000) return false;

	//キーコードに対応するビットが1だったらtrueが返る
	return m_GamePadState[deviceID].NowState & keycord;
}

XMFLOAT2 GamePad::GetAxis(GAMEPADAXIS axis, int deviceID) {
	//スティックに対応する入力値を返す(スティックごとに2軸まとめて)
	return m_GamePadState[deviceID].StickValue[axis];
}


void GamePad::ControllerDebug() {
	//デバッグ用プログラム。ボタン一個ずつ入力値を呼んでるだけ
	COORD coord;
	coord.X = 0;
	coord.Y = 0;
	SetConsoleCursorPosition(
		GetStdHandle(STD_OUTPUT_HANDLE),
		coord
	);
	printf("==============================================================\n");
	printf("\n");
	WORD State0 = m_GamePadState[0].NowState;
	for (int i = 0; i < 16; i++) {
		printf("%d", State0 % 2);
		State0 >>= 1;

	}
	printf("\n");
	printf("A : %d Start	: %d\n", GetKeyRepeat(GAMEPAD_A, 0), GetKeyRepeat(GAMEPAD_START, 0));
	printf("B : %d Back	: %d\n", GetKeyRepeat(GAMEPAD_B, 0), GetKeyRepeat(GAMEPAD_BACK, 0));
	printf("X : %d Up	: %d\n", GetKeyRepeat(GAMEPAD_X, 0), GetKeyRepeat(GAMEPAD_DPAD_UP, 0));
	printf("Y : %d Down	: %d\n", GetKeyRepeat(GAMEPAD_Y, 0), GetKeyRepeat(GAMEPAD_DPAD_DOWN, 0));
	printf("LB: %d Right	: %d\n", GetKeyRepeat(GAMEPAD_LEFT_BUTTON, 0), GetKeyRepeat(GAMEPAD_DPAD_RIGHT, 0));
	printf("RB: %d Left	: %d\n", GetKeyRepeat(GAMEPAD_RIGHT_BUTTON, 0), GetKeyRepeat(GAMEPAD_DPAD_LEFT, 0));
	printf("LT: %d LStick: %7.0f %7.0f\n", GetKeyRepeat(GAMEPAD_LEFT_TRIGGER, 0), GetAxis(GAMEPAD_STICK_LEFT, 0).x, GetAxis(GAMEPAD_STICK_LEFT, 0).y);
	printf("RT: %d RStick: %7.0f %7.0f\n", GetKeyRepeat(GAMEPAD_RIGHT_TRIGGER, 0), GetAxis(GAMEPAD_STICK_RIGHT, 0).x, GetAxis(GAMEPAD_STICK_RIGHT, 0).y);

	printf("\n");

}
