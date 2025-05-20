#pragma once

// ========================================================
//
// よく使うやつ置き場[FrameWork.h]
// 
//									Date:20250514
//									Author:Yuuki Otokawa
// ========================================================

//==========================================================================
// ヘッダーインクルード
//==========================================================================

#include <Windows.h>

#include <string>

#include <DirectXMath.h>
using namespace DirectX;

//==========================================================================
// 定数
//==========================================================================

constexpr auto SCREEN_WIDTH_DEFAULT = 1920;
constexpr auto SCREEN_HEIGHT_DEFAULT = 1080;

constexpr auto WINDOW_CAPTION = "OtokawaEngine";
constexpr auto WINDOW_CLASS_NAME = "OtokawaEngineClass";

constexpr auto FRAME_RATE_MAX = 240;
constexpr auto FRAME_RATE_MIN = 30;
constexpr auto FRAME_RATE_DEFAULT = 60;

//==========================================================================
// マクロ定義
//==========================================================================

#define SAFE_RELEASE(p) { if (p) { p->Release(); p = NULL; } }

//==========================================================================
// 構造体定義
//==========================================================================

struct ConstantBuffer {
	XMFLOAT4X4 wvp;
};

struct Vertex {
	Vector4O position;
	Vector4O normal;
	Vector4O color;
	Vector4O texcoord;
};

enum GameObjectTag {
	SystemTag,
};

enum GameObjectLayer {
	SystemLayer,
};


enum ComponentTag {
	TransformTag,
	MeshTag,
	TextureTag,
	MaterialTag,
	CameraTag,
	LightTag,
	ParticleTag,
	SoundTag,
	ScriptTag,
	InputSystemTag,
};
