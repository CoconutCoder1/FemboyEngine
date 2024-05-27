#include "rhi.h"

#include "dx11/renderdevicedx11.h"

#include <SDL3/SDL_assert.h>

namespace fe::render {

RHI::RHI(GraphicsAPI::Enum graphicsApi) {
	m_GraphicsApi = graphicsApi;
}

RenderDevice* RHI::CreateRenderDevice(const RenderDeviceParams_t& params) {
	RenderDevice* pDevice = nullptr;

	switch (m_GraphicsApi) {
	case GraphicsAPI::DirectX11:
		pDevice = m_pRenderDeviceList.emplace_back(new RenderDeviceDx11()).get();
		break;
	}

	if (!pDevice->Initialize(params)) {
		m_pRenderDeviceList.pop_back();
		return nullptr;
	}

	pDevice->SetDeviceParams(params);

	return pDevice;
}

void RHI::RemoveRenderDevice(RenderDevice* pDevice) {
	auto devicePos = std::find_if(m_pRenderDeviceList.begin(), m_pRenderDeviceList.end(), [pDevice](ScopedPtr<RenderDevice>& pRenderDevice) {
		return pRenderDevice.get() == pDevice;
		});

	SDL_assert(devicePos != m_pRenderDeviceList.end() && "Could not find device for removal");

	pDevice->Shutdown();

	if (pDevice->IsDebuggingEnabled()) {
		pDevice->ReportLiveObjects();
	}

	m_pRenderDeviceList.erase(devicePos);
}

}