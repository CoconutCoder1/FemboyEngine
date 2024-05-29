#pragma once

#include "typeinfo/object.h"

#include "fstdlib/linkedlist.h"

#include "sceneobject.h"

namespace fe {

class Scene : public Inherit<Object, Scene> {
public:
	Scene(std::string&& name);

	SceneObject* CreateSceneObject();

private:
	std::string m_Name;
	LinkedList<SceneObject> m_SceneObjects;
};

}