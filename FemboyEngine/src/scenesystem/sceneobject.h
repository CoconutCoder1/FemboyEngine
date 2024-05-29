#pragma once

#include "component.h"

#include "typeinfo/object.h"

#include "fstdlib/linkedlist.h"

namespace fe {

class Scene;

using SceneObjectList_t = LinkedList<class SceneObject>;
using SceneObjectIterator_t = SceneObjectList_t::Iterator_t;

class SceneObject : public Inherit<Object, SceneObject> {
public:
	SceneObject(Scene* pScene, SceneObjectIterator_t* pIterator);
	virtual ~SceneObject();

	ComponentIterator_t* AddComponent(Component* pComponent);

	// Component::m_pIterator
	void RemoveComponent(ComponentIterator_t* pIterator);



private:
	Scene* m_pScene;
	SceneObjectIterator_t* m_pIterator;
	LinkedList<Component> m_Components;
};

}