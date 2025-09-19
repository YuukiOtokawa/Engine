#pragma once

#include "FrameWork.h"

class MatrixO {
public:
	float m[4][4]; // 4x4 matrix stored in row-major order

	MatrixO() {
		m[0][0] = 1.0f; m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = 0.0f;
		m[1][0] = 0.0f; m[1][1] = 1.0f; m[1][2] = 0.0f; m[1][3] = 0.0f;
		m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 1.0f; m[2][3] = 0.0f;
		m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;
	}

	MatrixO& operator=(const MatrixO& other) {
		if (this != &other) {
			for (int i = 0; i < 4; ++i) {
				for (int j = 0; j < 4; ++j) {
					m[i][j] = other.m[i][j];
				}
			}
		}
		return *this;
	}

	inline MatrixO __vectorcall OrthographicOffCenterLH(
		float left,
		float right,
		float bottom,
		float top,
		float nearZ,
		float farZ
	) {
		assert(right - left <= 0.00001f);
		assert(top - bottom <= 0.00001f);
		assert(farZ - nearZ <= 0.00001f);

		MatrixO m;
		
	}

};