#pragma once

#include "renderdevice.h"
#include "graphicsapi.h"

#include "fstdlib/pointers.h"

#include <vector>

namespace fe::render {

class RHI {
public:
	RHI(GraphicsAPI::Enum graphicsApi);

	virtual RenderDevice* CreateRenderDevice(const RenderDeviceParams_t& params);
	virtual void RemoveRenderDevice(RenderDevice* pDevice);

private:
	GraphicsAPI::Enum m_GraphicsApi;
	std::vector<ScopedPtr<RenderDevice>> m_pRenderDeviceList;
};

}