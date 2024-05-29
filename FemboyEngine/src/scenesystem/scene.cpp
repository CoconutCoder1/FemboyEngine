#include "scene.h"

namespace fe {

Scene::Scene(std::string&& name)
	: m_Name(std::move(name))
{
	m_pDefaultCamera = ScopedPtr<Camera>(new Camera());
	m_pActiveCamera = m_pDefaultCamera.get();
}

SceneObject* Scene::CreateSceneObject() {
	SceneObjectIterator_t* pIterator = m_SceneObjects.AddToTail(nullptr);
	pIterator->pData = new SceneObject(this, pIterator);

	return pIterator->pData;
}

Camera* Scene::GetActiveCamera() const {
	return m_pActiveCamera;
}

void Scene::SetActiveCamera(Camera* pCamera) {
	m_pActiveCamera = pCamera;
}

}