#pragma once

namespace fe::render {

struct Float2 {
	float x, y;

	Float2() {
		x = y = 0.f;
	}

	Float2(float x, float y)
		: x(x), y(y) {}
};

struct Float3 {
	float x, y, z;

	Float3() {
		x = y = z = 0.f;
	}

	Float3(float x, float y, float z)
		: x(x), y(y), z(z) {}
};

struct Float4 {
	float x, y, z, w;

	Float4() {
		x = y = z = w = 0.f;
	}

	Float4(float x, float y, float z, float w)
		: x(x), y(y), z(z), w(w) {}
};

struct Float3x3 {
	Float3 m[3];

	Float3x3() {}

	Float3x3(const Float3& m0, const Float3& m1, const Float3& m2) {
		m[0] = m0;
		m[1] = m1;
		m[2] = m2;
	}
};

struct Float3x4 {
	Float4 m[3];

	Float3x4() {}

	Float3x4(const Float4& m0, const Float4& m1, const Float4& m2) {
		m[0] = m0;
		m[1] = m1;
		m[2] = m2;
	}
};

struct Float4x4 {
	Float4 m[4];

	Float4x4() {}

	Float4x4(const Float4& m0, const Float4& m1, const Float4& m2, const Float4& m3) {
		m[0] = m0;
		m[1] = m1;
		m[2] = m2;
		m[3] = m3;
	}
};

}