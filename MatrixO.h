#pragma once

#include "FrameWork.h"

#include <DirectXMath.h>

class MatrixO {
public:
	float m11, m12, m13, m14;
	float m21, m22, m23, m24;
	float m31, m32, m33, m34;
	float m41, m42, m43, m44;

	MatrixO() : m11(0), m12(0), m13(0), m14(0),
		m21(0), m22(0), m23(0), m24(0),
		m31(0), m32(0), m33(0), m34(0),
		m41(0), m42(0), m43(0), m44(0) {
	}

	MatrixO(float m11, float m12, float m13, float m14,
		float m21, float m22, float m23, float m24,
		float m31, float m32, float m33, float m34,
		float m41, float m42, float m43, float m44) :
		m11(m11), m12(m12), m13(m13), m14(m14),
		m21(m21), m22(m22), m23(m23), m24(m24),
		m31(m31), m32(m32), m33(m33), m34(m34),
		m41(m41), m42(m42), m43(m43), m44(m44) {
	}

	MatrixO(const MatrixO& other) :
		m11(other.m11), m12(other.m12), m13(other.m13), m14(other.m14),
		m21(other.m21), m22(other.m22), m23(other.m23), m24(other.m24),
		m31(other.m31), m32(other.m32), m33(other.m33), m34(other.m34),
		m41(other.m41), m42(other.m42), m43(other.m43), m44(other.m44) {
	}

	MatrixO& operator=(const MatrixO& other) {
		if (this != &other) {
			m11 = other.m11; m12 = other.m12; m13 = other.m13; m14 = other.m14;
			m21 = other.m21; m22 = other.m22; m23 = other.m23; m24 = other.m24;
			m31 = other.m31; m32 = other.m32; m33 = other.m33; m34 = other.m34;
			m41 = other.m41; m42 = other.m42; m43 = other.m43; m44 = other.m44;
		}
		return *this;
	}

	XMMATRIX ToXMMATRIX() const {
		return XMMATRIX(
			m11, m12, m13, m14,
			m21, m22, m23, m24,
			m31, m32, m33, m34,
			m41, m42, m43, m44
		);
	}
};