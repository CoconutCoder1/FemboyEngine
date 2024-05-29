#pragma once

#include "component.h"

#include "typeinfo/object.h"

#include <unordered_map>

namespace fe {

class ComponentManager : public Inherit<Object, ComponentManager> {
public:
	void RegisterComponent(Component* pComponent);
	void UnregisterComponent(Component* pComponent);

private:
	std::unordered_map<TypeIndex, LinkedList<Component>> m_Components;
};

}