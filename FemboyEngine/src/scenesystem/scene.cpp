#include "scene.h"

namespace fe {

Scene::Scene(std::string&& name)
	: m_Name(std::move(name))
{

}

SceneObject* Scene::CreateSceneObject() {
	SceneObjectIterator_t* pIterator = m_SceneObjects.AddToTail(nullptr);
	pIterator->pData = new SceneObject(this, pIterator);

	return pIterator->pData;
}

}