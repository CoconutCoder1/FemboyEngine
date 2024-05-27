#pragma once

#include <cstdint>
#include <string>
#include <vector>

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

struct RenderResourceIterator_t {
	class RenderResource* pResource;
	RenderResourceIterator_t* pPrevious;
	RenderResourceIterator_t* pNext;
};

class RenderResource {
	friend class RenderDevice;

public:
	virtual ~RenderResource() = default;
	virtual uint32_t Release() = 0;

private:
	RenderResourceIterator_t* m_pIterator = nullptr;
};

struct RenderResourceList_t {
	RenderResourceIterator_t* pHead;
	RenderResourceIterator_t* pTail;

	RenderResourceList_t() {
		pHead = nullptr;
		pTail = nullptr;
	}

	~RenderResourceList_t() {
		
	}

	void Clear() {
		while (pHead) {
			RenderResourceIterator_t* pNextIter = pHead->pNext;

			delete pHead;
			pHead = pNextIter;
		}

		pTail = nullptr;
		pHead = nullptr;
	}

	void AddToHead(RenderResource* pResource) {
		RenderResourceIterator_t* pPreviousHead = pHead;

		pHead = new RenderResourceIterator_t();
		pHead->pResource = pResource;
		pHead->pNext = nullptr;
		pHead->pPrevious = nullptr;

		if (!pTail) {
			pTail = pHead;
		}

		if (pPreviousHead) {
			pPreviousHead->pPrevious = pHead;
			pHead->pNext = pPreviousHead;
		}
	}

	void AddToTail(RenderResource* pResource) {
		RenderResourceIterator_t* pPreviousTail = pTail;

		pTail = new RenderResourceIterator_t();
		pTail->pResource = pResource;
		pTail->pNext = nullptr;
		pTail->pPrevious = nullptr;

		if (!pHead) {
			pHead = pTail;
		}

		if (pPreviousTail) {
			pPreviousTail->pNext = pTail;
			pTail->pPrevious = pPreviousTail;
		}
	}

	void Remove(RenderResourceIterator_t* pIterator) {
		RenderResourceIterator_t* pNext = pIterator->pNext;
		RenderResourceIterator_t* pPrevious = pIterator->pPrevious;

		if (pNext) {
			pNext->pPrevious = pPrevious;
		}
		if (pPrevious) {
			pPrevious->pNext = pNext;
		}

		if (pIterator == pHead) {
			pHead = pNext;
		}
		if (pIterator == pTail) {
			pTail = pPrevious;
		}

		delete pIterator;
	}
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