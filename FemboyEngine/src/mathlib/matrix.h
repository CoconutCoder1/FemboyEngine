#pragma once

#include "vector.h"

#include <memory>

namespace fe::math {

class Matrix3x3 {
public:
	Vector3 m[3];

	constexpr Matrix3x3() = default;

	constexpr Matrix3x3(const Vector3& m0, const Vector3& m1, const Vector3& m2) {
		m[0] = m0;
		m[1] = m1;
		m[2] = m2;
	}
};

class Matrix3x4 {
public:
	Vector4 m[3];

	constexpr Matrix3x4() = default;

	constexpr Matrix3x4(const Vector4& m0, const Vector4& m1, const Vector4& m2) {
		m[0] = m0;
		m[1] = m1;
		m[2] = m2;
	}
};

class Matrix4x4 {
public:
	Vector4 m[4];

	Vector4& operator[](size_t index) {
		return m[index];
	}

	const Vector4& operator[](size_t index) const {
		return m[index];
	}

	void Copy(float* pDst) {
		memcpy(pDst, &m[0].x, sizeof(float) * 4 * 4);
	}

	constexpr Matrix4x4() = default;

	constexpr Matrix4x4(const Vector4& m0, const Vector4& m1, const Vector4& m2, const Vector4& m3) {
		m[0] = m0;
		m[1] = m1;
		m[2] = m2;
		m[3] = m3;
	}
};

Matrix4x4 MakePerspectiveFovRH(float fov, float aspect, float nearPlane, float farPlane);
Matrix4x4 MakePerspectiveFovLH(float fov, float aspect, float nearPlane, float farPlane);
Matrix4x4 MakeLookToRH(const Vector3& eyePosition, const Vector3& forward, const Vector3& up);
Matrix4x4 MakeLookAtRH(const Vector3& eyePosition, const Vector3& focusPoint, const Vector3& up);

Matrix4x4 MatrixMultiply(const Matrix4x4& A, const Matrix4x4& B);
Matrix4x4 TransposeMatrix(const Matrix4x4& v);

}