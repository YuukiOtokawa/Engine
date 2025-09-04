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
#include "../VectorO.h"
//#include "MatrixO.h"

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
    X(NoObject, "NoObject")\
	X(SystemTag, "System")\
	X(CameraTag, "Camera")\
	X(ObjectTag, "Object")\
	X(InputSystemTag, "InputSystem")\
    X(BillBoardTag, "BillBoard") \
    X(SpriteTag, "Sprite") \
    X(ParticleTag, "Particle") \
    X(LightTag, "Light") \
    X(PackTag, "Pack") \

#define OBJECT_LAYER \
	X(SystemLayer, "System") \
    X(CameraLayer, "Camera") \
    X(ObjectLayer, "Object") \
    X(InputSystemLayer, "InputSystem") \
    X(LightLayer, "Light") \
    X(BillBoardLayer, "BillBoard") \
    X(SpriteLayer, "Sprite") \

#define COMPONENT_TAG \
	X(NoComponent, "NoComponent") \
	X(TransformTag, "Transform") \
	X(MeshTag, "Mesh") \
	X(TextureTag, "Texture") \
	X(MaterialTag, "Material") \
	X(CameraTag, "Camera") \
	X(LightTag, "Light") \
	X(ParticleTag, "Particle") \
	X(SoundTag, "Sound") \
	X(ScriptTag, "Script") \
	X(InputSystemTag, "InputSystem") \


//==========================================================================
// 構造体定義
//==========================================================================


struct VERTEX {
	Vector3O position;
	Vector3O normal;
	Vector4O color;
	Vector2O texcoord;
};


struct MATERIAL
{
    BOOL textureEnable = TRUE;
    FLOAT shininess = 0;
    FLOAT SpecularPower = 50.0f;
    FLOAT dummy[2] = {};

	Vector4O ambient;
	Vector4O diffuse;
	Vector4O specular;
	Vector4O emissive;

    Vector4O SkyColor;
    Vector4O GroundColor;
    Vector3O GroundNormal;
};

struct LIGHT {
	BOOL Enable = true;
	//BOOL Dummy[2] = {}; // Padding to make it 16 bytes
    FLOAT SpecularPower = 100.0f;
	Vector3O Direction;
	Vector4O Diffuse;
	Vector4O Ambient;

	Vector4O SkyColor;
	Vector4O GroundColor;
	Vector3O GroundNormal;

	Vector3O Position;
	float PointLightRange;

	Vector4O SpotLightAngle;
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
	const char* const ComponentTagString[] = {
		COMPONENT_TAG
	};
#undef X
}

