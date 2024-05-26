#include "rhi.h"

#include "dx11/renderdevicedx11.h"

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

	return pDevice;
}

}