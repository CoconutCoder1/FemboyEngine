#include "swapchaindx11.h"
#include "renderdevicedx11.h"

#pragma comment(lib, "dxgi.lib")

namespace fe::render {

/*
bool SwapChainDx11::Initialize(RenderDevice* pDevice, const SwapChainParams_t& params) {
	RenderDeviceDx11* pDeviceDx11 = static_cast<RenderDeviceDx11*>(pDevice);

	m_SwapChainParams = params;

	ThrowIfFailed(CreateDXGIFactory1(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(m_pFactory.GetAddressOf())));

	DXGI_SWAP_CHAIN_DESC scd = {};
	scd.BufferCount = params.bufferCount;
	scd.SampleDesc.Count = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.Windowed = TRUE;
	scd.OutputWindow = reinterpret_cast<HWND>(params.pOutputWindow);
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	if (scd.BufferCount > 1) {
		scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	}
	else {
		scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	}

	wrl::ComPtr<IDXGISwapChain> pSwapChain;
	ThrowIfFailed(m_pFactory->CreateSwapChain(pDeviceDx11->D3D11Device(), &scd, &pSwapChain));

	ThrowIfFailed(pSwapChain.As(&m_pSwapChain));

	for (uint32_t i = 0; i < params.bufferCount; i++) {
		wrl::ComPtr<ID3D11Texture2D> texturePtr;

	}

	return true;
}

void SwapChainDx11::Present() {
	m_pSwapChain->Present(m_SyncInterval, NULL);
}

uint32_t SwapChainDx11::GetSyncInterval() const {
	return m_SyncInterval;
}

void SwapChainDx11::SetSyncInterval(uint32_t syncInterval) {
	m_SyncInterval = syncInterval;
}

Texture2D* SwapChainDx11::GetBackBuffer(size_t index) {
	return m_pBackBufferList.size() < index ? m_pBackBufferList[index].Get() : nullptr;
}

uint32_t SwapChainDx11::GetNumBackBuffers() const {
	return m_SwapChainParams.bufferCount;
}*/

}