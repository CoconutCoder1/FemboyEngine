#pragma once

#include <cstdint>

namespace fe::render {

struct RenderFormat {
	enum Enum : uint32_t {
		R8G8B8A8_UNorm,
		R32_Float,
		R32G32B32A32_Float,
	};
};

struct PrimitiveToplogy {
	enum Enum {
		TriangleList,
	};
};

struct BufferUsage {
	enum Enum : uint32_t {
		Default,
		Immutable,
		Dynamic,
		Staging
	};
};

struct CPUAccessFlags {
	enum Enum : uint32_t {
		Read,
		Write,
		All = static_cast<uint32_t>(~0)
	};
};

struct MiscFlags {
	enum Enum : uint32_t {
		StructuredBuffer = (1 << 0),
	};
};

struct BindFlags {
	enum Enum {
		VertexBuffer = (1 << 0),
		IndexBuffer = (1 << 1),
		ConstantBuffer = (1 << 2),
		DepthStencil = (1 << 3),
		RenderTarget = (1 << 4),
		ShaderResource = (1 << 5),
		UnorderedAccess = (1 << 6),
	};
};

struct BufferDesc {
	const void* pInitData;
	uint32_t sizeInBytes;
	BufferUsage::Enum usage;
	uint32_t bindFlags;
	uint32_t cpuAccessFlags;
};

struct TextureDesc {
	const void* pInitData;
	uint32_t width;
	uint32_t height;
	uint32_t depth;
	RenderFormat::Enum format;
	uint32_t mipLevels;
	uint32_t bindFlags;
};

class Texture2D {
public:
	virtual ~Texture2D() = default;
};

class RenderTarget {
public:
	virtual ~RenderTarget() = default;
};

class Buffer {
public:
	virtual ~Buffer() = default;
};

}