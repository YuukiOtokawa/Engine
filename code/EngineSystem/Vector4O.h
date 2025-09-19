#pragma once
// ========================================================
//
// 閾ｪ菴�4谺｡蜈�繝吶け繝医Ν繧ｯ繝ｩ繧ｹ[Vector4O.h]
// 
//									Date:20250514
//									Author:Yuuki Otokawa
// ========================================================

#include <DirectXMath.h>
using namespace DirectX;

//==========================================================================
// 繧ｯ繝ｩ繧ｹ螳夂ｾｩ
//==========================================================================

struct Vector4O {
public:
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	float w = 0.0f;

	static constexpr float PI = 3.14159265358979323846f;

	Vector4O() = default;

	Vector4O(const Vector4O&) = default;

	Vector4O(const XMFLOAT2 vec2) : x(vec2.x), y(vec2.y), z(0.0f), w(0.0f) {}
	Vector4O(const XMFLOAT3 vec3) : x(vec3.x), y(vec3.y), z(vec3.z), w(0.0f) {}
	Vector4O(const XMFLOAT4 vec4) : x(vec4.x), y(vec4.y), z(vec4.z), w(vec4.w) {}

	Vector4O(const XMINT2 vec2) : x((float)vec2.x), y((float)vec2.y), z(0.0f), w(0.0f) {}
	Vector4O(const XMINT3 vec3) : x((float)vec3.x), y((float)vec3.y), z((float)vec3.z), w(0.0f) {}
	Vector4O(const XMINT4 vec4) : x((float)vec4.x), y((float)vec4.y), z((float)vec4.z), w((float)vec4.w) {}

	Vector4O(const float x, const float y = 0.0f, const float z = 0.0f, const float w = 0.0f) : x(x), y(y), z(z), w(w) {}

	XMFLOAT2* ToFloat2() const { return new XMFLOAT2(x, y); }
	XMFLOAT3* ToFloat3() const { return new XMFLOAT3(x, y, z); }
	XMFLOAT4* ToFloat4() const { return new XMFLOAT4(x, y, z, w); }
	XMINT2 ToInt2() const { return XMINT2((int)x, (int)y); }
	XMINT3 ToInt3() const { return XMINT3((int)x, (int)y, (int)z); }
	XMINT4 ToInt4() const { return XMINT4((int)x, (int)y, (int)z, (int)w); }

	static Vector4O Zero() { return Vector4O(0.0f, 0.0f, 0.0f, 0.0f); }
	static Vector4O One() { return Vector4O(1.0f, 1.0f, 1.0f, 1.0f); }
	static Vector4O Up() { return Vector4O(0.0f, 1.0f, 0.0f, 0.0f); }
	static Vector4O Down() { return Vector4O(0.0f, -1.0f, 0.0f, 0.0f); }
	static Vector4O Left() { return Vector4O(-1.0f, 0.0f, 0.0f, 0.0f); }
	static Vector4O Right() { return Vector4O(1.0f, 0.0f, 0.0f, 0.0f); }
	static Vector4O Forward() { return Vector4O(0.0f, 0.0f, 1.0f, 0.0f); }
	static Vector4O Backward() { return Vector4O(0.0f, 0.0f, -1.0f, 0.0f); }
	static Vector4O UnitX() { return Vector4O(1.0f, 0.0f, 0.0f, 0.0f); }
	static Vector4O UnitY() { return Vector4O(0.0f, 1.0f, 0.0f, 0.0f); }
	static Vector4O UnitZ() { return Vector4O(0.0f, 0.0f, 1.0f, 0.0f); }
	static Vector4O UnitW() { return Vector4O(0.0f, 0.0f, 0.0f, 1.0f); }
	static float Dot(const Vector4O& vec1, const Vector4O& vec2) { 
		return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z + vec1.w * vec2.w; 
	}

	float Length() const { return sqrtf(x * x + y * y + z * z + w * w); }
	Vector4O Normalize() { 
		float len = Length();
		if (len > 0.0f) { 
			x /= len; y /= len; z /= len; w /= len;
		}
        return *this;
	}

	Vector4O ToRadian() const {
		return Vector4O(
			x * (PI / 180.0f),
			y * (PI / 180.0f),
			z * (PI / 180.0f),
			w // w component remains unchanged
		);
	}
	Vector4O ToEuler() const {
		return Vector4O(
			x * (180.0f / PI),
			y * (180.0f / PI),
			z * (180.0f / PI),
			w // w component remains unchanged
		);
	}

	Vector4O Cross(const Vector4O& vec) const {
		return Vector4O(
			y * vec.z - z * vec.y,
			z * vec.x - x * vec.z,
			x * vec.y - y * vec.x,
			0.0f // w component is set to 0 for cross product
		);
	}

	auto operator=(const Vector4O&) -> Vector4O & = default;
	//auto operator=(const Vector4O&&) -> Vector4O & = default;

	auto operator=(const XMFLOAT2 vec2) -> Vector4O& { x = vec2.x; y = vec2.y; z = 0.0f; w = 0.0f; return *this; }
	auto operator=(const XMFLOAT3 vec3) -> Vector4O& { x = vec3.x; y = vec3.y; z = vec3.z; w = 0.0f; return *this; }
	auto operator=(const XMFLOAT4 vec4) -> Vector4O& { x = vec4.x; y = vec4.y; z = vec4.z; w = vec4.w; return *this; }
	auto operator=(const XMINT2 vec2) -> Vector4O& { x = (float)vec2.x; y = (float)vec2.y; z = 0.0f; w = 0.0f; return *this; }
	auto operator=(const XMINT3 vec3) -> Vector4O& { x = (float)vec3.x; y = (float)vec3.y; z = (float)vec3.z; w = 0.0f; return *this; }
	auto operator=(const XMINT4 vec4) -> Vector4O& { x = (float)vec4.x; y = (float)vec4.y; z = (float)vec4.z; w = (float)vec4.w; return *this; }

	auto operator +(const Vector4O& vec) const -> Vector4O { return Vector4O(x + vec.x, y + vec.y, z + vec.z, w + vec.w); }
	auto operator -(const Vector4O& vec) const -> Vector4O { return Vector4O(x - vec.x, y - vec.y, z - vec.z, w - vec.w); }
	auto operator *(const Vector4O& vec) const -> Vector4O { return Vector4O(x * vec.x, y * vec.y, z * vec.z, w * vec.w); }
	auto operator /(const Vector4O& vec) const -> Vector4O { return Vector4O(x / vec.x, y / vec.y, z / vec.z, w / vec.w); }
	auto operator *(const float scalar) const -> Vector4O { return Vector4O(x * scalar, y * scalar, z * scalar, w * scalar); }
	auto operator /(const float scalar) const -> Vector4O { return Vector4O(x / scalar, y / scalar, z / scalar, w / scalar); }
	auto operator +=(const Vector4O& vec) -> Vector4O& { x += vec.x; y += vec.y; z += vec.z; w += vec.w; return *this; }
	auto operator -=(const Vector4O& vec) -> Vector4O& { x -= vec.x; y -= vec.y; z -= vec.z; w -= vec.w; return *this; }
	auto operator *=(const Vector4O& vec) -> Vector4O& { x *= vec.x; y *= vec.y; z *= vec.z; w *= vec.w; return *this; }
	auto operator /=(const Vector4O& vec) -> Vector4O& { x /= vec.x; y /= vec.y; z /= vec.z; w /= vec.w; return *this; }
	auto operator *=(const float scalar) -> Vector4O& { x *= scalar; y *= scalar; z *= scalar; w *= scalar; return *this; }
	auto operator /=(const float scalar) -> Vector4O& { x /= scalar; y /= scalar; z /= scalar; w /= scalar; return *this; }
	auto operator ==(const Vector4O& vec) const -> bool { return (x == vec.x && y == vec.y && z == vec.z && w == vec.w); }
	auto operator !=(const Vector4O& vec) const -> bool { return (x != vec.x || y != vec.y || z != vec.z || w != vec.w); }
	auto operator <(const Vector4O& vec) const -> bool { return (x < vec.x && y < vec.y && z < vec.z && w < vec.w); }
	auto operator >(const Vector4O& vec) const -> bool { return (x > vec.x && y > vec.y && z > vec.z && w > vec.w); }
	auto operator <=(const Vector4O& vec) const -> bool { return (x <= vec.x && y <= vec.y && z <= vec.z && w <= vec.w); }
	auto operator >=(const Vector4O& vec) const -> bool { return (x >= vec.x && y >= vec.y && z >= vec.z && w >= vec.w); }

	auto operator [](const int index) const -> float { return (index == 0) ? x : (index == 1) ? y : (index == 2) ? z : w; }

	auto operator +(const int scalar) const -> Vector4O { return Vector4O(x + (float)scalar, y + (float)scalar, z + (float)scalar, w + (float)scalar); }
	auto operator -(const int scalar) const -> Vector4O { return Vector4O(x - (float)scalar, y - (float)scalar, z - (float)scalar, w - (float)scalar); }
	auto operator *(const int scalar) const -> Vector4O { return Vector4O(x * (float)scalar, y * (float)scalar, z * (float)scalar, w * (float)scalar); }
	auto operator /(const int scalar) const -> Vector4O { return Vector4O(x / (float)scalar, y / (float)scalar, z / (float)scalar, w / (float)scalar); }
	auto operator +=(const int scalar) -> Vector4O& { x += (float)scalar; y += (float)scalar; z += (float)scalar; w += (float)scalar; return *this; }
	auto operator -=(const int scalar) -> Vector4O& { x -= (float)scalar; y -= (float)scalar; z -= (float)scalar; w -= (float)scalar; return *this; }
	auto operator *=(const int scalar) -> Vector4O& { x *= (float)scalar; y *= (float)scalar; z *= (float)scalar; w *= (float)scalar; return *this; }
	auto operator /=(const int scalar) -> Vector4O& { x /= (float)scalar; y /= (float)scalar; z /= (float)scalar; w /= (float)scalar; return *this; }
	
	auto operator ==(const float scalar) const -> bool { return Length() == scalar; }
	auto operator !=(const float scalar) const -> bool { return Length() != scalar; }
	auto operator <(const float scalar) const -> bool { return Length() < scalar; }
	auto operator >(const float scalar) const -> bool { return Length() > scalar; }
	auto operator <=(const float scalar) const -> bool { return Length() <= scalar; }
	auto operator >=(const float scalar) const -> bool { return Length() >= scalar; }

	auto operator ()(const float x, const float y = 0.0f, const float z = 0.0f, const float w = 0.0f) -> Vector4O& { this->x = x; this->y = y; this->z = z; this->w = w; return *this; }
};
