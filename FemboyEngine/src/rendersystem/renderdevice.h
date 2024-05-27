#pragma once

#include "swapchain.h"
#include "rendercontext.h"
#include "renderresource.h"
#include "shadercompiler.h"

#include <list>

namespace fe::render {

struct RenderDeviceParams_t {
	bool enableDebugging;
};

class RenderDevice {
public:
	virtual ~RenderDevice() = default;

	virtual bool Initialize(const RenderDeviceParams_t& params) = 0;
	virtual uint32_t ReleaseResource(RenderResource* pResource) = 0;
	virtual void ReportLiveObjects() = 0;

	virtual RenderContext* GetImmediateContext() const = 0;
	virtual RenderContext* CreateDeferredContext() = 0;
	virtual SwapChain* CreateSwapChain(const SwapChainParams_t& params) = 0;

	virtual VertexShader* CreateVertexShader(const void* pBytecode, size_t bytecodeLength) = 0;
	virtual PixelShader* CreatePixelShader(const void* pBytecode, size_t bytecodeLength) = 0;
	virtual InputLayout* CreateInputLayout(InputElement_t const* const pElements, uint32_t numElements, const void* pShaderBytecode, size_t bytecodeLength) = 0;

	virtual Buffer* CreateVertexBuffer(uint32_t numVertices, uint32_t strideInBytes, BufferUsage::Enum usage, const void* pInitData) = 0;

	virtual Texture2D* CreateTexture2D(uint32_t width, uint32_t height, RenderFormat::Enum format, const void* pInitData = nullptr, uint32_t arraySize = 1u) = 0;

	virtual RenderTarget* CreateRenderTarget(Texture2D* pTexture) = 0;

	virtual ShaderCompiler* GetShaderCompiler() const = 0;

	inline void SetDeviceParams(const RenderDeviceParams_t& params) {
		m_DeviceParams = params;
	}

	inline const RenderDeviceParams_t& GetDeviceParams() const {
		return m_DeviceParams;
	}

	inline bool IsDebuggingEnabled() const {
		return m_DeviceParams.enableDebugging;
	}

	uint32_t ReleaseResourceEx(RenderResource* pResource, bool unregisterNullRef) {
		uint32_t newRefCount = pResource->Release();
		if (newRefCount == 0) {
			if (unregisterNullRef)
				UnregisterResource(pResource);
			delete pResource;
		}
		return newRefCount;
	}

	RenderResource* RegisterResource(RenderResource* pResource) {
		m_RenderResourceList.AddToTail(pResource);

		pResource->m_pIterator = m_RenderResourceList.pTail;

		return pResource;
	}

	void UnregisterResource(RenderResource* pResource) {
		if (pResource->m_pIterator) {
			m_RenderResourceList.Remove(pResource->m_pIterator);
		}
	}

	template<typename T>
	T* RegisterResource(T* pResource) {
		static_assert(std::is_base_of_v<RenderResource, T>);
		return static_cast<T*>(RegisterResource(static_cast<RenderResource*>(pResource)));
	}

protected:
	RenderDeviceParams_t m_DeviceParams;
	RenderResourceList_t m_RenderResourceList;
};

}