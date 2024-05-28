#include "vector.h"

#include <cmath>

namespace fe::math {

float VectorLengthSqr(const Vector3& v) {
	return v.x * v.x + v.y * v.y + v.z * v.z;
}

float VectorLength(const Vector3& v) {
	return sqrtf(VectorLengthSqr(v));
}

float DotProduct(const Vector3& A, const Vector3& B) {
	return (A.x * B.x + A.y * B.y + A.z * B.z);
}

Vector3 CrossProduct(const Vector3& A, const Vector3& B) {
	return Vector3(
		A.y * B.z - A.z * B.y,
		A.z * B.x - A.x * B.z,
		A.x * B.y - A.y * B.x
	);
}

Vector3 NormalizeVector(const Vector3& v) {
	float ratio = 1.f / VectorLength(v);

	return Vector3(
		v.x * ratio,
		v.y * ratio,
		v.z * ratio
	);
}

Vector3 VectorSubtract(const Vector3& A, const Vector3& B) {
	return Vector3(
		A.x - B.x,
		A.y - B.y,
		A.z - B.z
	);
}

}