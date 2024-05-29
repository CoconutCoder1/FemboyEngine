#include "component.h"
#include "sceneobject.h"

namespace fe {

Component::Component() {
	m_pOwner = nullptr;
	m_pIterator = nullptr;
}

Component::~Component() {
	if (m_pOwner && m_pIterator) {
		m_pOwner->RemoveComponent(m_pIterator);
	}
}

ComponentIterator_t* Component::GetIterator() const {
	return m_pIterator;
}

SceneObject* Component::GetOwner() const {
	return m_pOwner;
}

void Component::SetOwner(SceneObject* pOwner) {
	if (m_pOwner && m_pIterator) {
		m_pOwner->RemoveComponent(m_pIterator);
		m_pIterator = pOwner->AddComponent(this);
	}

	m_pOwner = pOwner;
}

}