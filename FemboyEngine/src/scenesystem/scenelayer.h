#pragma once

#include "rendersystem/layer.h"

namespace fe {

class SceneLayer : public render::Layer {
public:
	virtual ~SceneLayer() = default;

	virtual void OnAttach(render::RenderDevice* pDevice);
	virtual void OnDetach(render::RenderDevice* pDevice);
	virtual void Draw(render::RenderContext* pRenderContext);

	virtual std::string GetName() { return "Scene Layer"; }
	
private:

};

}