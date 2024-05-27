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

}