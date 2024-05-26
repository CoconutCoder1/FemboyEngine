#pragma once

namespace fe::render {

struct Viewport_t {
	float x, y;
	float width, height;
	float minDepth, maxDepth;

	Viewport_t()
		: x(0.f), y(0.f), width(0.f), height(0.f), minDepth(0.f), maxDepth(1.f) {}

	Viewport_t(float width, float height)
		: x(0.f), y(0.f), width(width), height(height), minDepth(0.f), maxDepth(1.f) {}

	Viewport_t(float x, float y, float width, float height, float minDepth = 0.f, float maxDepth = 1.f)
		: x(0.f), y(0.f), width(width), height(height), minDepth(minDepth), maxDepth(maxDepth) {}
};

}