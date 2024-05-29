#pragma once

#include "typeinfo/object.h"

#include "renderdevice.h"
#include "rendercontext.h"

namespace fe::render {

class Layer : public Inherit<Object, Layer> {
public:
	virtual void OnAttach() {}
	virtual void OnDetach() {}
	virtual void Draw(RenderContext* pRenderContext) {}

	virtual std::string GetName() { return "Unnamed Layer"; }
};

}