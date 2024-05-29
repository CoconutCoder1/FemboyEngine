#include "sceneobject.h"

namespace fe {

SceneObject::SceneObject(Scene* pScene, SceneObjectIterator_t* pIterator) {
	m_pScene = pScene;
	m_pIterator = pIterator;
}

SceneObject::~SceneObject() {
	/*ComponentIterator_t* pComponentIter = m_Components.GetHead();
	while (pComponentIter) {
		ComponentIterator_t* pNextIter = pComponentIter->pNext;

		if (pComponentIter->pData) {
			delete pComponentIter->pData;
		}

		pComponentIter = pNextIter;
	}*/

	m_Components.ClearAndDeleteElements();
}

ComponentIterator_t* SceneObject::AddComponent(Component* pComponent) {
	return m_Components.AddToTail(pComponent);
}

void SceneObject::RemoveComponent(ComponentIterator_t* pIterator) {
	if (pIterator) {
		m_Components.Remove(pIterator);
	}
}

}