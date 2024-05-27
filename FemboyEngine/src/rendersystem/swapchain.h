#pragma once

#include "renderresource.h"

#include <cstdint>

namespace fe::render {

class RenderDevice;

struct SwapChainParams_t {
	SwapChainParams_t() {
		width = 0;
		height = 0;
		bufferCount = 0;
		pOutputWindow = nullptr;
	}

	SwapChainParams_t(void* pOutputWindow, uint32_t bufferCount, uint32_t width, uint32_t height, bool isFullscreen)
		: pOutputWindow(pOutputWindow), bufferCount(bufferCount), width(width), height(height), isFullscreen(isFullscreen) {}

	uint32_t width;
	uint32_t height;
	uint32_t bufferCount;
	void* pOutputWindow;
	bool isFullscreen;
};

class SwapChain {
public:
	virtual ~SwapChain() = default;

	virtual bool Initialize(RenderDevice* pDevice, const SwapChainParams_t& params) = 0;

	virtual void ResizeBuffers() = 0;
	virtual void Present() = 0;

	virtual uint32_t GetSyncInterval() const = 0;
	virtual void SetSyncInterval(uint32_t syncInterval) = 0;

	virtual Texture2D* GetBackBuffer() = 0;
	virtual RenderTarget* GetBackBufferTarget() = 0;
	virtual uint32_t GetNumBackBuffers() const = 0;
};

}