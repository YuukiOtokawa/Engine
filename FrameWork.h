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

#undef GetObject

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

struct CONSTANTBUFFER {
	XMFLOAT4X4 wvp;
	XMFLOAT4X4 world;
};

struct VERTEX {
	XMFLOAT3 position;
	XMFLOAT3 normal;
	XMFLOAT4 color;
	XMFLOAT2 texcoord;
};

struct MATERIAL
{
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
	XMFLOAT4 specular;
	XMFLOAT4 emissive;

	BOOL textureEnable;
	float shininess;
	float dummy[2]; // Padding to make it 16 bytes
};

struct LIGHT {
	short Enable = true;
	BOOL Dummy[3]; // Padding to make it 16 bytes
	XMFLOAT4 Direction;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Ambient;

	XMFLOAT4 SkyColor;
	XMFLOAT4 GroundColor;
	XMFLOAT4 GroundNormal;

	XMFLOAT4 Position;
	XMFLOAT4 PointLightParam;
};

namespace GameObjectTagLayer {
	enum GameObjectTag {
		SystemTag,
		CameraTag,
		ObjectTag,
		InputSystemTag,
	};

	enum GameObjectLayer {
		SystemLayer,
	};
}

namespace ComponentTag {
	enum Tag {
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
}
