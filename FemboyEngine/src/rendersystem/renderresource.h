#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "fstdlib/linkedlist.h"

namespace fe::render {

struct RenderFormat {
	enum Enum : uint32_t {
		Unknown,
		R8G8B8A8_UNorm,
		R32_Float,
		R32G32B32_Float,
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

constexpr uint32_t InputElement_ByteOffset_Append = static_cast<uint32_t>(-1);

struct InputElement_t {
	std::string semanticName;
	uint32_t semanticIndex;
	RenderFormat::Enum format;
	uint32_t byteOffset;

	InputElement_t()
		: semanticName(""), semanticIndex(0), format(RenderFormat::Unknown), byteOffset(-1) {}

	InputElement_t(const std::string& semanticName, uint32_t semanticIndex, RenderFormat::Enum format, uint32_t byteOffset = InputElement_ByteOffset_Append)
		: semanticName(semanticName), semanticIndex(semanticIndex), format(format), byteOffset(byteOffset) {}
};

using RenderResourceList_t = LinkedList<class RenderResource>;
using RenderResourceIterator_t = RenderResourceList_t::Iterator_t;

class RenderResource {
	friend class RenderDevice;

public:
	virtual ~RenderResource() = default;
	virtual uint32_t Release() = 0;

private:
	RenderResourceIterator_t* m_pIterator = nullptr;
};

class Texture2D : public RenderResource {
public:
	virtual ~Texture2D() = default;
};

class RenderTarget : public RenderResource {
public:
	virtual ~RenderTarget() = default;
};

class Buffer : public RenderResource {
public:
	virtual ~Buffer() = default;
};

class VertexShader : public RenderResource {
public:
	virtual ~VertexShader() = default;
};

class PixelShader : public RenderResource {
public:
	virtual ~PixelShader() = default;
};

class InputLayout : public RenderResource {
public:
	virtual ~InputLayout() = default;
};

}