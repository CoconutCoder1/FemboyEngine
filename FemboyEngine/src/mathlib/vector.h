#pragma once

namespace fe::math {

class Vector3 {
public:
	float x, y, z;

	float& operator[](size_t index) {
		return ((float*)&x)[index];
	}

	const float& operator[](size_t index) const {
		return ((float*)&x)[index];
	}

	constexpr Vector3() {
		x = y = z = 0.f;
	}

	constexpr Vector3(float x, float y, float z)
		: x(x), y(y), z(z) {}
};

class Vector4 {
public:
	float x, y, z, w;

	float& operator[](size_t index) {
		return ((float*)&x)[index];
	}

	const float& operator[](size_t index) const {
		return ((float*)&x)[index];
	}

	constexpr Vector4() {
		x = y = z = w = 0.f;
	}

	constexpr Vector4(float x, float y, float z, float w)
		: x(x), y(y), z(z), w(w) {}

	constexpr Vector4(const Vector3& xyz, float w)
		: x(xyz.x), y(xyz.y), z(xyz.z), w(w) {}
};

float VectorLengthSqr(const Vector3& v);
float VectorLength(const Vector3& v);
float DotProduct(const Vector3& A, const Vector3& B);

Vector3 CrossProduct(const Vector3& A, const Vector3& B);
Vector3 NormalizeVector(const Vector3& v);
Vector3 VectorSubtract(const Vector3& A, const Vector3& B);

}