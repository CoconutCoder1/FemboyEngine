#include "componentmanager.h"

#include <SDL3/SDL_assert.h>

namespace fe {

void ComponentManager::RegisterComponent(Component* pComponent) {
	const TypeInfo* pTypeInfo = pComponent->GetTypeInfo();

	SDL_assert(pTypeInfo->isDerived<Component>() == true);

	m_Components[pTypeInfo->typeIndex].AddToTail(pComponent);
}

void ComponentManager::UnregisterComponent(Component* pComponent) {
	TypeIndex componentType = pComponent->GetTypeInfo()->typeIndex;

	auto componentListPos = m_Components.find(componentType);
	if (componentListPos != m_Components.end())
		componentListPos->second.Remove(pComponent->GetIterator());
}

}