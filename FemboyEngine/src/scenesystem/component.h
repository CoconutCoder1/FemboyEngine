#pragma once

#include "typeinfo/object.h"

#include "fstdlib/linkedlist.h"

namespace fe {

class SceneObject;

using ComponentList_t = LinkedList<class Component>;
using ComponentIterator_t = ComponentList_t::Iterator_t;

class Component : public Inherit<Object, Component> {
public:
	Component();
	virtual ~Component();

	ComponentIterator_t* GetIterator() const;

	SceneObject* GetOwner() const;
	void SetOwner(SceneObject* pOwner);

private:
	SceneObject* m_pOwner;
	ComponentIterator_t* m_pIterator;
};

}