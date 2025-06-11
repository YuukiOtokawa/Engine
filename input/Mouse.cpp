//==========================================================================
// マウス
//==========================================================================
#include "Mouse.h"
#include "framework.h"

#include <memory>

#include "DirectXMath.h"
using namespace DirectX;

#include <Windows.h>
#include <windowsx.h>
#include <windef.h>

#define SAFE_CLOSEHANDLE(h) if (h) {CloseHandle(h); h = NULL;}

Mouse* Mouse::m_Instance;

void Mouse::ClipToWindow() {
	assert(m_Window != NULL);

	RECT rect;
	GetClientRect(m_Window, &rect);

	POINT ul;
	ul.x = rect.left;
	ul.y = rect.top;

	POINT lr;
	lr.x = rect.right;
	lr.y = rect.bottom;

	MapWindowPoints(m_Window, NULL, &ul, 1);
	MapWindowPoints(m_Window, NULL, &lr, 1);

	rect.left = ul.x;
	rect.top = ul.y;

	rect.right = lr.x;
	rect.bottom = lr.y;

	ClipCursor(&rect);
}

Mouse::Mouse(HWND window) : m_Window(window) {
	RtlZeroMemory(&m_State, sizeof(m_State));

	assert(window != NULL);

	RAWINPUTDEVICE Rid;
	Rid.usUsagePage = 0x01;
	Rid.usUsage = 0x02;
	Rid.dwFlags = RIDEV_INPUTSINK;
	Rid.hwndTarget = window;
	RegisterRawInputDevices(&Rid, 1, sizeof(RAWINPUTDEVICE));

	m_Mode = MOUSE_POSITION_MODE_ABSOLUTE;	//絶対座標モード 実際の座標を取得 UIのクリック処理など
	//m_Mode = MOUSE_POSITION_MODE_RELATIVE;	//相対座標モード 前回の座標からの差分を取得 FPSゲームなど

	if (!m_ScrollWheelValue) { m_ScrollWheelValue = CreateEventEx(nullptr, nullptr, CREATE_EVENT_MANUAL_RESET, EVENT_MODIFY_STATE | SYNCHRONIZE); }
	if (!m_RelativeRead) { m_RelativeRead = CreateEventEx(nullptr, nullptr, CREATE_EVENT_MANUAL_RESET, EVENT_MODIFY_STATE | SYNCHRONIZE); }
	if (!m_AbsoluteMode) { m_AbsoluteMode = CreateEventEx(nullptr, nullptr, 0, EVENT_MODIFY_STATE | SYNCHRONIZE); }
	if (!m_RelativeMode) { m_RelativeMode = CreateEventEx(nullptr, nullptr, 0, EVENT_MODIFY_STATE | SYNCHRONIZE); }

	m_LastPosition = XMINT2(0, 0);
	m_RelativePosition = XMINT2(INT32_MAX, INT32_MAX);

	m_InFocus = true;
}

Mouse::~Mouse() {
	SAFE_CLOSEHANDLE(m_ScrollWheelValue);
	SAFE_CLOSEHANDLE(m_RelativeRead);
	SAFE_CLOSEHANDLE(m_AbsoluteMode);
	SAFE_CLOSEHANDLE(m_RelativeMode);
}

void Mouse::Update() {
	for (int i = 0; i < Button_Max; i++) {
		m_State.ButtonDown[i] = m_State.ButtonRepeat[i] ^ m_State.ButtonLast[i] & m_State.ButtonRepeat[i];
		m_State.ButtonUp[i] = m_State.ButtonRepeat[i] ^ m_State.ButtonLast[i] & m_State.ButtonLast[i];

		m_State.ButtonLast[i] = m_State.ButtonRepeat[i];
	}
	//m_State.Position = { 0,0 };
	MouseDebug();
}

void Mouse::ResetScrollWheelValue() {
	SetEvent(m_ScrollWheelValue);
}

void Mouse::SetMode(Mouse_PositionMode mode) {
	if (m_Mode == mode)
		return;


	SetEvent((mode == MOUSE_POSITION_MODE_ABSOLUTE) ? m_AbsoluteMode : m_RelativeMode);

	assert(m_Window != NULL);

	TRACKMOUSEEVENT tme;
	tme.cbSize = sizeof(tme);
	tme.dwFlags = TME_HOVER;
	tme.hwndTrack = m_Window;
	tme.dwHoverTime = 1;
	TrackMouseEvent(&tme);
}

bool Mouse::GetIsConnected() {
	return GetSystemMetrics(SM_MOUSEPRESENT) != 0;
}

bool Mouse::GetIsVisible() {
	if (m_Mode == MOUSE_POSITION_MODE_RELATIVE) {
		return false;
	}

	CURSORINFO info = { sizeof(CURSORINFO), 0, nullptr, {} };
	GetCursorInfo(&info);

	return (info.flags & CURSOR_SHOWING) != 0;
}

bool Mouse::GetUp(MOUSE_BUTTON button) {
	return m_State.ButtonUp[button];
}

bool Mouse::GetDown(MOUSE_BUTTON button) {
	return m_State.ButtonDown[button];
}

bool Mouse::GetRepeat(MOUSE_BUTTON button) {
	return m_State.ButtonRepeat[button];
}

XMINT2 Mouse::GetPosition() {
	return m_State.Position;
}

int Mouse::GetWheelValue() {
	return m_State.scrollWheelValue;
}

Mouse_PositionMode Mouse::GetMode() {
	return m_Mode;
}

void Mouse::SetIsVisible(bool visible) {
	if (m_Mode == MOUSE_POSITION_MODE_RELATIVE) {
		return;
	}

	CURSORINFO info = { sizeof(CURSORINFO), 0, nullptr, {} };
	GetCursorInfo(&info);

	bool isVisible = (info.flags & CURSOR_SHOWING) != 0;

	if (isVisible != visible) {
		ShowCursor(visible);
	}
}

void Mouse::SaveCursorPosition() {
	GetCursorPos(&m_SavedCursorPos);
}

void Mouse::RestoreCursorPosition() {
	SetCursorPos(m_SavedCursorPos.x, m_SavedCursorPos.y);
}

void Mouse::ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam) {
	HANDLE evts[3] = {
		m_ScrollWheelValue,
		m_AbsoluteMode,
		m_RelativeMode
	};

	switch (WaitForMultipleObjectsEx(_countof(evts), evts, FALSE, 0, FALSE)) {
	case WAIT_OBJECT_0:
		m_State.scrollWheelValue = 0;
		ResetEvent(evts[0]);
		break;

	case (WAIT_OBJECT_0 + 1):
	{
		m_Mode = MOUSE_POSITION_MODE_ABSOLUTE;
		ClipCursor(nullptr);

		POINT point;
		//m_LastPosition = XMINT2(point.x, point.y);

		// リモートディスクトップに対応するために移動前にカーソルを表示する
		ShowCursor(TRUE);

		if (MapWindowPoints(m_Window, nullptr, &point, 1)) {
			SetCursorPos(point.x, point.y);
		}

		m_LastPosition = XMINT2(m_State.Position);
	}
	break;

	case (WAIT_OBJECT_0 + 2):
	{
		ResetEvent(m_RelativeRead);

		m_Mode = MOUSE_POSITION_MODE_RELATIVE;
		m_State.Position = XMINT2(0, 0);
		m_RelativePosition = XMINT2(INT32_MAX, INT32_MAX);

		ShowCursor(FALSE);

		ClipToWindow();
	}
	break;

	case WAIT_FAILED:
		int a = GetLastError();
		return;
	}

	switch (message) {
	case WM_ACTIVATEAPP:
		if (wParam) {

			m_InFocus = true;

			if (m_Mode == MOUSE_POSITION_MODE_RELATIVE) {

				m_State.Position = XMINT2(0, 0);
				ShowCursor(FALSE);
				ClipToWindow();
			}
		}
		else {
			int scrollWheel = m_State.scrollWheelValue;
			memset(&m_State, 0, sizeof(m_State));
			m_State.scrollWheelValue = scrollWheel;
			m_InFocus = false;
		}
		return;

	case WM_INPUT:
		if (m_InFocus && m_Mode == MOUSE_POSITION_MODE_RELATIVE) {

			RAWINPUT raw;
			UINT rawSize = sizeof(raw);

			GetRawInputData((HRAWINPUT)lParam, RID_INPUT, &raw, &rawSize, sizeof(RAWINPUTHEADER));

			if (raw.header.dwType == RIM_TYPEMOUSE) {

				if (!(raw.data.mouse.usFlags & MOUSE_MOVE_ABSOLUTE)) {

					m_State.Position.x = raw.data.mouse.lLastX;
					m_State.Position.y = raw.data.mouse.lLastY;

					ResetEvent(m_RelativeRead);
				}
				else if (raw.data.mouse.usFlags & MOUSE_VIRTUAL_DESKTOP) {

					// リモートディスクトップなどに対応
					const int width = GetSystemMetrics(SM_CXVIRTUALSCREEN);
					const int height = GetSystemMetrics(SM_CYVIRTUALSCREEN);

					int x = (int)((raw.data.mouse.lLastX / 65535.0f) * width);
					int y = (int)((raw.data.mouse.lLastY / 65535.0f) * height);

					if (m_RelativePosition.x == INT32_MAX) {
						m_State.Position = XMINT2(0, 0);
					}
					else {
						m_State.Position.x = x - m_RelativePosition.x;
						m_State.Position.y = y - m_RelativePosition.y;
					}

					m_RelativePosition.x = x;
					m_RelativePosition.y = y;

					ResetEvent(m_RelativeRead);
				}
			}
		}
		return;


	case WM_MOUSEMOVE:
		break;

	case WM_LBUTTONDOWN:
		m_State.ButtonRepeat[LeftButton] = true;
		break;

	case WM_LBUTTONUP:
		m_State.ButtonRepeat[LeftButton] = false;
		break;

	case WM_RBUTTONDOWN:
		m_State.ButtonRepeat[RightButton] = true;
		break;

	case WM_RBUTTONUP:
		m_State.ButtonRepeat[RightButton] = false;
		break;

	case WM_MBUTTONDOWN:
		m_State.ButtonRepeat[MiddleButton] = true;
		break;

	case WM_MBUTTONUP:
		m_State.ButtonRepeat[MiddleButton] = false;
		break;

	case WM_MOUSEWHEEL:
		m_State.scrollWheelValue += GET_WHEEL_DELTA_WPARAM(wParam);
		return;

	case WM_XBUTTONDOWN:
		switch (GET_XBUTTON_WPARAM(wParam)) {
		case XBUTTON1:
			m_State.ButtonRepeat[XButton1] = true;
			break;

		case XBUTTON2:
			m_State.ButtonRepeat[XButton2] = true;
			break;
		}
		break;

	case WM_XBUTTONUP:
		switch (GET_XBUTTON_WPARAM(wParam)) {
		case XBUTTON1:
			m_State.ButtonRepeat[XButton1] = false;
			break;

		case XBUTTON2:
			m_State.ButtonRepeat[XButton2] = false;
			break;
		}
		break;

	case WM_MOUSEHOVER:
		break;

	default:
		// マウスに対するメッセージは無かった…
		return;
	}

	if (m_Mode == MOUSE_POSITION_MODE_ABSOLUTE) {

		// すべてのマウスメッセージに対して新しい座標を取得する
		int xPos = GET_X_LPARAM(lParam);
		int yPos = GET_Y_LPARAM(lParam);

		m_State.Position.x = m_LastPosition.x = xPos;
		m_State.Position.y = m_LastPosition.y = yPos;
	}
}

void Mouse::MouseDebug() {
	COORD coord;
	coord.X = 0;
	coord.Y = 14;
	SetConsoleCursorPosition(
		GetStdHandle(STD_OUTPUT_HANDLE),
		coord
	);
	printf("==============================================================\n");
	printf("\n");
	for (int i = 0; i < Button_Max; i++) {
		printf("%d", m_State.ButtonRepeat[i]);
	}
	printf("\n");
	printf("Left   %d\n", GetRepeat(LeftButton));
	printf("Middle %d\n", GetRepeat(MiddleButton));
	printf("Right  %d\n", GetRepeat(RightButton));
	printf("XB1    %d\n", GetRepeat(XButton1));
	printf("XM2    %d\n", GetRepeat(XButton2));
	printf("\n");
	printf("Pos    %5d %5d\n", GetPosition().x, GetPosition().y);
	printf("Wheel  %d\n", GetWheelValue());
	printf("\n");

}
