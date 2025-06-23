#pragma once

#include "FrameWork.h"

class Vector2O
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

