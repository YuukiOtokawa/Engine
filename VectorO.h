#pragma once

//#include "FrameWork.h"

#include <DirectXMath.h>
using namespace DirectX;

struct Vector2O
{
public:
    float x = 0.0f;
    float y = 0.0f;

    static constexpr float PI = 3.14159265358979323846f;

    Vector2O() = default;

    Vector2O(const Vector2O&) = default;

    Vector2O(float x, float y = 0.0f)
        : x(x), y(y) {
    }

    static Vector2O Zero() { return Vector2O(0.0f, 0.0f); }
    static Vector2O One() { return Vector2O(1.0f, 1.0f); }
    static Vector2O Up() { return Vector2O(0.0f, 1.0f); }
    static Vector2O Down() { return Vector2O(0.0f, -1.0f); }
    static Vector2O Left() { return Vector2O(-1.0f, 0.0f); }
    static Vector2O Right() { return Vector2O(1.0f, 0.0f); }
    static Vector2O UnitX() { return Vector2O(1.0f, 0.0f); }
    static Vector2O UnitY() { return Vector2O(0.0f, 1.0f); }
    static float Dot(const Vector2O& vec1, const Vector2O& vec2) {
        return vec1.x * vec2.x + vec1.y * vec2.y;
    }

    float Length() const { return sqrtf(x * x + y * y); }
    void Normalize() {
        float len = Length();
        if (len > 0.0f) {
            x /= len; y /= len;
        }
    }

    Vector2O ToRadian() const {
        return Vector2O(
            x * (PI / 180.0f),
            y * (PI / 180.0f)
        );
    }
    Vector2O ToEuler() const {
        return Vector2O(
            x * (180.0f / PI),
            y * (180.0f / PI)
        );
    }

    Vector2O Cross(const Vector2O& other) const {
        return Vector2O(
            y * other.x - x * other.y,
            x * other.y - y * other.x
        );
    }
    auto operator=(const Vector2O&) -> Vector2O & = default;
    auto operator=(const XMFLOAT2& vec) -> Vector2O& {
        x = vec.x; y = vec.y;
        return *this;
    }

    Vector2O operator+(const Vector2O& other) const {
        return Vector2O(x + other.x, y + other.y);
    }
    Vector2O operator-(const Vector2O& other) const {
        return Vector2O(x - other.x, y - other.y);
    }
    Vector2O operator*(float scalar) const {
        return Vector2O(x * scalar, y * scalar);
    }
    Vector2O operator/(float scalar) const {
        if (scalar != 0.0f) {
            return Vector2O(x / scalar, y / scalar);
        }
        return Vector2O(0.0f, 0.0f); // Avoid division by zero
    }
    bool operator==(const Vector2O& other) const {
        return (x == other.x && y == other.y);
    }
    bool operator!=(const Vector2O& other) const {
        return !(*this == other);
    }
    Vector2O& operator+=(const Vector2O& other) {
        x += other.x; y += other.y;
        return *this;
    }
    Vector2O& operator-=(const Vector2O& other) {
        x -= other.x; y -= other.y;
        return *this;
    }
    Vector2O& operator*=(float scalar) {
        x *= scalar; y *= scalar;
        return *this;
    }
    Vector2O& operator/=(float scalar) {
        if (scalar != 0.0f) {
            x /= scalar; y /= scalar;
        }
        return *this; // Avoid division by zero
    }
    Vector2O operator-() const {
        return Vector2O(-x, -y);
    }

};


struct Vector3O
{
public:
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    static constexpr float PI = 3.14159265358979323846f;

    Vector3O() = default;

    Vector3O(const Vector3O&) = default;

    Vector3O(float x, float y = 0.0f, float z = 0.0f)
        : x(x), y(y), z(z) {
    }

    static Vector3O Zero() { return Vector3O(0.0f, 0.0f, 0.0f); }
    static Vector3O One() { return Vector3O(1.0f, 1.0f, 1.0f); }
    static Vector3O Up() { return Vector3O(0.0f, 1.0f, 0.0f); }
    static Vector3O Down() { return Vector3O(0.0f, -1.0f, 0.0f); }
    static Vector3O Left() { return Vector3O(-1.0f, 0.0f, 0.0f); }
    static Vector3O Right() { return Vector3O(1.0f, 0.0f, 0.0f); }
    static Vector3O Forward() { return Vector3O(0.0f, 0.0f, 1.0f); }
    static Vector3O Backward() { return Vector3O(0.0f, 0.0f, -1.0f); }
    static Vector3O UnitX() { return Vector3O(1.0f, 0.0f, 0.0f); }
    static Vector3O UnitY() { return Vector3O(0.0f, 1.0f, 0.0f); }
    static Vector3O UnitZ() { return Vector3O(0.0f, 0.0f, 1.0f); }
    static float Dot(const Vector3O& vec1, const Vector3O& vec2) {
        return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
    }

    float Length() const { return sqrtf(x * x + y * y + z * z); }
    void Normalize() {
        float len = Length();
        if (len > 0.0f) {
            x /= len; y /= len; z /= len;
        }
    }

    Vector3O ToRadian() const {
        return Vector3O(
            x * (PI / 180.0f),
            y * (PI / 180.0f),
            z * (PI / 180.0f)
        );
    }
    Vector3O ToEuler() const {
        return Vector3O(
            x * (180.0f / PI),
            y * (180.0f / PI),
            z * (180.0f / PI)
        );
    }

    Vector3O Cross(const Vector3O& vec) const {
        return Vector3O(
            y * vec.z - z * vec.y,
            z * vec.x - x * vec.z,
            x * vec.y - y * vec.x
        );
    }

    auto operator=(const Vector3O&) -> Vector3O & = default;
    auto operator=(const XMFLOAT2& vec) -> Vector3O& {
        x = vec.x; y = vec.y; z = 0.0f;
        return *this;
    }
    auto operator=(const XMFLOAT3& vec) -> Vector3O& {
        x = vec.x; y = vec.y; z = vec.z;
        return *this;
    }


    auto operator +(const Vector3O& vec) const -> Vector3O { return Vector3O(x + vec.x, y + vec.y, z + vec.z); }
    auto operator -(const Vector3O& vec) const -> Vector3O { return Vector3O(x - vec.x, y - vec.y, z - vec.z); }
    auto operator *(const Vector3O& vec) const -> Vector3O { return Vector3O(x * vec.x, y * vec.y, z * vec.z); }
    auto operator /(const Vector3O& vec) const -> Vector3O { return Vector3O(x / vec.x, y / vec.y, z / vec.z); }
    auto operator *(const float scalar) const -> Vector3O { return Vector3O(x * scalar, y * scalar, z * scalar); }
    auto operator /(const float scalar) const -> Vector3O { return Vector3O(x / scalar, y / scalar, z / scalar); }
    auto operator +=(const Vector3O& vec) -> Vector3O& { x += vec.x; y += vec.y; z += vec.z; return *this; }
    auto operator -=(const Vector3O& vec) -> Vector3O& { x -= vec.x; y -= vec.y; z -= vec.z; return *this; }
    auto operator *=(const Vector3O& vec) -> Vector3O& { x *= vec.x; y *= vec.y; z *= vec.z; return *this; }
    auto operator /=(const Vector3O& vec) -> Vector3O& { x /= vec.x; y /= vec.y; z /= vec.z; return *this; }
    auto operator *=(const float scalar) -> Vector3O& { x *= scalar; y *= scalar; z *= scalar; return *this; }
    auto operator /=(const float scalar) -> Vector3O& { x /= scalar; y /= scalar; z /= scalar; return *this; }
    auto operator ==(const Vector3O& vec) const -> bool { return (x == vec.x && y == vec.y && z == vec.z); }
    auto operator !=(const Vector3O& vec) const -> bool { return (x != vec.x || y != vec.y || z != vec.z); }
    auto operator <(const Vector3O& vec) const -> bool { return (x < vec.x && y < vec.y && z < vec.z); }
    auto operator >(const Vector3O& vec) const -> bool { return (x > vec.x && y > vec.y && z > vec.z); }
    auto operator <=(const Vector3O& vec) const -> bool { return (x <= vec.x && y <= vec.y && z <= vec.z); }
    auto operator >=(const Vector3O& vec) const -> bool { return (x >= vec.x && y >= vec.y && z >= vec.z); }

    auto operator [](const int index) const -> float { return (index == 0) ? x : (index == 1) ? y : z; }

    auto operator +(const int scalar) const -> Vector3O { return Vector3O(x + (float)scalar, y + (float)scalar, z + (float)scalar); }
    auto operator -(const int scalar) const -> Vector3O { return Vector3O(x - (float)scalar, y - (float)scalar, z - (float)scalar); }
    auto operator *(const int scalar) const -> Vector3O { return Vector3O(x * (float)scalar, y * (float)scalar, z * (float)scalar); }
    auto operator /(const int scalar) const -> Vector3O { return Vector3O(x / (float)scalar, y / (float)scalar, z / (float)scalar); }
    auto operator +=(const int scalar) -> Vector3O& { x += (float)scalar; y += (float)scalar; z += (float)scalar; return *this; }
    auto operator -=(const int scalar) -> Vector3O& { x -= (float)scalar; y -= (float)scalar; z -= (float)scalar; return *this; }
    auto operator *=(const int scalar) -> Vector3O& { x *= (float)scalar; y *= (float)scalar; z *= (float)scalar; return *this; }
    auto operator /=(const int scalar) -> Vector3O& { x /= (float)scalar; y /= (float)scalar; z /= (float)scalar; return *this; }

    auto operator ==(const float scalar) const -> bool { return Length() == scalar; }
    auto operator !=(const float scalar) const -> bool { return Length() != scalar; }
    auto operator <(const float scalar) const -> bool { return Length() < scalar; }
    auto operator >(const float scalar) const -> bool { return Length() > scalar; }
    auto operator <=(const float scalar) const -> bool { return Length() <= scalar; }
    auto operator >=(const float scalar) const -> bool { return Length() >= scalar; }

    auto operator ()(const float x, const float y = 0.0f, const float z = 0.0f, const float w = 0.0f) -> Vector3O& { this->x = x; this->y = y; this->z = z; return *this; }

};


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
