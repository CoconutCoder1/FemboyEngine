#include "matrix.h"

#include <cmath>

namespace fe::math {

Matrix4x4 MakePerspectiveFovRH(float fov, float aspect, float nearPlane, float farPlane) {
	float halfFov = 1.f / tanf(fov * 0.5f);

	return Matrix4x4(
		Vector4(halfFov * aspect, 0.f, 0.f, 0.f),
		Vector4(0.f, halfFov, 0.f, 0.f),
		Vector4(0.f, 0.f, -(farPlane + nearPlane) / (farPlane - nearPlane), (-2.f * farPlane * nearPlane) / (farPlane - nearPlane)),
		Vector4(0.f, 0.f, -1.f, 0.f)
	);
}

Matrix4x4 MakePerspectiveFovLH(float fov, float aspect, float nearPlane, float farPlane) {
	float halfFov = 1.f / tanf(fov * 0.5f);

	return Matrix4x4(
		Vector4(halfFov * aspect, 0.f, 0.f, 0.f),
		Vector4(0.f, halfFov, 0.f, 0.f),
		Vector4(0.f, 0.f, (farPlane + nearPlane) / (farPlane - nearPlane), (-2.f * farPlane * nearPlane) / (farPlane - nearPlane)),
		Vector4(0.f, 0.f, 1.f, 0.f)
	);
}

Matrix4x4 MakeLookToRH(const Vector3& eyePosition, const Vector3& f, const Vector3& up) {
	Matrix4x4 result;

	Vector3 r = CrossProduct(f, up);
	Vector3 u = CrossProduct(r, f);

	result[0] = Vector4(-r.x, r.y, r.z, DotProduct(eyePosition, r));
	result[1] = Vector4(u.x, u.y, u.z, DotProduct(eyePosition, u));
	result[2] = Vector4(f.x, f.y, f.z, DotProduct(eyePosition, f));
	result[3] = Vector4(0.f, 0.f, 0.f, 1.f);

	return result;
}

Matrix4x4 MakeLookAtRH(const Vector3& eyePosition, const Vector3& focusPoint, const Vector3& up) {
	return MakeLookToRH(eyePosition, NormalizeVector(VectorSubtract(focusPoint, eyePosition)), up);
}

Matrix4x4 MatrixMultiply(const Matrix4x4& A, const Matrix4x4& B) {
	Matrix4x4 result;

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			for (int k = 0; k < 4; k++) {
				result[i][j] += A[i][k] * B[k][j];
			}
		}
	}

	return result;
}

Matrix4x4 TransposeMatrix(const Matrix4x4& v) {
	Matrix4x4 result;

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			result[j][i] = v[i][j];
		}
	}

	return result;
}

}