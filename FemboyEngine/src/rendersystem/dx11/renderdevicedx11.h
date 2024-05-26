#pragma once

#include "fstdlib/pointers.h"

#include "../renderdevice.h"
#include "../rendercontext.h"

#include "dx11.h"

#include <vector>
#include <array>

namespace fe::render {

class RenderContextDx11;

class SwapChainDx11 : public SwapChain {
public:
	virtual ~SwapChainDx11() = default;

	virtual bool Initialize(RenderDevice* pDevice, const SwapChainParams_t& params);

	virtual void Present();

	virtual uint32_t GetSyncInterval() const;
	virtual void SetSyncInterval(uint32_t syncInterval);

	virtual Texture2D* GetBackBuffer();
	virtual uint32_t GetNumBackBuffers() const;

private:
	wrl::ComPtr<IDXGIFactory1> m_pFactory;
	wrl::ComPtr<IDXGISwapChain3> m_pSwapChain;
	UINT m_SyncInterval = 1U;

	ScopedPtr<Texture2D> m_pBackBufferList;

	SwapChainParams_t m_SwapChainParams;
};

class RenderDeviceDx11 : public RenderDevice {
public:
	virtual ~RenderDeviceDx11();

	virtual bool Initialize(const RenderDeviceParams_t& params);
	virtual void ReleaseResource(RenderResource* pResource);

	virtual RenderContext* GetImmediateContext() const;
	virtual RenderContext* CreateDeferredContext();
	virtual SwapChain* CreateSwapChain(const SwapChainParams_t& params);

	virtual VertexShader* CreateVertexShader(const void* pBytecode, size_t bytecodeLength);
	virtual PixelShader* CreatePixelShader(const void* pBytecode, size_t bytecodeLength);
	virtual InputLayout* CreateInputLayout(InputElement_t const* const pElements, uint32_t numElements, const void* pShaderBytecode, size_t bytecodeLength);

	virtual Buffer* CreateVertexBuffer(uint32_t numVertices, uint32_t strideInBytes, BufferUsage::Enum usage, const void* pInitData);

	virtual Texture2D* CreateTexture2D(uint32_t width, uint32_t height, RenderFormat::Enum format, const void* pInitData, uint32_t arraySize);

	virtual RenderTarget* CreateRenderTarget(Texture2D* pTexture);

	ID3D11Device* D3D11Device() const;

private:
	wrl::ComPtr<ID3D11Device> m_pDevice;
	wrl::ComPtr<ID3D11DeviceContext> m_pDeviceContext;

	ScopedPtr<RenderContextDx11> m_pImmediateContext;
	std::vector<ScopedPtr<SwapChainDx11>> m_pSwapChainList;
};

class RenderContextDx11 : public RenderContext {
public:
	RenderContextDx11(wrl::ComPtr<ID3D11DeviceContext> pContext);
	virtual ~RenderContextDx11() = default;

	virtual void SetViewports(const Viewport_t* const ppViewports, uint32_t numViewports);

	virtual void ClearRenderTarget(const RenderTarget* pRenderTarget, const std::array<float, 4>& clearColor);
	virtual void SetRenderTargets(const RenderTarget** ppRenderTargets, uint32_t numRenderTargets);

	virtual void SetPrimtiveTopology(PrimitiveToplogy::Enum topology);
	virtual void SetVertexBuffer(Buffer* pVertexBuffer);

	virtual void Draw(uint32_t vertexCount, uint32_t startVertexLocation);

private:
	wrl::ComPtr<ID3D11DeviceContext> m_pContext;
};

}