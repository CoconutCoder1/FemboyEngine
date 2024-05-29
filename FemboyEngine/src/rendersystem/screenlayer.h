#pragma once

#include "typeinfo/object.h"

#include "layer.h"

namespace fe::render {

class ScreenLayer : public Inherit<Layer, ScreenLayer> {
public:
	virtual void Draw(RenderContext* pRenderContext) {
		LayerIterator_t* pLayerIter = GetChildLayers().GetHead();
		while (pLayerIter) {
			RenderLayer(pRenderContext, pLayerIter->pData);
			pLayerIter = pLayerIter->pNext;
		}
	}

	virtual std::string GetName() { return "Screen Layer"; }
	
private:
	void RenderLayer(RenderContext* pRenderContext, Layer* pLayer) {
		pLayer->Draw(pRenderContext);

		auto* pChildIter = pLayer->GetChildLayers().GetHead();
		if (pChildIter) {
			RenderLayer(pRenderContext, pChildIter->pData);

			pChildIter = pChildIter->pNext;
		}
	}
};

}