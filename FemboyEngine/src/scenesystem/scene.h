#pragma once

#include "typeinfo/object.h"

#include "fstdlib/linkedlist.h"

#include "sceneobject.h"
#include "camera.h"

namespace fe {

class Scene : public Inherit<Object, Scene> {
public:
	Scene(std::string&& name);

	SceneObject* CreateSceneObject();

	Camera* GetActiveCamera() const;
	void SetActiveCamera(Camera* pCamera);

private:
	std::string m_Name;
	LinkedList<SceneObject> m_SceneObjects;

	Camera* m_pActiveCamera;
	ScopedPtr<Camera> m_pDefaultCamera;
};

}