#pragma once

#include "FrameWork.h"

class Vector3O
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

    auto operator ()(const int x, const int y = 0, const int z = 0, const int w = 0) -> Vector3O& { this->x = (float)x; this->y = (float)y; this->z = (float)z; return *this; }
    auto operator ()(const float x, const float y = 0.0f, const float z = 0.0f, const float w = 0.0f) -> Vector3O& { this->x = x; this->y = y; this->z = z; return *this; }

};

