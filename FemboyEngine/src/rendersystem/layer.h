#pragma once

#include "typeinfo/object.h"

#include "fstdlib/linkedlist.h"

#include "renderdevice.h"
#include "rendercontext.h"

namespace fe::render {

using LayerList_t = LinkedList<class Layer>;
using LayerIterator_t = LayerList_t::Iterator_t;

class Layer : public Inherit<Object, Layer> {
public:
	virtual ~Layer() = default;

	virtual void OnAttach(RenderDevice* pDevice) {}
	virtual void OnDetach(RenderDevice* pDevice) {}
	virtual void Draw(RenderContext* pRenderContext) {}

	virtual std::string GetName() { return "Unnamed Layer"; }

	const LinkedList<Layer>& GetChildLayers() const { return m_ChildLayers; }

private:
	LayerIterator_t* m_pIterator;
	LinkedList<Layer> m_ChildLayers;
};

}