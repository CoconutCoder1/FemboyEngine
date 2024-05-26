#pragma once

#include "renderresource.h"
#include "viewport.h"

#include <array>

namespace fe::render {

class RenderDevice;

class RenderContext {
public:
	virtual ~RenderContext() = default;

	virtual void SetViewports(const Viewport_t* const ppViewports, uint32_t numViewports) = 0;

	virtual void ClearRenderTarget(const RenderTarget* pRenderTarget, const std::array<float, 4>& clearColor) = 0;
	virtual void SetRenderTargets(const RenderTarget** ppRenderTargets, uint32_t numRenderTargets) = 0;

	virtual void SetPrimtiveTopology(PrimitiveToplogy::Enum topology) = 0;
	virtual void SetVertexBuffer(Buffer* pVertexBuffer) = 0;

	virtual void Draw(uint32_t vertexCount, uint32_t startVertexLocation) = 0;
};

}