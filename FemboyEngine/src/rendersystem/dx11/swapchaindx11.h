#pragma once

#include "../swapchain.h"

#include "fstdlib/pointers.h"

#include "dx11.h"

#include <dxgi1_4.h>

namespace fe::render {

/*
class SwapChainDx11 : public SwapChain {
public:
	virtual ~SwapChainDx11() = default;

	virtual bool Initialize(RenderDevice* pDevice, const SwapChainParams_t& params);

	virtual void Present();

	virtual uint32_t GetSyncInterval() const;
	virtual void SetSyncInterval(uint32_t syncInterval);

	virtual Texture2D* GetBackBuffer(size_t index);
	virtual uint32_t GetNumBackBuffers() const;

private:
	wrl::ComPtr<IDXGIFactory1> m_pFactory;
	wrl::ComPtr<IDXGISwapChain3> m_pSwapChain;
	UINT m_SyncInterval;

	std::vector<ScopedPtr<Texture2D>> m_pBackBufferList;

	SwapChainParams_t m_SwapChainParams;
};*/

}