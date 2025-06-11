//==========================================================================
// キーボード
//==========================================================================
#include "Keyboard.h"
#include <Windows.h>
#include <assert.h>
#include "framework.h"

#include <memory>

#include "DirectXMath.h"
using namespace DirectX;

Keyboard* Keyboard::m_pInstance = nullptr;

static_assert(sizeof(Keyboard_State) == 256 / 8, "キーボード状態構造体のサイズ不一致");

void Keyboard::KeyUp(int key) {
	if (key < 0 || key > 0xfe) { return; }

	unsigned int* p = (unsigned int*)&m_RepeatState;
	unsigned int bf = 1u << (key & 0x1f);
	p[(key >> 5)] &= ~bf;
}

void Keyboard::KeyDown(int key) {
	if (key < 0 || key > 0xfe) { return; }

	unsigned int* p = (unsigned int*)&m_RepeatState;
	unsigned int bf = 1u << (key & 0x1f);
	p[(key >> 5)] |= bf;
}

void Keyboard::Reset() {
	ZeroMemory(&m_UpState, sizeof(Keyboard_State));
	ZeroMemory(&m_DownState, sizeof(Keyboard_State));
	ZeroMemory(&m_RepeatState, sizeof(Keyboard_State));
	ZeroMemory(&m_LastState, sizeof(Keyboard_State));
}

void Keyboard::Update() {
	const Keyboard_State* now_state = &m_RepeatState;

	LPBYTE pt = (LPBYTE)&m_DownState;
	LPBYTE pn = (LPBYTE)now_state;
	LPBYTE po = (LPBYTE)&m_LastState;

	for (int i = 0; i < sizeof(Keyboard_State); i++) {
		pt[i] = pn[i] ^ po[i] & pn[i];//押した瞬間のみpt[i]=1 それ以外は0
	}

	LPBYTE pu = (LPBYTE)&m_UpState;

	for (int n = 0; n < sizeof(WORD); n++) {
		pu[n] = (pn[n] ^ po[n]) & po[n];
	}

	m_LastState = *now_state;

	COORD coord;
	coord.X = 0;
	coord.Y = 25;
	SetConsoleCursorPosition(
		GetStdHandle(STD_OUTPUT_HANDLE),
		coord
	);
	printf("%d", GetKeyRepeat(KK_UP));
}

bool Keyboard::GetKeyUp(Keyboard_Keys key) {
	if (key >= 0xfe) return false;

	unsigned int* p = (unsigned int*)&m_UpState;
	unsigned int bf = 1u << (key & 0x1f);
	return (p[(key >> 5)] & bf) != 0;
}

bool Keyboard::GetKeyDown(Keyboard_Keys key) {
	if (key >= 0xfe) return false;

	unsigned int* p = (unsigned int*)&m_DownState;
	unsigned int bf = 1u << (key & 0x1f);
	return (p[(key >> 5)] & bf) != 0;
}

bool Keyboard::GetKeyRepeat(Keyboard_Keys key) {
	if (key >= 0xfe) return false;

	unsigned int* p = (unsigned int*)&m_RepeatState;
	unsigned int bf = 1u << (key & 0x1f);
	return (p[(key >> 5)] & bf) != 0;
}

void Keyboard::KeyboardProcessMessage(UINT message, WPARAM wParam, LPARAM lParam) {
	bool down = false;

	switch (message) {
	case WM_ACTIVATEAPP:
		Reset();
		return;

	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		down = true;
		break;

	case WM_KEYUP:
	case WM_SYSKEYUP:
		break;

	default:
		return;
	}

	int vk = (int)wParam;
	switch (vk) {
	case VK_SHIFT:
		vk = (int)MapVirtualKey(((unsigned int)lParam & 0x00ff0000) >> 16u, MAPVK_VSC_TO_VK_EX);
		if (!down) {
			// 左シフトと右シフトの両方が同時に押された場合にクリアされるようにするための回避策
			KeyUp(VK_LSHIFT);
			KeyUp(VK_RSHIFT);
		}
		break;

	case VK_CONTROL:
		vk = ((UINT)lParam & 0x01000000) ? VK_RCONTROL : VK_LCONTROL;
		break;

	case VK_MENU:
		vk = ((UINT)lParam & 0x01000000) ? VK_RMENU : VK_LMENU;
		break;
	}

	if (down) {
		KeyDown(vk);
	}
	else {
		KeyUp(vk);
	}
}
