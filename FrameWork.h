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

#include "imgui.h"

#undef GetObject

//==========================================================================
// 定数
//==========================================================================

constexpr auto SCREEN_WIDTH_DEFAULT = 1920;
constexpr auto SCREEN_HEIGHT_DEFAULT = 1080;

constexpr auto WINDOW_CAPTION = "Engine";
constexpr auto WINDOW_CLASS_NAME = "OtokawaEngineClass";

constexpr auto FRAME_RATE_MAX = 240;
constexpr auto FRAME_RATE_MIN = 30;
constexpr auto FRAME_RATE_DEFAULT = 60;

//==========================================================================
// マクロ定義
//==========================================================================

#define SAFE_RELEASE(p) { if (p) { p->Release(); p = NULL; } }

#define OBJECT_TAG \
	X(SystemTag, "System")\
	X(CameraTag, "Camera")\
	X(ObjectTag, "Object")\
	X(InputSystemTag, "InputSystem")

#define OBJECT_LAYER \
	X(SystemLayer, "System")

#define COMPONENT_TAG \
	X(TransformTag, "Transform")\
	X(MeshTag, "Mesh")\
	X(TextureTag, "Texture")\
	X(MaterialTag, "Material")\
	X(CameraTag, "Camera")\
	X(LightTag, "Light")\
	X(ParticleTag, "Particle")\
	X(SoundTag, "Sound")\
	X(ScriptTag, "Script")\
	X(InputSystemTag, "InputSystem")


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
	XMFLOAT4 Direction;//方向ベクトル
	XMFLOAT4 Diffuse;//拡散光
	XMFLOAT4 Ambient;//環境光

	XMFLOAT4 SkyColor;//空の色
	XMFLOAT4 GroundColor;//地面の色
	XMFLOAT4 GroundNormal;//地面の法線ベクトル

	XMFLOAT4 Position;//点光源の位置
	XMFLOAT4 PointLightParam;//点光源のパラメータ（半径など）
};

namespace GameObjectTagLayer {
#define X(EnumName, StringName) EnumName,
	enum GameObjectTag {
		OBJECT_TAG
	};
#undef X

#define X(EnumName, StringName) StringName,
	const char* const GameObjectTagString[] = {
		OBJECT_TAG
	};
#undef X

#define X(EnumName, StringName) EnumName,
	enum GameObjectLayer {
		OBJECT_LAYER
	};
#undef X

#define X(EnumName, StringName) StringName,
	const char* const GameObjectLayerString[] = {
		OBJECT_LAYER
	};
#undef X
}

namespace ComponentTag {
#define X(EnumName, StringName) EnumName,
	enum Tag {
		COMPONENT_TAG
	};
#undef X

#define X(EnumName, StringName) StringName,
	const char* const GameObjectTagString[] = {
		COMPONENT_TAG
	};
#undef X
}

